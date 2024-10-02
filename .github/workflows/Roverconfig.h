# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
#
# Find more information at:
# https://github.com/microsoft/msvc-code-analysis-action

name: Microsoft C++ Code Analysis

on: #include "mode.h"
#include "Rover.h"

bool ModeRTL::_enter()
{
    // refuse RTL if home has not been set
    if (!AP::ahrs().home_is_set()) {
        return false;
    }

    // initialise waypoint speed
    set_desired_speed_to_default(true);

    // set destination
    set_desired_location(rover.home);

    // RTL never reverses
    rover.set_reverse(false);

    return true;
}

void ModeRTL::update()
{
    // calculate distance to home
    _distance_to_destination = get_distance(rover.current_loc, _destination);
    const bool near_wp = _distance_to_destination <= rover.g.waypoint_radius;
    // check if we've reached the destination
    if (!_reached_destination && (near_wp || location_passed_point(rover.current_loc, _origin, _destination))) {
        // trigger reached
        _reached_destination = true;
        gcs().send_text(MAV_SEVERITY_INFO, "Reached destination");
    }
    // determine if we should keep navigating
    if (!_reached_destination || (rover.is_boat() && !near_wp)) {
        // continue driving towards destination
        calc_steering_to_waypoint(_reached_destination ? rover.current_loc :_origin, _destination);
        calc_throttle(calc_reduced_speed_for_turn_or_distance(_desired_speed), true, false);
    } else {
        // we've reached destination so stop
        stop_vehicle();
    }
}
  push: #include "mode.h"
#include "Rover.h"

bool ModeSmartRTL::_enter()
{
    // SmartRTL requires EKF (not DCM)
    Location ekf_origin;
    if (!ahrs.get_origin(ekf_origin)) {
        return false;
    }

    // refuse to enter SmartRTL if smart RTL's home has not been set
    if (!g2.smart_rtl.is_active()) {
        return false;
    }

    // initialise waypoint speed
    set_desired_speed_to_default(true);

    // init location target
    set_desired_location(rover.current_loc);

    // RTL never reverses
    rover.set_reverse(false);

    // init state
    smart_rtl_state = SmartRTL_WaitForPathCleanup;
    _reached_destination = false;

    return true;
}

void ModeSmartRTL::update()
{
    switch (smart_rtl_state) {
        case SmartRTL_WaitForPathCleanup:
            // check if return path is computed and if yes, begin journey home
            if (g2.smart_rtl.request_thorough_cleanup()) {
                smart_rtl_state = SmartRTL_PathFollow;
                _load_point = true;
            }
            // Note: this may lead to an unnecessary 20ms slow down of the vehicle (but it is unlikely)
            stop_vehicle();
            break;

        case SmartRTL_PathFollow:
            // load point if required
            if (_load_point) {
                Vector3f next_point;
                if (!g2.smart_rtl.pop_point(next_point)) {
                    // if not more points, we have reached home
                    gcs().send_text(MAV_SEVERITY_INFO, "Reached destination");
                    smart_rtl_state = SmartRTL_StopAtHome;
                    break;
                }
                _load_point = false;
                // set target destination to new point
                if (!set_desired_location_NED(next_point)) {
                    // this failure should never happen but we add it just in case
                    gcs().send_text(MAV_SEVERITY_INFO, "SmartRTL: failed to set destination");
                    smart_rtl_state = SmartRTL_Failure;
                }
            }
            // check if we've reached the next point
            _distance_to_destination = get_distance(rover.current_loc, _destination);
            if (_distance_to_destination <= rover.g.waypoint_radius || location_passed_point(rover.current_loc, _origin, _destination)) {
                _load_point = true;
            }
            // continue driving towards destination
            calc_steering_to_waypoint(_origin, _destination);
            calc_throttle(calc_reduced_speed_for_turn_or_distance(_desired_speed), true, false);
            break;

        case SmartRTL_StopAtHome:
        case SmartRTL_Failure:
            _reached_destination = true;
            if (rover.is_boat()) {
                // boats attempt to hold position at home
                calc_steering_to_waypoint(rover.current_loc, _destination);
                calc_throttle(calc_reduced_speed_for_turn_or_distance(_desired_speed), true, false);
            } else {
                // rovers stop
                stop_vehicle();
            }
            break;
    }
}

// save current position for use by the smart_rtl flight mode
void ModeSmartRTL::save_position()
{
    const bool save_pos = (rover.control_mode != &rover.mode_smartrtl);
    g2.smart_rtl.update(true, save_pos);
}
    branches: [ "main" ]
  pull_request: #include "mode.h"
#include "Rover.h"

void ModeSteering::update()
{
    float desired_steering, desired_throttle;
    get_pilot_desired_steering_and_throttle(desired_steering, desired_throttle);

    // convert pilot throttle input to desired speed (up to twice the cruise speed)
    float target_speed = desired_throttle * 0.01f * calc_speed_max(g.speed_cruise, g.throttle_cruise * 0.01f);

    // get speed forward
    float speed;
    if (!attitude_control.get_forward_speed(speed)) {
        // no valid speed so stop
        g2.motors.set_throttle(0.0f);
        g2.motors.set_steering(0.0f);
        return;
    }

    // determine if pilot is requesting pivot turn
    bool is_pivot_turning = g2.motors.have_skid_steering() && is_zero(target_speed) && (!is_zero(desired_steering));

    // In steering mode we control lateral acceleration directly.
    // For pivot steering vehicles we use the TURN_MAX_G parameter
    // For regular steering vehicles we use the maximum lateral acceleration at full steering lock for this speed: V^2/R where R is the radius of turn.
    float max_g_force;
    if (is_pivot_turning) {
        max_g_force = g.turn_max_g * GRAVITY_MSS;
    } else {
        max_g_force = speed * speed / MAX(g2.turn_radius, 0.1f);
    }

    // constrain to user set TURN_MAX_G
    max_g_force = constrain_float(max_g_force, 0.1f, g.turn_max_g * GRAVITY_MSS);

    // convert pilot steering input to desired lateral acceleration
    float desired_lat_accel = max_g_force * (desired_steering / 4500.0f);

    // reverse target lateral acceleration if backing up
    bool reversed = false;
    if (is_negative(target_speed)) {
        reversed = true;
        desired_lat_accel = -desired_lat_accel;
    }

    // mark us as in_reverse when using a negative throttle
    rover.set_reverse(reversed);

    // apply object avoidance to desired speed using half vehicle's maximum acceleration/deceleration
    rover.g2.avoid.adjust_speed(0.0f, 0.5f * attitude_control.get_accel_max(), ahrs.yaw, target_speed, rover.G_Dt);

    // run lateral acceleration to steering controller
    calc_steering_from_lateral_acceleration(desired_lat_accel, reversed);

    // run speed to throttle controller
    calc_throttle(target_speed, false, true);
}
    branches: [ "main" ]
  schedule: #include "Rover.h"

/*
  mavlink motor test - implements the MAV_CMD_DO_MOTOR_TEST mavlink command so that the GCS/pilot can test an individual motor or flaps
                       to ensure proper wiring, rotation.
 */

// motor test definitions
static const int16_t MOTOR_TEST_PWM_MAX = 2200; // max pwm value accepted by the test
static const int16_t MOTOR_TEST_TIMEOUT_MS_MAX = 30000; // max timeout is 30 seconds

static uint32_t motor_test_start_ms = 0;        // system time the motor test began
static uint32_t motor_test_timeout_ms = 0;      // test will timeout this many milliseconds after the motor_test_start_ms
static uint8_t motor_test_seq = 0;              // motor sequence number of motor being tested
static uint8_t motor_test_throttle_type = 0;    // motor throttle type (0=throttle percentage, 1=PWM, 2=pilot throttle channel pass-through)
static int16_t motor_test_throttle_value = 0;   // throttle to be sent to motor, value depends upon it's type

// motor_test_output - checks for timeout and sends updates to motors objects
void Rover::motor_test_output()
{
    // exit immediately if the motor test is not running
    if (!motor_test) {
        return;
    }

    // check for test timeout
    if ((AP_HAL::millis() - motor_test_start_ms) >= motor_test_timeout_ms) {
        // stop motor test
        motor_test_stop();
    } else {
        bool test_result = false;
        // calculate  based on throttle type
        switch (motor_test_throttle_type) {
            case MOTOR_TEST_THROTTLE_PERCENT:
                test_result = g2.motors.output_test_pct((AP_MotorsUGV::motor_test_order)motor_test_seq, motor_test_throttle_value);
                break;

            case MOTOR_TEST_THROTTLE_PWM:
                test_result = g2.motors.output_test_pwm((AP_MotorsUGV::motor_test_order)motor_test_seq, motor_test_throttle_value);
                break;

            case MOTOR_TEST_THROTTLE_PILOT:
                if ((AP_MotorsUGV::motor_test_order)motor_test_seq == AP_MotorsUGV::MOTOR_TEST_STEERING) {
                    test_result = g2.motors.output_test_pct((AP_MotorsUGV::motor_test_order)motor_test_seq, channel_steer->norm_input_dz() * 100.0f);
                } else {
                    test_result = g2.motors.output_test_pct((AP_MotorsUGV::motor_test_order)motor_test_seq, channel_throttle->get_control_in());
                }
                break;

            default:
                // do nothing
                return;
        }
        // stop motor test on failure
        if (!test_result) {
            motor_test_stop();
        }
    }
}

// mavlink_motor_test_check - perform checks before motor tests can begin
// return true if tests can continue, false if not
bool Rover::mavlink_motor_test_check(mavlink_channel_t chan, bool check_rc, uint8_t motor_seq, uint8_t throttle_type, int16_t throttle_value)
{
    GCS_MAVLINK_Rover &gcs_chan = gcs().chan(chan-MAVLINK_COMM_0);

    // check board has initialised
    if (!initialised) {
        gcs_chan.send_text(MAV_SEVERITY_CRITICAL, "Motor Test: Board initialising");
        return false;
    }

    // check rc has been calibrated
    if (check_rc && !arming.pre_arm_rc_checks(true)) {
        gcs_chan.send_text(MAV_SEVERITY_CRITICAL, "Motor Test: RC not calibrated");
        return false;
    }

    // check if safety switch has been pushed
    if (hal.util->safety_switch_state() == AP_HAL::Util::SAFETY_DISARMED) {
        gcs_chan.send_text(MAV_SEVERITY_CRITICAL, "Motor Test: Safety switch");
        return false;
    }

    // check motor_seq
    if (motor_seq > AP_MotorsUGV::MOTOR_TEST_THROTTLE_RIGHT) {
        gcs_chan.send_text(MAV_SEVERITY_CRITICAL, "Motor Test: invalid motor (%d)", (int)motor_seq);
        return false;
    }

    // check throttle type
    if (throttle_type > MOTOR_TEST_THROTTLE_PILOT) {
        gcs_chan.send_text(MAV_SEVERITY_CRITICAL, "Motor Test: invalid throttle type: %d", (int)throttle_type);
        return false;
    }

    // check throttle value
    if (throttle_type == MOTOR_TEST_THROTTLE_PWM && throttle_value > MOTOR_TEST_PWM_MAX) {
        gcs_chan.send_text(MAV_SEVERITY_CRITICAL, "Motor Test: pwm (%d) too high", (int)throttle_value);
        return false;
    }
    if (throttle_type == MOTOR_TEST_THROTTLE_PERCENT && throttle_value > 100) {
        gcs_chan.send_text(MAV_SEVERITY_CRITICAL, "Motor Test: percentage (%d) too high", (int)throttle_value);
        return false;
    }

    // if we got this far the check was successful and the motor test can continue
    return true;
}

// mavlink_motor_test_start - start motor test - spin a single motor at a specified pwm
// returns MAV_RESULT_ACCEPTED on success, MAV_RESULT_FAILED on failure
MAV_RESULT Rover::mavlink_motor_test_start(mavlink_channel_t chan, uint8_t motor_seq, uint8_t throttle_type, int16_t throttle_value, float timeout_sec)
{
    // if test has not started try to start it
    if (!motor_test) {
        /* perform checks that it is ok to start test
           The RC calibrated check can be skipped if direct pwm is
           supplied
        */
        if (!mavlink_motor_test_check(chan, throttle_type != 1, motor_seq, throttle_type, throttle_value)) {
            return MAV_RESULT_FAILED;
        } else {
            // start test
            motor_test = true;

            // arm motors
            if (!arming.is_armed()) {
                arm_motors(AP_Arming::NONE);
            }

            // disable failsafes
            g.fs_gcs_enabled = 0;
            g.fs_throttle_enabled = 0;
            g.fs_crash_check = 0;

            // turn on notify leds
            AP_Notify::flags.esc_calibration = true;
        }
    }

    // set timeout
    motor_test_start_ms = AP_HAL::millis();
    motor_test_timeout_ms = MIN(timeout_sec * 1000, MOTOR_TEST_TIMEOUT_MS_MAX);

    // store required output
    motor_test_seq = motor_seq;
    motor_test_throttle_type = throttle_type;
    motor_test_throttle_value = throttle_value;

    // return success
    return MAV_RESULT_ACCEPTED;
}

// motor_test_stop - stops the motor test
void Rover::motor_test_stop()
{
    // exit immediately if the test is not running
    if (!motor_test) {
        return;
    }

    // disarm motors
    disarm_motors();

    // reset timeout
    motor_test_start_ms = 0;
    motor_test_timeout_ms = 0;

    // re-enable failsafes
    g.fs_gcs_enabled.load();
    g.fs_throttle_enabled.load();
    g.fs_crash_check.load();

    // turn off notify leds
    AP_Notify::flags.esc_calibration = false;

    // flag test is complete
    motor_test = false;
}
    - cron: #include "Rover.h"

/*
  allow for runtime change of control channel ordering
 */
void Rover::set_control_channels(void)
{
    // check change on RCMAP
    channel_steer    = RC_Channels::rc_channel(rcmap.roll()-1);
    channel_throttle = RC_Channels::rc_channel(rcmap.throttle()-1);
    channel_aux      = RC_Channels::rc_channel(g.aux_channel-1);

    // set rc channel ranges
    channel_steer->set_angle(SERVO_MAX);
    channel_throttle->set_angle(100);

    // Allow to reconfigure ouput when not armed
    if (!arming.is_armed()) {
        g2.motors.setup_servo_output();
        // For a rover safety is TRIM throttle
        g2.motors.setup_safety_output();
    }
    // setup correct scaling for ESCs like the UAVCAN PX4ESC which
    // take a proportion of speed. Default to 1000 to 2000 for systems without
    // a k_throttle output
    hal.rcout->set_esc_scaling(1000, 2000);
    g2.servo_channels.set_esc_scaling_for(SRV_Channel::k_throttle);
}

void Rover::init_rc_in()
{
    // set rc dead zones
    channel_steer->set_default_dead_zone(30);
    channel_throttle->set_default_dead_zone(30);
}

void Rover::init_rc_out()
{
    // set auxiliary ranges
    update_aux();
}

/*
  check for driver input on rudder/steering stick for arming/disarming
*/
void Rover::rudder_arm_disarm_check()
{
    // In Rover we need to check that its set to the throttle trim and within the DZ
    // if throttle is not within trim dz, then pilot cannot rudder arm/disarm
    if (!channel_throttle->in_trim_dz()) {
        rudder_arm_timer = 0;
        return;
    }

    // check if arming/disarming allowed from this mode
    if (!control_mode->allows_arming_from_transmitter()) {
        rudder_arm_timer = 0;
        return;
    }

    if (!arming.is_armed()) {
        // when not armed, full right rudder starts arming counter
        if (channel_steer->get_control_in() > 4000) {
            const uint32_t now = millis();

            if (rudder_arm_timer == 0 ||
                now - rudder_arm_timer < ARM_DELAY_MS) {
                if (rudder_arm_timer == 0) {
                    rudder_arm_timer = now;
                }
            } else {
                // time to arm!
                arm_motors(AP_Arming::RUDDER);
                rudder_arm_timer = 0;
            }
        } else {
            // not at full right rudder
            rudder_arm_timer = 0;
        }
    } else if (!motor_active()) {
        // when armed and motor not active (not moving), full left rudder starts disarming counter
        if (channel_steer->get_control_in() < -4000) {
            const uint32_t now = millis();

            if (rudder_arm_timer == 0 ||
                now - rudder_arm_timer < ARM_DELAY_MS) {
                if (rudder_arm_timer == 0) {
                    rudder_arm_timer = now;
                }
            } else {
                // time to disarm!
                disarm_motors();
                rudder_arm_timer = 0;
            }
        } else {
            // not at full left rudder
            rudder_arm_timer = 0;
        }
    }
}

void Rover::read_radio()
{
    if (!RC_Channels::read_input()) {
        // check if we lost RC link
        control_failsafe(channel_throttle->get_radio_in());
        return;
    }

    failsafe.last_valid_rc_ms = AP_HAL::millis();
    // check that RC value are valid
    control_failsafe(channel_throttle->get_radio_in());

    // check if we try to do RC arm/disarm
    rudder_arm_disarm_check();
}

void Rover::control_failsafe(uint16_t pwm)
{
    if (!g.fs_throttle_enabled) {
        // no throttle failsafe
        return;
    }

    // Check for failsafe condition based on loss of GCS control
    if (rc_override_active) {
        failsafe_trigger(FAILSAFE_EVENT_RC, (millis() - failsafe.rc_override_timer) > 1500);
    } else if (g.fs_throttle_enabled) {
        bool failed = pwm < static_cast<uint16_t>(g.fs_throttle_value);
        if (AP_HAL::millis() - failsafe.last_valid_rc_ms > 2000) {
            failed = true;
        }
        failsafe_trigger(FAILSAFE_EVENT_THROTTLE, failed);
    }
}

void Rover::trim_control_surfaces()
{
    read_radio();
    // Store control surface trim values
    // ---------------------------------
    if ((channel_steer->get_radio_in() > 1400) && (channel_steer->get_radio_in() < 1600)) {
        channel_steer->set_radio_trim(channel_steer->get_radio_in());
        // save to eeprom
        channel_steer->save_eeprom();
    }
}

void Rover::trim_radio()
{
    for (uint8_t y = 0; y < 30; y++) {
        read_radio();
    }
    trim_control_surfaces();
}

env: APMrover2 Release Notes:
========================
Rover V3.3.0-rc1 19-Apr-2018
Changes from 3.2.3:
1) Simple object avoidance support
2) Circular and polygon fence support
3) Pivot turn improvements
    a) Heading control used to aim at new target 
    b) ATC_STR_ACC_MAX limits maximum rotational acceleration
    c) ATC_STR_RAT_MAX limits maximum rotation rate
4) Allow ACRO mode without GPS for skid-steering vehicles (reverts to manual throttle)
5) MOT_THR_MIN used as deadzone
6) Parameter default changes:
    a) PIVOT_TURN_ANGLE default raised from 30 to 60 degrees
    b) ATC_STR_RAT_P and I reduced to 0.2
    c) ATC_STR_RAT_FILT and ATC_SPEED_FILT reduced from 50hz to 10hz to better match vehicle response time
7) Boats report themselves as boats allowing ground stations to display boat icon on map
8) ChibiOS support
--------------------------------
Rover V3.2.3 09-Apr-2018 / V3.2.3-rc1/rc2 02-Apr-2018
Changes from 3.2.2:
1) Waypoint origin uses previous waypoint or vehicle stopping point
2) Boats send correct mav-type to ground station
3) TURN_MAX_G parameter description updates (allows lower values)
4) fix two-paddle input decoding
--------------------------------
Rover V3.2.2 19-Mar-2018 / V3.2.2-rc1 08-Mar-2018
Changes from 3.2.1:
1) Fix loss of steering control when stopping in Acro and Steering modes
--------------------------------
APM:Rover V3.2.1 29-Jan-2018 / V3.2.1-rc1 24-Jan-2018 
Changes from 3.2.0:
1) Fix mode switch logic to allow Auto, Guided, RTL, SmartRTL, Steering, Acro using non-GPS navigation (i.e. wheel encoders, visual odometry)
--------------------------------
APM:Rover V3.2.0 13-Jan-2018
Changes from 3.2.0-rc4:
1) Here GPS/compass default orientation fix (ICM20948)
2) PID values sent to ground station regardless of mode (see GCS_PID_MASK parameter)
3) Steering PID min and max ranges expanded
4) Mavlink message definition update
--------------------------------
APM:Rover V3.2.0-rc4 05-Jan-2018
Changes from 3.2.0-rc3:
1) Steering controller supports feed-forward (reduces wobble)
2) WP_SPEED, RTL_SPEED parameters allow speed independent of CRUISE_SPEED
3) Dataflash logging improvements for steering and throttle
4) Remove auto-trim at startup
5) GPS reports healthy even without 3d lock
---------------------------------
APM:Rover V3.2.0-rc3 05-Dec-2017
Changes from 3.2.0-rc2:
1) SmartRTL mode (retraces path back to home)
2) Acro mode (pilot controls speed and turn rate)
3) Guided mode ROS integration fixes
4) Steering mode allows pivot turns
5) Auto mode pivots on sharp corners
6) Aux switch allows arming/disarming and mode change
7) PILOT_STEER_TYPE parameter allows controlling turn direction when backing up
8) Mixer change to allow steering to use full motor range (removes need for MOT_SKID_FRIC)
---------------------------------
APM:Rover V3.2.0-rc2 28-Oct-2017
Changes from 3.2.0-rc1:
1) MOT_SKID_FRIC parameter allows increasing power for skid-steer vehicle pivot-turns
2) Bug Fixes:
    a) speed nudging fix in AUTO mode
    b) throttle slew range fix (was slightly incorrect when output range was not 1100 ~ 1900)
    c) PID desired and achieved reported to GCS when GCS_PID_MASK param set to non-zero value
    d) use-pivot fix to use absolute angle error
---------------------------------
APM:Rover V3.2.0-rc1 25-Aug-2017
Changes from 3.1.2:
1) Skid-steering vehicle support added (i.e. tank track or R2D2 style vehicles)
2) Brushless motor support
3) Improved speed/throttle and steering controllers:
    a) layered P and PID controllers with optional feedforward, input filtering and saturation handling (reduces unnecessary I-term build-up) 
    b) forward-back acceleration limited (see ATC_ACCEL_MAX parameter)
    c) pro-active slowing before waypoint in order to keep overshoot at or below WP_OVERSHOOT distance
    d) proper output scaling for skid-steering vs regular car steering controls
    e) TURN_RADIUS parameter added to allow better control of turn in Steering mode
    f) speed along forward-back axis used instead of total ground speed (resolves unusual behaviour for boats being washed downstream)
4) Auxiliary switch changes (see CH7_OPTION parameter):
    a) "Save Waypoint" saves the current location as a waypoint in all modes except AUTO if the vehicle is armed.  If disarmed the mission is cleared and home is set to the current location.
    b) "Learn Cruise" sets the THROTTLE_CRUISE and SPEED_CRUISE parameter values to the vehicle's current speed and throttle level 
5) Wheel encoder supported for non-GPS navigation (can also be used with GPS)
6) Visual Odometry support for non-GPS navigation (can also be used with GPS)
7) Guided mode improvements:
    a) for SET_ATTITUDE_TARGET accepts quaternions for target heading, valid thrust changed to -1 ~ +1 range (was previously 0 ~ 1)
    b) COMMAND_LONG.SET_YAW_SPEED support fixes (thrust field accepted as target speed in m/s)
    c) SET_POSITION_TARGET_GLOBAL_INT, _LOCAL_NED fixes and added support for yaw and yaw-rate fields
8) Bug Fixes:
    a) resolve occasional start of motors after power-on
    b) steering mode turn direction fix while reversing
    c) reversing in auto mode fixes (see DO_REVERSE mission command)
----------------------------------------------------------
Release 3.1.2, 15 March 2017
============================
Minor bugfix release.

- Crashing detection is off by default
- DISARMing of a rover via the transmitter stick works again
- If a user was driving in reverse in Manual and went into an AUTO
mode the rover would do the mission in reverse.  This is fixed.


Release 3.1.1, 31 January 2017
==============================
Minor bugfix release for a crash bug in the SRXL driver


Release 3.1.0, 22 December 2016
===============================
The ArduPilot development team is proud to announce the release of
version 3.1.0 of APM:Rover. This is a major release with a lot of
changes so please read the notes carefully!

A huge thanks to ALL the ArduPilot developers.  The Rover code
benefits tremendously from all the hard work that goes into the Copter
and Plane vehicle code.  Most of the code changes in this
release were not specifically for Rover however because of the
fantastic architecture of the ArduPilot code Rover automatically gets
those enhancements anyway.

Note that the documentation hasn't yet caught up with all the changes
in this release. We are still working on that, but meanwhile if you
see a feature that interests you and it isn't documented yet then
please ask.

The PX4-v2 build has had CANBUS support removed due to firmware size
issues.  If Rover users want CANBUS support you will need to install
the PX4-v3 build located in the firmware folder here:
http://firmware.ap.ardupilot.org/Rover/stable/PX4/

EKF1 has been removed as EKF2 has been the long term default and is
working extremely well and this has allowed room for EKF3.

EKF3 is included in this release but it is not the default.  Should
you want to experiment with it set the following parameters:
AHRS_EKF_TYPE=3
EK3_ENABLE=1
but note it is still experimental and you must fully understand the
implications.

New GUIDED Command
------------------
Rover now accepts a new message MAV_CMD_NAV_SET_YAW_SPEED which has an
angle in centidegrees and a speed scale and the rover will drive based
on these inputs.

The ArduPilot team would like to thank EnRoute for the sponsoring of
this feature
http://enroute.co.jp/

COMMAND_INT and ROI Commands
----------------------------
COMMAND_INT support has been added to Rover.  This has allowed the
implementation of SET_POSITION_TARGET_GLOBAL_INT, 
SET_POSITION_TARGET_LOCAL_NED and DO_SET_ROI as a COMMAND_INT

The ArduPilot team would like to thank EnRoute for the sponsoring of
this feature
http://enroute.co.jp/

Reverse
-------
Its now possible in a mission to tell the rover to drive in
Reverse. If using Mission Planner insert a new Waypoint using "Add
Below" button on the Flight Plan screen and select from the Command
drop down list you will see a new command "DO_SET_REVERSE". It takes 1
parameter - 0 for forward, 1 for reverse. It's that simple. Please give
it a try and report back any success or issues found or any questions
as well.

The ArduPilot team would like to thank the Institute for Intelligent
Systems Research at Deakin University
(http://www.deakin.edu.au/research/iisri16) for the sponsoring of the
reverse functionality.

Loiter
------
This changes brings the LOITER commands in line with other ArduPilot
vehicles so both NAV_LOITER_UNLIM and NAV_LOITER_TIME are supported and are
actively loitering. This means for instance if you have set a boat to
loiter at a particular position and the water current pushes the boat off
that position once the boat has drifted further then the WP_RADIUS
parameter distance setting from the position the motor(s) will be
engaged and the boat will return to the loiter position.

The ArduPilot team would like to thanko MarineTech for sponsoring this
enhancement.
http://www.marinetech.fr

Note: if you currently use Param1 of a NAV_WAYPOINT to loiter at a
waypoint this functionality has not changed and is NOT actively loitering.

Crash Check
-----------
Rover can now detect a crash in most circumstances - thanks Pierre
Kancir. It is enabled by default and will change the vehicle into HOLD
mode if a crash is detected.  FS_CRASH_CHECK is the parameter used to
control what action to take on a crash detection and it supports
0:Disabled, 1:HOLD, 2:HoldAndDisarm

Pixhawk2 heated IMU support
---------------------------
This release adds support for the IMU heater in the Pixhawk2,
allowing for more stable IMU temperatures. The Pixhawk2 is
automatically detected and the heater enabled at boot, with the target
IMU temperature controllable via BRD_IMU_TARGTEMP.
Using an IMU heater should improve IMU stability in environments with
significant temperature changes.

PH2SLIM Support
---------------
This release adds support for the PH2SLIM variant of the Pixhawk2,
which is a Pixhawk2 cube without the isolated sensor top board. This
makes for a very compact autopilot for small aircraft. To enable
PH2SLIM support set the BRD_TYPE parameter to 6 using a GCS connected
on USB.

AP_Module Support
-----------------
This is the first release of ArduPilot with loadable module support
for Linux based boards. The AP_Module system allows for externally
compiled modules to access sensor data from ArduPilot controlled
sensors. The initial AP_Module support is aimed at vendors integrating
high-rate digital image stabilisation using IMU data, but it is
expected this will be expanded to other use cases in future releases.

Major VRBrain Support Update
----------------------------
This release includes a major merge of support for the VRBrain family
of autopilots. Many thanks to the great work by Luke Mike in putting
together this merge!

Much Faster Boot Time
---------------------
Boot times on Pixhawk are now much faster due to a restructuring of
the driver startup code, with slow starting drivers not started unless
they are enabled with the appropriate parameters. The restructuring
also allows for support of a wide variety of board types, including
the PH2SLIM above.

This release includes many other updates right across the flight
stack, including several new features. Some of the changes include:

 - log all rally points on startup
 - the armed state is now logged
 - support added for MAV_CMD_ACCELCAL_VEHICLE_POS
 - support MAVLink based external GPS device
 - support LED_CONTROL MAVLink message
 - support PLAY_TUNE MAVLink message
 - added AP_Button support for remote button input reporting
 - support 16 channel SERVO_OUTPUT_RAW in MAVLink2
 - added MAVLink reporting of logging subsystem health
 - added BRD_SAFETY_MASK to allow for channel movement for selected channels with safety on
 - lots of HAL_Linux improvements to bus and thread handling
 - added IMU heater support on Pixhawk2
 - allow for faster accel bias learning in EKF2
 - added AP_Module support for loadable modules
 - merged support for wide range of VRBrain boards
 - added support for PH2SLIM and PHMINI boards with BRD_TYPE
 - greatly reduced boot time on Pixhawk and similar boards
 - fixed magic check for signing key in MAVLink2
 - fixed averaging of gyros for EKF2 gyro bias estimate
 - added support for ParametersG2
 - support added for the GPS_INPUT mavlink message



Release 3.0.1, 17 June 2016
===========================
The ArduPilot development team is proud to announce the release of
version 3.0.1 of APM:Rover. This is a minor release with small but
important bug fix changes.

The two main motivations for this release
1. Fixing the arming for skid steering Rovers
2. Fix to the rover steering that should really improve steering of
all rovers.

Skid Steering Arming
--------------------
Fixed arming for Skid Steering rovers. You should now be able to arm
your skid steering rover using the steering stick. NOTE skid steering
Rovers - you will not be able to disarm. The reason for this is zero
throttle full left turn is a perfectly valid move for a skid steering
rover as it can turn on the spot. You don't want to be executing this
and have the rover disarm part way through your turn so we have
disabled disarming via the steering stick. You can still disarm from
the GCS.  Thanks to Pierre Kancir for working on this.

Improved Steering Control
-------------------------
For historical reason's the steering controller was using the raw GPS
data for ground speed without any filtering. If you have every graphed
this data you will see on a rover its very spiky and all over the
place. This spiky'ness was feeding into the lateral accel demand and
causing inaccuracies/jitters. Now we using the EKF GPS filtered data
which is much smoother and accurate and the steering control has
greatly improved.

Improved Cornering
------------------
Previously when corning we didn't take into account any "lean or tilt"
in the rover - we assumed the turns were always flat. We have changed
this to now take into account any lean so turning should be more
accurate. Boat users should really benefit from this too.

MAVLink2 support has been added
-------------------------------
See this post by Tridge -
http://discuss.ardupilot.org/t/mavlink2-is-in-ardupilot-master/9188/1


The other changes in this release are:

 - setting your sonar_trigger_cm to 0 will now log the sonar data but
   not use it for vehicle avoidance.
 - the throttle PID is now being logged
 - range finder input is now being captured (thanks to Allan Matthew)
 - added LOG_DISARMED parameter
 - merge upstream PX4Firmware changes
 - numerous waf build improvements
 - numerous EKF2 improvements


Release 3.0.0, 5 April 2016
===========================
The ArduPilot development team is proud to announce the release of
version 3.0.0 of APM:Rover. This is a major release with a lot of
changes so please read the notes carefully!

A huge thanks to ALL the ArduPilot developers.  The Rover code
benefits tremendously from all the hard work that goes into the Copter
and Plane vehicle code.  Most of the code changes in this
release were not specifically for Rover however because of the
fantastic architecture of the ArduPilot code Rover automatically get's
those enhancements anyway.

Note that the documentation hasn't yet caught up with all the changes
in this release. We are still working on that, but meanwhile if you
see a feature that interests you and it isn't documented yet then
please ask.

The 3.x.x releases and above DON'T support APM1/APM2
----------------------------------------------------
This release DOES NOT SUPPORT the old APM1/APM2 AVR based boards. The
issue is the amount of effort required to keep the new code ported to
the old platforms.  We are very sorry this has to occur and if there
is someone who is willing and technically capable of doing this work
then please let us know.
There will be a discussion created on ArduPilot forums where people
can request features in the new code be backported to the APM code to
run on the AVR boards and if it is reasonably easy and they are
willing to do the beta testing we will do our best to make it happen.

EKF2 - New Kalman Filter
------------------------
Paul Riseborough has been working hard recently on the new
EKF2 variant which fixes many issues seen with the old estimator. The
key improvements are:

  - support for separate filters on each IMU for multi-IMU boards
    (such as the Pixhawk), giving a high degree of redundency
  - much better handling of gyro drift estimation, especially on
    startup
  - much faster recovery from attitude estimation errors

After extensive testing of the new EKF code we decided to make it the
default for this release. You can still use the old EKF if you want to
by setting AHRS_EKF_TYPE to 1, although it is recommended that the new
EKF be used for all vehicles.

In order to use the EKF we need to be a bit more careful about the
setup of the vehicle. That is why we enabled arming and pre-arm checks
by default. Please don't disable the arming checks, they are there for
very good reasons.

UAVCAN new protocol
-------------------
The uavcan change to the new protocol has been a long time coming, and
we'd like to thank Holger for both his great work on this and his
patience given how long it has taken to be in a release. This adds
support for automatic canbus node assignment which makes setup much
easier, and also supports the latest versions of the Zubax canbus GPS.

Support for 4 new Boards
------------------------
The porting of ArduPilot to more boards continues, with support
for 3 new boards in this release. They are:

 - the BHAT board
 - the PXFmini
 - the QualComm Flight
 - the Pixracer

More information about the list of supported boards is available here:
http://dev.ardupilot.org/wiki/supported-autopilot-controller-boards/

Startup on a moving platform
----------------------------
One of the benefits of the new EKF2 estimator is that it allows for
rapid estimation of gyro offset without doing a gyro calibration on
startup. This makes it possible to startup and arm on a moving
platform by setting the INS_GYR_CAL parameter to zero (to disable gyro
calibration on boot). This should be a big help for boats.

Improved Camera Trigger Logging
-------------------------------
This release adds new CAM_FEEDBACK_PIN and CAM_FEEDBACK_POL
parameters. These add support for separate CAM and TRIG log messages,
where TRIG is logged when the camera is triggered and the CAM message
is logged when an external pin indicates the camera has actually
fired. This pin is typically based on the flash hotshoe of a camera
and provides a way to log the exact time of camera triggering more
accurately. Many thanks to Dario Andres and Jaime Machuca for their
work on this feature.

PID Tuning
----------
You can now see the individual contributions of the P, I and D
components for the Steering PID in the logs (PIDY), allowing you to
get a much better picture of the performance.

Vibration Logging
-----------------
This release includes a lot more options for diagnosing vibration
issues. You will notice new VIBRATION messages in MAVLink and VIBE
messages in the dataflash logs. Those give you a good idea of your
(unfiltered) vibration levels. For really detailed analysis you can
setup your LOG_BITMASK to include raw logging, which gives you every
accel and gyro sample on your Pixhawk. You can then do a FFT on the
result and plot the distribution of vibration level with
frequency. That is great for finding the cause of vibration
issues. Note that you need a very fast microSD card for that to work!

More Sensors
------------
This release includes support for a bunch more sensors. It now supports
3 different interfaces for the LightWare range of Lidars (serial, I2C
and analog), and also supports the very nice Septentrio RTK
dual-frequency GPS (the first dual-frequency GPS we have support
for). It also supports the new "blue label" Lidar from Pulsed Light
(both on I2C and PWM).

For the uBlox GPS, we now have a lot more configurability of the
driver, with the ability to set the GNSS mode for different
constellations. Also in the uBlox driver we support logging of the raw
carrier phase and pseudo range data, which allows for post RTK
analysis with raw-capable receivers for really accurate photo
missions.

Better Linux support
--------------------
This release includes a lot of improvements to the Linux based
autopilot boards, including the NavIO+, the PXF and ERLE boards and
the BBBMini and the new RasPilot board.

On-board compass calibrator
---------------------------
We also have a new on-board compass calibrator, which also adds calibration
for soft iron effects, allowing for much more accurate compass
calibration.

Lots of other changes!
----------------------
The above list is just a taste of the changes that have gone into this
release. Thousands of small changes have gone into this release with
dozens of people contributing. Many thanks to everyone who helped!

Other key changes
-----------------
- fixed the MAV_CMD_DO_SET_HOME (thanks salonijain12)
- fixed bug when reverse throttle would increase speed in AUTO
- fixed a bug going into guided and rover still moving
- loitering at a waypoint if Param1 is non-zero
- update uavcan to new protocol
- fixed reporting of armed state with safety switch
- added optional arming check for minimum voltage
- improved text message queueing to ground stations
- re-organisation of HAL_Linux bus API
- improved NMEA parsing in GPS driver
- improved autoconfig of uBlox GPS driver
- support a wider range of Lightware serial Lidars
- improved non-GPS performance of EKF2
- improved compass fusion in EKF2
- improved support for Pixracer board
- improved NavIO2 support
- added BATT_WATT_MAX parameter
- enable messages for MAVLink gimbal support
- use 64 bit timestamps in dataflash logs
- added realtime PID tuning messages and PID logging
- fixed a failure case for the px4 failsafe mixer
- added DSM binding support on Pixhawk
- added vibration level logging
- ignore low voltage failsafe while disarmed
- added delta velocity and delta angle logging
- allow steering disarm based on ARMING_RUDDER parameter
- prevent mode switch changes changing WP tracking
- fixed parameter documentation spelling errors
- send MISSION_ITEM_REACHED messages on waypoint completion
- enable EKF by default on rover
- Improve gyro bias learning rate for plane and rover
- Allow switching primary GPS instance with 1 sat difference
- added NSH over MAVLink support
- added support for mpu9250 on pixhawk and pixhawk2
- Add support for logging ublox RXM-RAWX messages
- lots of updates to improve support for Linux based boards
- added ORGN message in dataflash
- added support for new "blue label" Lidar
- switched to real hdop in uBlox driver
- improved auto-config of uBlox
- raise accel discrepancy arming threshold to 0.75
- improved support for tcp and udp connections on Linux
- switched to delta-velocity and delta-angles in DCM
- improved detection of which accel to use in EKF
- improved auto-detections of flow control on pixhawk UARTs
- added HDOP to uavcan GPS driver
- improved sending of autopilot version
- log zero rangefinder distance when unhealthy
- added PRU firmware files for BeagleBoneBlack port
- fix for recent STORM32 gimbal support
- changed sending of STATUSTEXT severity to use correct values
- added new RSSI library with PWM input support
- fixed MAVLink heading report for UAVCAN GPS
- support LightWare I2C rangefinder on Linux
- improved staging of parameters and formats on startup to dataflash
- added new on-board compass calibrator
- improved RCOutput code for NavIO port
- added support for Septentrio GPS receiver
- support DO_MOUNT_CONTROl via command-long interface
- added CAM_RELAY_ON parameter
- moved SKIP_GYRO_CAL functionality to INS_GYR_CAL
- new waf build system
- new async accel calibrator
- better rangefinder power control
- dataflash over mavlink support
- settable main loop rate
- hideable parameters
- improved logging for dual-GPS setups
- improvements to multiple RTK GPS drivers
- numerous HAL_Linux improvements
- improved logging of CAM messages
- added support for IMU heaters in HAL_Linux
- support for RCInput over UDP in HAL_Linux
- improved EKF startup checks for GPS accuracy
- added raw IMU logging for all platforms
- configurable RGB LED brightness
- improvements to the lsm303d driver for Linux


Release 2.50, 19 June 2015
==========================

The ardupilot development team has released version 2.50 of
APM:Rover.  This release is mostly a backend improvement to ArduPilot
but a few new features and bug fixes are included.

Re-do Accelerometer Calibration
-------------------------------
Due to a change in the maximum accelerometer range on the Pixhawk all
users must re-do their accelerometer calibration for this release.


Only 3D accel calibration
-------------------------
The old "1D" accelerometer calibration method has now been removed, so
you must use the 3D accelerometer calibration method. The old method
was removed because a significant number of users had poor experiences.


Changes in this release are:

        - CLI_ENABLED parameter added so the CLI can now be accessed
          in Rover
        - PID logging for the steering controller.  It its now
          possible to graph what the P, I and D are doing as your
          driving the rover around to enable much better tuning of the
          vehicle.
        - Transition from .pde file to .cpp files for improved
          development.
        - GIT Submodules created for PX4Firmware, PX4Nuttx and uavcan
          git repositories for improved development.
        - Followme mode now works for Rover
        - GUIDED mode significantly improved.  If you have a GCS which is in
          Followme mode if the user then changes mode with the RC transmitter to
          HOLD or anything else then the Rover will STOP listening to the
          Followme updated guided mode waypoints.
        - When going into GUIDED mode the rover went into RTL - this
          is fixed.
        - Added EKF_STATUS_REPORT MAVLink message
        - 64-bit timestamps in dataflash logs
        - Numerous EKF improvements
        - Added support for 4th Mavlink channel
        - Added support for raw IMU logging
        - updated Piksi RTK GPS driver
        - improved support for GPS data injection (for Piksi RTK GPS)
        - The SITL software in the loop simulation system has been completely
          rewritten for this release. A major change is to make it possible to
          run SITL on native windows without needing a Linux virtual
          machine. (thanks Tridge)



Release 2.49, March 4th 2015
----------------------------

The ardupilot development team has released version 2.49 of
APM:Rover.  This release is a bug fix release with two important bugs
found by Marco Walther - Thanks Marco!

The bug fixes in this release are:

    - fixed a sonar problem where objects to the left wouldn't be
      identified - thanks Marco Walther!
    - Fixed the ordering of the AP_Notify call so the main indicator
      light would be correct on startup - thanks Marco Walther!



Release 2.48, February 20th 2015
--------------------------------

The ardupilot development team has released version 2.48 of
APM:Rover.  This release is a bug fix release with some important bugs
found by the users of ardupilot.

The changes in this release are:

  - fixed a bug that could cause short term loss of RC control with
    some receiver systems and configurations
  - allowed for shorter sync pulse widths for PPM-SUM receivers on
    APM1 and APM2
  - fix an issue where battery reporting could be intermittent (thanks
    Georgii Staroselskii!)
  - fixed a mission handling bug that could cause a crash if jump
    commands form an infinite loop (thanks to Dellarb for reporting
    this bug)
  - improved support for in-kernel SPI handling on Linux (thanks to John Williams)
  - support UAVCAN based ESCs and GPS modules on Pixhawk (thanks to
    Pavel, Holger and and PX4 dev team)
  - Multiple updates for the NavIO+ cape on RaspberryPi (thanks to
    Emlid)
  - Lots of EKF changes
  - added support for MAVLink packet routing
  - added detection and recovery from faulty gyro and accel sensors
  - added support for BBBMini Linux port
  - increased number of AVR input channels from 8 to 11
  - auto-set system clock based on GPS in Linux ports
  - added SBUS FrSky telemetry support (thanks to Mathias)
  - Added AK8963 MAG support (thanks Staroselskii Georgii)
  - Added support for second battery
  - Auto formatting of SDCard if it cannot be accessed on startup
  - A number of significant performance improvements for the PX4 platform

The most important bug fix is the one for short term loss of RC
control. This is a very long standing bug which didn't have a
noticeable impact for most people, but could cause loss of RC control
for around 1 or 2 seconds for some people in certain circumstances.

The bug was in the the AP_HAL RCInput API. Each HAL backend has a flag
that says whether there is a new RC input frame available. That flag
was cleared by the read() method (typically hal.rcin->read()). Callers
would check for new input by checking the boolean
hal.rcin->new_input() function.

The problem was that read() was called from multiple places. Normally
this is fine as reads from other than the main radio input loop happen
before the other reads, but if the timing of the new radio frame
exactly matched the loop frequency then a read from another place
could clear the new_input flag and we would not see the new RC input
frame. If that happened enough times we would go into a short term RC
failsafe and ignore RC inputs, even in manual mode.

The fix was very simple - it is the new_input() function itself that
should clear the flag, not read(). 

Many thanks to MarkM for helping us track down this bug by providing
us with sufficient detail on how to reproduce it. In Marks case his
OpenLRSng configuration happened to produce exactly the worst case
timing needed to reproduce the issue. Once Tridge copied his OpenLRS
settings to his TX/RX he was able to reproduce the problem and it was
easy to find and fix.

A number of users have reported occasional glitches in manual control
where servos pause for short periods in past releases. It is likely
that some of those issues were caused by this bug. The dev team would
like to apologise for taking so long to track down this bug!

The other main change was also related to RC input. Some receivers use
a PPM-SUM sync pulse width shorter than what the APM1/APM2 code was
setup to handle. The OpenLRSng default sync pulse width is 3000
microseconds, but the APM1/APM2 code was written for a mininum sync
pulse width of 4000 microseconds. For this release we have changed the
APM1/APM2 driver to accept a sync pulse width down to 2700
microseconds.

Auto format of SD Card
======================
From time to time the SD cards in the PX4 autopilots get corrupted.
This isn't a surprise considering what we do to them.  Your all
familiar with the windows "please unmount or eject your SDCard before
removing" process.  Well we don't do that.  In fact normal operation
is to just pull the power on the SDCard - whilst its being written
too!!  Not to metion the horrible vibration rich environment the
SDCard exists in.  If the autopilot is setup in the internal innards
of your plane/copter/rover this can be a nightmare to get to.  To
resolve that problem Tridge has added code at startup so when
ArduPilot tries to mount to SDCard to access it - if that fails it
will then try to format the SDCard and if successful mount the card
and proceed.  If the format fails then you will get the usual SOS
Audio that makes most of us want to find the buzzer and rip its heart
out.

I mention this in case anyone has precious logs saved on the SDCard or
they are using the SDCard out of their phone with their wedding
photo's on it.  Probably best not to do that and assume any data on
the SDCard can be deleted.

We are also looking to add a parameter to control whether the card is
auto formatted on startup or not but it isn't in there yet.



Release 2.47, November 15th 2014
--------------------------------

The ardupilot development team is proud to announce the release of 
version 2.47 of APM:Rover. This is a minor bug fix release.  The most 
important change in this release is the fixing of the skid steering 
support but there have been a number of fixes in other areas as well.

Full changes list for this release:

  - add support for controlling safety switch on Pixhawk from ground station

  - prevent reports of failed AHRS during initialisation

  - fixed skid steering that was broken in the last release

  - report gyro unhealthy if gyro calibration failed

  - fixed dual sonar support in CLI sonar test

  - fixed Nuttx crash on Pixhawk with bad I2C cables

  - added GPS_SBAS_MODE parameter - turns on/off satellite based augemtation system for GPS

  - added GPS_MIN_ELEV parameter - specifiy the elevation mask for GPS satellites

  - added RELAY_DEFAULT parameter to control default of relay on startup

  - fixed bug in FRAM storage on Pixhawk that could cause parameters changes not to be saved

  - better handling of compass errors in the EKF (Extended Kalman Filter)

  - improved support for linux based autopilots

  - added support for PulsedLight LIDAR as a range finder

Many thanks to everyone who contributed to this release, especially
Tom Coyle and Linus Penzlien for their excellent testing and feedback.

Happy driving!



Release 2.46, August 26th 2014
------------------------------

The ardupilot development team is proud to announce the release of
version 2.46 of APM:Rover. This is a major release with a lot of new
features and bug fixes.

This release is based on a lot of development and testing that
happened prior to the AVC competition where APM based vehicles
performed very well.

Full changes list for this release:

  - added support for higher baudrates on telemetry ports, to make it
    easier to use high rate telemetry to companion boards. Rates of up
    to 1.5MBit are now supported to companion boards.

  - new Rangefinder code with support for a wider range of rangefinder
    types including a range of Lidars (thanks to Allyson Kreft)

  - added logging of power status on Pixhawk

  - added PIVOT_TURN_ANGLE parameter for pivot based turns on skid
    steering rovers

  - lots of improvements to the EKF support for Rover, thanks to Paul
    Riseborough and testing from Tom Coyle. Using the EKF can greatly
    improve navigation accuracy for fast rovers. Enable with
    AHRS_EKF_USE=1. 

  - improved support for dual GPS on Pixhawk. Using a 2nd GPS can
    greatly improve performance when in an area with an obstructed
    view of the sky

  - support for up to 14 RC channels on Pihxawk

  - added BRAKING_PERCENT and BRAKING_SPEEDERR parameters for better
    breaking support when cornering

  - added support for FrSky telemetry via SERIAL2_PROTOCOL parameter
    (thanks to Matthias Badaire)

  - added support for Linux based autopilots, initially with the PXF
    BeagleBoneBlack cape and the Erle robotics board. Support for more
    boards is expected in future releases. Thanks to Victor, Sid and
    Anuj for their great work on the Linux port. 

  - added StorageManager library, which expands available FRAM storage
    on Pixhawk to 16 kByte. This allows for 724 waypoints, 50 rally
    points and 84 fence points on Pixhawk.

  - improved reporting of magnetometer and barometer errors to the GCS

  - fixed a bug in automatic flow control detection for serial ports
    in Pixhawk

  - fixed use of FMU servo pins as digital inputs on Pixhawk

  - imported latest updates for VRBrain boards (thanks to Emile
    Castelnuovo and Luca Micheletti)

  - updates to the Piksi GPS support (thanks to Niels Joubert)

  - improved gyro estimate in DCM (thanks to Jon Challinger)

  - improved position projection in DCM in wind (thanks to Przemek
    Lekston)

  - several updates to AP_NavEKF for more robust handling of errors
    (thanks to Paul Riseborough)

  - lots of small code cleanups thanks to Daniel Frenzel

  - initial support for NavIO board from Mikhail Avkhimenia

  - fixed logging of RCOU for up to 12 channels (thanks to Emile
    Castelnuovo)

  - code cleanups from Silvia Nunezrivero

  - improved parameter download speed on radio links with no flow
    control

Many thanks to everyone who contributed to this release, especially
Tom Coyle and Linus Penzlien for their excellent testing and feedback.

Happy driving!
  # Path to the CMake build directory.
  build: #include "Rover.h"

#include <AP_RangeFinder/RangeFinder_Backend.h>

// initialise compass
void Rover::init_compass()
{
    if (!g.compass_enabled) {
        return;
    }

    if (!compass.init()|| !compass.read()) {
        hal.console->printf("Compass initialisation failed!\n");
        g.compass_enabled = false;
    } else {
        ahrs.set_compass(&compass);
    }
}

/*
  if the compass is enabled then try to accumulate a reading
  also update initial location used for declination
 */
void Rover::compass_accumulate(void)
{
    if (!g.compass_enabled) {
        return;
    }

    compass.accumulate();

    // update initial location used for declination
    if (!compass_init_location) {
        Location loc;
        if (ahrs.get_position(loc)) {
            compass.set_initial_location(loc.lat, loc.lng);
            compass_init_location = true;
        }
    }
}

void Rover::init_rangefinder(void)
{
    rangefinder.init();
}

// init beacons used for non-gps position estimates
void Rover::init_beacon()
{
    g2.beacon.init();
}

// init visual odometry sensor
void Rover::init_visual_odom()
{
    g2.visual_odom.init();
}

// update visual odometry sensor
void Rover::update_visual_odom()
{
    // check for updates
    if (g2.visual_odom.enabled() && (g2.visual_odom.get_last_update_ms() != visual_odom_last_update_ms)) {
        visual_odom_last_update_ms = g2.visual_odom.get_last_update_ms();
        const float time_delta_sec = g2.visual_odom.get_time_delta_usec() / 1000000.0f;
        ahrs.writeBodyFrameOdom(g2.visual_odom.get_confidence(),
                                g2.visual_odom.get_position_delta(),
                                g2.visual_odom.get_angle_delta(),
                                time_delta_sec,
                                visual_odom_last_update_ms,
                                g2.visual_odom.get_pos_offset());
        // log sensor data
        DataFlash.Log_Write_VisualOdom(time_delta_sec,
                                       g2.visual_odom.get_angle_delta(),
                                       g2.visual_odom.get_position_delta(),
                                       g2.visual_odom.get_confidence());
    }
}

// update wheel encoders
void Rover::update_wheel_encoder()
{
    // exit immediately if not enabled
    if (g2.wheel_encoder.num_sensors() == 0) {
        return;
    }

    // update encoders
    g2.wheel_encoder.update();

    // initialise on first iteration
    const uint32_t now = AP_HAL::millis();
    if (wheel_encoder_last_ekf_update_ms == 0) {
        wheel_encoder_last_ekf_update_ms = now;
        for (uint8_t i = 0; i < g2.wheel_encoder.num_sensors(); i++) {
            wheel_encoder_last_angle_rad[i] = g2.wheel_encoder.get_delta_angle(i);
            wheel_encoder_last_update_ms[i] = g2.wheel_encoder.get_last_reading_ms(i);
        }
        return;
    }

    // calculate delta angle and delta time and send to EKF
    // use time of last ping from wheel encoder
    // send delta time (time between this wheel encoder time and previous wheel encoder time)
    // in case where wheel hasn't moved, count = 0 (cap the delta time at 50ms - or system time)
    //     use system clock of last update instead of time of last ping
    const float system_dt = (now - wheel_encoder_last_ekf_update_ms) / 1000.0f;
    for (uint8_t i = 0; i < g2.wheel_encoder.num_sensors(); i++) {
        // calculate angular change (in radians)
        const float curr_angle_rad = g2.wheel_encoder.get_delta_angle(i);
        const float delta_angle = curr_angle_rad - wheel_encoder_last_angle_rad[i];
        wheel_encoder_last_angle_rad[i] = curr_angle_rad;

        // calculate delta time
        float delta_time;
        const uint32_t latest_sensor_update_ms = g2.wheel_encoder.get_last_reading_ms(i);
        const uint32_t sensor_diff_ms = latest_sensor_update_ms - wheel_encoder_last_update_ms[i];

        // if we have not received any sensor updates, or time difference is too high then use time since last update to the ekf
        // check for old or insane sensor update times
        if (sensor_diff_ms == 0 || sensor_diff_ms > 100) {
            delta_time = system_dt;
            wheel_encoder_last_update_ms[i] = wheel_encoder_last_ekf_update_ms;
        } else {
            delta_time = sensor_diff_ms / 1000.0f;
            wheel_encoder_last_update_ms[i] = latest_sensor_update_ms;
        }

        /* delAng is the measured change in angular position from the previous measurement where a positive rotation is produced by forward motion of the vehicle (rad)
         * delTime is the time interval for the measurement of delAng (sec)
         * timeStamp_ms is the time when the rotation was last measured (msec)
         * posOffset is the XYZ body frame position of the wheel hub (m)
         */
        EKF3.writeWheelOdom(delta_angle, delta_time, wheel_encoder_last_update_ms[i], g2.wheel_encoder.get_position(i), g2.wheel_encoder.get_wheel_radius(i));

        // calculate rpm for reporting to GCS
        if (is_positive(delta_time)) {
            wheel_encoder_rpm[i] = (delta_angle / M_2PI) / (delta_time / 60.0f);
        } else {
            wheel_encoder_rpm[i] = 0.0f;
        }
    }

    // record system time update for next iteration
    wheel_encoder_last_ekf_update_ms = now;
}

// read the receiver RSSI as an 8 bit number for MAVLink
// RC_CHANNELS_SCALED message
void Rover::read_receiver_rssi(void)
{
    receiver_rssi = rssi.read_receiver_rssi_uint8();
}

// Calibrate compass
void Rover::compass_cal_update() {
    if (!hal.util->get_soft_armed()) {
        compass.compass_cal_update();
    }
}

// Accel calibration

void Rover::accel_cal_update() {
    if (hal.util->get_soft_armed()) {
        return;
    }
    ins.acal_update();
    // check if new trim values, and set them    float trim_roll, trim_pitch;
    float trim_roll, trim_pitch;
    if (ins.get_new_trim(trim_roll, trim_pitch)) {
        ahrs.set_trim(Vector3f(trim_roll, trim_pitch, 0));
    }
}

// read the rangefinders
void Rover::read_rangefinders(void)
{
    rangefinder.update();

    AP_RangeFinder_Backend *s0 = rangefinder.get_backend(0);
    AP_RangeFinder_Backend *s1 = rangefinder.get_backend(1);

    if (s0 == nullptr || s0->status() == RangeFinder::RangeFinder_NotConnected) {
        // this makes it possible to disable rangefinder at runtime
        return;
    }

    if (s1 != nullptr && s1->has_data()) {
        // we have two rangefinders
        obstacle.rangefinder1_distance_cm = s0->distance_cm();
        obstacle.rangefinder2_distance_cm = s1->distance_cm();
        if (obstacle.rangefinder1_distance_cm < static_cast<uint16_t>(g.rangefinder_trigger_cm) &&
            obstacle.rangefinder1_distance_cm < static_cast<uint16_t>(obstacle.rangefinder2_distance_cm))  {
            // we have an object on the left
            if (obstacle.detected_count < 127) {
                obstacle.detected_count++;
            }
            if (obstacle.detected_count == g.rangefinder_debounce) {
                gcs().send_text(MAV_SEVERITY_INFO, "Rangefinder1 obstacle %u cm",
                        static_cast<uint32_t>(obstacle.rangefinder1_distance_cm));
            }
            obstacle.detected_time_ms = AP_HAL::millis();
            obstacle.turn_angle = g.rangefinder_turn_angle;
        } else if (obstacle.rangefinder2_distance_cm < static_cast<uint16_t>(g.rangefinder_trigger_cm)) {
            // we have an object on the right
            if (obstacle.detected_count < 127) {
                obstacle.detected_count++;
            }
            if (obstacle.detected_count == g.rangefinder_debounce) {
                gcs().send_text(MAV_SEVERITY_INFO, "Rangefinder2 obstacle %u cm",
                        static_cast<uint32_t>(obstacle.rangefinder2_distance_cm));
            }
            obstacle.detected_time_ms = AP_HAL::millis();
            obstacle.turn_angle = -g.rangefinder_turn_angle;
        }
    } else {
        // we have a single rangefinder
        obstacle.rangefinder1_distance_cm = s0->distance_cm();
        obstacle.rangefinder2_distance_cm = 0;
        if (obstacle.rangefinder1_distance_cm < static_cast<uint16_t>(g.rangefinder_trigger_cm))  {
            // obstacle detected in front
            if (obstacle.detected_count < 127) {
                obstacle.detected_count++;
            }
            if (obstacle.detected_count == g.rangefinder_debounce) {
                gcs().send_text(MAV_SEVERITY_INFO, "Rangefinder obstacle %u cm",
                        static_cast<uint32_t>(obstacle.rangefinder1_distance_cm));
            }
            obstacle.detected_time_ms = AP_HAL::millis();
            obstacle.turn_angle = g.rangefinder_turn_angle;
        }
    }

    Log_Write_Rangefinder();

    // no object detected - reset after the turn time
    if (obstacle.detected_count >= g.rangefinder_debounce &&
        AP_HAL::millis() > obstacle.detected_time_ms + g.rangefinder_turn_time*1000) {
        gcs().send_text(MAV_SEVERITY_INFO, "Obstacle passed");
        obstacle.detected_count = 0;
        obstacle.turn_angle = 0;
    }
}

// initialise proximity sensor
void Rover::init_proximity(void)
{
    g2.proximity.init();
    g2.proximity.set_rangefinder(&rangefinder);
}

// update error mask of sensors and subsystems. The mask
// uses the MAV_SYS_STATUS_* values from mavlink. If a bit is set
// then it indicates that the sensor or subsystem is present but
// not functioning correctly.
void Rover::update_sensor_status_flags(void)
{
    // default sensors present
    control_sensors_present = MAVLINK_SENSOR_PRESENT_DEFAULT;

    // first what sensors/controllers we have
    if (g.compass_enabled) {
        control_sensors_present |= MAV_SYS_STATUS_SENSOR_3D_MAG;  // compass present
    }
    if (gps.status() > AP_GPS::NO_GPS) {
        control_sensors_present |= MAV_SYS_STATUS_SENSOR_GPS;
    }
    if (g2.visual_odom.enabled()) {
        control_sensors_present |= MAV_SYS_STATUS_SENSOR_VISION_POSITION;
    }
    if (rover.DataFlash.logging_present()) {  // primary logging only (usually File)
        control_sensors_present |= MAV_SYS_STATUS_LOGGING;
    }
    if (rover.g2.proximity.get_status() > AP_Proximity::Proximity_NotConnected) {
        control_sensors_present |= MAV_SYS_STATUS_SENSOR_LASER_POSITION;
    }

    // all present sensors enabled by default except rate control, attitude stabilization, yaw, altitude, position control and motor output which we will set individually
    control_sensors_enabled = control_sensors_present & (~MAV_SYS_STATUS_SENSOR_ANGULAR_RATE_CONTROL &
                                                         ~MAV_SYS_STATUS_SENSOR_ATTITUDE_STABILIZATION &
                                                         ~MAV_SYS_STATUS_SENSOR_YAW_POSITION &
                                                         ~MAV_SYS_STATUS_SENSOR_XY_POSITION_CONTROL &
                                                         ~MAV_SYS_STATUS_SENSOR_MOTOR_OUTPUTS &
                                                         ~MAV_SYS_STATUS_LOGGING &
                                                         ~MAV_SYS_STATUS_SENSOR_BATTERY);
    if (control_mode->attitude_stabilized()) {
        control_sensors_enabled |= MAV_SYS_STATUS_SENSOR_ANGULAR_RATE_CONTROL; // 3D angular rate control
        control_sensors_enabled |= MAV_SYS_STATUS_SENSOR_ATTITUDE_STABILIZATION; // 3D angular rate control
    }
    if (control_mode->is_autopilot_mode()) {
        control_sensors_enabled |= MAV_SYS_STATUS_SENSOR_YAW_POSITION; // yaw position
        control_sensors_enabled |= MAV_SYS_STATUS_SENSOR_XY_POSITION_CONTROL; // X/Y position control
    }

    if (rover.DataFlash.logging_enabled()) {
        control_sensors_enabled |= MAV_SYS_STATUS_LOGGING;
    }

    // set motors outputs as enabled if safety switch is not disarmed (i.e. either NONE or ARMED)
    if (hal.util->safety_switch_state() != AP_HAL::Util::SAFETY_DISARMED) {
        control_sensors_enabled |= MAV_SYS_STATUS_SENSOR_MOTOR_OUTPUTS;
    }

    if (battery.num_instances() > 0) {
        control_sensors_enabled |= MAV_SYS_STATUS_SENSOR_BATTERY;
    }

    // default to all healthy except compass and gps which we set individually
    control_sensors_health = control_sensors_present & (~MAV_SYS_STATUS_SENSOR_3D_MAG & ~MAV_SYS_STATUS_SENSOR_GPS);
    if (g.compass_enabled && compass.healthy(0) && ahrs.use_compass()) {
        control_sensors_health |= MAV_SYS_STATUS_SENSOR_3D_MAG;
    }
    if (gps.is_healthy()) {
        control_sensors_health |= MAV_SYS_STATUS_SENSOR_GPS;
    }
    if (g2.visual_odom.enabled() && !g2.visual_odom.healthy()) {
        control_sensors_health &= ~MAV_SYS_STATUS_SENSOR_VISION_POSITION;
    }
    if (!ins.get_gyro_health_all() || !ins.gyro_calibrated_ok_all()) {
        control_sensors_health &= ~MAV_SYS_STATUS_SENSOR_3D_GYRO;
    }
    if (!ins.get_accel_health_all()) {
        control_sensors_health &= ~MAV_SYS_STATUS_SENSOR_3D_ACCEL;
    }

    if (ahrs.initialised() && !ahrs.healthy()) {
        // AHRS subsystem is unhealthy
        control_sensors_health &= ~MAV_SYS_STATUS_AHRS;
    }

    if (rangefinder.num_sensors() > 0) {
        control_sensors_present |= MAV_SYS_STATUS_SENSOR_LASER_POSITION;
        if (g.rangefinder_trigger_cm > 0) {
            control_sensors_enabled |= MAV_SYS_STATUS_SENSOR_LASER_POSITION;
        }
        AP_RangeFinder_Backend *s = rangefinder.get_backend(0);
        if (s != nullptr && s->has_data()) {
            control_sensors_health |= MAV_SYS_STATUS_SENSOR_LASER_POSITION;
        }
    }
    if (rover.g2.proximity.get_status() < AP_Proximity::Proximity_Good) {
        control_sensors_health &= ~MAV_SYS_STATUS_SENSOR_LASER_POSITION;
    }
    if (rover.DataFlash.logging_failed()) {
        control_sensors_health &= ~MAV_SYS_STATUS_LOGGING;
    }

    if (!battery.healthy() || battery.has_failsafed()) {
        control_sensors_enabled &= ~MAV_SYS_STATUS_SENSOR_BATTERY;
    }

    if (!initialised || ins.calibrating()) {
        // while initialising the gyros and accels are not enabled
        control_sensors_enabled &= ~(MAV_SYS_STATUS_SENSOR_3D_GYRO | MAV_SYS_STATUS_SENSOR_3D_ACCEL);
        control_sensors_health &= ~(MAV_SYS_STATUS_SENSOR_3D_GYRO | MAV_SYS_STATUS_SENSOR_3D_ACCEL);
    }
#if FRSKY_TELEM_ENABLED == ENABLED
    // give mask of error flags to Frsky_Telemetry
    frsky_telemetry.update_sensor_status_flags(~control_sensors_health & control_sensors_enabled & control_sensors_present);
#endif
}

permissions: /*****************************************************************************
The init_ardupilot function processes everything we need for an in - air restart
    We will determine later if we are actually on the ground and process a
    ground start in that case.

*****************************************************************************/

#include "Rover.h"

static void mavlink_delay_cb_static()
{
    rover.mavlink_delay_cb();
}

static void failsafe_check_static()
{
    rover.failsafe_check();
}

void Rover::init_ardupilot()
{
    // initialise console serial port
    serial_manager.init_console();

    hal.console->printf("\n\nInit %s"
                        "\n\nFree RAM: %u\n",
                        fwver.fw_string,
                        hal.util->available_memory());

    //
    // Check the EEPROM format version before loading any parameters from EEPROM.
    //

    load_parameters();
#if STATS_ENABLED == ENABLED
    // initialise stats module
    g2.stats.init();
#endif

    gcs().set_dataflash(&DataFlash);

    mavlink_system.sysid = g.sysid_this_mav;

    // initialise serial ports
    serial_manager.init();

    // setup first port early to allow BoardConfig to report errors
    gcs().chan(0).setup_uart(serial_manager, AP_SerialManager::SerialProtocol_MAVLink, 0);

    // Register mavlink_delay_cb, which will run anytime you have
    // more than 5ms remaining in your call to hal.scheduler->delay
    hal.scheduler->register_delay_callback(mavlink_delay_cb_static, 5);

    BoardConfig.init();
#if HAL_WITH_UAVCAN
    BoardConfig_CAN.init();
#endif

    // initialise notify system
    notify.init(false);
    AP_Notify::flags.failsafe_battery = false;
    notify_mode(control_mode);

    ServoRelayEvents.set_channel_mask(0xFFF0);

    battery.init();

    rssi.init();

    // init baro before we start the GCS, so that the CLI baro test works
    barometer.init();

    // we start by assuming USB connected, as we initialed the serial
    // port with SERIAL0_BAUD. check_usb_mux() fixes this if need be.
    usb_connected = true;
    check_usb_mux();

    // setup telem slots with serial ports
    gcs().setup_uarts(serial_manager);

    // setup frsky telemetry
#if FRSKY_TELEM_ENABLED == ENABLED
    frsky_telemetry.init(serial_manager, fwver.fw_string, (is_boat() ? MAV_TYPE_SURFACE_BOAT : MAV_TYPE_GROUND_ROVER));
#endif
#if DEVO_TELEM_ENABLED == ENABLED
    devo_telemetry.init(serial_manager);
#endif

#if LOGGING_ENABLED == ENABLED
    log_init();
#endif

    // initialise compass
    init_compass();

    // initialise rangefinder
    init_rangefinder();

    // init proximity sensor
    init_proximity();

    // init beacons used for non-gps position estimation
    init_beacon();

    // init visual odometry
    init_visual_odom();

    // and baro for EKF
    barometer.set_log_baro_bit(MASK_LOG_IMU);
    barometer.calibrate();

    // Do GPS init
    gps.set_log_gps_bit(MASK_LOG_GPS);
    gps.init(serial_manager);

    ins.set_log_raw_bit(MASK_LOG_IMU_RAW);

    set_control_channels();  // setup radio channels and ouputs ranges
    init_rc_in();            // sets up rc channels deadzone
    g2.motors.init();        // init motors including setting servo out channels ranges
    init_rc_out();           // enable output

    // init wheel encoders
    g2.wheel_encoder.init();

    relay.init();

#if MOUNT == ENABLED
    // initialise camera mount
    camera_mount.init(serial_manager);
#endif

    /*
      setup the 'main loop is dead' check. Note that this relies on
      the RC library being initialised.
     */
    hal.scheduler->register_timer_failsafe(failsafe_check_static, 1000);

    // give AHRS the range beacon sensor
    ahrs.set_beacon(&g2.beacon);

    // initialize SmartRTL
    g2.smart_rtl.init();

    init_capabilities();

    startup_ground();

    Mode *initial_mode = mode_from_mode_num((enum mode)g.initial_mode.get());
    if (initial_mode == nullptr) {
        initial_mode = &mode_initializing;
    }
    set_mode(*initial_mode, MODE_REASON_INITIALISED);


    // set the correct flight mode
    // ---------------------------
    reset_control_switch();
    init_aux_switch();

    // disable safety if requested
    BoardConfig.init_safety();

    // flag that initialisation has completed
    initialised = true;
}

//*********************************************************************************
// This function does all the calibrations, etc. that we need during a ground start
//*********************************************************************************
void Rover::startup_ground(void)
{
    set_mode(mode_initializing, MODE_REASON_INITIALISED);

    gcs().send_text(MAV_SEVERITY_INFO, "<startup_ground> Ground start");

    #if(GROUND_START_DELAY > 0)
        gcs().send_text(MAV_SEVERITY_NOTICE, "<startup_ground> With delay");
        delay(GROUND_START_DELAY * 1000);
    #endif

    // IMU ground start
    //------------------------
    //

    startup_INS_ground();

    // initialise mission library
    mission.init();

    // initialise DataFlash library
    DataFlash.set_mission(&mission);
    DataFlash.setVehicle_Startup_Log_Writer(
        FUNCTOR_BIND(&rover, &Rover::Log_Write_Vehicle_Startup_Messages, void)
        );

    // we don't want writes to the serial port to cause us to pause
    // so set serial ports non-blocking once we are ready to drive
    serial_manager.set_blocking_writes_all(false);

    gcs().send_text(MAV_SEVERITY_INFO, "Ready to drive");
}

/*
  set the in_reverse flag
  reset the throttle integrator if this changes in_reverse
 */
void Rover::set_reverse(bool reverse)
{
    if (in_reverse == reverse) {
        return;
    }
    in_reverse = reverse;
}

bool Rover::set_mode(Mode &new_mode, mode_reason_t reason)
{
    if (control_mode == &new_mode) {
        // don't switch modes if we are already in the correct mode.
        return true;
    }

    Mode &old_mode = *control_mode;
    if (!new_mode.enter()) {
        // Log error that we failed to enter desired flight mode
        Log_Write_Error(ERROR_SUBSYSTEM_FLIGHT_MODE, new_mode.mode_number());
        gcs().send_text(MAV_SEVERITY_WARNING, "Flight mode change failed");
        return false;
    }

    control_mode = &new_mode;

    // pilot requested flight mode change during a fence breach indicates pilot is attempting to manually recover
    // this flight mode change could be automatic (i.e. fence, battery, GPS or GCS failsafe)
    // but it should be harmless to disable the fence temporarily in these situations as well
    g2.fence.manual_recovery_start();

#if FRSKY_TELEM_ENABLED == ENABLED
    frsky_telemetry.update_control_mode(control_mode->mode_number());
#endif
#if DEVO_TELEM_ENABLED == ENABLED
    devo_telemetry.update_control_mode(control_mode->mode_number());
#endif

#if CAMERA == ENABLED
    camera.set_is_auto_mode(control_mode->mode_number() == AUTO);
#endif

    old_mode.exit();

    control_mode_reason = reason;
    DataFlash.Log_Write_Mode(control_mode->mode_number(), control_mode_reason);

    notify_mode(control_mode);
    return true;
}

void Rover::startup_INS_ground(void)
{
    gcs().send_text(MAV_SEVERITY_INFO, "Beginning INS calibration. Do not move vehicle");
    hal.scheduler->delay(100);

    ahrs.init();
    // say to EKF that rover only move by goind forward
    ahrs.set_fly_forward(true);
    ahrs.set_vehicle_class(AHRS_VEHICLE_GROUND);

    ins.init(scheduler.get_loop_rate_hz());
    ahrs.reset();
}

void Rover::check_usb_mux(void)
{
    bool usb_check = hal.gpio->usb_connected();
    if (usb_check == usb_connected) {
        return;
    }

    // the user has switched to/from the telemetry port
    usb_connected = usb_check;
}

// update notify with mode change
void Rover::notify_mode(const Mode *mode)
{
    notify.flags.flight_mode = mode->mode_number();
    notify.set_flight_mode_str(mode->name4());
}

/*
  check a digitial pin for high,low (1/0)
 */
uint8_t Rover::check_digital_pin(uint8_t pin)
{
    const int8_t dpin = hal.gpio->analogPinToDigitalPin(pin);
    if (dpin == -1) {
        return 0;
    }
    // ensure we are in input mode
    hal.gpio->pinMode(dpin, HAL_GPIO_INPUT);

    // enable pullup
    hal.gpio->write(dpin, 1);

    return hal.gpio->read(dpin);
}

/*
  should we log a message type now?
 */
bool Rover::should_log(uint32_t mask)
{
    return DataFlash.should_log(mask);
}

/*
  update AHRS soft arm state and log as needed
 */
void Rover::change_arm_state(void)
{
    Log_Arm_Disarm();
    update_soft_armed();
}

/*
  arm motors
 */
bool Rover::arm_motors(AP_Arming::ArmingMethod method)
{
    if (!arming.arm(method)) {
        AP_Notify::events.arming_failed = true;
        return false;
    }

    // Set the SmartRTL home location. If activated, SmartRTL will ultimately try to land at this point
    g2.smart_rtl.set_home(true);

    change_arm_state();
    return true;
}

/*
  disarm motors
 */
bool Rover::disarm_motors(void)
{
    if (!arming.disarm()) {
        return false;
    }
    if (control_mode != &mode_auto) {
        // reset the mission on disarm if we are not in auto
        mission.reset();
    }

    // only log if disarming was successful
    change_arm_state();

    return true;
}

// returns true if vehicle is a boat
// this affects whether the vehicle tries to maintain position after reaching waypoints
bool Rover::is_boat() const
{
    return ((enum frame_class)g2.frame_class.get() == FRAME_BOAT);
}
  contents: /*
 * This file is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This file is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Rover.h"

#define FORCE_VERSION_H_INCLUDE
#include "version.h"
#undef FORCE_VERSION_H_INCLUDE

#include <AP_Common/AP_FWVersion.h>

const AP_FWVersion Rover::fwver{
    .major = FW_MAJOR,
    .minor = FW_MINOR,
    .patch = FW_PATCH,
    .fw_type = FW_TYPE,
#ifndef GIT_VERSION
    .fw_string = THISFIRMWARE,
#else
    .fw_string = THISFIRMWARE " (" GIT_VERSION ")",
    .fw_hash_str = GIT_VERSION,
#endif
#ifdef PX4_GIT_VERSION
    .middleware_hash_str = PX4_GIT_VERSION,
#endif
#ifdef NUTTX_GIT_VERSION
    .os_hash_str = NUTTX_GIT_VERSION,
#endif
};

jobs: #pragma once

#ifndef FORCE_VERSION_H_INCLUDE
#error version.h should never be included directly. You probably want to include AP_Common/AP_FWVersion.h
#endif

#include "ap_version.h"

#define THISFIRMWARE "ArduRover V3.3.0-dev"

// the following line is parsed by the autotest scripts
#define FIRMWARE_VERSION 3,3,0,FIRMWARE_VERSION_TYPE_DEV

#define FW_MAJOR 3
#define FW_MINOR 3
#define FW_PATCH 0
#define FW_TYPE FIRMWARE_VERSION_TYPE_DEV
  analyze: #!/usr/bin/env python
# encoding: utf-8

def build(bld):
    vehicle = bld.path.name
    bld.ap_stlib(
        name=vehicle + '_libs',
        ap_vehicle=vehicle,
        ap_libraries=bld.ap_common_vehicle_libraries() + [
            'APM_Control',
            'AP_Arming',
            'AP_Camera',
            'AP_L1_Control',
            'AP_Mount',
            'AP_Navigation',
            'AP_RCMapper',
            'AP_RSSI',
            'AC_PID',
            'AP_Stats',
            'AP_Beacon',
            'AP_AdvancedFailsafe',
            'AP_WheelEncoder',
            'AP_SmartRTL',
            'AC_Fence',
            'AP_Proximity',
            'AC_Avoidance',
            'AC_AttitudeControl',
            'AP_Devo_Telem',
        ],
    )

    bld.ap_program(
        program_name='ardurover',
        program_groups=['bin', 'rover'],
        use=vehicle + '_libs',
    )
    permissions: migrator_ts: 1695775149
__migrator:
  kind: version
  migration_number: 1
  bump_number: 1
  commit_message: "Rebuild for libboost 1.82"
  # limit the number of prs for ramp-up
  pr_limit: 10
libboost_devel:
  - 1.82
# This migration is matched with a piggy-back migrator
# (see https://github.com/regro/cf-scripts/pull/1668)
# that will replace boost-cpp with libboost-devel
boost_cpp:
  - 1.82
# same for boost -> libboost-python-devel
libboost_python_devel:
  - 1.82
boost:
  - 1.82
      contents: __migrator:
  build_number: 1
  kind: version
  migration_number: 1
fmt:
- '10'
migrator_ts: 1683802784.4940007
      security-events: __migrator:
  build_number: 1
  kind: version
  migration_number: 1
libthrift:
- 0.19.0
migrator_ts: 1693762377.7427814
      actions:  migrator_ts: 1698047052
__migrator:
  kind: version
  migration_number: 1
  bump_number: 1

zeromq:
  - '4.3.5'
    name: This file is automatically generated by conda-smithy. If any
particular build configuration is expected, but it is not found,
please make sure all dependencies are satisfiable. To add/modify any
matrix elements, you should create/change conda-smithy's input
recipe/conda_build_config.yaml and re-render the recipe, rather than
editing these files directly.
    runs-on: alsa_lib:
- '1.2'
c_compiler:
- gcc
c_compiler_version:
- '12'
cdt_name:
- cos7
channel_sources:
- conda-forge
channel_targets:
- gnuradio main
cxx_compiler:
- gxx
cxx_compiler_version:
- '12'
docker_image:
- quay.io/condaforge/linux-anvil-cos7-x86_64
fftw:
- '3'
gmp:
- '6'
gsl:
- '2.7'
libboost_devel:
- '1.82'
libiio:
- '0'
libsndfile:
- '1.2'
libthrift:
- 0.20.0
numpy:
- '1.23'
pin_run_as_build:
  python:
    min_pin: x.x
    max_pin: x.x
pybind11_abi:
- '4'
pyqt:
- '5.15'
python:
- 3.11.* *_cpython
qt_main:
- '5.15'
soapysdr:
- '0.8'
spdlog:
- '1.12'
target_platform:
- linux-64
uhd:
- 4.6.0
volk:
- '3.1'
zeromq:
- 4.3.5
zip_keys:
- - c_compiler_version
  - cxx_compiler_version
- - python
  - numpy

    steps: MACOSX_DEPLOYMENT_TARGET:
- '10.13'
MACOSX_SDK_VERSION:
- '10.13'
c_compiler:
- clang
c_compiler_version:
- '16'
channel_sources:
- conda-forge
channel_targets:
- gnuradio main
cxx_compiler:
- clangxx
cxx_compiler_version:
- '16'
fftw:
- '3'
gmp:
- '6'
gsl:
- '2.7'
libboost_devel:
- '1.82'
libiio:
- '0'
libsndfile:
- '1.2'
libthrift:
- 0.20.0
macos_machine:
- x86_64-apple-darwin13.4.0
numpy:
- '1.23'
pin_run_as_build:
  python:
    min_pin: x.x
    max_pin: x.x
pybind11_abi:
- '4'
pyqt:
- '5.15'
python:
- 3.11.* *_cpython
qt_main:
- '5.15'
soapysdr:
- '0.8'
spdlog:
- '1.12'
target_platform:
- osx-64
uhd:
- 4.6.0
volk:
- '3.1'
zeromq:
- 4.3.5
zip_keys:
- - c_compiler_version
  - cxx_compiler_version
- - python
  - numpy
      - name: c_compiler:
- vs2022
channel_sources:
- conda-forge
channel_targets:
- gnuradio main
cxx_compiler:
- vs2022
fftw:
- '3'
gsl:
- '2.7'
libboost_devel:
- '1.82'
libiio:
- '0'
libsndfile:
- '1.2'
numpy:
- '1.23'
pin_run_as_build:
  python:
    min_pin: x.x
    max_pin: x.x
pybind11_abi:
- '4'
pyqt:
- '5.15'
python:
- 3.11.* *_cpython
qt_main:
- '5.15'
soapysdr:
- '0.8'
spdlog:
- '1.12'
target_platform:
- win-64
uhd:
- 4.6.0
volk:
- '3.1'
zeromq:
- 4.3.5
zip_keys:
- - python
  - numpy
        uses: setlocal EnableDelayedExpansion
@echo on

:: Make a build folder and change to it
cmake -E make_directory build
if errorlevel 1 exit 1
cd build
if errorlevel 1 exit 1

:: configure
cmake -G "Ninja" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_INSTALL_PREFIX="%LIBRARY_PREFIX%" ^
    -DCMAKE_PREFIX_PATH="%LIBRARY_PREFIX%" ^
    -DPYTHON_EXECUTABLE="%PYTHON%" ^
    -DGR_PYTHON_DIR="%SP_DIR%" ^
    ..
if errorlevel 1 exit 1

:: build
cmake --build . --config Release -- -j%CPU_COUNT%
if errorlevel 1 exit 1

:: install
cmake --build . --config Release --target install
if errorlevel 1 exit 1

:: test
set SKIP_TESTS=^
%=EMPTY=%

ctest --build-config Release --output-on-failure --timeout 120 -j%CPU_COUNT% -E "%SKIP_TESTS%"
if errorlevel 1 exit 1

:: now run the skipped tests to see failures, but exit without error anyway
ctest --build-config Release --output-on-failure --timeout 120 -j%CPU_COUNT% -R "%SKIP_TESTS%"
exit 0

      - name: #!/usr/bin/env bash

if [[ $target_platform == osx* ]] ; then
    CXXFLAGS="${CXXFLAGS} -D_LIBCPP_DISABLE_AVAILABILITY"
fi

# Workaround for no std::aligned_alloc with osx-64
# https://github.com/chriskohlhoff/asio/issues/1090
# Maybe remove when boost is updated to 1.80.0?
if [[ "${target_platform}" == "osx-64" ]]; then
  export CXXFLAGS="-DBOOST_ASIO_DISABLE_STD_ALIGNED_ALLOC ${CXXFLAGS}"
fi

cmake -E make_directory build
cd build

cmake_config_args=(
    -DCMAKE_BUILD_TYPE=Release
    -DCMAKE_PREFIX_PATH=$PREFIX
    -DCMAKE_INSTALL_PREFIX=$PREFIX
    -DLIB_SUFFIX=""
    -DPYTHON_EXECUTABLE=$PYTHON
    -DGR_PYTHON_DIR=$SP_DIR
)

cmake ${CMAKE_ARGS} -G "Ninja" .. "${cmake_config_args[@]}"
cmake --build . --config Release -- -j${CPU_COUNT}

cmake --build . --config Release --target install

if [[ $target_platform == linux* ]] ; then
    export QT_QPA_PLATFORM=offscreen
    SKIP_TESTS=(
    )
else
    SKIP_TESTS=(
    )
fi
SKIP_TESTS_STR=$( IFS="|"; echo "^(${SKIP_TESTS[*]})$" )

ctest --build-config Release --output-on-failure --timeout 120 -j${CPU_COUNT} -E "$SKIP_TESTS_STR"

# now run the skipped tests to see the failures, but don't error out
ctest --build-config Release --output-on-failure --timeout 120 -j${CPU_COUNT} -R "$SKIP_TESTS_STR" || exit 0
        run: channel_targets:
  - gnuradio main
c_compiler:    # [win]
- vs2022       # [win]
cxx_compiler:  # [win]
- vs2022       # [win]

      # Build is not required unless generated source files are used
      # - name: {% set name = "gnuradio-dev" %}
# Set package version from cleaned up git tags if possible,
# otherwise fall back to date-based version.
{% set tag_version = environ.get("GIT_DESCRIBE_TAG", "")|string|replace("-","_")|replace("v","")|replace("git","") %}
{% set post_commit = environ.get("GIT_DESCRIBE_NUMBER", 0)|string %}
{% set hash = environ.get("GIT_DESCRIBE_HASH", "local")|string %}
{% set fallback_version = "0.0.0.{0}.dev+g{1}".format(datetime.datetime.now().strftime("%Y%m%d"), environ.get("GIT_FULL_HASH", "local")[:9]) %}
{% set version = (tag_version if post_commit == "0" else "{0}.post{1}+{2}".format(tag_version, post_commit, hash)) if tag_version else fallback_version %}

package:
  name: {{ name|lower }}
  version: {{ version }}

source:
  # use local path or git repository depending on if the build is local or done on CI
  path: "../.."  # [not os.environ.get("CI")]
  git_url: {{ environ.get('FEEDSTOCK_ROOT', "../..") }}  # [os.environ.get("CI")]

build:
  number: 0
  skip: true  # [py!=311]
  entry_points:
    - gnuradio-companion = gnuradio.grc.main:main  # [win]
    - gr_filter_design = gnuradio.filter.filter_design:main  # [win]
    - gr_modtool = gnuradio.modtool.cli.base:cli  # [win]
    - grcc = gnuradio.grc.compiler:main  # [win]
    - uhd_siggen = gnuradio.uhd.uhd_siggen_base:main  # [win]
  run_exports:
    - {{ pin_subpackage('gnuradio-dev', max_pin='x.x.x') }}

requirements:
  build:
    - {{ compiler('c') }}
    - {{ compiler('cxx') }}
    - cmake >=3.8
    - git
    - ninja
    - pkg-config  # [not win]
    - sysroot_linux-64 2.17  # [linux64]
    - thrift-compiler
    # cross-compilation requirements
    - python                              # [build_platform != target_platform]
    - cross-python_{{ target_platform }}  # [build_platform != target_platform]
    - pybind11                            # [build_platform != target_platform]
    - numpy                               # [build_platform != target_platform]
   # below are needed to link with Qt for qtgui
    - {{ cdt('mesa-dri-drivers') }}  # [linux]
    - {{ cdt('mesa-libgl-devel') }}  # [linux]
  host:
    - click
    - click-plugins
    - codec2
    - fftw
    - gmp  # [not win]
    - gsl
    - libboost-devel
    - libsndfile
    - libthrift  # [not win]
    - mako
    - mpir  # [win]
    - numpy
    - packaging
    - pip  # [win]
    - pybind11
    - pybind11-abi
    - python
    - spdlog
    - thrift  # [not win]
    - volk
  # gnuradio.audio
    - alsa-lib  # [linux]
    - jack  # [linux]
    - portaudio
  # gnuradio companion
    - gtk3
    - lxml
    - pygobject
    - pyyaml
    - jsonschema
  # gnuradio.iio
    - libiio
    - libad9361-iio
  # gnuradio.qtgui
    - pyqt
    - qt-main
    - qwt
  # gnuradio.soapy
    - soapysdr
  # gnuradio.uhd
    - uhd
  # gnuradio.video_sdl
    - sdl
  # gnuradio.zeromq
    - cppzmq
    - zeromq
  # below needed only to run tests in host environment
    - pytest
    - pyzmq
    - setuptools
  run:
    - alsa-plugins  # [linux]
    - click
    - click-plugins
    - fftw
    - lxml
    - mako
    - matplotlib-base
    - menuinst  # [win]
    - numpy
    - packaging
    - {{ pin_compatible('portaudio') }}
    - pygobject
    - pyqt
    - pyqtgraph
    - python
    - pyyaml
    - jsonschema
    - pyzmq
    - {{ pin_compatible('qwt', max_pin='x.x') }}
    - scipy
    # need setuptools because modtool uses pkg_resources
    - setuptools
    - thrift  # [not win]
  run_constrained:
  # conflict with the non-dev conda-forge packages
  # by constraining to non-existent version
    - gnuradio-core ==9999999999
    - gnuradio-grc ==9999999999
    - gnuradio-qtgui ==9999999999
    - gnuradio-soapy ==9999999999
    - gnuradio-uhd ==9999999999
    - gnuradio-video-sdl ==9999999999
    - gnuradio-zeromq ==9999999999

test:
  commands:
    - gnuradio-config-info -v --prefix --sysconfdir --prefsdir --userprefsdir --prefs --builddate --enabled-components --cc --cxx --cflags
    - gr_modtool --help
  imports:
    - gnuradio.analog
    - gnuradio.audio
    - gnuradio.blocks
    - gnuradio.channels
    - gnuradio.digital
    - gnuradio.dtv
    - gnuradio.fec
    - gnuradio.fft
    - gnuradio.filter
    - gnuradio.gr
    - gnuradio.iio
    - gnuradio.network
    - gnuradio.pdu
    - gnuradio.qtgui
    - gnuradio.soapy
    - gnuradio.trellis
    - gnuradio.uhd
    - gnuradio.video_sdl
    - gnuradio.vocoder
    - gnuradio.wavelet
    - gnuradio.zeromq
    - pmt

about:
  home: https://gnuradio.org/
  license: GPL-3.0-or-later
  license_file: COPYING
  summary: The free and open software radio ecosystem
  description: >
    GNU Radio is a free software development toolkit that provides the signal
    processing runtime and processing blocks to implement software radios using
    readily-available, low-cost external RF hardware and commodity processors.
    It is widely used in hobbyist, academic and commercial environments to
    support wireless communications research as well as to implement real-world
    radio systems.

    GNU Radio applications are primarily written using the Python programming
    language, while the supplied, performance-critical signal processing path
    is implemented in C++ using processor floating point extensions where
    available. Thus, the developer is able to implement real-time, high-
    throughput radio systems in a simple-to-use, rapid-application-development
    environment.
  doc_url: https://gnuradio.org/doc/doxygen/
  dev_url: https://github.com/gnuradio/gnuradio

extra:
  recipe-maintainers:
    - ryanvolz
      #   run: mesa-libGL
mesa-dri-drivers
xorg-x11-server-Xvfb
      - name: Users
*****

This recipe is primarily used for CI builds, although it is possible to use it
locally from a checked-out git repository with "conda-build".

Developers
**********

CI builds use this recipe and the settings in .conda/conda-forge.yml. Any changes
to the recipe should be accompanied by running

    conda-smithy rerender --feedstock_config .conda/conda-forge.yml -c auto
    git rm -f .github/workflows/automerge.yml .github/workflows/webservices.yml .circleci/config.yml
    git commit --amend -s

so that the generated build scripts are updated when necessary.
***** RUN THE ABOVE TO CHECK FOR UPDATES WHENEVER EDITS ARE MADE in .conda *****

You can also re-render from a pull request by starting a comment with "/rerender", which will trigger a Github workflow to perform the above steps.

The CI will build conda packages for commits and pull requests, and it will
upload the packages to Anaconda Cloud on commits to the branch specified
in the configuration below. The channel and label that the packages are
uploaded to are set in the recipe directory in conda_build_config.yaml.
Uploads to Anaconda Cloud also require an API token to be set to the
BINSTAR_TOKEN environment variable. Documentation for token generation:
https://docs.anaconda.com/anacondaorg/user-guide/tasks/work-with-accounts/#creating-access-tokens
To populate BINSTAR_TOKEN for CI jobs, add the token as a secret, e.g.:
https://docs.github.com/en/actions/reference/encrypted-secrets
        uses: # This file defines the configuration for building conda packages with CI.
# The actual files for running the CI are created/updated by "conda-smithy".
# You should edit only this file and the recipe files (in recipe_dir, below)
# and not any of the automatically-generated files in e.g. .ci_support,
# .scripts, or .github/workflows/conda-build.yml.

# See https://conda-forge.org/docs/maintainer/conda_forge_yml.html for
# documentation on possible keys and values.

clone_depth: 0
github_actions:
  cancel_in_progress: false
  store_build_artifacts: true
os_version:
  linux_64: cos7
provider:
  linux: github_actions
  osx: github_actions
  win: github_actions
recipe_dir: .conda/recipe
# skip unnecessary files since this is not a full-fledged conda-forge feedstock
skip_render:
  - README.md
  - LICENSE.txt
  - .gitattributes
  - .gitignore
  - build-locally.py
  - LICENSE
test: native_and_emulated
# enable uploads to Anaconda Cloud from specified branches only
# ***** UPDATE THIS FOR MAINTENANCE BRANCHES ********************************
upload_on_branch: main
                # Provide a unique ID to access the sarif output path
        id: #include "AC_AttitudeControl.h"
#include <AP_HAL/AP_HAL.h>
#include <AP_Math/AP_Math.h>

#if APM_BUILD_TYPE(APM_BUILD_ArduPlane)
 // default gains for Plane
 # define AC_ATTITUDE_CONTROL_INPUT_TC_DEFAULT  0.2f    // Soft
#else
 // default gains for Copter and Sub
 # define AC_ATTITUDE_CONTROL_INPUT_TC_DEFAULT  0.15f   // Medium
#endif

// table of user settable parameters
const AP_Param::GroupInfo AC_AttitudeControl::var_info[] = {

    // 0, 1 were RATE_RP_MAX, RATE_Y_MAX

    // @Param: SLEW_YAW
    // @DisplayName: Yaw target slew rate
    // @Description: Maximum rate the yaw target can be updated in Loiter, RTL, Auto flight modes
    // @Units: cdeg/s
    // @Range: 500 18000
    // @Increment: 100
    // @User: Advanced
    AP_GROUPINFO("SLEW_YAW",    2, AC_AttitudeControl, _slew_yaw, AC_ATTITUDE_CONTROL_SLEW_YAW_DEFAULT_CDS),

    // 3 was for ACCEL_RP_MAX

    // @Param: ACCEL_Y_MAX
    // @DisplayName: Acceleration Max for Yaw
    // @Description: Maximum acceleration in yaw axis
    // @Units: cdeg/s/s
    // @Range: 0 72000
    // @Values: 0:Disabled, 9000:VerySlow, 18000:Slow, 36000:Medium, 54000:Fast
    // @Increment: 1000
    // @User: Advanced
    AP_GROUPINFO("ACCEL_Y_MAX",  4, AC_AttitudeControl, _accel_yaw_max, AC_ATTITUDE_CONTROL_ACCEL_Y_MAX_DEFAULT_CDSS),

    // @Param: RATE_FF_ENAB
    // @DisplayName: Rate Feedforward Enable
    // @Description: Controls whether body-frame rate feedfoward is enabled or disabled
    // @Values: 0:Disabled, 1:Enabled
    // @User: Advanced
    AP_GROUPINFO("RATE_FF_ENAB", 5, AC_AttitudeControl, _rate_bf_ff_enabled, AC_ATTITUDE_CONTROL_RATE_BF_FF_DEFAULT),

    // @Param: ACCEL_R_MAX
    // @DisplayName: Acceleration Max for Roll
    // @Description: Maximum acceleration in roll axis
    // @Units: cdeg/s/s
    // @Range: 0 180000
    // @Increment: 1000
    // @Values: 0:Disabled, 30000:VerySlow, 72000:Slow, 108000:Medium, 162000:Fast
    // @User: Advanced
    AP_GROUPINFO("ACCEL_R_MAX", 6, AC_AttitudeControl, _accel_roll_max, AC_ATTITUDE_CONTROL_ACCEL_RP_MAX_DEFAULT_CDSS),

    // @Param: ACCEL_P_MAX
    // @DisplayName: Acceleration Max for Pitch
    // @Description: Maximum acceleration in pitch axis
    // @Units: cdeg/s/s
    // @Range: 0 180000
    // @Increment: 1000
    // @Values: 0:Disabled, 30000:VerySlow, 72000:Slow, 108000:Medium, 162000:Fast
    // @User: Advanced
    AP_GROUPINFO("ACCEL_P_MAX", 7, AC_AttitudeControl, _accel_pitch_max, AC_ATTITUDE_CONTROL_ACCEL_RP_MAX_DEFAULT_CDSS),

    // IDs 8,9,10,11 RESERVED (in use on Solo)

    // @Param: ANGLE_BOOST
    // @DisplayName: Angle Boost
    // @Description: Angle Boost increases output throttle as the vehicle leans to reduce loss of altitude
    // @Values: 0:Disabled, 1:Enabled
    // @User: Advanced
    AP_GROUPINFO("ANGLE_BOOST", 12, AC_AttitudeControl, _angle_boost_enabled, 1),

    // @Param: ANG_RLL_P
    // @DisplayName: Roll axis angle controller P gain
    // @Description: Roll axis angle controller P gain.  Converts the error between the desired roll angle and actual angle to a desired roll rate
    // @Range: 3.000 12.000
    // @Range{Sub}: 0.0 12.000
    // @User: Standard
    AP_SUBGROUPINFO(_p_angle_roll, "ANG_RLL_", 13, AC_AttitudeControl, AC_P),

    // @Param: ANG_PIT_P
    // @DisplayName: Pitch axis angle controller P gain
    // @Description: Pitch axis angle controller P gain.  Converts the error between the desired pitch angle and actual angle to a desired pitch rate
    // @Range: 3.000 12.000
    // @Range{Sub}: 0.0 12.000
    // @User: Standard
    AP_SUBGROUPINFO(_p_angle_pitch, "ANG_PIT_", 14, AC_AttitudeControl, AC_P),

    // @Param: ANG_YAW_P
    // @DisplayName: Yaw axis angle controller P gain
    // @Description: Yaw axis angle controller P gain.  Converts the error between the desired yaw angle and actual angle to a desired yaw rate
    // @Range: 3.000 6.000
    // @Range{Sub}: 0.0 6.000
    // @User: Standard
    AP_SUBGROUPINFO(_p_angle_yaw, "ANG_YAW_", 15, AC_AttitudeControl, AC_P),

    // @Param: ANG_LIM_TC
    // @DisplayName: Angle Limit (to maintain altitude) Time Constant
    // @Description: Angle Limit (to maintain altitude) Time Constant
    // @Range: 0.5 10.0
    // @User: Advanced
    AP_GROUPINFO("ANG_LIM_TC", 16, AC_AttitudeControl, _angle_limit_tc, AC_ATTITUDE_CONTROL_ANGLE_LIMIT_TC_DEFAULT),

    // @Param: RATE_R_MAX
    // @DisplayName: Angular Velocity Max for Roll
    // @Description: Maximum angular velocity in roll axis
    // @Units: deg/s
    // @Range: 0 1080
    // @Increment: 1
    // @Values: 0:Disabled, 360:Slow, 720:Medium, 1080:Fast
    // @User: Advanced
    AP_GROUPINFO("RATE_R_MAX", 17, AC_AttitudeControl, _ang_vel_roll_max, 0.0f),

    // @Param: RATE_P_MAX
    // @DisplayName: Angular Velocity Max for Pitch
    // @Description: Maximum angular velocity in pitch axis
    // @Units: deg/s
    // @Range: 0 1080
    // @Increment: 1
    // @Values: 0:Disabled, 360:Slow, 720:Medium, 1080:Fast
    // @User: Advanced
    AP_GROUPINFO("RATE_P_MAX", 18, AC_AttitudeControl, _ang_vel_pitch_max, 0.0f),

    // @Param: RATE_Y_MAX
    // @DisplayName: Angular Velocity Max for Pitch
    // @Description: Maximum angular velocity in pitch axis
    // @Units: deg/s
    // @Range: 0 1080
    // @Increment: 1
    // @Values: 0:Disabled, 360:Slow, 720:Medium, 1080:Fast
    // @User: Advanced
    AP_GROUPINFO("RATE_Y_MAX", 19, AC_AttitudeControl, _ang_vel_yaw_max, 0.0f),

    // @Param: INPUT_TC
    // @DisplayName: Attitude control input time constant (aka smoothing)
    // @Description: Attitude control input time constant.  Low numbers lead to sharper response, higher numbers to softer response
    // @Units: s
    // @Range: 0 1
    // @Increment: 0.01
    // @Values: 0.5:Very Soft, 0.2:Soft, 0.15:Medium, 0.1:Crisp, 0.05:Very Crisp
    // @User: Standard
    AP_GROUPINFO("INPUT_TC", 20, AC_AttitudeControl, _input_tc, AC_ATTITUDE_CONTROL_INPUT_TC_DEFAULT),

    AP_GROUPEND
};

// Set output throttle and disable stabilization
void AC_AttitudeControl::set_throttle_out_unstabilized(float throttle_in, bool reset_attitude_control, float filter_cutoff)
{
    _throttle_in = throttle_in;
    _motors.set_throttle_filter_cutoff(filter_cutoff);
    if (reset_attitude_control) {
        relax_attitude_controllers();
    }
    _motors.set_throttle(throttle_in);
    _angle_boost = 0.0f;
}

// Ensure attitude controller have zero errors to relax rate controller output
void AC_AttitudeControl::relax_attitude_controllers()
{
    // TODO add _ahrs.get_quaternion()
    _attitude_target_quat.from_rotation_matrix(_ahrs.get_rotation_body_to_ned());
    _attitude_target_ang_vel = _ahrs.get_gyro();
    _attitude_target_euler_angle = Vector3f(_ahrs.roll, _ahrs.pitch, _ahrs.yaw);

    // Set reference angular velocity used in angular velocity controller equal
    // to the input angular velocity and reset the angular velocity integrators.
    // This zeros the output of the angular velocity controller.
    _rate_target_ang_vel = _ahrs.get_gyro();
    get_rate_roll_pid().reset_I();
    get_rate_pitch_pid().reset_I();
    get_rate_yaw_pid().reset_I();
}

void AC_AttitudeControl::reset_rate_controller_I_terms()
{
    get_rate_roll_pid().reset_I();
    get_rate_pitch_pid().reset_I();
    get_rate_yaw_pid().reset_I();
}

// The attitude controller works around the concept of the desired attitude, target attitude
// and measured attitude. The desired attitude is the attitude input into the attitude controller
// that expresses where the higher level code would like the aircraft to move to. The target attitude is moved
// to the desired attitude with jerk, acceleration, and velocity limits. The target angular velocities are fed
// directly into the rate controllers. The angular error between the measured attitude and the target attitude is
// fed into the angle controller and the output of the angle controller summed at the input of the rate controllers.
// By feeding the target angular velocity directly into the rate controllers the measured and target attitudes
// remain very close together.
//
// All input functions below follow the same procedure
// 1. define the desired attitude the aircraft should attempt to achieve using the input variables
// 2. using the desired attitude and input variables, define the target angular velocity so that it should
//    move the target attitude towards the desired attitude
// 3. if _rate_bf_ff_enabled is not being used then make the target attitude
//    and target angular velocities equal to the desired attitude and desired angular velocities.
// 4. ensure _attitude_target_quat, _attitude_target_euler_angle, _attitude_target_euler_rate and
//    _attitude_target_ang_vel have been defined. This ensures input modes can be changed without discontinuity.
// 5. attitude_controller_run_quat is then run to pass the target angular velocities to the rate controllers and
//    integrate them into the target attitude. Any errors between the target attitude and the measured attitude are
//    corrected by first correcting the thrust vector until the angle between the target thrust vector measured
//    trust vector drops below 2*AC_ATTITUDE_THRUST_ERROR_ANGLE. At this point the heading is also corrected.



// Command a Quaternion attitude with feedforward and smoothing
void AC_AttitudeControl::input_quaternion(Quaternion attitude_desired_quat)
{
    // calculate the attitude target euler angles
    _attitude_target_quat.to_euler(_attitude_target_euler_angle.x, _attitude_target_euler_angle.y, _attitude_target_euler_angle.z);

    Quaternion attitude_error_quat = _attitude_target_quat.inverse() * attitude_desired_quat;
    Vector3f attitude_error_angle;
    attitude_error_quat.to_axis_angle(attitude_error_angle);

    if (_rate_bf_ff_enabled) {
        // When acceleration limiting and feedforward are enabled, the sqrt controller is used to compute an euler
        // angular velocity that will cause the euler angle to smoothly stop at the input angle with limited deceleration
        // and an exponential decay specified by _input_tc at the end.
        _attitude_target_ang_vel.x = input_shaping_angle(wrap_PI(attitude_error_angle.x), _input_tc, get_accel_roll_max_radss(), _attitude_target_ang_vel.x, _dt);
        _attitude_target_ang_vel.y = input_shaping_angle(wrap_PI(attitude_error_angle.y), _input_tc, get_accel_pitch_max_radss(), _attitude_target_ang_vel.y, _dt);
        _attitude_target_ang_vel.z = input_shaping_angle(wrap_PI(attitude_error_angle.z), _input_tc, get_accel_yaw_max_radss(), _attitude_target_ang_vel.z, _dt);

        // Limit the angular velocity
        ang_vel_limit(_attitude_target_ang_vel, radians(_ang_vel_roll_max), radians(_ang_vel_pitch_max), radians(_ang_vel_yaw_max));
        // Convert body-frame angular velocity into euler angle derivative of desired attitude
        ang_vel_to_euler_rate(_attitude_target_euler_angle, _attitude_target_ang_vel, _attitude_target_euler_rate);
    } else {
        _attitude_target_quat = attitude_desired_quat;

        // Set rate feedforward requests to zero
        _attitude_target_euler_rate = Vector3f(0.0f, 0.0f, 0.0f);
        _attitude_target_ang_vel = Vector3f(0.0f, 0.0f, 0.0f);
    }

    // Call quaternion attitude controller
    attitude_controller_run_quat();
}

// Command an euler roll and pitch angle and an euler yaw rate with angular velocity feedforward and smoothing
void AC_AttitudeControl::input_euler_angle_roll_pitch_euler_rate_yaw(float euler_roll_angle_cd, float euler_pitch_angle_cd, float euler_yaw_rate_cds)
{
    // Convert from centidegrees on public interface to radians
    float euler_roll_angle = radians(euler_roll_angle_cd*0.01f);
    float euler_pitch_angle = radians(euler_pitch_angle_cd*0.01f);
    float euler_yaw_rate = radians(euler_yaw_rate_cds*0.01f);

    // calculate the attitude target euler angles
    _attitude_target_quat.to_euler(_attitude_target_euler_angle.x, _attitude_target_euler_angle.y, _attitude_target_euler_angle.z);

    // Add roll trim to compensate tail rotor thrust in heli (will return zero on multirotors)
    euler_roll_angle += get_roll_trim_rad();

    if (_rate_bf_ff_enabled) {
        // translate the roll pitch and yaw acceleration limits to the euler axis
        Vector3f euler_accel = euler_accel_limit(_attitude_target_euler_angle, Vector3f(get_accel_roll_max_radss(), get_accel_pitch_max_radss(), get_accel_yaw_max_radss()));

        // When acceleration limiting and feedforward are enabled, the sqrt controller is used to compute an euler
        // angular velocity that will cause the euler angle to smoothly stop at the input angle with limited deceleration
        // and an exponential decay specified by smoothing_gain at the end.
        _attitude_target_euler_rate.x = input_shaping_angle(wrap_PI(euler_roll_angle-_attitude_target_euler_angle.x), _input_tc, euler_accel.x, _attitude_target_euler_rate.x, _dt);
        _attitude_target_euler_rate.y = input_shaping_angle(wrap_PI(euler_pitch_angle-_attitude_target_euler_angle.y), _input_tc, euler_accel.y, _attitude_target_euler_rate.y, _dt);

        // When yaw acceleration limiting is enabled, the yaw input shaper constrains angular acceleration about the yaw axis, slewing
        // the output rate towards the input rate.
        _attitude_target_euler_rate.z = input_shaping_ang_vel(_attitude_target_euler_rate.z, euler_yaw_rate, euler_accel.z, _dt);

        // Convert euler angle derivative of desired attitude into a body-frame angular velocity vector for feedforward
        euler_rate_to_ang_vel(_attitude_target_euler_angle, _attitude_target_euler_rate, _attitude_target_ang_vel);
        // Limit the angular velocity
        ang_vel_limit(_attitude_target_ang_vel, radians(_ang_vel_roll_max), radians(_ang_vel_pitch_max), radians(_ang_vel_yaw_max));
        // Convert body-frame angular velocity into euler angle derivative of desired attitude
        ang_vel_to_euler_rate(_attitude_target_euler_angle, _attitude_target_ang_vel, _attitude_target_euler_rate);
    } else {
        // When feedforward is not enabled, the target euler angle is input into the target and the feedforward rate is zeroed.
        _attitude_target_euler_angle.x = euler_roll_angle;
        _attitude_target_euler_angle.y = euler_pitch_angle;
        _attitude_target_euler_angle.z += euler_yaw_rate*_dt;
        // Compute quaternion target attitude
        _attitude_target_quat.from_euler(_attitude_target_euler_angle.x, _attitude_target_euler_angle.y, _attitude_target_euler_angle.z);

        // Set rate feedforward requests to zero
        _attitude_target_euler_rate = Vector3f(0.0f, 0.0f, 0.0f);
        _attitude_target_ang_vel = Vector3f(0.0f, 0.0f, 0.0f);
    }

    // Call quaternion attitude controller
    attitude_controller_run_quat();
}

// Command an euler roll, pitch and yaw angle with angular velocity feedforward and smoothing
void AC_AttitudeControl::input_euler_angle_roll_pitch_yaw(float euler_roll_angle_cd, float euler_pitch_angle_cd, float euler_yaw_angle_cd, bool slew_yaw)
{
    // Convert from centidegrees on public interface to radians
    float euler_roll_angle = radians(euler_roll_angle_cd*0.01f);
    float euler_pitch_angle = radians(euler_pitch_angle_cd*0.01f);
    float euler_yaw_angle = radians(euler_yaw_angle_cd*0.01f);

    // calculate the attitude target euler angles
    _attitude_target_quat.to_euler(_attitude_target_euler_angle.x, _attitude_target_euler_angle.y, _attitude_target_euler_angle.z);

    // Add roll trim to compensate tail rotor thrust in heli (will return zero on multirotors)
    euler_roll_angle += get_roll_trim_rad();

    if (_rate_bf_ff_enabled) {
        // translate the roll pitch and yaw acceleration limits to the euler axis
        Vector3f euler_accel = euler_accel_limit(_attitude_target_euler_angle, Vector3f(get_accel_roll_max_radss(), get_accel_pitch_max_radss(), get_accel_yaw_max_radss()));

        // When acceleration limiting and feedforward are enabled, the sqrt controller is used to compute an euler
        // angular velocity that will cause the euler angle to smoothly stop at the input angle with limited deceleration
        // and an exponential decay specified by _input_tc at the end.
        _attitude_target_euler_rate.x = input_shaping_angle(wrap_PI(euler_roll_angle-_attitude_target_euler_angle.x), _input_tc, euler_accel.x, _attitude_target_euler_rate.x, _dt);
        _attitude_target_euler_rate.y = input_shaping_angle(wrap_PI(euler_pitch_angle-_attitude_target_euler_angle.y), _input_tc, euler_accel.y, _attitude_target_euler_rate.y, _dt);
        _attitude_target_euler_rate.z = input_shaping_angle(wrap_PI(euler_yaw_angle-_attitude_target_euler_angle.z), _input_tc, euler_accel.z, _attitude_target_euler_rate.z, _dt);
        if (slew_yaw) {
            _attitude_target_euler_rate.z = constrain_float(_attitude_target_euler_rate.z, -get_slew_yaw_rads(), get_slew_yaw_rads());
        }

        // Convert euler angle derivative of desired attitude into a body-frame angular velocity vector for feedforward
        euler_rate_to_ang_vel(_attitude_target_euler_angle, _attitude_target_euler_rate, _attitude_target_ang_vel);
        // Limit the angular velocity
        ang_vel_limit(_attitude_target_ang_vel, radians(_ang_vel_roll_max), radians(_ang_vel_pitch_max), radians(_ang_vel_yaw_max));
        // Convert body-frame angular velocity into euler angle derivative of desired attitude
        ang_vel_to_euler_rate(_attitude_target_euler_angle, _attitude_target_ang_vel, _attitude_target_euler_rate);
    } else {
        // When feedforward is not enabled, the target euler angle is input into the target and the feedforward rate is zeroed.
        _attitude_target_euler_angle.x = euler_roll_angle;
        _attitude_target_euler_angle.y = euler_pitch_angle;
        if (slew_yaw) {
            // Compute constrained angle error
            float angle_error = constrain_float(wrap_PI(euler_yaw_angle-_attitude_target_euler_angle.z), -get_slew_yaw_rads()*_dt, get_slew_yaw_rads()*_dt);
            // Update attitude target from constrained angle error
            _attitude_target_euler_angle.z = wrap_PI(angle_error + _attitude_target_euler_angle.z);
        } else {
            _attitude_target_euler_angle.z = euler_yaw_angle;
        }
        // Compute quaternion target attitude
        _attitude_target_quat.from_euler(_attitude_target_euler_angle.x, _attitude_target_euler_angle.y, _attitude_target_euler_angle.z);

        // Set rate feedforward requests to zero
        _attitude_target_euler_rate = Vector3f(0.0f, 0.0f, 0.0f);
        _attitude_target_ang_vel = Vector3f(0.0f, 0.0f, 0.0f);
    }

    // Call quaternion attitude controller
    attitude_controller_run_quat();
}

// Command an euler roll, pitch, and yaw rate with angular velocity feedforward and smoothing
void AC_AttitudeControl::input_euler_rate_roll_pitch_yaw(float euler_roll_rate_cds, float euler_pitch_rate_cds, float euler_yaw_rate_cds)
{
    // Convert from centidegrees on public interface to radians
    float euler_roll_rate = radians(euler_roll_rate_cds*0.01f);
    float euler_pitch_rate = radians(euler_pitch_rate_cds*0.01f);
    float euler_yaw_rate = radians(euler_yaw_rate_cds*0.01f);

    // calculate the attitude target euler angles
    _attitude_target_quat.to_euler(_attitude_target_euler_angle.x, _attitude_target_euler_angle.y, _attitude_target_euler_angle.z);

    if (_rate_bf_ff_enabled) {
        // translate the roll pitch and yaw acceleration limits to the euler axis
        Vector3f euler_accel = euler_accel_limit(_attitude_target_euler_angle, Vector3f(get_accel_roll_max_radss(), get_accel_pitch_max_radss(), get_accel_yaw_max_radss()));

        // When acceleration limiting is enabled, the input shaper constrains angular acceleration, slewing
        // the output rate towards the input rate.
        _attitude_target_euler_rate.x = input_shaping_ang_vel(_attitude_target_euler_rate.x, euler_roll_rate, euler_accel.x, _dt);
        _attitude_target_euler_rate.y = input_shaping_ang_vel(_attitude_target_euler_rate.y, euler_pitch_rate, euler_accel.y, _dt);
        _attitude_target_euler_rate.z = input_shaping_ang_vel(_attitude_target_euler_rate.z, euler_yaw_rate, euler_accel.z, _dt);

        // Convert euler angle derivative of desired attitude into a body-frame angular velocity vector for feedforward
        euler_rate_to_ang_vel(_attitude_target_euler_angle, _attitude_target_euler_rate, _attitude_target_ang_vel);
    } else {
        // When feedforward is not enabled, the target euler angle is input into the target and the feedforward rate is zeroed.
        // Pitch angle is restricted to +- 85.0 degrees to avoid gimbal lock discontinuities.
        _attitude_target_euler_angle.x = wrap_PI(_attitude_target_euler_angle.x + euler_roll_rate*_dt);
        _attitude_target_euler_angle.y = constrain_float(_attitude_target_euler_angle.y + euler_pitch_rate*_dt, radians(-85.0f), radians(85.0f));
        _attitude_target_euler_angle.z = wrap_2PI(_attitude_target_euler_angle.z + euler_yaw_rate*_dt);

        // Set rate feedforward requests to zero
        _attitude_target_euler_rate = Vector3f(0.0f, 0.0f, 0.0f);
        _attitude_target_ang_vel = Vector3f(0.0f, 0.0f, 0.0f);

        // Compute quaternion target attitude
        _attitude_target_quat.from_euler(_attitude_target_euler_angle.x, _attitude_target_euler_angle.y, _attitude_target_euler_angle.z);
    }

    // Call quaternion attitude controller
    attitude_controller_run_quat();
}

// Command an angular velocity with angular velocity feedforward and smoothing
void AC_AttitudeControl::input_rate_bf_roll_pitch_yaw(float roll_rate_bf_cds, float pitch_rate_bf_cds, float yaw_rate_bf_cds)
{
    // Convert from centidegrees on public interface to radians
    float roll_rate_rads = radians(roll_rate_bf_cds*0.01f);
    float pitch_rate_rads = radians(pitch_rate_bf_cds*0.01f);
    float yaw_rate_rads = radians(yaw_rate_bf_cds*0.01f);

    // calculate the attitude target euler angles
    _attitude_target_quat.to_euler(_attitude_target_euler_angle.x, _attitude_target_euler_angle.y, _attitude_target_euler_angle.z);

    if (_rate_bf_ff_enabled) {
        // Compute acceleration-limited body frame rates
        // When acceleration limiting is enabled, the input shaper constrains angular acceleration about the axis, slewing
        // the output rate towards the input rate.
        _attitude_target_ang_vel.x = input_shaping_ang_vel(_attitude_target_ang_vel.x, roll_rate_rads, get_accel_roll_max_radss(), _dt);
        _attitude_target_ang_vel.y = input_shaping_ang_vel(_attitude_target_ang_vel.y, pitch_rate_rads, get_accel_pitch_max_radss(), _dt);
        _attitude_target_ang_vel.z = input_shaping_ang_vel(_attitude_target_ang_vel.z, yaw_rate_rads, get_accel_yaw_max_radss(), _dt);

        // Convert body-frame angular velocity into euler angle derivative of desired attitude
        ang_vel_to_euler_rate(_attitude_target_euler_angle, _attitude_target_ang_vel, _attitude_target_euler_rate);
    } else {
        // When feedforward is not enabled, the quaternion is calculated and is input into the target and the feedforward rate is zeroed.
        Quaternion attitude_target_update_quat;
        attitude_target_update_quat.from_axis_angle(Vector3f(roll_rate_rads * _dt, pitch_rate_rads * _dt, yaw_rate_rads * _dt));
        _attitude_target_quat = _attitude_target_quat * attitude_target_update_quat;
        _attitude_target_quat.normalize();

        // Set rate feedforward requests to zero
        _attitude_target_euler_rate = Vector3f(0.0f, 0.0f, 0.0f);
        _attitude_target_ang_vel = Vector3f(0.0f, 0.0f, 0.0f);
    }

    // Call quaternion attitude controller
    attitude_controller_run_quat();
}

// Command an angular velocity with angular velocity smoothing using rate loops only with no attitude loop stabilization
void AC_AttitudeControl::input_rate_bf_roll_pitch_yaw_2(float roll_rate_bf_cds, float pitch_rate_bf_cds, float yaw_rate_bf_cds)
{
    // Convert from centidegrees on public interface to radians
    float roll_rate_rads = radians(roll_rate_bf_cds*0.01f);
    float pitch_rate_rads = radians(pitch_rate_bf_cds*0.01f);
    float yaw_rate_rads = radians(yaw_rate_bf_cds*0.01f);

    // Compute acceleration-limited body frame rates
    // When acceleration limiting is enabled, the input shaper constrains angular acceleration about the axis, slewing
    // the output rate towards the input rate.
    _attitude_target_ang_vel.x = input_shaping_ang_vel(_attitude_target_ang_vel.x, roll_rate_rads, get_accel_roll_max_radss(), _dt);
    _attitude_target_ang_vel.y = input_shaping_ang_vel(_attitude_target_ang_vel.y, pitch_rate_rads, get_accel_pitch_max_radss(), _dt);
    _attitude_target_ang_vel.z = input_shaping_ang_vel(_attitude_target_ang_vel.z, yaw_rate_rads, get_accel_yaw_max_radss(), _dt);

    // Update the unused targets attitude based on current attitude to condition mode change
    _attitude_target_quat.from_rotation_matrix(_ahrs.get_rotation_body_to_ned());
    _attitude_target_quat.to_euler(_attitude_target_euler_angle.x, _attitude_target_euler_angle.y, _attitude_target_euler_angle.z);
    // Convert body-frame angular velocity into euler angle derivative of desired attitude
    ang_vel_to_euler_rate(_attitude_target_euler_angle, _attitude_target_ang_vel, _attitude_target_euler_rate);
    _rate_target_ang_vel = _attitude_target_ang_vel;
}

// Command an angular velocity with angular velocity smoothing using rate loops only with integrated rate error stabilization
void AC_AttitudeControl::input_rate_bf_roll_pitch_yaw_3(float roll_rate_bf_cds, float pitch_rate_bf_cds, float yaw_rate_bf_cds)
{
    // Convert from centidegrees on public interface to radians
    float roll_rate_rads = radians(roll_rate_bf_cds*0.01f);
    float pitch_rate_rads = radians(pitch_rate_bf_cds*0.01f);
    float yaw_rate_rads = radians(yaw_rate_bf_cds*0.01f);

    // Update attitude error
    Vector3f gyro_latest = _ahrs.get_gyro_latest();
    Quaternion attitude_ang_error_update_quat;
    attitude_ang_error_update_quat.from_axis_angle(Vector3f((_attitude_target_ang_vel.x-gyro_latest.x) * _dt, (_attitude_target_ang_vel.y-gyro_latest.y) * _dt, (_attitude_target_ang_vel.z-gyro_latest.z) * _dt));
    _attitude_ang_error = attitude_ang_error_update_quat * _attitude_ang_error;

    // Compute acceleration-limited body frame rates
    // When acceleration limiting is enabled, the input shaper constrains angular acceleration about the axis, slewing
    // the output rate towards the input rate.
    _attitude_target_ang_vel.x = input_shaping_ang_vel(_attitude_target_ang_vel.x, roll_rate_rads, get_accel_roll_max_radss(), _dt);
    _attitude_target_ang_vel.y = input_shaping_ang_vel(_attitude_target_ang_vel.y, pitch_rate_rads, get_accel_pitch_max_radss(), _dt);
    _attitude_target_ang_vel.z = input_shaping_ang_vel(_attitude_target_ang_vel.z, yaw_rate_rads, get_accel_yaw_max_radss(), _dt);

    // Retrieve quaternion vehicle attitude
    // TODO add _ahrs.get_quaternion()
    Quaternion attitude_vehicle_quat;
    attitude_vehicle_quat.from_rotation_matrix(_ahrs.get_rotation_body_to_ned());

    // Update the unused targets attitude based on current attitude to condition mode change
    _attitude_target_quat = attitude_vehicle_quat*_attitude_ang_error;

    // calculate the attitude target euler angles
    _attitude_target_quat.to_euler(_attitude_target_euler_angle.x, _attitude_target_euler_angle.y, _attitude_target_euler_angle.z);

    // Convert body-frame angular velocity into euler angle derivative of desired attitude
    ang_vel_to_euler_rate(_attitude_target_euler_angle, _attitude_target_ang_vel, _attitude_target_euler_rate);

    // Compute the angular velocity target from the integrated rate error
    Vector3f attitude_error_vector;
    _attitude_ang_error.to_axis_angle(attitude_error_vector);
    _rate_target_ang_vel = update_ang_vel_target_from_att_error(attitude_error_vector);
    _rate_target_ang_vel += _attitude_target_ang_vel;

    // ensure Quaternions stay normalized
    _attitude_ang_error.normalize();
}

// Command an angular step (i.e change) in body frame angle
// Used to command a step in angle without exciting the orthogonal axis during autotune
void AC_AttitudeControl::input_angle_step_bf_roll_pitch_yaw(float roll_angle_step_bf_cd, float pitch_angle_step_bf_cd, float yaw_angle_step_bf_cd)
{
    // Convert from centidegrees on public interface to radians
    float roll_step_rads = radians(roll_angle_step_bf_cd*0.01f);
    float pitch_step_rads = radians(pitch_angle_step_bf_cd*0.01f);
    float yaw_step_rads = radians(yaw_angle_step_bf_cd*0.01f);

    // rotate attitude target by desired step
    Quaternion attitude_target_update_quat;
    attitude_target_update_quat.from_axis_angle(Vector3f(roll_step_rads, pitch_step_rads, yaw_step_rads));
    _attitude_target_quat = _attitude_target_quat * attitude_target_update_quat;
    _attitude_target_quat.normalize();

    // calculate the attitude target euler angles
    _attitude_target_quat.to_euler(_attitude_target_euler_angle.x, _attitude_target_euler_angle.y, _attitude_target_euler_angle.z);

    // Set rate feedforward requests to zero
    _attitude_target_euler_rate = Vector3f(0.0f, 0.0f, 0.0f);
    _attitude_target_ang_vel = Vector3f(0.0f, 0.0f, 0.0f);

    // Call quaternion attitude controller
    attitude_controller_run_quat();
}

// Calculates the body frame angular velocities to follow the target attitude
void AC_AttitudeControl::attitude_controller_run_quat()
{
    // Retrieve quaternion vehicle attitude
    // TODO add _ahrs.get_quaternion()
    Quaternion attitude_vehicle_quat;
    attitude_vehicle_quat.from_rotation_matrix(_ahrs.get_rotation_body_to_ned());

    // Compute attitude error
    Vector3f attitude_error_vector;
    thrust_heading_rotation_angles(_attitude_target_quat, attitude_vehicle_quat, attitude_error_vector, _thrust_error_angle);

    // Compute the angular velocity target from the attitude error
    _rate_target_ang_vel = update_ang_vel_target_from_att_error(attitude_error_vector);

    // Add feedforward term that attempts to ensure that roll and pitch errors rotate with the body frame rather than the reference frame.
    // todo: this should probably be a matrix that couples yaw as well.
    _rate_target_ang_vel.x += attitude_error_vector.y * _ahrs.get_gyro().z;
    _rate_target_ang_vel.y += -attitude_error_vector.x * _ahrs.get_gyro().z;

    ang_vel_limit(_rate_target_ang_vel, radians(_ang_vel_roll_max), radians(_ang_vel_pitch_max), radians(_ang_vel_yaw_max));

    // Add the angular velocity feedforward, rotated into vehicle frame
    Quaternion attitude_target_ang_vel_quat = Quaternion(0.0f, _attitude_target_ang_vel.x, _attitude_target_ang_vel.y, _attitude_target_ang_vel.z);
    Quaternion to_to_from_quat = attitude_vehicle_quat.inverse() * _attitude_target_quat;
    Quaternion desired_ang_vel_quat = to_to_from_quat.inverse()*attitude_target_ang_vel_quat*to_to_from_quat;

    // Correct the thrust vector and smoothly add feedforward and yaw input
    if(_thrust_error_angle > AC_ATTITUDE_THRUST_ERROR_ANGLE*2.0f){
        _rate_target_ang_vel.z = _ahrs.get_gyro().z;
    }else if(_thrust_error_angle > AC_ATTITUDE_THRUST_ERROR_ANGLE){
        float feedforward_scalar = (1.0f - (_thrust_error_angle-AC_ATTITUDE_THRUST_ERROR_ANGLE)/AC_ATTITUDE_THRUST_ERROR_ANGLE);
        _rate_target_ang_vel.x += desired_ang_vel_quat.q2*feedforward_scalar;
        _rate_target_ang_vel.y += desired_ang_vel_quat.q3*feedforward_scalar;
        _rate_target_ang_vel.z += desired_ang_vel_quat.q4;
        _rate_target_ang_vel.z = _ahrs.get_gyro().z*(1.0-feedforward_scalar) + _rate_target_ang_vel.z*feedforward_scalar;
    } else {
        _rate_target_ang_vel.x += desired_ang_vel_quat.q2;
        _rate_target_ang_vel.y += desired_ang_vel_quat.q3;
        _rate_target_ang_vel.z += desired_ang_vel_quat.q4;
    }

    if (_rate_bf_ff_enabled) {
        // rotate target and normalize
        Quaternion attitude_target_update_quat;
        attitude_target_update_quat.from_axis_angle(Vector3f(_attitude_target_ang_vel.x * _dt, _attitude_target_ang_vel.y * _dt, _attitude_target_ang_vel.z * _dt));
        _attitude_target_quat = _attitude_target_quat * attitude_target_update_quat;
        _attitude_target_quat.normalize();
    }

    // ensure Quaternions stay normalized
    _attitude_target_quat.normalize();

    // Record error to handle EKF resets
    _attitude_ang_error = attitude_vehicle_quat.inverse() * _attitude_target_quat;
}

// thrust_heading_rotation_angles - calculates two ordered rotations to move the att_from_quat quaternion to the att_to_quat quaternion.
// The first rotation corrects the thrust vector and the second rotation corrects the heading vector.
void AC_AttitudeControl::thrust_heading_rotation_angles(Quaternion& att_to_quat, const Quaternion& att_from_quat, Vector3f& att_diff_angle, float& thrust_vec_dot)
{
    Matrix3f att_to_rot_matrix; // rotation from the target body frame to the inertial frame.
    att_to_quat.rotation_matrix(att_to_rot_matrix);
    Vector3f att_to_thrust_vec = att_to_rot_matrix*Vector3f(0.0f,0.0f,1.0f);

    Matrix3f att_from_rot_matrix; // rotation from the current body frame to the inertial frame.
    att_from_quat.rotation_matrix(att_from_rot_matrix);
    Vector3f att_from_thrust_vec = att_from_rot_matrix*Vector3f(0.0f,0.0f,1.0f);

    // the cross product of the desired and target thrust vector defines the rotation vector
    Vector3f thrust_vec_cross = att_from_thrust_vec % att_to_thrust_vec;

    // the dot product is used to calculate the angle between the target and desired thrust vectors
    thrust_vec_dot = acosf(constrain_float(att_from_thrust_vec * att_to_thrust_vec,-1.0f,1.0f));

    // Normalize the thrust rotation vector
    float thrust_vector_length = thrust_vec_cross.length();
    if(is_zero(thrust_vector_length) || is_zero(thrust_vec_dot)){
        thrust_vec_cross = Vector3f(0,0,1);
        thrust_vec_dot = 0.0f;
    }else{
        thrust_vec_cross /= thrust_vector_length;
    }
    Quaternion thrust_vec_correction_quat;
    thrust_vec_correction_quat.from_axis_angle(thrust_vec_cross, thrust_vec_dot);

    // Rotate thrust_vec_correction_quat to the att_from frame
    thrust_vec_correction_quat = att_from_quat.inverse()*thrust_vec_correction_quat*att_from_quat;

    // calculate the remaining rotation required after thrust vector is rotated transformed to the att_from frame
    Quaternion yaw_vec_correction_quat = thrust_vec_correction_quat.inverse()*att_from_quat.inverse()*att_to_quat;

    // calculate the angle error in x and y.
    Vector3f rotation;
    thrust_vec_correction_quat.to_axis_angle(rotation);
    att_diff_angle.x = rotation.x;
    att_diff_angle.y = rotation.y;

    // calculate the angle error in z (x and y should be zero here).
    yaw_vec_correction_quat.to_axis_angle(rotation);
    att_diff_angle.z = rotation.z;

    // Todo: Limit roll an pitch error based on output saturation and maximum error.

    // Limit Yaw Error based on maximum acceleration - Update to include output saturation and maximum error.
    // Currently the limit is based on the maximum acceleration using the linear part of the SQRT controller.
    // This should be updated to be based on an angle limit, saturation, or unlimited based on user defined parameters.
    if(!is_zero(_p_angle_yaw.kP()) && fabsf(att_diff_angle.z) > AC_ATTITUDE_ACCEL_Y_CONTROLLER_MAX_RADSS/_p_angle_yaw.kP()){
        att_diff_angle.z = constrain_float(wrap_PI(att_diff_angle.z), -AC_ATTITUDE_ACCEL_Y_CONTROLLER_MAX_RADSS/_p_angle_yaw.kP(), AC_ATTITUDE_ACCEL_Y_CONTROLLER_MAX_RADSS/_p_angle_yaw.kP());
        yaw_vec_correction_quat.from_axis_angle(Vector3f(0.0f,0.0f,att_diff_angle.z));
        att_to_quat = att_from_quat*thrust_vec_correction_quat*yaw_vec_correction_quat;
    }
}

// calculates the velocity correction from an angle error. The angular velocity has acceleration and
// deceleration limits including basic jerk limiting using _input_tc
float AC_AttitudeControl::input_shaping_angle(float error_angle, float input_tc, float accel_max, float target_ang_vel, float dt)
{
    // Calculate the velocity as error approaches zero with acceleration limited by accel_max_radss
    float desired_ang_vel = sqrt_controller(error_angle, 1.0f / MAX(input_tc, 0.01f), accel_max, dt);

    // Acceleration is limited directly to smooth the beginning of the curve.
    return input_shaping_ang_vel(target_ang_vel, desired_ang_vel, accel_max, dt);
}

// limits the acceleration and deceleration of a velocity request
float AC_AttitudeControl::input_shaping_ang_vel(float target_ang_vel, float desired_ang_vel, float accel_max, float dt)
{
    // Acceleration is limited directly to smooth the beginning of the curve.
    if (is_positive(accel_max)) {
        float delta_ang_vel = accel_max * dt;
        return constrain_float(desired_ang_vel, target_ang_vel-delta_ang_vel, target_ang_vel+delta_ang_vel);
    } else {
        return desired_ang_vel;
    }
}

// calculates the expected angular velocity correction from an angle error based on the AC_AttitudeControl settings.
// This function can be used to predict the delay associated with angle requests.
void AC_AttitudeControl::input_shaping_rate_predictor(Vector2f error_angle, Vector2f& target_ang_vel, float dt) const
{
    if (_rate_bf_ff_enabled) {
        // translate the roll pitch and yaw acceleration limits to the euler axis
        target_ang_vel.x = input_shaping_angle(wrap_PI(error_angle.x), _input_tc, get_accel_roll_max_radss(), target_ang_vel.x, dt);
        target_ang_vel.y = input_shaping_angle(wrap_PI(error_angle.y), _input_tc, get_accel_pitch_max_radss(), target_ang_vel.y, dt);
    } else {
        target_ang_vel.x =  _p_angle_roll.get_p(wrap_PI(error_angle.x));
        target_ang_vel.y =  _p_angle_pitch.get_p(wrap_PI(error_angle.y));
    }
    // Limit the angular velocity correction
    Vector3f ang_vel(target_ang_vel.x, target_ang_vel.y, 0.0f);
    ang_vel_limit(ang_vel, radians(_ang_vel_roll_max), radians(_ang_vel_pitch_max), 0.0f);

    target_ang_vel.x =  ang_vel.x;
    target_ang_vel.y =  ang_vel.y;
}

// limits angular velocity
void AC_AttitudeControl::ang_vel_limit(Vector3f& euler_rad, float ang_vel_roll_max, float ang_vel_pitch_max, float ang_vel_yaw_max) const
{
    if (is_zero(ang_vel_roll_max) || is_zero(ang_vel_pitch_max)) {
        if (!is_zero(ang_vel_roll_max)) {
            euler_rad.x = constrain_float(euler_rad.x, -ang_vel_roll_max, ang_vel_roll_max);
        }
        if (!is_zero(ang_vel_pitch_max)) {
            euler_rad.y = constrain_float(euler_rad.y, -ang_vel_pitch_max, ang_vel_pitch_max);
        }
    } else {
        Vector2f thrust_vector_ang_vel(euler_rad.x/ang_vel_roll_max, euler_rad.y/ang_vel_pitch_max);
        float thrust_vector_length = thrust_vector_ang_vel.length();
        if (thrust_vector_length > 1.0f) {
            euler_rad.x = thrust_vector_ang_vel.x * ang_vel_roll_max / thrust_vector_length;
            euler_rad.y = thrust_vector_ang_vel.y * ang_vel_pitch_max / thrust_vector_length;
        }
    }
    if (!is_zero(ang_vel_yaw_max)) {
        euler_rad.z = constrain_float(euler_rad.z, -ang_vel_yaw_max, ang_vel_yaw_max);
    }
}

// translates body frame acceleration limits to the euler axis
Vector3f AC_AttitudeControl::euler_accel_limit(Vector3f euler_rad, Vector3f euler_accel)
{
    float sin_phi = constrain_float(fabsf(sinf(euler_rad.x)), 0.1f, 1.0f);
    float cos_phi = constrain_float(fabsf(cosf(euler_rad.x)), 0.1f, 1.0f);
    float sin_theta = constrain_float(fabsf(sinf(euler_rad.y)), 0.1f, 1.0f);

    Vector3f rot_accel;
    if(is_zero(euler_accel.x) || is_zero(euler_accel.y) || is_zero(euler_accel.z) || is_negative(euler_accel.x) || is_negative(euler_accel.y) || is_negative(euler_accel.z)) {
        rot_accel.x = euler_accel.x;
        rot_accel.y = euler_accel.y;
        rot_accel.z = euler_accel.z;
    } else {
        rot_accel.x = euler_accel.x;
        rot_accel.y = MIN(euler_accel.y/cos_phi, euler_accel.z/sin_phi);
        rot_accel.z = MIN(MIN(euler_accel.x/sin_theta, euler_accel.y/sin_phi), euler_accel.z/cos_phi);
    }
    return rot_accel;
}

// Shifts earth frame yaw target by yaw_shift_cd. yaw_shift_cd should be in centidegrees and is added to the current target heading
void AC_AttitudeControl::shift_ef_yaw_target(float yaw_shift_cd)
{
    float yaw_shift = radians(yaw_shift_cd*0.01f);
    Quaternion _attitude_target_update_quat;
    _attitude_target_update_quat.from_axis_angle(Vector3f(0.0f, 0.0f, yaw_shift));
    _attitude_target_quat = _attitude_target_update_quat*_attitude_target_quat;
}

// Shifts earth frame yaw target by yaw_shift_cd. yaw_shift_cd should be in centidegrees and is added to the current target heading
void AC_AttitudeControl::inertial_frame_reset()
{
    // Retrieve quaternion vehicle attitude
    // TODO add _ahrs.get_quaternion()
    Quaternion attitude_vehicle_quat;
    attitude_vehicle_quat.from_rotation_matrix(_ahrs.get_rotation_body_to_ned());

    // Recalculate the target quaternion
    _attitude_target_quat = attitude_vehicle_quat * _attitude_ang_error;

    // calculate the attitude target euler angles
    _attitude_target_quat.to_euler(_attitude_target_euler_angle.x, _attitude_target_euler_angle.y, _attitude_target_euler_angle.z);
}

// Convert a 321-intrinsic euler angle derivative to an angular velocity vector
void AC_AttitudeControl::euler_rate_to_ang_vel(const Vector3f& euler_rad, const Vector3f& euler_rate_rads, Vector3f& ang_vel_rads)
{
    float sin_theta = sinf(euler_rad.y);
    float cos_theta = cosf(euler_rad.y);
    float sin_phi = sinf(euler_rad.x);
    float cos_phi = cosf(euler_rad.x);

    ang_vel_rads.x = euler_rate_rads.x - sin_theta * euler_rate_rads.z;
    ang_vel_rads.y = cos_phi  * euler_rate_rads.y + sin_phi * cos_theta * euler_rate_rads.z;
    ang_vel_rads.z = -sin_phi * euler_rate_rads.y + cos_theta * cos_phi * euler_rate_rads.z;
}

// Convert an angular velocity vector to a 321-intrinsic euler angle derivative
// Returns false if the vehicle is pitched 90 degrees up or down
bool AC_AttitudeControl::ang_vel_to_euler_rate(const Vector3f& euler_rad, const Vector3f& ang_vel_rads, Vector3f& euler_rate_rads)
{
    float sin_theta = sinf(euler_rad.y);
    float cos_theta = cosf(euler_rad.y);
    float sin_phi = sinf(euler_rad.x);
    float cos_phi = cosf(euler_rad.x);

    // When the vehicle pitches all the way up or all the way down, the euler angles become discontinuous. In this case, we just return false.
    if (is_zero(cos_theta)) {
        return false;
    }

    euler_rate_rads.x = ang_vel_rads.x + sin_phi * (sin_theta/cos_theta) * ang_vel_rads.y + cos_phi * (sin_theta/cos_theta) * ang_vel_rads.z;
    euler_rate_rads.y = cos_phi  * ang_vel_rads.y - sin_phi * ang_vel_rads.z;
    euler_rate_rads.z = (sin_phi / cos_theta) * ang_vel_rads.y + (cos_phi / cos_theta) * ang_vel_rads.z;
    return true;
}

// Update rate_target_ang_vel using attitude_error_rot_vec_rad
Vector3f AC_AttitudeControl::update_ang_vel_target_from_att_error(Vector3f attitude_error_rot_vec_rad)
{
    Vector3f rate_target_ang_vel;
    // Compute the roll angular velocity demand from the roll angle error
    if (_use_sqrt_controller) {
        rate_target_ang_vel.x = sqrt_controller(attitude_error_rot_vec_rad.x, _p_angle_roll.kP(), constrain_float(get_accel_roll_max_radss()/2.0f,  AC_ATTITUDE_ACCEL_RP_CONTROLLER_MIN_RADSS, AC_ATTITUDE_ACCEL_RP_CONTROLLER_MAX_RADSS), _dt);
    }else{
        rate_target_ang_vel.x = _p_angle_roll.kP() * attitude_error_rot_vec_rad.x;
    }
    // todo: Add Angular Velocity Limit

    // Compute the pitch angular velocity demand from the roll angle error
    if (_use_sqrt_controller) {
        rate_target_ang_vel.y = sqrt_controller(attitude_error_rot_vec_rad.y, _p_angle_pitch.kP(), constrain_float(get_accel_pitch_max_radss()/2.0f,  AC_ATTITUDE_ACCEL_RP_CONTROLLER_MIN_RADSS, AC_ATTITUDE_ACCEL_RP_CONTROLLER_MAX_RADSS), _dt);
    }else{
        rate_target_ang_vel.y = _p_angle_pitch.kP() * attitude_error_rot_vec_rad.y;
    }
    // todo: Add Angular Velocity Limit

    // Compute the yaw angular velocity demand from the roll angle error
    if (_use_sqrt_controller) {
        rate_target_ang_vel.z = sqrt_controller(attitude_error_rot_vec_rad.z, _p_angle_yaw.kP(), constrain_float(get_accel_yaw_max_radss()/2.0f,  AC_ATTITUDE_ACCEL_Y_CONTROLLER_MIN_RADSS, AC_ATTITUDE_ACCEL_Y_CONTROLLER_MAX_RADSS), _dt);
    }else{
        rate_target_ang_vel.z = _p_angle_yaw.kP() * attitude_error_rot_vec_rad.z;
    }
    // todo: Add Angular Velocity Limit
    return rate_target_ang_vel;
}

// Run the roll angular velocity PID controller and return the output
float AC_AttitudeControl::rate_target_to_motor_roll(float rate_actual_rads, float rate_target_rads)
{
    float rate_error_rads = rate_target_rads - rate_actual_rads;

    // pass error to PID controller
    get_rate_roll_pid().set_input_filter_d(rate_error_rads);
    get_rate_roll_pid().set_desired_rate(rate_target_rads);

    float integrator = get_rate_roll_pid().get_integrator();

    // Ensure that integrator can only be reduced if the output is saturated
    if (!_motors.limit.roll_pitch || ((is_positive(integrator) && is_negative(rate_error_rads)) || (is_negative(integrator) && is_positive(rate_error_rads)))) {
        integrator = get_rate_roll_pid().get_i();
    }

    // Compute output in range -1 ~ +1
    float output = get_rate_roll_pid().get_p() + integrator + get_rate_roll_pid().get_d() + get_rate_roll_pid().get_ff(rate_target_rads);

    // Constrain output
    return constrain_float(output, -1.0f, 1.0f);
}

// Run the pitch angular velocity PID controller and return the output
float AC_AttitudeControl::rate_target_to_motor_pitch(float rate_actual_rads, float rate_target_rads)
{
    float rate_error_rads = rate_target_rads - rate_actual_rads;

    // pass error to PID controller
    get_rate_pitch_pid().set_input_filter_d(rate_error_rads);
    get_rate_pitch_pid().set_desired_rate(rate_target_rads);

    float integrator = get_rate_pitch_pid().get_integrator();

    // Ensure that integrator can only be reduced if the output is saturated
    if (!_motors.limit.roll_pitch || ((is_positive(integrator) && is_negative(rate_error_rads)) || (is_negative(integrator) && is_positive(rate_error_rads)))) {
        integrator = get_rate_pitch_pid().get_i();
    }

    // Compute output in range -1 ~ +1
    float output = get_rate_pitch_pid().get_p() + integrator + get_rate_pitch_pid().get_d() + get_rate_pitch_pid().get_ff(rate_target_rads);

    // Constrain output
    return constrain_float(output, -1.0f, 1.0f);
}

// Run the yaw angular velocity PID controller and return the output
float AC_AttitudeControl::rate_target_to_motor_yaw(float rate_actual_rads, float rate_target_rads)
{
    float rate_error_rads = rate_target_rads - rate_actual_rads;

    // pass error to PID controller
    get_rate_yaw_pid().set_input_filter_all(rate_error_rads);
    get_rate_yaw_pid().set_desired_rate(rate_target_rads);

    float integrator = get_rate_yaw_pid().get_integrator();

    // Ensure that integrator can only be reduced if the output is saturated
    if (!_motors.limit.yaw || ((is_positive(integrator) && is_negative(rate_error_rads)) || (is_negative(integrator) && is_positive(rate_error_rads)))) {
        integrator = get_rate_yaw_pid().get_i();
    }

    // Compute output in range -1 ~ +1
    float output = get_rate_yaw_pid().get_p() + integrator + get_rate_yaw_pid().get_d() + get_rate_yaw_pid().get_ff(rate_target_rads);

    // Constrain output
    return constrain_float(output, -1.0f, 1.0f);
}

// Enable or disable body-frame feed forward
void AC_AttitudeControl::accel_limiting(bool enable_limits)
{
    if (enable_limits) {
        // If enabling limits, reload from eeprom or set to defaults
        if (is_zero(_accel_roll_max)) {
            _accel_roll_max.load();
        }
        if (is_zero(_accel_pitch_max)) {
            _accel_pitch_max.load();
        }
        if (is_zero(_accel_yaw_max)) {
            _accel_yaw_max.load();
        }
    } else {
        _accel_roll_max = 0.0f;
        _accel_pitch_max = 0.0f;
        _accel_yaw_max = 0.0f;
    }
}

// Return tilt angle limit for pilot input that prioritises altitude hold over lean angle
float AC_AttitudeControl::get_althold_lean_angle_max() const
{
    // convert to centi-degrees for public interface
    return ToDeg(_althold_lean_angle_max) * 100.0f;
}

// Proportional controller with piecewise sqrt sections to constrain second derivative
float AC_AttitudeControl::sqrt_controller(float error, float p, float second_ord_lim, float dt)
{
    float correction_rate;
    if (is_negative(second_ord_lim) || is_zero(second_ord_lim)) {
        // second order limit is zero or negative.
        correction_rate = error*p;
    } else if (is_zero(p)) {
        // P term is zero but we have a second order limit.
        if (is_positive(error)) {
            correction_rate = safe_sqrt(2.0f*second_ord_lim*(error));
        } else if (is_negative(error)) {
            correction_rate = -safe_sqrt(2.0f*second_ord_lim*(-error));
        } else {
            correction_rate = 0.0f;
        }
    } else {
        // Both the P and second order limit have been defined.
        float linear_dist = second_ord_lim/sq(p);
        if (error > linear_dist) {
            correction_rate = safe_sqrt(2.0f*second_ord_lim*(error-(linear_dist/2.0f)));
        } else if (error < -linear_dist) {
            correction_rate = -safe_sqrt(2.0f*second_ord_lim*(-error-(linear_dist/2.0f)));
        } else {
            correction_rate = error*p;
        }
    }
    if (!is_zero(dt)) {
        // this ensures we do not get small oscillations by over shooting the error correction in the last time step.
        return constrain_float(correction_rate, -fabsf(error)/dt, fabsf(error)/dt);
    } else {
        return correction_rate;
    }
}

// Inverse proportional controller with piecewise sqrt sections to constrain second derivative
float AC_AttitudeControl::stopping_point(float first_ord_mag, float p, float second_ord_lim)
{
    if (is_positive(second_ord_lim) && !is_zero(second_ord_lim) && is_zero(p)) {
        return (first_ord_mag*first_ord_mag)/(2.0f*second_ord_lim);
    } else if ((is_negative(second_ord_lim) || is_zero(second_ord_lim)) && !is_zero(p)) {
        return first_ord_mag/p;
    } else if ((is_negative(second_ord_lim) || is_zero(second_ord_lim)) && is_zero(p)) {
        return 0.0f;
    }

    // calculate the velocity at which we switch from calculating the stopping point using a linear function to a sqrt function
    float linear_velocity = second_ord_lim/p;

    if (fabsf(first_ord_mag) < linear_velocity) {
        // if our current velocity is below the cross-over point we use a linear function
        return first_ord_mag/p;
    } else {
        float linear_dist = second_ord_lim/sq(p);
        float overshoot = (linear_dist*0.5f) + sq(first_ord_mag)/(2.0f*second_ord_lim);
        if (is_positive(first_ord_mag)){
            return overshoot;
        } else {
            return -overshoot;
        }
    }
}

// Return roll rate step size in centidegrees/s that results in maximum output after 4 time steps
float AC_AttitudeControl::max_rate_step_bf_roll()
{
    float alpha = get_rate_roll_pid().get_filt_alpha();
    float alpha_remaining = 1-alpha;
    return 2.0f*_motors.get_throttle_hover()*AC_ATTITUDE_RATE_RP_CONTROLLER_OUT_MAX/((alpha_remaining*alpha_remaining*alpha_remaining*alpha*get_rate_roll_pid().kD())/_dt + get_rate_roll_pid().kP());
}

// Return pitch rate step size in centidegrees/s that results in maximum output after 4 time steps
float AC_AttitudeControl::max_rate_step_bf_pitch()
{
    float alpha = get_rate_pitch_pid().get_filt_alpha();
    float alpha_remaining = 1-alpha;
    return 2.0f*_motors.get_throttle_hover()*AC_ATTITUDE_RATE_RP_CONTROLLER_OUT_MAX/((alpha_remaining*alpha_remaining*alpha_remaining*alpha*get_rate_pitch_pid().kD())/_dt + get_rate_pitch_pid().kP());
}

// Return yaw rate step size in centidegrees/s that results in maximum output after 4 time steps
float AC_AttitudeControl::max_rate_step_bf_yaw()
{
    float alpha = get_rate_yaw_pid().get_filt_alpha();
    float alpha_remaining = 1-alpha;
    return 2.0f*_motors.get_throttle_hover()*AC_ATTITUDE_RATE_RP_CONTROLLER_OUT_MAX/((alpha_remaining*alpha_remaining*alpha_remaining*alpha*get_rate_yaw_pid().kD())/_dt + get_rate_yaw_pid().kP());
}
        with: #pragma once

/// @file    AC_AttitudeControl.h
/// @brief   ArduCopter attitude control library

#include <AP_Common/AP_Common.h>
#include <AP_Param/AP_Param.h>
#include <AP_Math/AP_Math.h>
#include <AP_InertialSensor/AP_InertialSensor.h>
#include <AP_AHRS/AP_AHRS_View.h>
#include <AP_Motors/AP_Motors.h>
#include <AC_PID/AC_PID.h>
#include <AC_PID/AC_P.h>

#define AC_ATTITUDE_CONTROL_ANGLE_P                     4.5f             // default angle P gain for roll, pitch and yaw

#define AC_ATTITUDE_ACCEL_RP_CONTROLLER_MIN_RADSS       radians(40.0f)   // minimum body-frame acceleration limit for the stability controller (for roll and pitch axis)
#define AC_ATTITUDE_ACCEL_RP_CONTROLLER_MAX_RADSS       radians(720.0f)  // maximum body-frame acceleration limit for the stability controller (for roll and pitch axis)
#define AC_ATTITUDE_ACCEL_Y_CONTROLLER_MIN_RADSS        radians(10.0f)   // minimum body-frame acceleration limit for the stability controller (for yaw axis)
#define AC_ATTITUDE_ACCEL_Y_CONTROLLER_MAX_RADSS        radians(120.0f)  // maximum body-frame acceleration limit for the stability controller (for yaw axis)
#define AC_ATTITUDE_CONTROL_SLEW_YAW_DEFAULT_CDS        6000      // constraint on yaw angle error in degrees.  This should lead to maximum turn rate of 10deg/sec * Stab Rate P so by default will be 45deg/sec.
#define AC_ATTITUDE_CONTROL_ACCEL_RP_MAX_DEFAULT_CDSS   110000.0f // default maximum acceleration for roll/pitch axis in centidegrees/sec/sec
#define AC_ATTITUDE_CONTROL_ACCEL_Y_MAX_DEFAULT_CDSS    27000.0f  // default maximum acceleration for yaw axis in centidegrees/sec/sec

#define AC_ATTITUDE_RATE_CONTROLLER_TIMEOUT             1.0f    // body-frame rate controller timeout in seconds
#define AC_ATTITUDE_RATE_RP_CONTROLLER_OUT_MAX          1.0f    // body-frame rate controller maximum output (for roll-pitch axis)
#define AC_ATTITUDE_RATE_YAW_CONTROLLER_OUT_MAX         1.0f    // body-frame rate controller maximum output (for yaw axis)

#define AC_ATTITUDE_THRUST_ERROR_ANGLE                  radians(30.0f) // Thrust angle error above which yaw corrections are limited

#define AC_ATTITUDE_400HZ_DT                            0.0025f // delta time in seconds for 400hz update rate

#define AC_ATTITUDE_CONTROL_RATE_BF_FF_DEFAULT          1       // body-frame rate feedforward enabled by default

#define AC_ATTITUDE_CONTROL_ANGLE_LIMIT_TC_DEFAULT      1.0f    // Time constant used to limit lean angle so that vehicle does not lose altitude
#define AC_ATTITUDE_CONTROL_ANGLE_LIMIT_THROTTLE_MAX    0.8f    // Max throttle used to limit lean angle so that vehicle does not lose altitude

#define AC_ATTITUDE_CONTROL_MIN_DEFAULT                 0.1f    // minimum throttle mix default
#define AC_ATTITUDE_CONTROL_MAN_DEFAULT                 0.5f    // manual throttle mix default
#define AC_ATTITUDE_CONTROL_MAX_DEFAULT                 0.5f    // maximum throttle mix default
#define AC_ATTITUDE_CONTROL_MAX                         5.0f    // maximum throttle mix default

#define AC_ATTITUDE_CONTROL_THR_MIX_DEFAULT             0.5f  // ratio controlling the max throttle output during competing requests of low throttle from the pilot (or autopilot) and higher throttle for attitude control.  Higher favours Attitude over pilot input

class AC_AttitudeControl {
public:
    AC_AttitudeControl( AP_AHRS_View &ahrs,
                        const AP_Vehicle::MultiCopter &aparm,
                        AP_Motors& motors,
                        float dt) :
        _p_angle_roll(AC_ATTITUDE_CONTROL_ANGLE_P),
        _p_angle_pitch(AC_ATTITUDE_CONTROL_ANGLE_P),
        _p_angle_yaw(AC_ATTITUDE_CONTROL_ANGLE_P),
        _dt(dt),
        _angle_boost(0),
        _use_sqrt_controller(true),
        _throttle_rpy_mix_desired(AC_ATTITUDE_CONTROL_THR_MIX_DEFAULT),
        _throttle_rpy_mix(AC_ATTITUDE_CONTROL_THR_MIX_DEFAULT),
        _ahrs(ahrs),
        _aparm(aparm),
        _motors(motors)
        {
            AP_Param::setup_object_defaults(this, var_info);
        }

    // Empty destructor to suppress compiler warning
    virtual ~AC_AttitudeControl() {}

    // pid accessors
    AC_P& get_angle_roll_p() { return _p_angle_roll; }
    AC_P& get_angle_pitch_p() { return _p_angle_pitch; }
    AC_P& get_angle_yaw_p() { return _p_angle_yaw; }
    virtual AC_PID& get_rate_roll_pid() = 0;
    virtual AC_PID& get_rate_pitch_pid() = 0;
    virtual AC_PID& get_rate_yaw_pid() = 0;

    // get the roll acceleration limit in centidegrees/s/s or radians/s/s
    float get_accel_roll_max() const { return _accel_roll_max; }
    float get_accel_roll_max_radss() const { return radians(_accel_roll_max*0.01f); }

    // Sets the roll acceleration limit in centidegrees/s/s
    void set_accel_roll_max(float accel_roll_max) { _accel_roll_max = accel_roll_max; }

    // Sets and saves the roll acceleration limit in centidegrees/s/s
    void save_accel_roll_max(float accel_roll_max) { _accel_roll_max.set_and_save(accel_roll_max); }

    // get the pitch acceleration limit in centidegrees/s/s or radians/s/s
    float get_accel_pitch_max() const { return _accel_pitch_max; }
    float get_accel_pitch_max_radss() const { return radians(_accel_pitch_max*0.01f); }

    // Sets the pitch acceleration limit in centidegrees/s/s
    void set_accel_pitch_max(float accel_pitch_max) { _accel_pitch_max = accel_pitch_max; }

    // Sets and saves the pitch acceleration limit in centidegrees/s/s
    void save_accel_pitch_max(float accel_pitch_max) { _accel_pitch_max.set_and_save(accel_pitch_max); }

    // get the yaw acceleration limit in centidegrees/s/s or radians/s/s
    float get_accel_yaw_max() const { return _accel_yaw_max; }
    float get_accel_yaw_max_radss() const { return radians(_accel_yaw_max*0.01f); }

    // Sets the yaw acceleration limit in centidegrees/s/s
    void set_accel_yaw_max(float accel_yaw_max) { _accel_yaw_max = accel_yaw_max; }

    // Sets and saves the yaw acceleration limit in centidegrees/s/s
    void save_accel_yaw_max(float accel_yaw_max) { _accel_yaw_max.set_and_save(accel_yaw_max); }

    // set the rate control input smoothing time constant
    void set_input_tc(float input_tc) { _input_tc = constrain_float(input_tc, 0.0f, 1.0f); }

    // Ensure attitude controller have zero errors to relax rate controller output
    void relax_attitude_controllers();

    // reset rate controller I terms
    void reset_rate_controller_I_terms();

    // Sets attitude target to vehicle attitude
    void set_attitude_target_to_current_attitude() { _attitude_target_quat.from_rotation_matrix(_ahrs.get_rotation_body_to_ned()); }

    // Sets yaw target to vehicle heading
    void set_yaw_target_to_current_heading() { shift_ef_yaw_target(degrees(_ahrs.yaw - _attitude_target_euler_angle.z)*100.0f); }

    // Shifts earth frame yaw target by yaw_shift_cd. yaw_shift_cd should be in centidegrees and is added to the current target heading
    void shift_ef_yaw_target(float yaw_shift_cd);

    // handle reset of attitude from EKF since the last iteration
    void inertial_frame_reset();

    // Command a Quaternion attitude with feedforward and smoothing
    void input_quaternion(Quaternion attitude_desired_quat);

    // Command an euler roll and pitch angle and an euler yaw rate with angular velocity feedforward and smoothing
    virtual void input_euler_angle_roll_pitch_euler_rate_yaw(float euler_roll_angle_cd, float euler_pitch_angle_cd, float euler_yaw_rate_cds);

    // Command an euler roll, pitch and yaw angle with angular velocity feedforward and smoothing
    virtual void input_euler_angle_roll_pitch_yaw(float euler_roll_angle_cd, float euler_pitch_angle_cd, float euler_yaw_angle_cd, bool slew_yaw);

    // Command an euler roll, pitch, and yaw rate with angular velocity feedforward and smoothing
    void input_euler_rate_roll_pitch_yaw(float euler_roll_rate_cds, float euler_pitch_rate_cds, float euler_yaw_rate_cds);

    // Command an angular velocity with angular velocity feedforward and smoothing
    virtual void input_rate_bf_roll_pitch_yaw(float roll_rate_bf_cds, float pitch_rate_bf_cds, float yaw_rate_bf_cds);

    // Command an angular velocity with angular velocity feedforward and smoothing
    void input_rate_bf_roll_pitch_yaw_2(float roll_rate_bf_cds, float pitch_rate_bf_cds, float yaw_rate_bf_cds);

    // Command an angular velocity with angular velocity smoothing using rate loops only with integrated rate error stabilization
    void input_rate_bf_roll_pitch_yaw_3(float roll_rate_bf_cds, float pitch_rate_bf_cds, float yaw_rate_bf_cds);

    // Command an angular step (i.e change) in body frame angle
    virtual void input_angle_step_bf_roll_pitch_yaw(float roll_angle_step_bf_cd, float pitch_angle_step_bf_cd, float yaw_angle_step_bf_cd);

    // Run angular velocity controller and send outputs to the motors
    virtual void rate_controller_run() = 0;

    // Convert a 321-intrinsic euler angle derivative to an angular velocity vector
    void euler_rate_to_ang_vel(const Vector3f& euler_rad, const Vector3f& euler_rate_rads, Vector3f& ang_vel_rads);

    // Convert an angular velocity vector to a 321-intrinsic euler angle derivative
    // Returns false if the vehicle is pitched 90 degrees up or down
    bool ang_vel_to_euler_rate(const Vector3f& euler_rad, const Vector3f& ang_vel_rads, Vector3f& euler_rate_rads);

    // Specifies whether the attitude controller should use the square root controller in the attitude correction.
    // This is used during Autotune to ensure the P term is tuned without being influenced by the acceleration limit of the square root controller.
    void use_sqrt_controller(bool use_sqrt_cont) { _use_sqrt_controller = use_sqrt_cont; }

    // Return 321-intrinsic euler angles in centidegrees representing the rotation from NED earth frame to the
    // attitude controller's target attitude.
    // **NOTE** Using vector3f*deg(100) is more efficient than deg(vector3f)*100 or deg(vector3d*100) because it gives the
    // same result with the fewest multiplications. Even though it may look like a bug, it is intentional. See issue 4895.
    Vector3f get_att_target_euler_cd() const { return _attitude_target_euler_angle*degrees(100.0f); }

    // Return the angle between the target thrust vector and the current thrust vector.
    float get_att_error_angle_deg() const { return degrees(_thrust_error_angle); }

    // Set x-axis angular velocity in centidegrees/s
    void rate_bf_roll_target(float rate_cds) { _rate_target_ang_vel.x = radians(rate_cds*0.01f); }

    // Set y-axis angular velocity in centidegrees/s
    void rate_bf_pitch_target(float rate_cds) { _rate_target_ang_vel.y = radians(rate_cds*0.01f); }

    // Set z-axis angular velocity in centidegrees/s
    void rate_bf_yaw_target(float rate_cds) { _rate_target_ang_vel.z = radians(rate_cds*0.01f); }

    // Return roll rate step size in radians/s that results in maximum output after 4 time steps
    float max_rate_step_bf_roll();

    // Return pitch rate step size in radians/s that results in maximum output after 4 time steps
    float max_rate_step_bf_pitch();

    // Return yaw rate step size in radians/s that results in maximum output after 4 time steps
    float max_rate_step_bf_yaw();

    // Return roll step size in radians that results in maximum output after 4 time steps
    float max_angle_step_bf_roll() { return max_rate_step_bf_roll()/_p_angle_roll.kP(); }

    // Return pitch step size in radians that results in maximum output after 4 time steps
    float max_angle_step_bf_pitch() { return max_rate_step_bf_pitch()/_p_angle_pitch.kP(); }

    // Return yaw step size in radians that results in maximum output after 4 time steps
    float max_angle_step_bf_yaw() { return max_rate_step_bf_yaw()/_p_angle_yaw.kP(); }

    // Return angular velocity in radians used in the angular velocity controller
    Vector3f rate_bf_targets() const { return _rate_target_ang_vel; }

    // Enable or disable body-frame feed forward
    void bf_feedforward(bool enable_or_disable) { _rate_bf_ff_enabled = enable_or_disable; }

    // Enable or disable body-frame feed forward and save
    void bf_feedforward_save(bool enable_or_disable) { _rate_bf_ff_enabled.set_and_save(enable_or_disable); }

    // Return body-frame feed forward setting
    bool get_bf_feedforward() { return _rate_bf_ff_enabled; }

    // Enable or disable body-frame feed forward
    void accel_limiting(bool enable_or_disable);

    // Update Alt_Hold angle maximum
    virtual void update_althold_lean_angle_max(float throttle_in) = 0;

    // Set output throttle
    virtual void set_throttle_out(float throttle_in, bool apply_angle_boost, float filt_cutoff) = 0;

    // Set output throttle and disable stabilization
    void set_throttle_out_unstabilized(float throttle_in, bool reset_attitude_control, float filt_cutoff);

    // get throttle passed into attitude controller (i.e. throttle_in provided to set_throttle_out)
    float get_throttle_in() const { return _throttle_in; }

    // Return throttle increase applied for tilt compensation
    float angle_boost() const { return _angle_boost; }

    // Return tilt angle limit for pilot input that prioritises altitude hold over lean angle
    float get_althold_lean_angle_max() const;

    // Return configured tilt angle limit in centidegrees
    float lean_angle_max() const { return _aparm.angle_max; }

    // Proportional controller with piecewise sqrt sections to constrain second derivative
    static float sqrt_controller(float error, float p, float second_ord_lim, float dt);

    // Inverse proportional controller with piecewise sqrt sections to constrain second derivative
    static float stopping_point(float first_ord_mag, float p, float second_ord_lim);

    // calculates the velocity correction from an angle error. The angular velocity has acceleration and
    // deceleration limits including basic jerk limiting using smoothing_gain
    static float input_shaping_angle(float error_angle, float smoothing_gain, float accel_max, float target_ang_vel, float dt);

    // limits the acceleration and deceleration of a velocity request
    static float input_shaping_ang_vel(float target_ang_vel, float desired_ang_vel, float accel_max, float dt);

    // calculates the expected angular velocity correction from an angle error based on the AC_AttitudeControl settings.
    // This function can be used to predict the delay associated with angle requests.
    void input_shaping_rate_predictor(Vector2f error_angle, Vector2f& target_ang_vel, float dt) const;

    // translates body frame acceleration limits to the euler axis
    void ang_vel_limit(Vector3f& euler_rad, float ang_vel_roll_max, float ang_vel_pitch_max, float ang_vel_yaw_max) const;

    // translates body frame acceleration limits to the euler axis
    Vector3f euler_accel_limit(Vector3f euler_rad, Vector3f euler_accel);

    // thrust_heading_rotation_angles - calculates two ordered rotations to move the att_from_quat quaternion to the att_to_quat quaternion.
    // The first rotation corrects the thrust vector and the second rotation corrects the heading vector.
    void thrust_heading_rotation_angles(Quaternion& att_to_quat, const Quaternion& att_from_quat, Vector3f& att_diff_angle, float& thrust_vec_dot);

    // Calculates the body frame angular velocities to follow the target attitude
    void attitude_controller_run_quat();

    // sanity check parameters.  should be called once before take-off
    virtual void parameter_sanity_check() {}

    // return true if the rpy mix is at lowest value
    virtual bool is_throttle_mix_min() const { return true; }

    // control rpy throttle mix
    virtual void set_throttle_mix_min() {}
    virtual void set_throttle_mix_man() {}
    virtual void set_throttle_mix_max() {}
    virtual void set_throttle_mix_value(float value) {}
    virtual float get_throttle_mix(void) const { return 0; }

    // enable use of flybass passthrough on heli
    virtual void use_flybar_passthrough(bool passthrough, bool tail_passthrough) {}

        // use_leaky_i - controls whether we use leaky i term for body-frame to motor output stage on heli
        virtual void use_leaky_i(bool leaky_i) {}

    // set_hover_roll_scalar - scales Hover Roll Trim parameter. To be used by vehicle code according to vehicle condition.
    virtual void set_hover_roll_trim_scalar(float scalar) {}

    // passthrough_bf_roll_pitch_rate_yaw - roll and pitch are passed through directly, body-frame rate target for yaw
    virtual void passthrough_bf_roll_pitch_rate_yaw(float roll_passthrough, float pitch_passthrough, float yaw_rate_bf_cds) {};

    // enable inverted flight on backends that support it
    virtual void set_inverted_flight(bool inverted) {}

    // User settable parameters
    static const struct AP_Param::GroupInfo var_info[];

protected:

    // Update rate_target_ang_vel using attitude_error_rot_vec_rad
    Vector3f update_ang_vel_target_from_att_error(Vector3f attitude_error_rot_vec_rad);

    // Run the roll angular velocity PID controller and return the output
    float rate_target_to_motor_roll(float rate_actual_rads, float rate_target_rads);

    // Run the pitch angular velocity PID controller and return the output
    float rate_target_to_motor_pitch(float rate_actual_rads, float rate_target_rads);

    // Run the yaw angular velocity PID controller and return the output
    virtual float rate_target_to_motor_yaw(float rate_actual_rads, float rate_target_rads);

    // Return angle in radians to be added to roll angle. Used by heli to counteract
    // tail rotor thrust in hover. Overloaded by AC_Attitude_Heli to return angle.
    virtual float get_roll_trim_rad() { return 0;}

    // Return the yaw slew rate limit in radians/s
    float get_slew_yaw_rads() { return radians(_slew_yaw*0.01f); }

    // Maximum rate the yaw target can be updated in Loiter, RTL, Auto flight modes
    AP_Float            _slew_yaw;

    // Maximum angular velocity (in degrees/second) for earth-frame roll, pitch and yaw axis
    AP_Float            _ang_vel_roll_max;
    AP_Float            _ang_vel_pitch_max;
    AP_Float            _ang_vel_yaw_max;

    // Maximum rotation acceleration for earth-frame roll axis
    AP_Float            _accel_roll_max;

    // Maximum rotation acceleration for earth-frame pitch axis
    AP_Float            _accel_pitch_max;

    // Maximum rotation acceleration for earth-frame yaw axis
    AP_Float            _accel_yaw_max;

    // Enable/Disable body frame rate feed forward
    AP_Int8             _rate_bf_ff_enabled;

    // Enable/Disable angle boost
    AP_Int8             _angle_boost_enabled;

    // angle controller P objects
    AC_P                _p_angle_roll;
    AC_P                _p_angle_pitch;
    AC_P                _p_angle_yaw;

    // Angle limit time constant (to maintain altitude)
    AP_Float            _angle_limit_tc;

    // rate controller input smoothing time constant
    AP_Float            _input_tc;

    // Intersampling period in seconds
    float               _dt;

    // This represents a 321-intrinsic rotation in NED frame to the target (setpoint)
    // attitude used in the attitude controller, in radians.
    Vector3f            _attitude_target_euler_angle;

    // This represents the angular velocity of the target (setpoint) attitude used in
    // the attitude controller as 321-intrinsic euler angle derivatives, in radians per
    // second.
    Vector3f            _attitude_target_euler_rate;

    // This represents a quaternion rotation in NED frame to the target (setpoint)
    // attitude used in the attitude controller.
    Quaternion          _attitude_target_quat;

    // This represents the angular velocity of the target (setpoint) attitude used in
    // the attitude controller as an angular velocity vector, in radians per second in
    // the target attitude frame.
    Vector3f            _attitude_target_ang_vel;

    // This represents the angular velocity in radians per second in the body frame, used in the angular
    // velocity controller.
    Vector3f            _rate_target_ang_vel;

    // This represents a quaternion attitude error in the body frame, used for inertial frame reset handling.
    Quaternion          _attitude_ang_error;

    // The angle between the target thrust vector and the current thrust vector.
    float               _thrust_error_angle;

    // throttle provided as input to attitude controller.  This does not include angle boost.
    float               _throttle_in = 0.0f;

    // This represents the throttle increase applied for tilt compensation.
    // Used only for logging.
    float               _angle_boost;

    // Specifies whether the attitude controller should use the square root controller in the attitude correction.
    // This is used during Autotune to ensure the P term is tuned without being influenced by the acceleration limit of the square root controller.
    bool                _use_sqrt_controller;

    // Filtered Alt_Hold lean angle max - used to limit lean angle when throttle is saturated using Alt_Hold
    float               _althold_lean_angle_max = 0.0f;

    // desired throttle_low_comp value, actual throttle_low_comp is slewed towards this value over 1~2 seconds
    float               _throttle_rpy_mix_desired;

    // mix between throttle and hover throttle for 0 to 1 and ratio above hover throttle for >1
    float               _throttle_rpy_mix;

    // References to external libraries
    const AP_AHRS_View&  _ahrs;
    const AP_Vehicle::MultiCopter &_aparm;
    AP_Motors&          _motors;

protected:
    /*
      state of control monitoring
    */
    struct {
        float rms_roll_P;
        float rms_roll_D;
        float rms_pitch_P;
        float rms_pitch_D;
        float rms_yaw;
    } _control_monitor;

    // update state in ControlMonitor
    void control_monitor_filter_pid(float value, float &rms_P);
    void control_monitor_update(void);

    // true in inverted flight mode
    bool _inverted_flight;

public:
    // log a CTRL message
    void control_monitor_log(void);

    // return current RMS controller filter for each axis
    float control_monitor_rms_output_roll(void) const;
    float control_monitor_rms_output_roll_P(void) const;
    float control_monitor_rms_output_roll_D(void) const;
    float control_monitor_rms_output_pitch_P(void) const;
    float control_monitor_rms_output_pitch_D(void) const;
    float control_monitor_rms_output_pitch(void) const;
    float control_monitor_rms_output_yaw(void) const;
};

#define AC_ATTITUDE_CONTROL_LOG_FORMAT(msg) { msg, sizeof(AC_AttitudeControl::log_Attitude),        \
                            "ATT", "cccccCC",      "RollIn,Roll,PitchIn,Pitch,YawIn,Yaw,NavYaw" }
          cmakeBuildDirectory: #include "AC_AttitudeControl_Heli.h"
#include <AP_HAL/AP_HAL.h>

// table of user settable parameters
const AP_Param::GroupInfo AC_AttitudeControl_Heli::var_info[] = {
    // parameters from parent vehicle
    AP_NESTEDGROUPINFO(AC_AttitudeControl, 0),

    // @Param: HOVR_ROL_TRM
    // @DisplayName: Hover Roll Trim
    // @Description: Trim the hover roll angle to counter tail rotor thrust in a hover
    // @Units: cdeg
    // @Range: 0 1000
    // @User: Advanced
    AP_GROUPINFO("HOVR_ROL_TRM",    1, AC_AttitudeControl_Heli, _hover_roll_trim, AC_ATTITUDE_HELI_HOVER_ROLL_TRIM_DEFAULT),

    // @Param: RAT_RLL_P
    // @DisplayName: Roll axis rate controller P gain
    // @Description: Roll axis rate controller P gain.  Converts the difference between desired roll rate and actual roll rate into a motor speed output
    // @Range: 0.08 0.35
    // @Increment: 0.005
    // @User: Standard

    // @Param: RAT_RLL_I
    // @DisplayName: Roll axis rate controller I gain
    // @Description: Roll axis rate controller I gain.  Corrects long-term difference in desired roll rate vs actual roll rate
    // @Range: 0.01 0.6
    // @Increment: 0.01
    // @User: Standard

    // @Param: RAT_RLL_IMAX
    // @DisplayName: Roll axis rate controller I gain maximum
    // @Description: Roll axis rate controller I gain maximum.  Constrains the maximum motor output that the I gain will output
    // @Range: 0 1
    // @Increment: 0.01
    // @User: Standard

    // @Param: RAT_RLL_D
    // @DisplayName: Roll axis rate controller D gain
    // @Description: Roll axis rate controller D gain.  Compensates for short-term change in desired roll rate vs actual roll rate
    // @Range: 0.001 0.03
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_RLL_FF
    // @DisplayName: Roll axis rate controller feed forward
    // @Description: Roll axis rate controller feed forward
    // @Range: 0 0.5
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_RLL_FILT
    // @DisplayName: Roll axis rate controller input frequency in Hz
    // @Description: Roll axis rate controller input frequency in Hz
    // @Units: Hz
    // @Range: 1 20
    // @Increment: 1
    AP_SUBGROUPINFO(_pid_rate_roll, "RAT_RLL_", 2, AC_AttitudeControl_Heli, AC_HELI_PID),

    // @Param: RAT_PIT_P
    // @DisplayName: Pitch axis rate controller P gain
    // @Description: Pitch axis rate controller P gain.  Converts the difference between desired pitch rate and actual pitch rate into a motor speed output
    // @Range: 0.08 0.35
    // @Increment: 0.005
    // @User: Standard

    // @Param: RAT_PIT_I
    // @DisplayName: Pitch axis rate controller I gain
    // @Description: Pitch axis rate controller I gain.  Corrects long-term difference in desired pitch rate vs actual pitch rate
    // @Range: 0.01 0.6
    // @Increment: 0.01
    // @User: Standard

    // @Param: RAT_PIT_IMAX
    // @DisplayName: Pitch axis rate controller I gain maximum
    // @Description: Pitch axis rate controller I gain maximum.  Constrains the maximum motor output that the I gain will output
    // @Range: 0 1
    // @Increment: 0.01
    // @User: Standard

    // @Param: RAT_PIT_D
    // @DisplayName: Pitch axis rate controller D gain
    // @Description: Pitch axis rate controller D gain.  Compensates for short-term change in desired pitch rate vs actual pitch rate
    // @Range: 0.001 0.03
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_PIT_FF
    // @DisplayName: Pitch axis rate controller feed forward
    // @Description: Pitch axis rate controller feed forward
    // @Range: 0 0.5
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_PIT_FILT
    // @DisplayName: Pitch axis rate controller input frequency in Hz
    // @Description: Pitch axis rate controller input frequency in Hz
    // @Units: Hz
    // @Range: 1 20
    // @Increment: 1
    AP_SUBGROUPINFO(_pid_rate_pitch, "RAT_PIT_", 3, AC_AttitudeControl_Heli, AC_HELI_PID),

    // @Param: RAT_YAW_P
    // @DisplayName: Yaw axis rate controller P gain
    // @Description: Yaw axis rate controller P gain.  Converts the difference between desired yaw rate and actual yaw rate into a motor speed output
    // @Range: 0.180 0.60
    // @Increment: 0.005
    // @User: Standard

    // @Param: RAT_YAW_I
    // @DisplayName: Yaw axis rate controller I gain
    // @Description: Yaw axis rate controller I gain.  Corrects long-term difference in desired yaw rate vs actual yaw rate
    // @Range: 0.01 0.06
    // @Increment: 0.01
    // @User: Standard

    // @Param: RAT_YAW_IMAX
    // @DisplayName: Yaw axis rate controller I gain maximum
    // @Description: Yaw axis rate controller I gain maximum.  Constrains the maximum motor output that the I gain will output
    // @Range: 0 1
    // @Increment: 0.01
    // @User: Standard

    // @Param: RAT_YAW_D
    // @DisplayName: Yaw axis rate controller D gain
    // @Description: Yaw axis rate controller D gain.  Compensates for short-term change in desired yaw rate vs actual yaw rate
    // @Range: 0.000 0.02
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_YAW_FF
    // @DisplayName: Yaw axis rate controller feed forward
    // @Description: Yaw axis rate controller feed forward
    // @Range: 0 0.5
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_YAW_FILT
    // @DisplayName: Yaw axis rate controller input frequency in Hz
    // @Description: Yaw axis rate controller input frequency in Hz
    // @Units: Hz
    // @Range: 1 20
    // @Increment: 1
    AP_SUBGROUPINFO(_pid_rate_yaw, "RAT_YAW_", 4, AC_AttitudeControl_Heli, AC_HELI_PID),

    // @Param: PIRO_COMP
    // @DisplayName: Piro Comp Enable
    // @Description: Pirouette compensation enabled
    // @Values: 0:Disabled,1:Enabled
    // @User: Advanced
    AP_GROUPINFO("PIRO_COMP",    5, AC_AttitudeControl_Heli, _piro_comp_enabled, 0),

    AP_GROUPEND
};

// passthrough_bf_roll_pitch_rate_yaw - passthrough the pilots roll and pitch inputs directly to swashplate for flybar acro mode
void AC_AttitudeControl_Heli::passthrough_bf_roll_pitch_rate_yaw(float roll_passthrough, float pitch_passthrough, float yaw_rate_bf_cds)
{
    // convert from centidegrees on public interface to radians
    float yaw_rate_bf_rads = radians(yaw_rate_bf_cds*0.01f);

    // store roll, pitch and passthroughs
    // NOTE: this abuses yaw_rate_bf_rads
    _passthrough_roll = roll_passthrough;
    _passthrough_pitch = pitch_passthrough;
    _passthrough_yaw = degrees(yaw_rate_bf_rads)*100.0f;

    // set rate controller to use pass through
    _flags_heli.flybar_passthrough = true;

    // set bf rate targets to current body frame rates (i.e. relax and be ready for vehicle to switch out of acro)
    _attitude_target_ang_vel.x = _ahrs.get_gyro().x;
    _attitude_target_ang_vel.y = _ahrs.get_gyro().y;

    // accel limit desired yaw rate
    if (get_accel_yaw_max_radss() > 0.0f) {
        float rate_change_limit_rads = get_accel_yaw_max_radss() * _dt;
        float rate_change_rads = yaw_rate_bf_rads - _attitude_target_ang_vel.z;
        rate_change_rads = constrain_float(rate_change_rads, -rate_change_limit_rads, rate_change_limit_rads);
        _attitude_target_ang_vel.z += rate_change_rads;
    } else {
        _attitude_target_ang_vel.z = yaw_rate_bf_rads;
    }

    integrate_bf_rate_error_to_angle_errors();
    _att_error_rot_vec_rad.x = 0;
    _att_error_rot_vec_rad.y = 0;

    // update our earth-frame angle targets
    Vector3f att_error_euler_rad;

    // convert angle error rotation vector into 321-intrinsic euler angle difference
    // NOTE: this results an an approximation linearized about the vehicle's attitude
    if (ang_vel_to_euler_rate(Vector3f(_ahrs.roll,_ahrs.pitch,_ahrs.yaw), _att_error_rot_vec_rad, att_error_euler_rad)) {
        _attitude_target_euler_angle.x = wrap_PI(att_error_euler_rad.x + _ahrs.roll);
        _attitude_target_euler_angle.y = wrap_PI(att_error_euler_rad.y + _ahrs.pitch);
        _attitude_target_euler_angle.z = wrap_2PI(att_error_euler_rad.z + _ahrs.yaw);
    }

    // handle flipping over pitch axis
    if (_attitude_target_euler_angle.y > M_PI/2.0f) {
        _attitude_target_euler_angle.x = wrap_PI(_attitude_target_euler_angle.x + M_PI);
        _attitude_target_euler_angle.y = wrap_PI(M_PI - _attitude_target_euler_angle.x);
        _attitude_target_euler_angle.z = wrap_2PI(_attitude_target_euler_angle.z + M_PI);
    }
    if (_attitude_target_euler_angle.y < -M_PI/2.0f) {
        _attitude_target_euler_angle.x = wrap_PI(_attitude_target_euler_angle.x + M_PI);
        _attitude_target_euler_angle.y = wrap_PI(-M_PI - _attitude_target_euler_angle.x);
        _attitude_target_euler_angle.z = wrap_2PI(_attitude_target_euler_angle.z + M_PI);
    }

    // convert body-frame angle errors to body-frame rate targets
    _rate_target_ang_vel = update_ang_vel_target_from_att_error(_att_error_rot_vec_rad);

    // set body-frame roll/pitch rate target to current desired rates which are the vehicle's actual rates
    _rate_target_ang_vel.x = _attitude_target_ang_vel.x;
    _rate_target_ang_vel.y = _attitude_target_ang_vel.y;

    // add desired target to yaw
    _rate_target_ang_vel.z += _attitude_target_ang_vel.z;
    _thrust_error_angle = norm(_att_error_rot_vec_rad.x, _att_error_rot_vec_rad.y);
}

void AC_AttitudeControl_Heli::integrate_bf_rate_error_to_angle_errors()
{
    // Integrate the angular velocity error into the attitude error
    _att_error_rot_vec_rad += (_attitude_target_ang_vel - _ahrs.get_gyro()) * _dt;

    // Constrain attitude error
    _att_error_rot_vec_rad.x = constrain_float(_att_error_rot_vec_rad.x, -AC_ATTITUDE_HELI_ACRO_OVERSHOOT_ANGLE_RAD, AC_ATTITUDE_HELI_ACRO_OVERSHOOT_ANGLE_RAD);
    _att_error_rot_vec_rad.y = constrain_float(_att_error_rot_vec_rad.y, -AC_ATTITUDE_HELI_ACRO_OVERSHOOT_ANGLE_RAD, AC_ATTITUDE_HELI_ACRO_OVERSHOOT_ANGLE_RAD);
    _att_error_rot_vec_rad.z = constrain_float(_att_error_rot_vec_rad.z, -AC_ATTITUDE_HELI_ACRO_OVERSHOOT_ANGLE_RAD, AC_ATTITUDE_HELI_ACRO_OVERSHOOT_ANGLE_RAD);
}

// subclass non-passthrough too, for external gyro, no flybar
void AC_AttitudeControl_Heli::input_rate_bf_roll_pitch_yaw(float roll_rate_bf_cds, float pitch_rate_bf_cds, float yaw_rate_bf_cds)
{
    _passthrough_yaw = yaw_rate_bf_cds;

    AC_AttitudeControl::input_rate_bf_roll_pitch_yaw(roll_rate_bf_cds, pitch_rate_bf_cds, yaw_rate_bf_cds);
}

//
// rate controller (body-frame) methods
//

// rate_controller_run - run lowest level rate controller and send outputs to the motors
// should be called at 100hz or more
void AC_AttitudeControl_Heli::rate_controller_run()
{        
    Vector3f gyro_latest = _ahrs.get_gyro_latest();

    // call rate controllers and send output to motors object
    // if using a flybar passthrough roll and pitch directly to motors
    if (_flags_heli.flybar_passthrough) {
        _motors.set_roll(_passthrough_roll/4500.0f);
        _motors.set_pitch(_passthrough_pitch/4500.0f);
    } else {
        rate_bf_to_motor_roll_pitch(gyro_latest, _rate_target_ang_vel.x, _rate_target_ang_vel.y);
    }
    if (_flags_heli.tail_passthrough) {
        _motors.set_yaw(_passthrough_yaw/4500.0f);
    } else {
        _motors.set_yaw(rate_target_to_motor_yaw(gyro_latest.z, _rate_target_ang_vel.z));
    }
}

// Update Alt_Hold angle maximum
void AC_AttitudeControl_Heli::update_althold_lean_angle_max(float throttle_in)
{
    float althold_lean_angle_max = acos(constrain_float(_throttle_in/AC_ATTITUDE_HELI_ANGLE_LIMIT_THROTTLE_MAX, 0.0f, 1.0f));
    _althold_lean_angle_max = _althold_lean_angle_max + (_dt/(_dt+_angle_limit_tc))*(althold_lean_angle_max-_althold_lean_angle_max);
}

//
// private methods
//

//
// body-frame rate controller
//

// rate_bf_to_motor_roll_pitch - ask the rate controller to calculate the motor outputs to achieve the target rate in radians/second
void AC_AttitudeControl_Heli::rate_bf_to_motor_roll_pitch(const Vector3f &rate_rads, float rate_roll_target_rads, float rate_pitch_target_rads)
{
    float roll_pd, roll_i, roll_ff;             // used to capture pid values
    float pitch_pd, pitch_i, pitch_ff;          // used to capture pid values
    float rate_roll_error_rads, rate_pitch_error_rads;    // simply target_rate - current_rate
    float roll_out, pitch_out;

    // calculate error
    rate_roll_error_rads = rate_roll_target_rads - rate_rads.x;
    rate_pitch_error_rads = rate_pitch_target_rads - rate_rads.y;

    // pass error to PID controller
    _pid_rate_roll.set_input_filter_all(rate_roll_error_rads);
    _pid_rate_roll.set_desired_rate(rate_roll_target_rads);
    _pid_rate_pitch.set_input_filter_all(rate_pitch_error_rads);
    _pid_rate_pitch.set_desired_rate(rate_pitch_target_rads);

    // call p and d controllers
    roll_pd = _pid_rate_roll.get_p() + _pid_rate_roll.get_d();
    pitch_pd = _pid_rate_pitch.get_p() + _pid_rate_pitch.get_d();

    // get roll i term
    roll_i = _pid_rate_roll.get_integrator();

    // update i term as long as we haven't breached the limits or the I term will certainly reduce
    if (!_flags_heli.limit_roll || ((roll_i>0&&rate_roll_error_rads<0)||(roll_i<0&&rate_roll_error_rads>0))){
                if (_flags_heli.leaky_i){
                        roll_i = _pid_rate_roll.get_leaky_i(AC_ATTITUDE_HELI_RATE_INTEGRATOR_LEAK_RATE);
                }else{
                        roll_i = _pid_rate_roll.get_i();
                }
    }

    // get pitch i term
    pitch_i = _pid_rate_pitch.get_integrator();

    // update i term as long as we haven't breached the limits or the I term will certainly reduce
    if (!_flags_heli.limit_pitch || ((pitch_i>0&&rate_pitch_error_rads<0)||(pitch_i<0&&rate_pitch_error_rads>0))){
                if (_flags_heli.leaky_i) {
                        pitch_i = _pid_rate_pitch.get_leaky_i(AC_ATTITUDE_HELI_RATE_INTEGRATOR_LEAK_RATE);
                }else{
                        pitch_i = _pid_rate_pitch.get_i();
                }
    }

    // For legacy reasons, we convert to centi-degrees before inputting to the feedforward
    roll_ff = roll_feedforward_filter.apply(_pid_rate_roll.get_ff(rate_roll_target_rads), _dt);
    pitch_ff = pitch_feedforward_filter.apply(_pid_rate_pitch.get_ff(rate_pitch_target_rads), _dt);

    // add feed forward and final output
    roll_out = roll_pd + roll_i + roll_ff;
    pitch_out = pitch_pd + pitch_i + pitch_ff;

    // constrain output and update limit flags
    if (fabsf(roll_out) > AC_ATTITUDE_RATE_RP_CONTROLLER_OUT_MAX) {
        roll_out = constrain_float(roll_out,-AC_ATTITUDE_RATE_RP_CONTROLLER_OUT_MAX,AC_ATTITUDE_RATE_RP_CONTROLLER_OUT_MAX);
        _flags_heli.limit_roll = true;
    }else{
        _flags_heli.limit_roll = false;
    }
    if (fabsf(pitch_out) > AC_ATTITUDE_RATE_RP_CONTROLLER_OUT_MAX) {
        pitch_out = constrain_float(pitch_out,-AC_ATTITUDE_RATE_RP_CONTROLLER_OUT_MAX,AC_ATTITUDE_RATE_RP_CONTROLLER_OUT_MAX);
        _flags_heli.limit_pitch = true;
    }else{
        _flags_heli.limit_pitch = false;
    }

    // output to motors
    _motors.set_roll(roll_out);
    _motors.set_pitch(pitch_out);

    // Piro-Comp, or Pirouette Compensation is a pre-compensation calculation, which basically rotates the Roll and Pitch Rate I-terms as the
    // helicopter rotates in yaw.  Much of the built-up I-term is needed to tip the disk into the incoming wind.  Fast yawing can create an instability
    // as the built-up I-term in one axis must be reduced, while the other increases.  This helps solve that by rotating the I-terms before the error occurs.
    // It does assume that the rotor aerodynamics and mechanics are essentially symmetrical about the main shaft, which is a generally valid assumption. 
    if (_piro_comp_enabled){

        int32_t         piro_roll_i, piro_pitch_i;            // used to hold I-terms while doing piro comp

        piro_roll_i  = roll_i;
        piro_pitch_i = pitch_i;

        Vector2f yawratevector;
        yawratevector.x     = cosf(-rate_rads.z * _dt);
        yawratevector.y     = sinf(-rate_rads.z * _dt);
        yawratevector.normalize();

        roll_i      = piro_roll_i * yawratevector.x - piro_pitch_i * yawratevector.y;
        pitch_i     = piro_pitch_i * yawratevector.x + piro_roll_i * yawratevector.y;

        _pid_rate_pitch.set_integrator(pitch_i);
        _pid_rate_roll.set_integrator(roll_i);
    }

}

// rate_bf_to_motor_yaw - ask the rate controller to calculate the motor outputs to achieve the target rate in radians/second
float AC_AttitudeControl_Heli::rate_target_to_motor_yaw(float rate_yaw_actual_rads, float rate_target_rads)
{
    float pd,i,vff;     // used to capture pid values for logging
    float rate_error_rads;       // simply target_rate - current_rate
    float yaw_out;

    // calculate error and call pid controller
    rate_error_rads  = rate_target_rads - rate_yaw_actual_rads;

    // pass error to PID controller
    _pid_rate_yaw.set_input_filter_all(rate_error_rads);
    _pid_rate_yaw.set_desired_rate(rate_target_rads);

    // get p and d
    pd = _pid_rate_yaw.get_p() + _pid_rate_yaw.get_d();

    // get i term
    i = _pid_rate_yaw.get_integrator();

    // update i term as long as we haven't breached the limits or the I term will certainly reduce
    if (!_flags_heli.limit_yaw || ((i>0&&rate_error_rads<0)||(i<0&&rate_error_rads>0))) {
        if (((AP_MotorsHeli&)_motors).rotor_runup_complete()) {
            i = _pid_rate_yaw.get_i();
        } else {
            i = ((AC_HELI_PID&)_pid_rate_yaw).get_leaky_i(AC_ATTITUDE_HELI_RATE_INTEGRATOR_LEAK_RATE);    // If motor is not running use leaky I-term to avoid excessive build-up
        }
    }

    // For legacy reasons, we convert to centi-degrees before inputting to the feedforward
    vff = yaw_velocity_feedforward_filter.apply(_pid_rate_yaw.get_ff(rate_target_rads), _dt);

    // add feed forward
    yaw_out = pd + i + vff;

    // constrain output and update limit flag
    if (fabsf(yaw_out) > AC_ATTITUDE_RATE_YAW_CONTROLLER_OUT_MAX) {
        yaw_out = constrain_float(yaw_out,-AC_ATTITUDE_RATE_YAW_CONTROLLER_OUT_MAX,AC_ATTITUDE_RATE_YAW_CONTROLLER_OUT_MAX);
        _flags_heli.limit_yaw = true;
    }else{
        _flags_heli.limit_yaw = false;
    }

    // output to motors
    return yaw_out;
}

//
// throttle functions
//

void AC_AttitudeControl_Heli::set_throttle_out(float throttle_in, bool apply_angle_boost, float filter_cutoff)
{
    _throttle_in = throttle_in;
    update_althold_lean_angle_max(throttle_in);
    _motors.set_throttle_filter_cutoff(filter_cutoff);
    _motors.set_throttle(throttle_in);
    // Clear angle_boost for logging purposes
    _angle_boost = 0.0f;
}

// Command an euler roll and pitch angle and an euler yaw rate with angular velocity feedforward and smoothing
void AC_AttitudeControl_Heli::input_euler_angle_roll_pitch_euler_rate_yaw(float euler_roll_angle_cd, float euler_pitch_angle_cd, float euler_yaw_rate_cds)
{
    if (_inverted_flight) {
        euler_roll_angle_cd = wrap_180_cd(euler_roll_angle_cd + 18000);
    }
    AC_AttitudeControl::input_euler_angle_roll_pitch_euler_rate_yaw(euler_roll_angle_cd, euler_pitch_angle_cd, euler_yaw_rate_cds);
}

// Command an euler roll, pitch and yaw angle with angular velocity feedforward and smoothing
void AC_AttitudeControl_Heli::input_euler_angle_roll_pitch_yaw(float euler_roll_angle_cd, float euler_pitch_angle_cd, float euler_yaw_angle_cd, bool slew_yaw)
{
    if (_inverted_flight) {
        euler_roll_angle_cd = wrap_180_cd(euler_roll_angle_cd + 18000);
    }
    AC_AttitudeControl::input_euler_angle_roll_pitch_yaw(euler_roll_angle_cd, euler_pitch_angle_cd, euler_yaw_angle_cd, slew_yaw);
}
          # Ruleset file that will determine what checks will be run
          ruleset: #pragma once

/// @file    AC_AttitudeControl_Heli.h
/// @brief   ArduCopter attitude control library for traditional helicopters

#include "AC_AttitudeControl.h"
#include <AP_Motors/AP_MotorsHeli.h>
#include <AC_PID/AC_HELI_PID.h>
#include <Filter/Filter.h>

// default rate controller PID gains
#define AC_ATC_HELI_RATE_RP_P                       0.024f
#define AC_ATC_HELI_RATE_RP_I                       0.6f
#define AC_ATC_HELI_RATE_RP_D                       0.001f
#define AC_ATC_HELI_RATE_RP_IMAX                    1.0f
#define AC_ATC_HELI_RATE_RP_FF                      0.060f
#define AC_ATC_HELI_RATE_RP_FILT_HZ                 20.0f
#define AC_ATC_HELI_RATE_YAW_P                      0.18f
#define AC_ATC_HELI_RATE_YAW_I                      0.12f
#define AC_ATC_HELI_RATE_YAW_D                      0.003f
#define AC_ATC_HELI_RATE_YAW_IMAX                   1.0f
#define AC_ATC_HELI_RATE_YAW_FF                     0.024f
#define AC_ATC_HELI_RATE_YAW_FILT_HZ                20.0f

#define AC_ATTITUDE_HELI_ANGLE_LIMIT_THROTTLE_MAX   0.95f    // Heli's use 95% of max collective before limiting frame angle
#define AC_ATTITUDE_HELI_RATE_INTEGRATOR_LEAK_RATE  0.02f
#define AC_ATTITUDE_HELI_RATE_RP_FF_FILTER          10.0f
#define AC_ATTITUDE_HELI_RATE_Y_VFF_FILTER          10.0f
#define AC_ATTITUDE_HELI_HOVER_ROLL_TRIM_DEFAULT    300
#define AC_ATTITUDE_HELI_ACRO_OVERSHOOT_ANGLE_RAD   ToRad(30.0f)

class AC_AttitudeControl_Heli : public AC_AttitudeControl {
public:
    AC_AttitudeControl_Heli( AP_AHRS_View &ahrs,
                        const AP_Vehicle::MultiCopter &aparm,
                        AP_MotorsHeli& motors,
                        float dt) :
        AC_AttitudeControl(ahrs, aparm, motors, dt),
        _passthrough_roll(0), _passthrough_pitch(0), _passthrough_yaw(0),
        _pid_rate_roll(AC_ATC_HELI_RATE_RP_P, AC_ATC_HELI_RATE_RP_I, AC_ATC_HELI_RATE_RP_D, AC_ATC_HELI_RATE_RP_IMAX, AC_ATC_HELI_RATE_RP_FILT_HZ, dt, AC_ATC_HELI_RATE_RP_FF),
        _pid_rate_pitch(AC_ATC_HELI_RATE_RP_P, AC_ATC_HELI_RATE_RP_I, AC_ATC_HELI_RATE_RP_D, AC_ATC_HELI_RATE_RP_IMAX, AC_ATC_HELI_RATE_RP_FILT_HZ, dt, AC_ATC_HELI_RATE_RP_FF),
        _pid_rate_yaw(AC_ATC_HELI_RATE_YAW_P, AC_ATC_HELI_RATE_YAW_I, AC_ATC_HELI_RATE_YAW_D, AC_ATC_HELI_RATE_YAW_IMAX, AC_ATC_HELI_RATE_YAW_FILT_HZ, dt, AC_ATC_HELI_RATE_YAW_FF),
        pitch_feedforward_filter(AC_ATTITUDE_HELI_RATE_RP_FF_FILTER),
        roll_feedforward_filter(AC_ATTITUDE_HELI_RATE_RP_FF_FILTER),
        yaw_velocity_feedforward_filter(AC_ATTITUDE_HELI_RATE_Y_VFF_FILTER)
        {
            AP_Param::setup_object_defaults(this, var_info);

            // initialise flags
            _flags_heli.limit_roll = false;
            _flags_heli.limit_pitch = false;
            _flags_heli.limit_yaw = false;
            _flags_heli.leaky_i = true;
            _flags_heli.flybar_passthrough = false;
            _flags_heli.tail_passthrough = false;
            _flags_heli.do_piro_comp = false;
        }

    // pid accessors
    AC_PID& get_rate_roll_pid() { return _pid_rate_roll; }
    AC_PID& get_rate_pitch_pid() { return _pid_rate_pitch; }
    AC_PID& get_rate_yaw_pid() { return _pid_rate_yaw; }

    // passthrough_bf_roll_pitch_rate_yaw - roll and pitch are passed through directly, body-frame rate target for yaw
    void passthrough_bf_roll_pitch_rate_yaw(float roll_passthrough, float pitch_passthrough, float yaw_rate_bf_cds) override;

    // Integrate vehicle rate into _att_error_rot_vec_rad
    void integrate_bf_rate_error_to_angle_errors();

    // subclass non-passthrough too, for external gyro, no flybar
    void input_rate_bf_roll_pitch_yaw(float roll_rate_bf_cds, float pitch_rate_bf_cds, float yaw_rate_bf_cds) override;

        // rate_controller_run - run lowest level body-frame rate controller and send outputs to the motors
        // should be called at 100hz or more
        virtual void rate_controller_run();

    // Update Alt_Hold angle maximum
    void update_althold_lean_angle_max(float throttle_in) override;

        // use_leaky_i - controls whether we use leaky i term for body-frame to motor output stage
        void use_leaky_i(bool leaky_i) override {  _flags_heli.leaky_i = leaky_i; }

    // use_flybar_passthrough - controls whether we pass-through
    // control inputs to swash-plate and tail
    void use_flybar_passthrough(bool passthrough, bool tail_passthrough) override {  
        _flags_heli.flybar_passthrough = passthrough; 
        _flags_heli.tail_passthrough = tail_passthrough; 
    }

    // do_piro_comp - controls whether piro-comp is active or not
    void do_piro_comp(bool piro_comp) { _flags_heli.do_piro_comp = piro_comp; }

    // set_hover_roll_scalar - scales Hover Roll Trim parameter. To be used by vehicle code according to vehicle condition.
    void set_hover_roll_trim_scalar(float scalar) override {_hover_roll_trim_scalar = constrain_float(scalar, 0.0f, 1.0f);}

    // Set output throttle
    void set_throttle_out(float throttle_in, bool apply_angle_boost, float filt_cutoff) override;

    // Command an euler roll and pitch angle and an euler yaw rate with angular velocity feedforward and smoothing
    void input_euler_angle_roll_pitch_euler_rate_yaw(float euler_roll_angle_cd, float euler_pitch_angle_cd, float euler_yaw_rate_cds) override;

    // Command an euler roll, pitch and yaw angle with angular velocity feedforward and smoothing
    void input_euler_angle_roll_pitch_yaw(float euler_roll_angle_cd, float euler_pitch_angle_cd, float euler_yaw_angle_cd, bool slew_yaw) override;

    // enable/disable inverted flight
    void set_inverted_flight(bool inverted) override {
        _inverted_flight = inverted;
    }

    // user settable parameters
    static const struct AP_Param::GroupInfo var_info[];

private:

    // To-Do: move these limits flags into the heli motors class
    struct AttControlHeliFlags {
        uint8_t limit_roll          :   1;  // 1 if we have requested larger roll angle than swash can physically move
        uint8_t limit_pitch         :   1;  // 1 if we have requested larger pitch angle than swash can physically move
        uint8_t limit_yaw           :   1;  // 1 if we have requested larger yaw angle than tail servo can physically move
        uint8_t leaky_i             :   1;  // 1 if we should use leaky i term for body-frame rate to motor stage
        uint8_t flybar_passthrough  :   1;  // 1 if we should pass through pilots roll & pitch input directly to swash-plate
        uint8_t tail_passthrough    :   1;  // 1 if we should pass through pilots yaw input to tail
        uint8_t do_piro_comp        :   1;  // 1 if we should do pirouette compensation on roll/pitch
    } _flags_heli;

    //
    // body-frame rate controller
    //
        // rate_bf_to_motor_roll_pitch - ask the rate controller to calculate the motor outputs to achieve the target body-frame rate (in radians/sec) for roll, pitch and yaw
    // outputs are sent directly to motor class
    void rate_bf_to_motor_roll_pitch(const Vector3f &rate_rads, float rate_roll_target_rads, float rate_pitch_target_rads);
    float rate_target_to_motor_yaw(float rate_yaw_actual_rads, float rate_yaw_rads) override;

    //
    // throttle methods
    //

    // pass through for roll and pitch
    int16_t _passthrough_roll;
    int16_t _passthrough_pitch;

    // pass through for yaw if tail_passthrough is set
    int16_t _passthrough_yaw;

    // get_roll_trim - angle in centi-degrees to be added to roll angle. Used by helicopter to counter tail rotor thrust in hover
    float get_roll_trim_rad() { return constrain_float(radians(_hover_roll_trim_scalar * _hover_roll_trim * 0.01f), -radians(10.0f),radians(10.0f));}

    // internal variables
    float _hover_roll_trim_scalar = 0;              // scalar used to suppress Hover Roll Trim


    // This represents an euler axis-angle rotation vector from the vehicles
    // estimated attitude to the reference (setpoint) attitude used in the attitude
    // controller, in radians in the vehicle body frame of reference.
    Vector3f            _att_error_rot_vec_rad;

    // parameters
    AP_Int8         _piro_comp_enabled;             // Flybar present or not.  Affects attitude controller used during ACRO flight mode
    AP_Int16        _hover_roll_trim;               // Angle in centi-degrees used to counter tail rotor thrust in hover
    AC_HELI_PID     _pid_rate_roll;
    AC_HELI_PID     _pid_rate_pitch;
    AC_HELI_PID     _pid_rate_yaw;

    // LPF filters to act on Rate Feedforward terms to linearize output.
    // Due to complicated aerodynamic effects, feedforwards acting too fast can lead
    // to jerks on rate change requests.
    LowPassFilterFloat pitch_feedforward_filter;
    LowPassFilterFloat roll_feedforward_filter;
    LowPassFilterFloat yaw_velocity_feedforward_filter;
    LowPassFilterFloat yaw_acceleration_feedforward_filter;

};

      # Upload SARIF file to GitHub Code Scanning Alerts
      - name: #include "AC_AttitudeControl_Multi.h"
#include <AP_HAL/AP_HAL.h>
#include <AP_Math/AP_Math.h>

// table of user settable parameters
const AP_Param::GroupInfo AC_AttitudeControl_Multi::var_info[] = {
    // parameters from parent vehicle
    AP_NESTEDGROUPINFO(AC_AttitudeControl, 0),

    // @Param: RAT_RLL_P
    // @DisplayName: Roll axis rate controller P gain
    // @Description: Roll axis rate controller P gain.  Converts the difference between desired roll rate and actual roll rate into a motor speed output
    // @Range: 0.05 0.5
    // @Increment: 0.005
    // @User: Standard

    // @Param: RAT_RLL_I
    // @DisplayName: Roll axis rate controller I gain
    // @Description: Roll axis rate controller I gain.  Corrects long-term difference in desired roll rate vs actual roll rate
    // @Range: 0.01 2.0
    // @Increment: 0.01
    // @User: Standard

    // @Param: RAT_RLL_IMAX
    // @DisplayName: Roll axis rate controller I gain maximum
    // @Description: Roll axis rate controller I gain maximum.  Constrains the maximum motor output that the I gain will output
    // @Range: 0 1
    // @Increment: 0.01
    // @Units: %
    // @User: Standard

    // @Param: RAT_RLL_D
    // @DisplayName: Roll axis rate controller D gain
    // @Description: Roll axis rate controller D gain.  Compensates for short-term change in desired roll rate vs actual roll rate
    // @Range: 0.0 0.02
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_RLL_FF
    // @DisplayName: Roll axis rate controller feed forward
    // @Description: Roll axis rate controller feed forward
    // @Range: 0 0.5
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_RLL_FILT
    // @DisplayName: Roll axis rate controller input frequency in Hz
    // @Description: Roll axis rate controller input frequency in Hz
    // @Range: 1 100
    // @Increment: 1
    // @Units: Hz
    // @User: Standard
    AP_SUBGROUPINFO(_pid_rate_roll, "RAT_RLL_", 1, AC_AttitudeControl_Multi, AC_PID),

    // @Param: RAT_PIT_P
    // @DisplayName: Pitch axis rate controller P gain
    // @Description: Pitch axis rate controller P gain.  Converts the difference between desired pitch rate and actual pitch rate into a motor speed output
    // @Range: 0.05 0.50
    // @Increment: 0.005
    // @User: Standard

    // @Param: RAT_PIT_I
    // @DisplayName: Pitch axis rate controller I gain
    // @Description: Pitch axis rate controller I gain.  Corrects long-term difference in desired pitch rate vs actual pitch rate
    // @Range: 0.01 2.0
    // @Increment: 0.01
    // @User: Standard

    // @Param: RAT_PIT_IMAX
    // @DisplayName: Pitch axis rate controller I gain maximum
    // @Description: Pitch axis rate controller I gain maximum.  Constrains the maximum motor output that the I gain will output
    // @Range: 0 1
    // @Increment: 0.01
    // @Units: %
    // @User: Standard

    // @Param: RAT_PIT_D
    // @DisplayName: Pitch axis rate controller D gain
    // @Description: Pitch axis rate controller D gain.  Compensates for short-term change in desired pitch rate vs actual pitch rate
    // @Range: 0.0 0.02
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_PIT_FF
    // @DisplayName: Pitch axis rate controller feed forward
    // @Description: Pitch axis rate controller feed forward
    // @Range: 0 0.5
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_PIT_FILT
    // @DisplayName: Pitch axis rate controller input frequency in Hz
    // @Description: Pitch axis rate controller input frequency in Hz
    // @Range: 1 100
    // @Increment: 1
    // @Units: Hz
    // @User: Standard
    AP_SUBGROUPINFO(_pid_rate_pitch, "RAT_PIT_", 2, AC_AttitudeControl_Multi, AC_PID),

    // @Param: RAT_YAW_P
    // @DisplayName: Yaw axis rate controller P gain
    // @Description: Yaw axis rate controller P gain.  Converts the difference between desired yaw rate and actual yaw rate into a motor speed output
    // @Range: 0.10 2.50
    // @Increment: 0.005
    // @User: Standard

    // @Param: RAT_YAW_I
    // @DisplayName: Yaw axis rate controller I gain
    // @Description: Yaw axis rate controller I gain.  Corrects long-term difference in desired yaw rate vs actual yaw rate
    // @Range: 0.010 1.0
    // @Increment: 0.01
    // @User: Standard

    // @Param: RAT_YAW_IMAX
    // @DisplayName: Yaw axis rate controller I gain maximum
    // @Description: Yaw axis rate controller I gain maximum.  Constrains the maximum motor output that the I gain will output
    // @Range: 0 1
    // @Increment: 0.01
    // @Units: %
    // @User: Standard

    // @Param: RAT_YAW_D
    // @DisplayName: Yaw axis rate controller D gain
    // @Description: Yaw axis rate controller D gain.  Compensates for short-term change in desired yaw rate vs actual yaw rate
    // @Range: 0.000 0.02
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_YAW_FF
    // @DisplayName: Yaw axis rate controller feed forward
    // @Description: Yaw axis rate controller feed forward
    // @Range: 0 0.5
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_YAW_FILT
    // @DisplayName: Yaw axis rate controller input frequency in Hz
    // @Description: Yaw axis rate controller input frequency in Hz
    // @Range: 1 10
    // @Increment: 1
    // @Units: Hz
    // @User: Standard
    AP_SUBGROUPINFO(_pid_rate_yaw, "RAT_YAW_", 3, AC_AttitudeControl_Multi, AC_PID),

    // @Param: THR_MIX_MIN
    // @DisplayName: Throttle Mix Minimum
    // @Description: Throttle vs attitude control prioritisation used when landing (higher values mean we prioritise attitude control over throttle)
    // @Range: 0.1 0.25
    // @User: Advanced
    AP_GROUPINFO("THR_MIX_MIN", 4, AC_AttitudeControl_Multi, _thr_mix_min, AC_ATTITUDE_CONTROL_MIN_DEFAULT),

    // @Param: THR_MIX_MAX
    // @DisplayName: Throttle Mix Maximum
    // @Description: Throttle vs attitude control prioritisation used during active flight (higher values mean we prioritise attitude control over throttle)
    // @Range: 0.5 0.9
    // @User: Advanced
    AP_GROUPINFO("THR_MIX_MAX", 5, AC_AttitudeControl_Multi, _thr_mix_max, AC_ATTITUDE_CONTROL_MAX_DEFAULT),

    // @Param: THR_MIX_MAN
    // @DisplayName: Throttle Mix Manual
    // @Description: Throttle vs attitude control prioritisation used during manual flight (higher values mean we prioritise attitude control over throttle)
    // @Range: 0.1 0.9
    // @User: Advanced
    AP_GROUPINFO("THR_MIX_MAN", 6, AC_AttitudeControl_Multi, _thr_mix_man, AC_ATTITUDE_CONTROL_MAN_DEFAULT),

    AP_GROUPEND
};

AC_AttitudeControl_Multi::AC_AttitudeControl_Multi(AP_AHRS_View &ahrs, const AP_Vehicle::MultiCopter &aparm, AP_MotorsMulticopter& motors, float dt) :
    AC_AttitudeControl(ahrs, aparm, motors, dt),
    _motors_multi(motors),
    _pid_rate_roll(AC_ATC_MULTI_RATE_RP_P, AC_ATC_MULTI_RATE_RP_I, AC_ATC_MULTI_RATE_RP_D, AC_ATC_MULTI_RATE_RP_IMAX, AC_ATC_MULTI_RATE_RP_FILT_HZ, dt),
    _pid_rate_pitch(AC_ATC_MULTI_RATE_RP_P, AC_ATC_MULTI_RATE_RP_I, AC_ATC_MULTI_RATE_RP_D, AC_ATC_MULTI_RATE_RP_IMAX, AC_ATC_MULTI_RATE_RP_FILT_HZ, dt),
    _pid_rate_yaw(AC_ATC_MULTI_RATE_YAW_P, AC_ATC_MULTI_RATE_YAW_I, AC_ATC_MULTI_RATE_YAW_D, AC_ATC_MULTI_RATE_YAW_IMAX, AC_ATC_MULTI_RATE_YAW_FILT_HZ, dt)
{
    AP_Param::setup_object_defaults(this, var_info);
}

// Update Alt_Hold angle maximum
void AC_AttitudeControl_Multi::update_althold_lean_angle_max(float throttle_in)
{
    // calc maximum tilt angle based on throttle
    float thr_max = _motors_multi.get_throttle_thrust_max();

    // divide by zero check
    if (is_zero(thr_max)) {
        _althold_lean_angle_max = 0.0f;
        return;
    }

    float althold_lean_angle_max = acos(constrain_float(_throttle_in/(AC_ATTITUDE_CONTROL_ANGLE_LIMIT_THROTTLE_MAX * thr_max), 0.0f, 1.0f));
    _althold_lean_angle_max = _althold_lean_angle_max + (_dt/(_dt+_angle_limit_tc))*(althold_lean_angle_max-_althold_lean_angle_max);
}

void AC_AttitudeControl_Multi::set_throttle_out(float throttle_in, bool apply_angle_boost, float filter_cutoff)
{
    _throttle_in = throttle_in;
    update_althold_lean_angle_max(throttle_in);
    _motors.set_throttle_filter_cutoff(filter_cutoff);
    if (apply_angle_boost) {
        // Apply angle boost
        throttle_in = get_throttle_boosted(throttle_in);
    }else{
        // Clear angle_boost for logging purposes
        _angle_boost = 0.0f;
    }
    _motors.set_throttle(throttle_in);
    _motors.set_throttle_avg_max(get_throttle_avg_max(MAX(throttle_in, _throttle_in)));
}

// returns a throttle including compensation for roll/pitch angle
// throttle value should be 0 ~ 1
float AC_AttitudeControl_Multi::get_throttle_boosted(float throttle_in)
{
    if (!_angle_boost_enabled) {
        _angle_boost = 0;
        return throttle_in;
    }
    // inverted_factor is 1 for tilt angles below 60 degrees
    // inverted_factor reduces from 1 to 0 for tilt angles between 60 and 90 degrees

    float cos_tilt = _ahrs.cos_pitch() * _ahrs.cos_roll();
    float inverted_factor = constrain_float(2.0f*cos_tilt, 0.0f, 1.0f);
    float boost_factor = 1.0f/constrain_float(cos_tilt, 0.5f, 1.0f);

    float throttle_out = throttle_in*inverted_factor*boost_factor;
    _angle_boost = constrain_float(throttle_out - throttle_in,-1.0f,1.0f);
    return throttle_out;
}

// returns a throttle including compensation for roll/pitch angle
// throttle value should be 0 ~ 1
float AC_AttitudeControl_Multi::get_throttle_avg_max(float throttle_in)
{
    throttle_in = constrain_float(throttle_in, 0.0f, 1.0f);
    return MAX(throttle_in, throttle_in*MAX(0.0f,1.0f-_throttle_rpy_mix)+_motors.get_throttle_hover()*_throttle_rpy_mix);
}

// update_throttle_rpy_mix - slew set_throttle_rpy_mix to requested value
void AC_AttitudeControl_Multi::update_throttle_rpy_mix()
{
    // slew _throttle_rpy_mix to _throttle_rpy_mix_desired
    if (_throttle_rpy_mix < _throttle_rpy_mix_desired) {
        // increase quickly (i.e. from 0.1 to 0.9 in 0.4 seconds)
        _throttle_rpy_mix += MIN(2.0f*_dt, _throttle_rpy_mix_desired-_throttle_rpy_mix);
    } else if (_throttle_rpy_mix > _throttle_rpy_mix_desired) {
        // reduce more slowly (from 0.9 to 0.1 in 1.6 seconds)
        _throttle_rpy_mix -= MIN(0.5f*_dt, _throttle_rpy_mix-_throttle_rpy_mix_desired);
    }
    _throttle_rpy_mix = constrain_float(_throttle_rpy_mix, 0.1f, AC_ATTITUDE_CONTROL_MAX);
}

void AC_AttitudeControl_Multi::rate_controller_run()
{
    // move throttle vs attitude mixing towards desired (called from here because this is conveniently called on every iteration)
    update_throttle_rpy_mix();

    Vector3f gyro_latest = _ahrs.get_gyro_latest();
    _motors.set_roll(rate_target_to_motor_roll(gyro_latest.x, _rate_target_ang_vel.x));
    _motors.set_pitch(rate_target_to_motor_pitch(gyro_latest.y, _rate_target_ang_vel.y));
    _motors.set_yaw(rate_target_to_motor_yaw(gyro_latest.z, _rate_target_ang_vel.z));

    control_monitor_update();
}

// sanity check parameters.  should be called once before takeoff
void AC_AttitudeControl_Multi::parameter_sanity_check()
{
    // sanity check throttle mix parameters
    if (_thr_mix_man < 0.1f || _thr_mix_man > 4.0f) {
        // parameter description recommends thr-mix-man be no higher than 0.9 but we allow up to 4.0
        // which can be useful for very high powered copters with very low hover throttle
        _thr_mix_man.set_and_save(AC_ATTITUDE_CONTROL_MAN_DEFAULT);
    }
    if (_thr_mix_min < 0.1f || _thr_mix_min > 0.25f) {
        _thr_mix_min.set_and_save(AC_ATTITUDE_CONTROL_MIN_DEFAULT);
    }
    if (_thr_mix_max < 0.5f || _thr_mix_max > AC_ATTITUDE_CONTROL_MAX) {
        // parameter description recommends thr-mix-max be no higher than 0.9 but we allow up to 5.0
        // which can be useful for very high powered copters with very low hover throttle
        _thr_mix_max.set_and_save(AC_ATTITUDE_CONTROL_MAX_DEFAULT);
    }
    if (_thr_mix_min > _thr_mix_max) {
        _thr_mix_min.set_and_save(AC_ATTITUDE_CONTROL_MIN_DEFAULT);
        _thr_mix_max.set_and_save(AC_ATTITUDE_CONTROL_MAX_DEFAULT);
    }
}
        uses: #pragma once

/// @file    AC_AttitudeControl_Multi.h
/// @brief   ArduCopter attitude control library

#include "AC_AttitudeControl.h"
#include <AP_Motors/AP_MotorsMulticopter.h>

// default rate controller PID gains
#ifndef AC_ATC_MULTI_RATE_RP_P
  # define AC_ATC_MULTI_RATE_RP_P           0.135f
#endif
#ifndef AC_ATC_MULTI_RATE_RP_I
  # define AC_ATC_MULTI_RATE_RP_I           0.090f
#endif
#ifndef AC_ATC_MULTI_RATE_RP_D
  # define AC_ATC_MULTI_RATE_RP_D           0.0036f
#endif
#ifndef AC_ATC_MULTI_RATE_RP_IMAX
 # define AC_ATC_MULTI_RATE_RP_IMAX         0.5f
#endif
#ifndef AC_ATC_MULTI_RATE_RP_FILT_HZ
 # define AC_ATC_MULTI_RATE_RP_FILT_HZ      20.0f
#endif
#ifndef AC_ATC_MULTI_RATE_YAW_P
 # define AC_ATC_MULTI_RATE_YAW_P           0.180f
#endif
#ifndef AC_ATC_MULTI_RATE_YAW_I
 # define AC_ATC_MULTI_RATE_YAW_I           0.018f
#endif
#ifndef AC_ATC_MULTI_RATE_YAW_D
 # define AC_ATC_MULTI_RATE_YAW_D           0.0f
#endif
#ifndef AC_ATC_MULTI_RATE_YAW_IMAX
 # define AC_ATC_MULTI_RATE_YAW_IMAX        0.5f
#endif
#ifndef AC_ATC_MULTI_RATE_YAW_FILT_HZ
 # define AC_ATC_MULTI_RATE_YAW_FILT_HZ     2.5f
#endif


class AC_AttitudeControl_Multi : public AC_AttitudeControl {
public:
        AC_AttitudeControl_Multi(AP_AHRS_View &ahrs, const AP_Vehicle::MultiCopter &aparm, AP_MotorsMulticopter& motors, float dt);

        // empty destructor to suppress compiler warning
        virtual ~AC_AttitudeControl_Multi() {}

    // pid accessors
    AC_PID& get_rate_roll_pid() { return _pid_rate_roll; }
    AC_PID& get_rate_pitch_pid() { return _pid_rate_pitch; }
    AC_PID& get_rate_yaw_pid() { return _pid_rate_yaw; }

    // Update Alt_Hold angle maximum
    void update_althold_lean_angle_max(float throttle_in) override;

    // Set output throttle
    void set_throttle_out(float throttle_in, bool apply_angle_boost, float filt_cutoff) override;

    // calculate total body frame throttle required to produce the given earth frame throttle
    float get_throttle_boosted(float throttle_in);

    // set desired throttle vs attitude mixing (actual mix is slewed towards this value over 1~2 seconds)
    //  low values favour pilot/autopilot throttle over attitude control, high values favour attitude control over throttle
    //  has no effect when throttle is above hover throttle
    void set_throttle_mix_min() override { _throttle_rpy_mix_desired = _thr_mix_min; }
    void set_throttle_mix_man() override { _throttle_rpy_mix_desired = _thr_mix_man; }
    void set_throttle_mix_max() override { _throttle_rpy_mix_desired = _thr_mix_max; }
    void set_throttle_mix_value(float value) override { _throttle_rpy_mix_desired = _throttle_rpy_mix = value; }
    float get_throttle_mix(void) const override { return _throttle_rpy_mix; }

    // are we producing min throttle?
    bool is_throttle_mix_min() const override { return (_throttle_rpy_mix < 1.25f*_thr_mix_min); }

    // run lowest level body-frame rate controller and send outputs to the motors
    void rate_controller_run();

    // sanity check parameters.  should be called once before take-off
    void parameter_sanity_check();

    // user settable parameters
    static const struct AP_Param::GroupInfo var_info[];

protected:

    // update_throttle_rpy_mix - updates thr_low_comp value towards the target
    void update_throttle_rpy_mix();

    // get maximum value throttle can be raised to based on throttle vs attitude prioritisation
    float get_throttle_avg_max(float throttle_in);

    AP_MotorsMulticopter& _motors_multi;
    AC_PID                _pid_rate_roll;
    AC_PID                _pid_rate_pitch;
    AC_PID                _pid_rate_yaw;

    AP_Float              _thr_mix_man;     // throttle vs attitude control prioritisation used when using manual throttle (higher values mean we prioritise attitude control over throttle)
    AP_Float              _thr_mix_min;     // throttle vs attitude control prioritisation used when landing (higher values mean we prioritise attitude control over throttle)
    AP_Float              _thr_mix_max;     // throttle vs attitude control prioritisation used during active flight (higher values mean we prioritise attitude control over throttle)
};
        with: #include "AC_AttitudeControl_Sub.h"
#include <AP_HAL/AP_HAL.h>
#include <AP_Math/AP_Math.h>

// table of user settable parameters
const AP_Param::GroupInfo AC_AttitudeControl_Sub::var_info[] = {
    // parameters from parent vehicle
    AP_NESTEDGROUPINFO(AC_AttitudeControl, 0),

    // @Param: RAT_RLL_P
    // @DisplayName: Roll axis rate controller P gain
    // @Description: Roll axis rate controller P gain.  Converts the difference between desired roll rate and actual roll rate into a motor speed output
    // @Range: 0.0 0.30
    // @Increment: 0.005
    // @User: Standard

    // @Param: RAT_RLL_I
    // @DisplayName: Roll axis rate controller I gain
    // @Description: Roll axis rate controller I gain.  Corrects long-term difference in desired roll rate vs actual roll rate
    // @Range: 0.0 0.5
    // @Increment: 0.01
    // @User: Standard

    // @Param: RAT_RLL_IMAX
    // @DisplayName: Roll axis rate controller I gain maximum
    // @Description: Roll axis rate controller I gain maximum.  Constrains the maximum motor output that the I gain will output
    // @Range: 0 1
    // @Increment: 0.01
    // @Units: %
    // @User: Standard

    // @Param: RAT_RLL_D
    // @DisplayName: Roll axis rate controller D gain
    // @Description: Roll axis rate controller D gain.  Compensates for short-term change in desired roll rate vs actual roll rate
    // @Range: 0.0 0.02
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_RLL_FF
    // @DisplayName: Roll axis rate controller feed forward
    // @Description: Roll axis rate controller feed forward
    // @Range: 0 0.5
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_RLL_FILT
    // @DisplayName: Roll axis rate controller input frequency in Hz
    // @Description: Roll axis rate controller input frequency in Hz
    // @Range: 1 100
    // @Increment: 1
    // @Units: Hz
    // @User: Standard
    AP_SUBGROUPINFO(_pid_rate_roll, "RAT_RLL_", 1, AC_AttitudeControl_Sub, AC_PID),

    // @Param: RAT_PIT_P
    // @DisplayName: Pitch axis rate controller P gain
    // @Description: Pitch axis rate controller P gain.  Converts the difference between desired pitch rate and actual pitch rate into a motor speed output
    // @Range: 0.0 0.30
    // @Increment: 0.005
    // @User: Standard

    // @Param: RAT_PIT_I
    // @DisplayName: Pitch axis rate controller I gain
    // @Description: Pitch axis rate controller I gain.  Corrects long-term difference in desired pitch rate vs actual pitch rate
    // @Range: 0.0 0.5
    // @Increment: 0.01
    // @User: Standard

    // @Param: RAT_PIT_IMAX
    // @DisplayName: Pitch axis rate controller I gain maximum
    // @Description: Pitch axis rate controller I gain maximum.  Constrains the maximum motor output that the I gain will output
    // @Range: 0 1
    // @Increment: 0.01
    // @Units: %
    // @User: Standard

    // @Param: RAT_PIT_D
    // @DisplayName: Pitch axis rate controller D gain
    // @Description: Pitch axis rate controller D gain.  Compensates for short-term change in desired pitch rate vs actual pitch rate
    // @Range: 0.0 0.02
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_PIT_FF
    // @DisplayName: Pitch axis rate controller feed forward
    // @Description: Pitch axis rate controller feed forward
    // @Range: 0 0.5
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_PIT_FILT
    // @DisplayName: Pitch axis rate controller input frequency in Hz
    // @Description: Pitch axis rate controller input frequency in Hz
    // @Range: 1 100
    // @Increment: 1
    // @Units: Hz
    // @User: Standard
    AP_SUBGROUPINFO(_pid_rate_pitch, "RAT_PIT_", 2, AC_AttitudeControl_Sub, AC_PID),

    // @Param: RAT_YAW_P
    // @DisplayName: Yaw axis rate controller P gain
    // @Description: Yaw axis rate controller P gain.  Converts the difference between desired yaw rate and actual yaw rate into a motor speed output
    // @Range: 0.0 0.50
    // @Increment: 0.005
    // @User: Standard

    // @Param: RAT_YAW_I
    // @DisplayName: Yaw axis rate controller I gain
    // @Description: Yaw axis rate controller I gain.  Corrects long-term difference in desired yaw rate vs actual yaw rate
    // @Range: 0.0 0.05
    // @Increment: 0.01
    // @User: Standard

    // @Param: RAT_YAW_IMAX
    // @DisplayName: Yaw axis rate controller I gain maximum
    // @Description: Yaw axis rate controller I gain maximum.  Constrains the maximum motor output that the I gain will output
    // @Range: 0 1
    // @Increment: 0.01
    // @Units: %
    // @User: Standard

    // @Param: RAT_YAW_D
    // @DisplayName: Yaw axis rate controller D gain
    // @Description: Yaw axis rate controller D gain.  Compensates for short-term change in desired yaw rate vs actual yaw rate
    // @Range: 0.000 0.02
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_YAW_FF
    // @DisplayName: Yaw axis rate controller feed forward
    // @Description: Yaw axis rate controller feed forward
    // @Range: 0 0.5
    // @Increment: 0.001
    // @User: Standard

    // @Param: RAT_YAW_FILT
    // @DisplayName: Yaw axis rate controller input frequency in Hz
    // @Description: Yaw axis rate controller input frequency in Hz
    // @Range: 1 100
    // @Increment: 1
    // @Units: Hz
    // @User: Standard
    AP_SUBGROUPINFO(_pid_rate_yaw, "RAT_YAW_", 3, AC_AttitudeControl_Sub, AC_PID),

    // @Param: THR_MIX_MIN
    // @DisplayName: Throttle Mix Minimum
    // @Description: Throttle vs attitude control prioritisation used when landing (higher values mean we prioritise attitude control over throttle)
    // @Range: 0.1 0.25
    // @User: Advanced
    AP_GROUPINFO("THR_MIX_MIN", 4, AC_AttitudeControl_Sub, _thr_mix_min, AC_ATTITUDE_CONTROL_MIN_DEFAULT),

    // @Param: THR_MIX_MAX
    // @DisplayName: Throttle Mix Maximum
    // @Description: Throttle vs attitude control prioritisation used during active flight (higher values mean we prioritise attitude control over throttle)
    // @Range: 0.5 0.9
    // @User: Advanced
    AP_GROUPINFO("THR_MIX_MAX", 5, AC_AttitudeControl_Sub, _thr_mix_max, AC_ATTITUDE_CONTROL_MAX_DEFAULT),

    // @Param: THR_MIX_MAN
    // @DisplayName: Throttle Mix Manual
    // @Description: Throttle vs attitude control prioritisation used during manual flight (higher values mean we prioritise attitude control over throttle)
    // @Range: 0.5 0.9
    // @User: Advanced
    AP_GROUPINFO("THR_MIX_MAN", 6, AC_AttitudeControl_Sub, _thr_mix_man, AC_ATTITUDE_CONTROL_MAN_DEFAULT),

    AP_GROUPEND
};

AC_AttitudeControl_Sub::AC_AttitudeControl_Sub(AP_AHRS_View &ahrs, const AP_Vehicle::MultiCopter &aparm, AP_MotorsMulticopter& motors, float dt) :
    AC_AttitudeControl(ahrs, aparm, motors, dt),
    _motors_multi(motors),
    _pid_rate_roll(AC_ATC_SUB_RATE_RP_P, AC_ATC_SUB_RATE_RP_I, AC_ATC_SUB_RATE_RP_D, AC_ATC_SUB_RATE_RP_IMAX, AC_ATC_SUB_RATE_RP_FILT_HZ, dt),
    _pid_rate_pitch(AC_ATC_SUB_RATE_RP_P, AC_ATC_SUB_RATE_RP_I, AC_ATC_SUB_RATE_RP_D, AC_ATC_SUB_RATE_RP_IMAX, AC_ATC_SUB_RATE_RP_FILT_HZ, dt),
        _pid_rate_yaw(AC_ATC_SUB_RATE_YAW_P, AC_ATC_SUB_RATE_YAW_I, AC_ATC_SUB_RATE_YAW_D, AC_ATC_SUB_RATE_YAW_IMAX, AC_ATC_SUB_RATE_YAW_FILT_HZ, dt)
{
    AP_Param::setup_object_defaults(this, var_info);

    // Sub-specific defaults for parent class
    _p_angle_roll.kP().set_default(AC_ATC_SUB_ANGLE_P);
    _p_angle_pitch.kP().set_default(AC_ATC_SUB_ANGLE_P);
    _p_angle_yaw.kP().set_default(AC_ATC_SUB_ANGLE_P);

    _accel_yaw_max.set_default(AC_ATC_SUB_ACCEL_Y_MAX);
}

// Update Alt_Hold angle maximum
void AC_AttitudeControl_Sub::update_althold_lean_angle_max(float throttle_in)
{
    // calc maximum tilt angle based on throttle
    float thr_max = _motors_multi.get_throttle_thrust_max();

    // divide by zero check
    if (is_zero(thr_max)) {
        _althold_lean_angle_max = 0.0f;
        return;
    }

    float althold_lean_angle_max = acos(constrain_float(_throttle_in/(AC_ATTITUDE_CONTROL_ANGLE_LIMIT_THROTTLE_MAX * thr_max), 0.0f, 1.0f));
    _althold_lean_angle_max = _althold_lean_angle_max + (_dt/(_dt+_angle_limit_tc))*(althold_lean_angle_max-_althold_lean_angle_max);
}

void AC_AttitudeControl_Sub::set_throttle_out(float throttle_in, bool apply_angle_boost, float filter_cutoff)
{
    _throttle_in = throttle_in;
    update_althold_lean_angle_max(throttle_in);
    _motors.set_throttle_filter_cutoff(filter_cutoff);
    if (apply_angle_boost) {
        // Apply angle boost
        throttle_in = get_throttle_boosted(throttle_in);
    }else{
        // Clear angle_boost for logging purposes
        _angle_boost = 0.0f;
    }
    _motors.set_throttle(throttle_in);
    _motors.set_throttle_avg_max(get_throttle_avg_max(MAX(throttle_in, _throttle_in)));
}

// returns a throttle including compensation for roll/pitch angle
// throttle value should be 0 ~ 1
float AC_AttitudeControl_Sub::get_throttle_boosted(float throttle_in)
{
    if (!_angle_boost_enabled) {
        _angle_boost = 0;
        return throttle_in;
    }
    // inverted_factor is 1 for tilt angles below 60 degrees
    // inverted_factor reduces from 1 to 0 for tilt angles between 60 and 90 degrees

    float cos_tilt = _ahrs.cos_pitch() * _ahrs.cos_roll();
    float inverted_factor = constrain_float(2.0f*cos_tilt, 0.0f, 1.0f);
    float boost_factor = 1.0f/constrain_float(cos_tilt, 0.5f, 1.0f);

    float throttle_out = throttle_in*inverted_factor*boost_factor;
    _angle_boost = constrain_float(throttle_out - throttle_in,-1.0f,1.0f);
    return throttle_out;
}

// returns a throttle including compensation for roll/pitch angle
// throttle value should be 0 ~ 1
float AC_AttitudeControl_Sub::get_throttle_avg_max(float throttle_in)
{
    throttle_in = constrain_float(throttle_in, 0.0f, 1.0f);
    return MAX(throttle_in, throttle_in*MAX(0.0f,1.0f-_throttle_rpy_mix)+_motors.get_throttle_hover()*_throttle_rpy_mix);
}

// update_throttle_rpy_mix - slew set_throttle_rpy_mix to requested value
void AC_AttitudeControl_Sub::update_throttle_rpy_mix()
{
    // slew _throttle_rpy_mix to _throttle_rpy_mix_desired
    if (_throttle_rpy_mix < _throttle_rpy_mix_desired) {
        // increase quickly (i.e. from 0.1 to 0.9 in 0.4 seconds)
        _throttle_rpy_mix += MIN(2.0f*_dt, _throttle_rpy_mix_desired-_throttle_rpy_mix);
    } else if (_throttle_rpy_mix > _throttle_rpy_mix_desired) {
        // reduce more slowly (from 0.9 to 0.1 in 1.6 seconds)
        _throttle_rpy_mix -= MIN(0.5f*_dt, _throttle_rpy_mix-_throttle_rpy_mix_desired);
    }
    _throttle_rpy_mix = constrain_float(_throttle_rpy_mix, 0.1f, AC_ATTITUDE_CONTROL_MAX);
}

void AC_AttitudeControl_Sub::rate_controller_run()
{
    // move throttle vs attitude mixing towards desired (called from here because this is conveniently called on every iteration)
    update_throttle_rpy_mix();

    Vector3f gyro_latest = _ahrs.get_gyro_latest();
    _motors.set_roll(rate_target_to_motor_roll(gyro_latest.x, _rate_target_ang_vel.x));
    _motors.set_pitch(rate_target_to_motor_pitch(gyro_latest.y, _rate_target_ang_vel.y));
    _motors.set_yaw(rate_target_to_motor_yaw(gyro_latest.z, _rate_target_ang_vel.z));

    control_monitor_update();
}

// sanity check parameters.  should be called once before takeoff
void AC_AttitudeControl_Sub::parameter_sanity_check()
{
    // sanity check throttle mix parameters
    if (_thr_mix_man < 0.1f || _thr_mix_man > 4.0f) {
        // parameter description recommends thr-mix-man be no higher than 0.9 but we allow up to 4.0
        // which can be useful for very high powered copters with very low hover throttle
        _thr_mix_man.set_and_save(AC_ATTITUDE_CONTROL_MAN_DEFAULT);
    }
    if (_thr_mix_min < 0.1f || _thr_mix_min > 0.25f) {
        _thr_mix_min.set_and_save(AC_ATTITUDE_CONTROL_MIN_DEFAULT);
    }
    if (_thr_mix_max < 0.5f || _thr_mix_max > AC_ATTITUDE_CONTROL_MAX) {
        // parameter description recommends thr-mix-max be no higher than 0.9 but we allow up to 5.0
        // which can be useful for very high powered copters with very low hover throttle
        _thr_mix_max.set_and_save(AC_ATTITUDE_CONTROL_MAX_DEFAULT);
    }
    if (_thr_mix_min > _thr_mix_max) {
        _thr_mix_min.set_and_save(AC_ATTITUDE_CONTROL_MIN_DEFAULT);
        _thr_mix_max.set_and_save(AC_ATTITUDE_CONTROL_MAX_DEFAULT);
    }
}
          sarif_file: #pragma once

/// @file    AC_AttitudeControl_Sub.h
/// @brief   ArduSub attitude control library

#include "AC_AttitudeControl.h"
#include <AP_Motors/AP_MotorsMulticopter.h>

// default angle controller PID gains
// (Sub-specific defaults for parent class)
#define AC_ATC_SUB_ANGLE_P             6.0f
#define AC_ATC_SUB_ACCEL_Y_MAX         110000.0f

// default rate controller PID gains
#define AC_ATC_SUB_RATE_RP_P           0.135f
#define AC_ATC_SUB_RATE_RP_I           0.090f
#define AC_ATC_SUB_RATE_RP_D           0.0036f
#define AC_ATC_SUB_RATE_RP_IMAX        0.444f
#define AC_ATC_SUB_RATE_RP_FILT_HZ     30.0f
#define AC_ATC_SUB_RATE_YAW_P          0.180f
#define AC_ATC_SUB_RATE_YAW_I          0.018f
#define AC_ATC_SUB_RATE_YAW_D          0.0f
#define AC_ATC_SUB_RATE_YAW_IMAX       0.222f
#define AC_ATC_SUB_RATE_YAW_FILT_HZ    5.0f

class AC_AttitudeControl_Sub : public AC_AttitudeControl {
public:
    AC_AttitudeControl_Sub(AP_AHRS_View &ahrs, const AP_Vehicle::MultiCopter &aparm, AP_MotorsMulticopter& motors, float dt);

    // empty destructor to suppress compiler warning
    virtual ~AC_AttitudeControl_Sub() {}

    // pid accessors
    AC_PID& get_rate_roll_pid() { return _pid_rate_roll; }
    AC_PID& get_rate_pitch_pid() { return _pid_rate_pitch; }
    AC_PID& get_rate_yaw_pid() { return _pid_rate_yaw; }

    // Update Alt_Hold angle maximum
    void update_althold_lean_angle_max(float throttle_in) override;

    // Set output throttle
    void set_throttle_out(float throttle_in, bool apply_angle_boost, float filt_cutoff) override;

    // calculate total body frame throttle required to produce the given earth frame throttle
        float get_throttle_boosted(float throttle_in);

    // set desired throttle vs attitude mixing (actual mix is slewed towards this value over 1~2 seconds)
    //  low values favour pilot/autopilot throttle over attitude control, high values favour attitude control over throttle
    //  has no effect when throttle is above hover throttle
    void set_throttle_mix_min() override { _throttle_rpy_mix_desired = _thr_mix_min; }
    void set_throttle_mix_man() override { _throttle_rpy_mix_desired = _thr_mix_man; }
    void set_throttle_mix_max() override { _throttle_rpy_mix_desired = _thr_mix_max; }

    // are we producing min throttle?
    bool is_throttle_mix_min() const override { return (_throttle_rpy_mix < 1.25f*_thr_mix_min); }

    // run lowest level body-frame rate controller and send outputs to the motors
    void rate_controller_run();

    // sanity check parameters.  should be called once before take-off
    void parameter_sanity_check();

    // user settable parameters
    static const struct AP_Param::GroupInfo var_info[];

protected:

    // update_throttle_rpy_mix - updates thr_low_comp value towards the target
    void update_throttle_rpy_mix();

    // get maximum value throttle can be raised to based on throttle vs attitude prioritisation
    float get_throttle_avg_max(float throttle_in);

    AP_MotorsMulticopter& _motors_multi;
    AC_PID                _pid_rate_roll;
    AC_PID                _pid_rate_pitch;
    AC_PID                _pid_rate_yaw;

    AP_Float              _thr_mix_man;     // throttle vs attitude control prioritisation used when using manual throttle (higher values mean we prioritise attitude control over throttle)
    AP_Float              _thr_mix_min;     // throttle vs attitude control prioritisation used when landing (higher values mean we prioritise attitude control over throttle)
    AP_Float              _thr_mix_max;     // throttle vs attitude control prioritisation used during active flight (higher values mean we prioritise attitude control over throttle)
};

      # Upload SARIF file as an Artifact to download and view
      # - name: #include <AP_HAL/AP_HAL.h>
#include "AC_PosControl.h"
#include <AP_Math/AP_Math.h>
#include <DataFlash/DataFlash.h>

extern const AP_HAL::HAL& hal;

#if APM_BUILD_TYPE(APM_BUILD_ArduPlane)
 // default gains for Plane
 # define POSCONTROL_POS_Z_P                    1.0f    // vertical position controller P gain default
 # define POSCONTROL_VEL_Z_P                    5.0f    // vertical velocity controller P gain default
 # define POSCONTROL_ACC_Z_P                    0.3f    // vertical acceleration controller P gain default
 # define POSCONTROL_ACC_Z_I                    1.0f    // vertical acceleration controller I gain default
 # define POSCONTROL_ACC_Z_D                    0.0f    // vertical acceleration controller D gain default
 # define POSCONTROL_ACC_Z_IMAX                 800     // vertical acceleration controller IMAX gain default
 # define POSCONTROL_ACC_Z_FILT_HZ              10.0f   // vertical acceleration controller input filter default
 # define POSCONTROL_ACC_Z_DT                   0.02f   // vertical acceleration controller dt default
 # define POSCONTROL_POS_XY_P                   1.0f    // horizontal position controller P gain default
 # define POSCONTROL_VEL_XY_P                   1.4f    // horizontal velocity controller P gain default
 # define POSCONTROL_VEL_XY_I                   0.7f    // horizontal velocity controller I gain default
 # define POSCONTROL_VEL_XY_D                   0.35f   // horizontal velocity controller D gain default
 # define POSCONTROL_VEL_XY_IMAX                1000.0f // horizontal velocity controller IMAX gain default
 # define POSCONTROL_VEL_XY_FILT_HZ             5.0f    // horizontal velocity controller input filter
 # define POSCONTROL_VEL_XY_FILT_D_HZ           5.0f    // horizontal velocity controller input filter for D
#elif APM_BUILD_TYPE(APM_BUILD_ArduSub)
 // default gains for Sub
 # define POSCONTROL_POS_Z_P                    3.0f    // vertical position controller P gain default
 # define POSCONTROL_VEL_Z_P                    8.0f    // vertical velocity controller P gain default
 # define POSCONTROL_ACC_Z_P                    0.5f    // vertical acceleration controller P gain default
 # define POSCONTROL_ACC_Z_I                    0.1f    // vertical acceleration controller I gain default
 # define POSCONTROL_ACC_Z_D                    0.0f    // vertical acceleration controller D gain default
 # define POSCONTROL_ACC_Z_IMAX                 100     // vertical acceleration controller IMAX gain default
 # define POSCONTROL_ACC_Z_FILT_HZ              20.0f   // vertical acceleration controller input filter default
 # define POSCONTROL_ACC_Z_DT                   0.0025f // vertical acceleration controller dt default
 # define POSCONTROL_POS_XY_P                   1.0f    // horizontal position controller P gain default
 # define POSCONTROL_VEL_XY_P                   1.0f    // horizontal velocity controller P gain default
 # define POSCONTROL_VEL_XY_I                   0.5f    // horizontal velocity controller I gain default
 # define POSCONTROL_VEL_XY_D                   0.0f    // horizontal velocity controller D gain default
 # define POSCONTROL_VEL_XY_IMAX                1000.0f // horizontal velocity controller IMAX gain default
 # define POSCONTROL_VEL_XY_FILT_HZ             5.0f    // horizontal velocity controller input filter
 # define POSCONTROL_VEL_XY_FILT_D_HZ           5.0f    // horizontal velocity controller input filter for D
#else
 // default gains for Copter / TradHeli
 # define POSCONTROL_POS_Z_P                    1.0f    // vertical position controller P gain default
 # define POSCONTROL_VEL_Z_P                    5.0f    // vertical velocity controller P gain default
 # define POSCONTROL_ACC_Z_P                    0.5f    // vertical acceleration controller P gain default
 # define POSCONTROL_ACC_Z_I                    1.0f    // vertical acceleration controller I gain default
 # define POSCONTROL_ACC_Z_D                    0.0f    // vertical acceleration controller D gain default
 # define POSCONTROL_ACC_Z_IMAX                 800     // vertical acceleration controller IMAX gain default
 # define POSCONTROL_ACC_Z_FILT_HZ              20.0f   // vertical acceleration controller input filter default
 # define POSCONTROL_ACC_Z_DT                   0.0025f // vertical acceleration controller dt default
 # define POSCONTROL_POS_XY_P                   1.0f    // horizontal position controller P gain default
 # define POSCONTROL_VEL_XY_P                   2.0f    // horizontal velocity controller P gain default
 # define POSCONTROL_VEL_XY_I                   1.0f    // horizontal velocity controller I gain default
 # define POSCONTROL_VEL_XY_D                   0.5f    // horizontal velocity controller D gain default
 # define POSCONTROL_VEL_XY_IMAX                1000.0f // horizontal velocity controller IMAX gain default
 # define POSCONTROL_VEL_XY_FILT_HZ             5.0f    // horizontal velocity controller input filter
 # define POSCONTROL_VEL_XY_FILT_D_HZ           5.0f    // horizontal velocity controller input filter for D
#endif

const AP_Param::GroupInfo AC_PosControl::var_info[] = {
    // 0 was used for HOVER

    // @Param: _ACC_XY_FILT
    // @DisplayName: XY Acceleration filter cutoff frequency
    // @Description: Lower values will slow the response of the navigation controller and reduce twitchiness
    // @Units: Hz
    // @Range: 0.5 5
    // @Increment: 0.1
    // @User: Advanced
    AP_GROUPINFO("_ACC_XY_FILT", 1, AC_PosControl, _accel_xy_filt_hz, POSCONTROL_ACCEL_FILTER_HZ),

    // @Param: _POSZ_P
    // @DisplayName: Position (vertical) controller P gain
    // @Description: Position (vertical) controller P gain.  Converts the difference between the desired altitude and actual altitude into a climb or descent rate which is passed to the throttle rate controller
    // @Range: 1.000 3.000
    // @User: Standard
    AP_SUBGROUPINFO(_p_pos_z, "_POSZ_", 2, AC_PosControl, AC_P),

    // @Param: _VELZ_P
    // @DisplayName: Velocity (vertical) controller P gain
    // @Description: Velocity (vertical) controller P gain.  Converts the difference between desired vertical speed and actual speed into a desired acceleration that is passed to the throttle acceleration controller
    // @Range: 1.000 8.000
    // @User: Standard
    AP_SUBGROUPINFO(_p_vel_z, "_VELZ_", 3, AC_PosControl, AC_P),

    // @Param: _ACCZ_P
    // @DisplayName: Acceleration (vertical) controller P gain
    // @Description: Acceleration (vertical) controller P gain.  Converts the difference between desired vertical acceleration and actual acceleration into a motor output
    // @Range: 0.500 1.500
    // @Increment: 0.05
    // @User: Standard

    // @Param: _ACCZ_I
    // @DisplayName: Acceleration (vertical) controller I gain
    // @Description: Acceleration (vertical) controller I gain.  Corrects long-term difference in desired vertical acceleration and actual acceleration
    // @Range: 0.000 3.000
    // @User: Standard

    // @Param: _ACCZ_IMAX
    // @DisplayName: Acceleration (vertical) controller I gain maximum
    // @Description: Acceleration (vertical) controller I gain maximum.  Constrains the maximum pwm that the I term will generate
    // @Range: 0 1000
    // @Units: d%
    // @User: Standard

    // @Param: _ACCZ_D
    // @DisplayName: Acceleration (vertical) controller D gain
    // @Description: Acceleration (vertical) controller D gain.  Compensates for short-term change in desired vertical acceleration vs actual acceleration
    // @Range: 0.000 0.400
    // @User: Standard

    // @Param: _ACCZ_FILT
    // @DisplayName: Acceleration (vertical) controller filter
    // @Description: Filter applied to acceleration to reduce noise.  Lower values reduce noise but add delay.
    // @Range: 1.000 100.000
    // @Units: Hz
    // @User: Standard
    AP_SUBGROUPINFO(_pid_accel_z, "_ACCZ_", 4, AC_PosControl, AC_PID),

    // @Param: _POSXY_P
    // @DisplayName: Position (horizonal) controller P gain
    // @Description: Position controller P gain.  Converts the distance (in the latitude direction) to the target location into a desired speed which is then passed to the loiter latitude rate controller
    // @Range: 0.500 2.000
    // @User: Standard
    AP_SUBGROUPINFO(_p_pos_xy, "_POSXY_", 5, AC_PosControl, AC_P),

    // @Param: _VELXY_P
    // @DisplayName: Velocity (horizontal) P gain
    // @Description: Velocity (horizontal) P gain.  Converts the difference between desired velocity to a target acceleration
    // @Range: 0.1 6.0
    // @Increment: 0.1
    // @User: Advanced

    // @Param: _VELXY_I
    // @DisplayName: Velocity (horizontal) I gain
    // @Description: Velocity (horizontal) I gain.  Corrects long-term difference in desired velocity to a target acceleration
    // @Range: 0.02 1.00
    // @Increment: 0.01
    // @User: Advanced

    // @Param: _VELXY_D
    // @DisplayName: Velocity (horizontal) D gain
    // @Description: Velocity (horizontal) D gain.  Corrects short-term changes in velocity
    // @Range: 0.00 1.00
    // @Increment: 0.001
    // @User: Advanced

    // @Param: _VELXY_IMAX
    // @DisplayName: Velocity (horizontal) integrator maximum
    // @Description: Velocity (horizontal) integrator maximum.  Constrains the target acceleration that the I gain will output
    // @Range: 0 4500
    // @Increment: 10
    // @Units: cm/s/s
    // @User: Advanced

    // @Param: _VELXY_FILT
    // @DisplayName: Velocity (horizontal) input filter
    // @Description: Velocity (horizontal) input filter.  This filter (in hz) is applied to the input for P and I terms
    // @Range: 0 100
    // @Units: Hz
    // @User: Advanced

    // @Param: _VELXY_D_FILT
    // @DisplayName: Velocity (horizontal) input filter
    // @Description: Velocity (horizontal) input filter.  This filter (in hz) is applied to the input for P and I terms
    // @Range: 0 100
    // @Units: Hz
    // @User: Advanced
    AP_SUBGROUPINFO(_pid_vel_xy, "_VELXY_", 6, AC_PosControl, AC_PID_2D),

    // @Param: _ANGLE_MAX
    // @DisplayName: Position Control Angle Max
    // @Description: Maximum lean angle autopilot can request.  Set to zero to use ANGLE_MAX parameter value
    // @Units: deg
    // @Range: 0 45
    // @Increment: 1
    // @User: Advanced
    AP_GROUPINFO("_ANGLE_MAX",  7, AC_PosControl, _lean_angle_max, 0.0f),

    AP_GROUPEND
};

// Default constructor.
// Note that the Vector/Matrix constructors already implicitly zero
// their values.
//
AC_PosControl::AC_PosControl(const AP_AHRS_View& ahrs, const AP_InertialNav& inav,
                             const AP_Motors& motors, AC_AttitudeControl& attitude_control) :
    _ahrs(ahrs),
    _inav(inav),
    _motors(motors),
    _attitude_control(attitude_control),
    _p_pos_z(POSCONTROL_POS_Z_P),
    _p_vel_z(POSCONTROL_VEL_Z_P),
    _pid_accel_z(POSCONTROL_ACC_Z_P, POSCONTROL_ACC_Z_I, POSCONTROL_ACC_Z_D, POSCONTROL_ACC_Z_IMAX, POSCONTROL_ACC_Z_FILT_HZ, POSCONTROL_ACC_Z_DT),
    _p_pos_xy(POSCONTROL_POS_XY_P),
    _pid_vel_xy(POSCONTROL_VEL_XY_P, POSCONTROL_VEL_XY_I, POSCONTROL_VEL_XY_D, POSCONTROL_VEL_XY_IMAX, POSCONTROL_VEL_XY_FILT_HZ, POSCONTROL_VEL_XY_FILT_D_HZ, POSCONTROL_DT_50HZ),
    _dt(POSCONTROL_DT_400HZ),
    _last_update_xy_ms(0),
    _last_update_z_ms(0),
    _speed_down_cms(POSCONTROL_SPEED_DOWN),
    _speed_up_cms(POSCONTROL_SPEED_UP),
    _speed_cms(POSCONTROL_SPEED),
    _accel_z_cms(POSCONTROL_ACCEL_Z),
    _accel_last_z_cms(0.0f),
    _accel_cms(POSCONTROL_ACCEL_XY),
    _leash(POSCONTROL_LEASH_LENGTH_MIN),
    _leash_down_z(POSCONTROL_LEASH_LENGTH_MIN),
    _leash_up_z(POSCONTROL_LEASH_LENGTH_MIN),
    _roll_target(0.0f),
    _pitch_target(0.0f),
    _distance_to_target(0.0f),
    _accel_target_filter(POSCONTROL_ACCEL_FILTER_HZ)
{
    AP_Param::setup_object_defaults(this, var_info);

    // initialise flags
    _flags.recalc_leash_z = true;
    _flags.recalc_leash_xy = true;
    _flags.reset_desired_vel_to_pos = true;
    _flags.reset_accel_to_lean_xy = true;
    _flags.reset_rate_to_accel_z = true;
    _flags.reset_accel_to_throttle = true;
    _flags.freeze_ff_z = true;
    _flags.use_desvel_ff_z = true;
    _limit.pos_up = true;
    _limit.pos_down = true;
    _limit.vel_up = true;
    _limit.vel_down = true;
    _limit.accel_xy = true;
}

///
/// z-axis position controller
///


/// set_dt - sets time delta in seconds for all controllers (i.e. 100hz = 0.01, 400hz = 0.0025)
void AC_PosControl::set_dt(float delta_sec)
{
    _dt = delta_sec;

    // update PID controller dt
    _pid_accel_z.set_dt(_dt);
    _pid_vel_xy.set_dt(_dt);

    // update rate z-axis velocity error and accel error filters
    _vel_error_filter.set_cutoff_frequency(POSCONTROL_VEL_ERROR_CUTOFF_FREQ);
}

/// set_speed_z - sets maximum climb and descent rates
/// To-Do: call this in the main code as part of flight mode initialisation
///     calc_leash_length_z should be called afterwards
///     speed_down should be a negative number
void AC_PosControl::set_speed_z(float speed_down, float speed_up)
{
    // ensure speed_down is always negative
    speed_down = -fabsf(speed_down);

    if ((fabsf(_speed_down_cms-speed_down) > 1.0f) || (fabsf(_speed_up_cms-speed_up) > 1.0f)) {
        _speed_down_cms = speed_down;
        _speed_up_cms = speed_up;
        _flags.recalc_leash_z = true;
        calc_leash_length_z();
    }
}

/// set_accel_z - set vertical acceleration in cm/s/s
void AC_PosControl::set_accel_z(float accel_cmss)
{
    if (fabsf(_accel_z_cms-accel_cmss) > 1.0f) {
        _accel_z_cms = accel_cmss;
        _flags.recalc_leash_z = true;
        calc_leash_length_z();
    }
}

/// set_alt_target_with_slew - adjusts target towards a final altitude target
///     should be called continuously (with dt set to be the expected time between calls)
///     actual position target will be moved no faster than the speed_down and speed_up
///     target will also be stopped if the motors hit their limits or leash length is exceeded
void AC_PosControl::set_alt_target_with_slew(float alt_cm, float dt)
{
    float alt_change = alt_cm-_pos_target.z;

    // do not use z-axis desired velocity feed forward
    _flags.use_desvel_ff_z = false;

    // adjust desired alt if motors have not hit their limits
    if ((alt_change<0 && !_motors.limit.throttle_lower) || (alt_change>0 && !_motors.limit.throttle_upper)) {
        if (!is_zero(dt)) {
            float climb_rate_cms = constrain_float(alt_change/dt, _speed_down_cms, _speed_up_cms);
            _pos_target.z += climb_rate_cms*dt;
            _vel_desired.z = climb_rate_cms;    // recorded for reporting purposes
        }
    }

    // do not let target get too far from current altitude
    float curr_alt = _inav.get_altitude();
    _pos_target.z = constrain_float(_pos_target.z,curr_alt-_leash_down_z,curr_alt+_leash_up_z);
}


/// set_alt_target_from_climb_rate - adjusts target up or down using a climb rate in cm/s
///     should be called continuously (with dt set to be the expected time between calls)
///     actual position target will be moved no faster than the speed_down and speed_up
///     target will also be stopped if the motors hit their limits or leash length is exceeded
void AC_PosControl::set_alt_target_from_climb_rate(float climb_rate_cms, float dt, bool force_descend)
{
    // adjust desired alt if motors have not hit their limits
    // To-Do: add check of _limit.pos_down?
    if ((climb_rate_cms<0 && (!_motors.limit.throttle_lower || force_descend)) || (climb_rate_cms>0 && !_motors.limit.throttle_upper && !_limit.pos_up)) {
        _pos_target.z += climb_rate_cms * dt;
    }

    // do not use z-axis desired velocity feed forward
    // vel_desired set to desired climb rate for reporting and land-detector
    _flags.use_desvel_ff_z = false;
    _vel_desired.z = climb_rate_cms;
}

/// set_alt_target_from_climb_rate_ff - adjusts target up or down using a climb rate in cm/s using feed-forward
///     should be called continuously (with dt set to be the expected time between calls)
///     actual position target will be moved no faster than the speed_down and speed_up
///     target will also be stopped if the motors hit their limits or leash length is exceeded
///     set force_descend to true during landing to allow target to move low enough to slow the motors
void AC_PosControl::set_alt_target_from_climb_rate_ff(float climb_rate_cms, float dt, bool force_descend)
{
    // calculated increased maximum acceleration if over speed
    float accel_z_cms = _accel_z_cms;
    if (_vel_desired.z < _speed_down_cms && !is_zero(_speed_down_cms)) {
        accel_z_cms *= POSCONTROL_OVERSPEED_GAIN_Z * _vel_desired.z / _speed_down_cms;
    }
    if (_vel_desired.z > _speed_up_cms && !is_zero(_speed_up_cms)) {
        accel_z_cms *= POSCONTROL_OVERSPEED_GAIN_Z * _vel_desired.z / _speed_up_cms;
    }
    accel_z_cms = constrain_float(accel_z_cms, 0.0f, 750.0f);

    // jerk_z is calculated to reach full acceleration in 1000ms.
    float jerk_z = accel_z_cms * POSCONTROL_JERK_RATIO;

    float accel_z_max = MIN(accel_z_cms, safe_sqrt(2.0f*fabsf(_vel_desired.z - climb_rate_cms)*jerk_z));

    _accel_last_z_cms += jerk_z * dt;
    _accel_last_z_cms = MIN(accel_z_max, _accel_last_z_cms);

    float vel_change_limit = _accel_last_z_cms * dt;
    _vel_desired.z = constrain_float(climb_rate_cms, _vel_desired.z-vel_change_limit, _vel_desired.z+vel_change_limit);
    _flags.use_desvel_ff_z = true;

    // adjust desired alt if motors have not hit their limits
    // To-Do: add check of _limit.pos_down?
    if ((_vel_desired.z<0 && (!_motors.limit.throttle_lower || force_descend)) || (_vel_desired.z>0 && !_motors.limit.throttle_upper && !_limit.pos_up)) {
        _pos_target.z += _vel_desired.z * dt;
    }
}

/// add_takeoff_climb_rate - adjusts alt target up or down using a climb rate in cm/s
///     should be called continuously (with dt set to be the expected time between calls)
///     almost no checks are performed on the input
void AC_PosControl::add_takeoff_climb_rate(float climb_rate_cms, float dt)
{
    _pos_target.z += climb_rate_cms * dt;
}

/// shift altitude target (positive means move altitude up)
void AC_PosControl::shift_alt_target(float z_cm)
{
    _pos_target.z += z_cm;

    // freeze feedforward to avoid jump
    if (!is_zero(z_cm)) {
        freeze_ff_z();
    }
}

/// relax_alt_hold_controllers - set all desired and targets to measured
void AC_PosControl::relax_alt_hold_controllers(float throttle_setting)
{
    _pos_target.z = _inav.get_altitude();
    _vel_desired.z = 0.0f;
    _flags.use_desvel_ff_z = false;
    _vel_target.z = _inav.get_velocity_z();
    _vel_last.z = _inav.get_velocity_z();
    _accel_desired.z = 0.0f;
    _accel_last_z_cms = 0.0f;
    _accel_target.z = -(_ahrs.get_accel_ef_blended().z + GRAVITY_MSS) * 100.0f;
    _flags.reset_accel_to_throttle = true;
    _pid_accel_z.set_integrator((throttle_setting-_motors.get_throttle_hover())*1000.0f);
}

// get_alt_error - returns altitude error in cm
float AC_PosControl::get_alt_error() const
{
    return (_pos_target.z - _inav.get_altitude());
}

/// set_target_to_stopping_point_z - returns reasonable stopping altitude in cm above home
void AC_PosControl::set_target_to_stopping_point_z()
{
    // check if z leash needs to be recalculated
    calc_leash_length_z();

    get_stopping_point_z(_pos_target);
}

/// get_stopping_point_z - calculates stopping point based on current position, velocity, vehicle acceleration
void AC_PosControl::get_stopping_point_z(Vector3f& stopping_point) const
{
    const float curr_pos_z = _inav.get_altitude();
    float curr_vel_z = _inav.get_velocity_z();

    float linear_distance;  // half the distance we swap between linear and sqrt and the distance we offset sqrt
    float linear_velocity;  // the velocity we swap between linear and sqrt

    // if position controller is active add current velocity error to avoid sudden jump in acceleration
    if (is_active_z()) {
        curr_vel_z += _vel_error.z;
        if (_flags.use_desvel_ff_z) {
            curr_vel_z -= _vel_desired.z;
        }
    }

    // avoid divide by zero by using current position if kP is very low or acceleration is zero
    if (_p_pos_z.kP() <= 0.0f || _accel_z_cms <= 0.0f) {
        stopping_point.z = curr_pos_z;
        return;
    }

    // calculate the velocity at which we switch from calculating the stopping point using a linear function to a sqrt function
    linear_velocity = _accel_z_cms/_p_pos_z.kP();

    if (fabsf(curr_vel_z) < linear_velocity) {
        // if our current velocity is below the cross-over point we use a linear function
        stopping_point.z = curr_pos_z + curr_vel_z/_p_pos_z.kP();
    } else {
        linear_distance = _accel_z_cms/(2.0f*_p_pos_z.kP()*_p_pos_z.kP());
        if (curr_vel_z > 0){
            stopping_point.z = curr_pos_z + (linear_distance + curr_vel_z*curr_vel_z/(2.0f*_accel_z_cms));
        } else {
            stopping_point.z = curr_pos_z - (linear_distance + curr_vel_z*curr_vel_z/(2.0f*_accel_z_cms));
        }
    }
    stopping_point.z = constrain_float(stopping_point.z, curr_pos_z - POSCONTROL_STOPPING_DIST_DOWN_MAX, curr_pos_z + POSCONTROL_STOPPING_DIST_UP_MAX);
}

/// init_takeoff - initialises target altitude if we are taking off
void AC_PosControl::init_takeoff()
{
    const Vector3f& curr_pos = _inav.get_position();

    _pos_target.z = curr_pos.z;

    // freeze feedforward to avoid jump
    freeze_ff_z();

    // shift difference between last motor out and hover throttle into accelerometer I
    _pid_accel_z.set_integrator((_motors.get_throttle()-_motors.get_throttle_hover())*1000.0f);

    // initialise ekf reset handler
    init_ekf_z_reset();
}

// is_active_z - returns true if the z-axis position controller has been run very recently
bool AC_PosControl::is_active_z() const
{
    return ((AP_HAL::millis() - _last_update_z_ms) <= POSCONTROL_ACTIVE_TIMEOUT_MS);
}

/// update_z_controller - fly to altitude in cm above home
void AC_PosControl::update_z_controller()
{
    // check time since last cast
    uint32_t now = AP_HAL::millis();
    if (now - _last_update_z_ms > POSCONTROL_ACTIVE_TIMEOUT_MS) {
        _flags.reset_rate_to_accel_z = true;
        _flags.reset_accel_to_throttle = true;
    }
    _last_update_z_ms = now;

    // check for ekf altitude reset
    check_for_ekf_z_reset();

    // check if leash lengths need to be recalculated
    calc_leash_length_z();

    // call z-axis position controller
    run_z_controller();
}

/// calc_leash_length - calculates the vertical leash lengths from maximum speed, acceleration
///     called by update_z_controller if z-axis speed or accelerations are changed
void AC_PosControl::calc_leash_length_z()
{
    if (_flags.recalc_leash_z) {
        _leash_up_z = calc_leash_length(_speed_up_cms, _accel_z_cms, _p_pos_z.kP());
        _leash_down_z = calc_leash_length(-_speed_down_cms, _accel_z_cms, _p_pos_z.kP());
        _flags.recalc_leash_z = false;
    }
}

// run position control for Z axis
// target altitude should be set with one of these functions: set_alt_target, set_target_to_stopping_point_z, init_takeoff
// calculates desired rate in earth-frame z axis and passes to rate controller
// vel_up_max, vel_down_max should have already been set before calling this method
void AC_PosControl::run_z_controller()
{
    float curr_alt = _inav.get_altitude();

    // clear position limit flags
    _limit.pos_up = false;
    _limit.pos_down = false;

    // calculate altitude error
    _pos_error.z = _pos_target.z - curr_alt;

    // do not let target altitude get too far from current altitude
    if (_pos_error.z > _leash_up_z) {
        _pos_target.z = curr_alt + _leash_up_z;
        _pos_error.z = _leash_up_z;
        _limit.pos_up = true;
    }
    if (_pos_error.z < -_leash_down_z) {
        _pos_target.z = curr_alt - _leash_down_z;
        _pos_error.z = -_leash_down_z;
        _limit.pos_down = true;
    }

    // calculate _vel_target.z using from _pos_error.z using sqrt controller
    _vel_target.z = AC_AttitudeControl::sqrt_controller(_pos_error.z, _p_pos_z.kP(), _accel_z_cms, _dt);

    // check speed limits
    // To-Do: check these speed limits here or in the pos->rate controller
    _limit.vel_up = false;
    _limit.vel_down = false;
    if (_vel_target.z < _speed_down_cms) {
        _vel_target.z = _speed_down_cms;
        _limit.vel_down = true;
    }
    if (_vel_target.z > _speed_up_cms) {
        _vel_target.z = _speed_up_cms;
        _limit.vel_up = true;
    }

    // add feed forward component
    if (_flags.use_desvel_ff_z) {
        _vel_target.z += _vel_desired.z;
    }

    // the following section calculates acceleration required to achieve the velocity target

    const Vector3f& curr_vel = _inav.get_velocity();

    // TODO: remove velocity derivative calculation
    // reset last velocity target to current target
    if (_flags.reset_rate_to_accel_z) {
        _vel_last.z = _vel_target.z;
    }

    // feed forward desired acceleration calculation
    if (_dt > 0.0f) {
            if (!_flags.freeze_ff_z) {
                _accel_desired.z = (_vel_target.z - _vel_last.z)/_dt;
        } else {
                    // stop the feed forward being calculated during a known discontinuity
                    _flags.freeze_ff_z = false;
            }
    } else {
        _accel_desired.z = 0.0f;
    }

    // store this iteration's velocities for the next iteration
    _vel_last.z = _vel_target.z;

    // reset velocity error and filter if this controller has just been engaged
    if (_flags.reset_rate_to_accel_z) {
        // Reset Filter
        _vel_error.z = 0;
        _vel_error_filter.reset(0);
        _flags.reset_rate_to_accel_z = false;
    } else {
        // calculate rate error and filter with cut off frequency of 2 Hz
        _vel_error.z = _vel_error_filter.apply(_vel_target.z - curr_vel.z, _dt);
    }

    _accel_target.z = _p_vel_z.get_p(_vel_error.z);

    _accel_target.z += _accel_desired.z;


    // the following section calculates a desired throttle needed to achieve the acceleration target
    float z_accel_meas;         // actual acceleration
    float p,i,d;              // used to capture pid values for logging

    // Calculate Earth Frame Z acceleration
    z_accel_meas = -(_ahrs.get_accel_ef_blended().z + GRAVITY_MSS) * 100.0f;

    // reset target altitude if this controller has just been engaged
    if (_flags.reset_accel_to_throttle) {
        // Reset Filter
        _accel_error.z = 0;
        _flags.reset_accel_to_throttle = false;
    } else {
        // calculate accel error
        _accel_error.z = _accel_target.z - z_accel_meas;
    }

    // set input to PID
    _pid_accel_z.set_input_filter_all(_accel_error.z);
    _pid_accel_z.set_desired_rate(_accel_target.z);

    // separately calculate p, i, d values for logging
    p = _pid_accel_z.get_p();

    // get i term
    i = _pid_accel_z.get_integrator();

    // ensure imax is always large enough to overpower hover throttle
    if (_motors.get_throttle_hover() * 1000.0f > _pid_accel_z.imax()) {
        _pid_accel_z.imax(_motors.get_throttle_hover() * 1000.0f);
    }

    // update i term as long as we haven't breached the limits or the I term will certainly reduce
    // To-Do: should this be replaced with limits check from attitude_controller?
    if ((!_motors.limit.throttle_lower && !_motors.limit.throttle_upper) || (i>0&&_accel_error.z<0) || (i<0&&_accel_error.z>0)) {
        i = _pid_accel_z.get_i();
    }

    // get d term
    d = _pid_accel_z.get_d();

    float thr_out = (p+i+d)*0.001f +_motors.get_throttle_hover();

    // send throttle to attitude controller with angle boost
    _attitude_control.set_throttle_out(thr_out, true, POSCONTROL_THROTTLE_CUTOFF_FREQ);
}

///
/// lateral position controller
///

/// set_accel_xy - set horizontal acceleration in cm/s/s
void AC_PosControl::set_accel_xy(float accel_cmss)
{
    if (fabsf(_accel_cms-accel_cmss) > 1.0f) {
        _accel_cms = accel_cmss;
        _flags.recalc_leash_xy = true;
        calc_leash_length_xy();
    }
}

/// set_speed_xy - set horizontal speed maximum in cm/s
void AC_PosControl::set_speed_xy(float speed_cms)
{
    if (fabsf(_speed_cms-speed_cms) > 1.0f) {
        _speed_cms = speed_cms;
        _flags.recalc_leash_xy = true;
        calc_leash_length_xy();
    }
}

/// set_pos_target in cm from home
void AC_PosControl::set_pos_target(const Vector3f& position)
{
    _pos_target = position;

    _flags.use_desvel_ff_z = false;
    _vel_desired.z = 0.0f;
    // initialise roll and pitch to current roll and pitch.  This avoids a twitch between when the target is set and the pos controller is first run
    // To-Do: this initialisation of roll and pitch targets needs to go somewhere between when pos-control is initialised and when it completes it's first cycle
    //_roll_target = constrain_int32(_ahrs.roll_sensor,-_attitude_control.lean_angle_max(),_attitude_control.lean_angle_max());
    //_pitch_target = constrain_int32(_ahrs.pitch_sensor,-_attitude_control.lean_angle_max(),_attitude_control.lean_angle_max());
}

/// set_xy_target in cm from home
void AC_PosControl::set_xy_target(float x, float y)
{
    _pos_target.x = x;
    _pos_target.y = y;
}

/// shift position target target in x, y axis
void AC_PosControl::shift_pos_xy_target(float x_cm, float y_cm)
{
    // move pos controller target
    _pos_target.x += x_cm;
    _pos_target.y += y_cm;
}

/// set_target_to_stopping_point_xy - sets horizontal target to reasonable stopping position in cm from home
void AC_PosControl::set_target_to_stopping_point_xy()
{
    // check if xy leash needs to be recalculated
    calc_leash_length_xy();

    get_stopping_point_xy(_pos_target);
}

/// get_stopping_point_xy - calculates stopping point based on current position, velocity, vehicle acceleration
///     distance_max allows limiting distance to stopping point
///     results placed in stopping_position vector
///     set_accel_xy() should be called before this method to set vehicle acceleration
///     set_leash_length() should have been called before this method
void AC_PosControl::get_stopping_point_xy(Vector3f &stopping_point) const
{
    const Vector3f curr_pos = _inav.get_position();
    Vector3f curr_vel = _inav.get_velocity();
    float linear_distance;      // the distance at which we swap from a linear to sqrt response
    float linear_velocity;      // the velocity above which we swap from a linear to sqrt response
    float stopping_dist;                // the distance within the vehicle can stop
    float kP = _p_pos_xy.kP();

    // add velocity error to current velocity
    if (is_active_xy()) {
        curr_vel.x += _vel_error.x;
        curr_vel.y += _vel_error.y;
    }

    // calculate current velocity
    float vel_total = norm(curr_vel.x, curr_vel.y);

    // avoid divide by zero by using current position if the velocity is below 10cm/s, kP is very low or acceleration is zero
    if (kP <= 0.0f || _accel_cms <= 0.0f || is_zero(vel_total)) {
        stopping_point.x = curr_pos.x;
        stopping_point.y = curr_pos.y;
        return;
    }

    // calculate point at which velocity switches from linear to sqrt
    linear_velocity = _accel_cms/kP;

    // calculate distance within which we can stop
    if (vel_total < linear_velocity) {
            stopping_dist = vel_total/kP;
    } else {
        linear_distance = _accel_cms/(2.0f*kP*kP);
        stopping_dist = linear_distance + (vel_total*vel_total)/(2.0f*_accel_cms);
    }

    // constrain stopping distance
    stopping_dist = constrain_float(stopping_dist, 0, _leash);

    // convert the stopping distance into a stopping point using velocity vector
    stopping_point.x = curr_pos.x + (stopping_dist * curr_vel.x / vel_total);
    stopping_point.y = curr_pos.y + (stopping_dist * curr_vel.y / vel_total);
}

/// get_distance_to_target - get horizontal distance to target position in cm
float AC_PosControl::get_distance_to_target() const
{
    return _distance_to_target;
}

/// get_bearing_to_target - get bearing to target position in centi-degrees
int32_t AC_PosControl::get_bearing_to_target() const
{
    return get_bearing_cd(_inav.get_position(), _pos_target);
}

// is_active_xy - returns true if the xy position controller has been run very recently
bool AC_PosControl::is_active_xy() const
{
    return ((AP_HAL::millis() - _last_update_xy_ms) <= POSCONTROL_ACTIVE_TIMEOUT_MS);
}

/// get_lean_angle_max_cd - returns the maximum lean angle the autopilot may request
float AC_PosControl::get_lean_angle_max_cd() const
{
    if (is_zero(_lean_angle_max)) {
        return _attitude_control.lean_angle_max();
    }
    return _lean_angle_max * 100.0f;
}

/// init_xy_controller - initialise the xy controller
///     sets target roll angle, pitch angle and I terms based on vehicle current lean angles
///     should be called once whenever significant changes to the position target are made
///     this does not update the xy target
void AC_PosControl::init_xy_controller(bool reset_I)
{
    // set roll, pitch lean angle targets to current attitude
    _roll_target = _ahrs.roll_sensor;
    _pitch_target = _ahrs.pitch_sensor;

    // initialise I terms from lean angles
    if (reset_I) {
        // reset last velocity if this controller has just been engaged or dt is zero
        lean_angles_to_accel(_accel_target.x, _accel_target.y);
        _pid_vel_xy.set_integrator(_accel_target-_accel_desired);
    }

    // flag reset required in rate to accel step
    _flags.reset_desired_vel_to_pos = true;
    _flags.reset_accel_to_lean_xy = true;

    // initialise ekf xy reset handler
    init_ekf_xy_reset();
}

/// update_xy_controller - run the horizontal position controller - should be called at 100hz or higher
void AC_PosControl::update_xy_controller(float ekfNavVelGainScaler)
{
    // compute dt
    uint32_t now = AP_HAL::millis();
    float dt = (now - _last_update_xy_ms)*0.001f;

    // sanity check dt
    if (dt >= POSCONTROL_ACTIVE_TIMEOUT_MS*1.0e-3f) {
        dt = 0.0f;
    }

    // check for ekf xy position reset
    check_for_ekf_xy_reset();

    // check if xy leash needs to be recalculated
    calc_leash_length_xy();

    // translate any adjustments from pilot to loiter target
    desired_vel_to_pos(dt);

    // run horizontal position controller
    run_xy_controller(dt, ekfNavVelGainScaler);

    // update xy update time
    _last_update_xy_ms = now;
}

float AC_PosControl::time_since_last_xy_update() const
{
    uint32_t now = AP_HAL::millis();
    return (now - _last_update_xy_ms)*0.001f;
}

// write log to dataflash
void AC_PosControl::write_log()
{
    const Vector3f &pos_target = get_pos_target();
    const Vector3f &vel_target = get_vel_target();
    const Vector3f &accel_target = get_accel_target();
    const Vector3f &position = _inav.get_position();
    const Vector3f &velocity = _inav.get_velocity();
    float accel_x, accel_y;
    lean_angles_to_accel(accel_x, accel_y);

    DataFlash_Class::instance()->Log_Write("PSC", "TimeUS,TPX,TPY,PX,PY,TVX,TVY,VX,VY,TAX,TAY,AX,AY",
                                           "smmmmnnnnoooo", "FBBBBBBBBBBBB", "Qffffffffffff",
                                           AP_HAL::micros64(),
                                           (double)pos_target.x,
                                           (double)pos_target.y,
                                           (double)position.x,
                                           (double)position.y,
                                           (double)vel_target.x,
                                           (double)vel_target.y,
                                           (double)velocity.x,
                                           (double)velocity.y,
                                           (double)accel_target.x,
                                           (double)accel_target.y,
                                           (double)accel_x,
                                           (double)accel_y);
}

/// init_vel_controller_xyz - initialise the velocity controller - should be called once before the caller attempts to use the controller
void AC_PosControl::init_vel_controller_xyz()
{
    // set roll, pitch lean angle targets to current attitude
    _roll_target = _ahrs.roll_sensor;
    _pitch_target = _ahrs.pitch_sensor;

    // reset last velocity if this controller has just been engaged or dt is zero
    lean_angles_to_accel(_accel_target.x, _accel_target.y);
    _pid_vel_xy.set_integrator(_accel_target);

    // flag reset required in rate to accel step
    _flags.reset_desired_vel_to_pos = true;
    _flags.reset_accel_to_lean_xy = true;

    // set target position
    const Vector3f& curr_pos = _inav.get_position();
    set_xy_target(curr_pos.x, curr_pos.y);
    set_alt_target(curr_pos.z);

    // move current vehicle velocity into feed forward velocity
    const Vector3f& curr_vel = _inav.get_velocity();
    set_desired_velocity(curr_vel);

    // set vehicle acceleration to zero
    set_desired_accel_xy(0.0f,0.0f);

    // initialise ekf reset handlers
    init_ekf_xy_reset();
    init_ekf_z_reset();
}

/// update_velocity_controller_xy - run the velocity controller - should be called at 100hz or higher
///     velocity targets should we set using set_desired_velocity_xy() method
///     callers should use get_roll() and get_pitch() methods and sent to the attitude controller
///     throttle targets will be sent directly to the motors
void AC_PosControl::update_vel_controller_xy(float ekfNavVelGainScaler)
{
    // capture time since last iteration
    uint32_t now = AP_HAL::millis();
    float dt = (now - _last_update_xy_ms)*0.001f;

    // sanity check dt
    if (dt >= 0.2f) {
        dt = 0.0f;
    }

    // check for ekf xy position reset
    check_for_ekf_xy_reset();

    // check if xy leash needs to be recalculated
    calc_leash_length_xy();

    // apply desired velocity request to position target
    // TODO: this will need to be removed and added to the calling function.
    desired_vel_to_pos(dt);

    // run position controller
    run_xy_controller(dt, ekfNavVelGainScaler);

    // update xy update time
    _last_update_xy_ms = now;
}


/// update_velocity_controller_xyz - run the velocity controller - should be called at 100hz or higher
///     velocity targets should we set using set_desired_velocity_xyz() method
///     callers should use get_roll() and get_pitch() methods and sent to the attitude controller
///     throttle targets will be sent directly to the motors
void AC_PosControl::update_vel_controller_xyz(float ekfNavVelGainScaler)
{
    update_vel_controller_xy(ekfNavVelGainScaler);

    // update altitude target
    set_alt_target_from_climb_rate_ff(_vel_desired.z, _dt, false);

    // run z-axis position controller
    update_z_controller();
}

float AC_PosControl::get_horizontal_error() const
{
    return norm(_pos_error.x, _pos_error.y);
}

///
/// private methods
///

/// calc_leash_length - calculates the horizontal leash length given a maximum speed, acceleration
///     should be called whenever the speed, acceleration or position kP is modified
void AC_PosControl::calc_leash_length_xy()
{
    // todo: remove _flags.recalc_leash_xy or don't call this function after each variable change.
    if (_flags.recalc_leash_xy) {
        _leash = calc_leash_length(_speed_cms, _accel_cms, _p_pos_xy.kP());
        _flags.recalc_leash_xy = false;
    }
}

/// move velocity target using desired acceleration
void AC_PosControl::desired_accel_to_vel(float nav_dt)
{
    // range check nav_dt
    if (nav_dt < 0) {
        return;
    }

    // update target velocity
    if (_flags.reset_desired_vel_to_pos) {
        _flags.reset_desired_vel_to_pos = false;
    } else {
        _vel_desired.x += _accel_desired.x * nav_dt;
        _vel_desired.y += _accel_desired.y * nav_dt;
    }
}

/// desired_vel_to_pos - move position target using desired velocities
void AC_PosControl::desired_vel_to_pos(float nav_dt)
{
    // range check nav_dt
    if( nav_dt < 0 ) {
        return;
    }

    // update target position
    if (_flags.reset_desired_vel_to_pos) {
        _flags.reset_desired_vel_to_pos = false;
    } else {
        _pos_target.x += _vel_desired.x * nav_dt;
        _pos_target.y += _vel_desired.y * nav_dt;
    }
}

/// run horizontal position controller correcting position and velocity
///     converts position (_pos_target) to target velocity (_vel_target)
///     desired velocity (_vel_desired) is combined into final target velocity
///     converts desired velocities in lat/lon directions to accelerations in lat/lon frame
///     converts desired accelerations provided in lat/lon frame to roll/pitch angles
void AC_PosControl::run_xy_controller(float dt, float ekfNavVelGainScaler)
{
    Vector3f curr_pos = _inav.get_position();
    float kP = ekfNavVelGainScaler * _p_pos_xy.kP(); // scale gains to compensate for noisy optical flow measurement in the EKF

    // avoid divide by zero
    if (kP <= 0.0f) {
        _vel_target.x = 0.0f;
        _vel_target.y = 0.0f;
    }else{
        // calculate distance error
        _pos_error.x = _pos_target.x - curr_pos.x;
        _pos_error.y = _pos_target.y - curr_pos.y;

        // Constrain _pos_error and target position
        // Constrain the maximum length of _vel_target to the maximum position correction velocity
        // TODO: replace the leash length with a user definable maximum position correction
        if (limit_vector_length(_pos_error.x, _pos_error.y, _leash))
        {
            _pos_target.x = curr_pos.x + _pos_error.x;
            _pos_target.y = curr_pos.y + _pos_error.y;
        }
        _distance_to_target = norm(_pos_error.x, _pos_error.y);

        _vel_target = sqrt_controller(_pos_error, kP, _accel_cms);
    }

    // add velocity feed-forward
    _vel_target.x += _vel_desired.x;
    _vel_target.y += _vel_desired.y;

    // the following section converts desired velocities in lat/lon directions to accelerations in lat/lon frame

    Vector2f accel_target, vel_xy_p, vel_xy_i, vel_xy_d;

    // check if vehicle velocity is being overridden
    if (_flags.vehicle_horiz_vel_override) {
        _flags.vehicle_horiz_vel_override = false;
    } else {
        _vehicle_horiz_vel.x = _inav.get_velocity().x;
        _vehicle_horiz_vel.y = _inav.get_velocity().y;
    }

    // calculate velocity error
    _vel_error.x = _vel_target.x - _vehicle_horiz_vel.x;
    _vel_error.y = _vel_target.y - _vehicle_horiz_vel.y;
    // TODO: constrain velocity error and velocity target

    // call pi controller
    _pid_vel_xy.set_input(_vel_error);

    // get p
    vel_xy_p = _pid_vel_xy.get_p();

    // update i term if we have not hit the accel or throttle limits OR the i term will reduce
    // TODO: move limit handling into the PI and PID controller
    if (!_limit.accel_xy && !_motors.limit.throttle_upper) {
        vel_xy_i = _pid_vel_xy.get_i();
    } else {
        vel_xy_i = _pid_vel_xy.get_i_shrink();
    }

    // get d
    vel_xy_d = _pid_vel_xy.get_d();


    // acceleration to correct for velocity error and scale PID output to compensate for optical flow measurement induced EKF noise
    accel_target.x = (vel_xy_p.x + vel_xy_i.x + vel_xy_d.x) * ekfNavVelGainScaler;
    accel_target.y = (vel_xy_p.y + vel_xy_i.y + vel_xy_d.y) * ekfNavVelGainScaler;

    // reset accel to current desired acceleration
     if (_flags.reset_accel_to_lean_xy) {
         _accel_target_filter.reset(Vector2f(accel_target.x, accel_target.y));
         _flags.reset_accel_to_lean_xy = false;
     }

    // filter correction acceleration
    _accel_target_filter.set_cutoff_frequency(MIN(_accel_xy_filt_hz, 5.0f*ekfNavVelGainScaler));
    _accel_target_filter.apply(accel_target, dt);

    // pass the correction acceleration to the target acceleration output
    _accel_target.x = _accel_target_filter.get().x;
    _accel_target.y = _accel_target_filter.get().y;

    // Add feed forward into the target acceleration output
    _accel_target.x += _accel_desired.x;
    _accel_target.y += _accel_desired.y;

    // the following section converts desired accelerations provided in lat/lon frame to roll/pitch angles

    // limit acceleration using maximum lean angles
    float angle_max = MIN(_attitude_control.get_althold_lean_angle_max(), get_lean_angle_max_cd());
    float accel_max = MIN(GRAVITY_MSS * 100.0f * tanf(ToRad(angle_max * 0.01f)), POSCONTROL_ACCEL_XY_MAX);
    _limit.accel_xy = limit_vector_length(_accel_target.x, _accel_target.y, accel_max);

    // update angle targets that will be passed to stabilize controller
    accel_to_lean_angles(_accel_target.x, _accel_target.y, _roll_target, _pitch_target);
}

// get_lean_angles_to_accel - convert roll, pitch lean angles to lat/lon frame accelerations in cm/s/s
void AC_PosControl::accel_to_lean_angles(float accel_x_cmss, float accel_y_cmss, float& roll_target, float& pitch_target) const
{
    float accel_right, accel_forward;

    // rotate accelerations into body forward-right frame
    accel_forward = accel_x_cmss*_ahrs.cos_yaw() + accel_y_cmss*_ahrs.sin_yaw();
    accel_right = -accel_x_cmss*_ahrs.sin_yaw() + accel_y_cmss*_ahrs.cos_yaw();

    // update angle targets that will be passed to stabilize controller
    pitch_target = atanf(-accel_forward/(GRAVITY_MSS * 100.0f))*(18000.0f/M_PI);
    float cos_pitch_target = cosf(pitch_target*M_PI/18000.0f);
    roll_target = atanf(accel_right*cos_pitch_target/(GRAVITY_MSS * 100.0f))*(18000.0f/M_PI);
}

// get_lean_angles_to_accel - convert roll, pitch lean angles to lat/lon frame accelerations in cm/s/s
void AC_PosControl::lean_angles_to_accel(float& accel_x_cmss, float& accel_y_cmss) const
{
    // rotate our roll, pitch angles into lat/lon frame
    accel_x_cmss = (GRAVITY_MSS * 100) * (-_ahrs.cos_yaw() * _ahrs.sin_pitch() * _ahrs.cos_roll() - _ahrs.sin_yaw() * _ahrs.sin_roll()) / MAX(_ahrs.cos_roll()*_ahrs.cos_pitch(), 0.5f);
    accel_y_cmss = (GRAVITY_MSS * 100) * (-_ahrs.sin_yaw() * _ahrs.sin_pitch() * _ahrs.cos_roll() + _ahrs.cos_yaw() * _ahrs.sin_roll()) / MAX(_ahrs.cos_roll()*_ahrs.cos_pitch(), 0.5f);
}

/// calc_leash_length - calculates the horizontal leash length given a maximum speed, acceleration and position kP gain
float AC_PosControl::calc_leash_length(float speed_cms, float accel_cms, float kP) const
{
    float leash_length;

    // sanity check acceleration and avoid divide by zero
    if (accel_cms <= 0.0f) {
        accel_cms = POSCONTROL_ACCELERATION_MIN;
    }

    // avoid divide by zero
    if (kP <= 0.0f) {
        return POSCONTROL_LEASH_LENGTH_MIN;
    }

    // calculate leash length
    if(speed_cms <= accel_cms / kP) {
        // linear leash length based on speed close in
        leash_length = speed_cms / kP;
    }else{
        // leash length grows at sqrt of speed further out
        leash_length = (accel_cms / (2.0f*kP*kP)) + (speed_cms*speed_cms / (2.0f*accel_cms));
    }

    // ensure leash is at least 1m long
    if( leash_length < POSCONTROL_LEASH_LENGTH_MIN ) {
        leash_length = POSCONTROL_LEASH_LENGTH_MIN;
    }

    return leash_length;
}

/// initialise ekf xy position reset check
void AC_PosControl::init_ekf_xy_reset()
{
    Vector2f pos_shift;
    _ekf_xy_reset_ms = _ahrs.getLastPosNorthEastReset(pos_shift);
}

/// check for ekf position reset and adjust loiter or brake target position
void AC_PosControl::check_for_ekf_xy_reset()
{
    // check for position shift
    Vector2f pos_shift;
    uint32_t reset_ms = _ahrs.getLastPosNorthEastReset(pos_shift);
    if (reset_ms != _ekf_xy_reset_ms) {
        shift_pos_xy_target(pos_shift.x * 100.0f, pos_shift.y * 100.0f);
        _ekf_xy_reset_ms = reset_ms;
    }
}

/// initialise ekf z axis reset check
void AC_PosControl::init_ekf_z_reset()
{
    float alt_shift;
    _ekf_z_reset_ms = _ahrs.getLastPosDownReset(alt_shift);
}

/// check for ekf position reset and adjust loiter or brake target position
void AC_PosControl::check_for_ekf_z_reset()
{
    // check for position shift
    float alt_shift;
    uint32_t reset_ms = _ahrs.getLastPosDownReset(alt_shift);
    if (reset_ms != 0 && reset_ms != _ekf_z_reset_ms) {
        shift_alt_target(-alt_shift * 100.0f);
        _ekf_z_reset_ms = reset_ms;
    }
}


/// limit vector to a given length, returns true if vector was limited
bool AC_PosControl::limit_vector_length(float& vector_x, float& vector_y, float max_length)
{
    float vector_length = norm(vector_x, vector_y);
    if ((vector_length > max_length) && is_positive(max_length)) {
        vector_x *= (max_length / vector_length);
        vector_y *= (max_length / vector_length);
        return true;
    }
    return false;
}


/// Proportional controller with piecewise sqrt sections to constrain second derivative
Vector3f AC_PosControl::sqrt_controller(const Vector3f& error, float p, float second_ord_lim)
{
    if (second_ord_lim < 0.0f || is_zero(second_ord_lim) || is_zero(p)) {
        return Vector3f(error.x*p, error.y*p, error.z);
    }

    float linear_dist = second_ord_lim/sq(p);
    float error_length = norm(error.x, error.y);
    if (error_length > linear_dist) {
        float first_order_scale = safe_sqrt(2.0f*second_ord_lim*(error_length-(linear_dist * 0.5f)))/error_length;
        return Vector3f(error.x*first_order_scale, error.y*first_order_scale, error.z);
    } else {
        return Vector3f(error.x*p, error.y*p, error.z);
    }
}
      #   uses: #pragma once

#include <AP_Common/AP_Common.h>
#include <AP_Param/AP_Param.h>
#include <AP_Math/AP_Math.h>
#include <AC_PID/AC_P.h>               // P library
#include <AC_PID/AC_PID.h>             // PID library
#include <AC_PID/AC_PI_2D.h>           // PI library (2-axis)
#include <AC_PID/AC_PID_2D.h>          // PID library (2-axis)
#include <AP_InertialNav/AP_InertialNav.h>     // Inertial Navigation library
#include "AC_AttitudeControl.h" // Attitude control library
#include <AP_Motors/AP_Motors.h>          // motors library
#include <AP_Vehicle/AP_Vehicle.h>         // common vehicle parameters


// position controller default definitions
#define POSCONTROL_ACCELERATION_MIN             50.0f   // minimum horizontal acceleration in cm/s/s - used for sanity checking acceleration in leash length calculation
#define POSCONTROL_ACCEL_XY                     100.0f  // default horizontal acceleration in cm/s/s.  This is overwritten by waypoint and loiter controllers
#define POSCONTROL_ACCEL_XY_MAX                 980.0f  // max horizontal acceleration in cm/s/s that the position velocity controller will ask from the lower accel controller
#define POSCONTROL_STOPPING_DIST_UP_MAX         300.0f  // max stopping distance (in cm) vertically while climbing
#define POSCONTROL_STOPPING_DIST_DOWN_MAX       200.0f  // max stopping distance (in cm) vertically while descending

#define POSCONTROL_SPEED                        500.0f  // default horizontal speed in cm/s
#define POSCONTROL_SPEED_DOWN                  -150.0f  // default descent rate in cm/s
#define POSCONTROL_SPEED_UP                     250.0f  // default climb rate in cm/s

#define POSCONTROL_ACCEL_Z                      250.0f  // default vertical acceleration in cm/s/s.

#define POSCONTROL_LEASH_LENGTH_MIN             100.0f  // minimum leash lengths in cm

#define POSCONTROL_DT_50HZ                      0.02f   // time difference in seconds for 50hz update rate
#define POSCONTROL_DT_400HZ                     0.0025f // time difference in seconds for 400hz update rate

#define POSCONTROL_ACTIVE_TIMEOUT_MS            200     // position controller is considered active if it has been called within the past 0.2 seconds

#define POSCONTROL_VEL_ERROR_CUTOFF_FREQ        4.0f    // low-pass filter on velocity error (unit: hz)
#define POSCONTROL_THROTTLE_CUTOFF_FREQ         2.0f    // low-pass filter on accel error (unit: hz)
#define POSCONTROL_ACCEL_FILTER_HZ              2.0f    // low-pass filter on acceleration (unit: hz)
#define POSCONTROL_JERK_RATIO                   1.0f    // Defines the time it takes to reach the requested acceleration

#define POSCONTROL_OVERSPEED_GAIN_Z             2.0f    // gain controlling rate at which z-axis speed is brought back within SPEED_UP and SPEED_DOWN range

class AC_PosControl
{
public:

    /// Constructor
    AC_PosControl(const AP_AHRS_View& ahrs, const AP_InertialNav& inav,
                  const AP_Motors& motors, AC_AttitudeControl& attitude_control);

    ///
    /// initialisation functions
    ///

    /// set_dt - sets time delta in seconds for all controllers (i.e. 100hz = 0.01, 400hz = 0.0025)
    ///     updates z axis accel controller's D term filter
    void set_dt(float delta_sec);
    float get_dt() const { return _dt; }

    ///
    /// z position controller
    ///

    /// set_speed_z - sets maximum climb and descent rates
    ///     speed_down can be positive or negative but will always be interpreted as a descent speed
    ///     leash length will be recalculated the next time update_z_controller() is called
    void set_speed_z(float speed_down, float speed_up);

    /// get_speed_up - accessor for current up speed in cm/s
    float get_speed_up() const { return _speed_up_cms; }

    /// get_speed_down - accessors for current down speed in cm/s.  Will be a negative number
    float get_speed_down() const { return _speed_down_cms; }

    /// get_vel_target_z - returns current vertical speed in cm/s
    float get_vel_target_z() const { return _vel_target.z; }

    /// set_accel_z - set vertical acceleration in cm/s/s
    ///     leash length will be recalculated the next time update_z_controller() is called
    void set_accel_z(float accel_cmss);

    /// get_accel_z - returns current vertical acceleration in cm/s/s
    float get_accel_z() const { return _accel_z_cms; }

    /// calc_leash_length - calculates the vertical leash lengths from maximum speed, acceleration
    ///     called by update_z_controller if z-axis speed or accelerations are changed
    void calc_leash_length_z();

    /// set_alt_target - set altitude target in cm above home
    void set_alt_target(float alt_cm) { _pos_target.z = alt_cm; }

    /// set_alt_target_with_slew - adjusts target towards a final altitude target
    ///     should be called continuously (with dt set to be the expected time between calls)
    ///     actual position target will be moved no faster than the speed_down and speed_up
    ///     target will also be stopped if the motors hit their limits or leash length is exceeded
    void set_alt_target_with_slew(float alt_cm, float dt);

    /// set_alt_target_from_climb_rate - adjusts target up or down using a climb rate in cm/s
    ///     should be called continuously (with dt set to be the expected time between calls)
    ///     actual position target will be moved no faster than the speed_down and speed_up
    ///     target will also be stopped if the motors hit their limits or leash length is exceeded
    ///     set force_descend to true during landing to allow target to move low enough to slow the motors
    virtual void set_alt_target_from_climb_rate(float climb_rate_cms, float dt, bool force_descend);

    /// set_alt_target_from_climb_rate_ff - adjusts target up or down using a climb rate in cm/s using feed-forward
    ///     should be called continuously (with dt set to be the expected time between calls)
    ///     actual position target will be moved no faster than the speed_down and speed_up
    ///     target will also be stopped if the motors hit their limits or leash length is exceeded
    ///     set force_descend to true during landing to allow target to move low enough to slow the motors
    virtual void set_alt_target_from_climb_rate_ff(float climb_rate_cms, float dt, bool force_descend);

    /// add_takeoff_climb_rate - adjusts alt target up or down using a climb rate in cm/s
    ///     should be called continuously (with dt set to be the expected time between calls)
    ///     almost no checks are performed on the input
    void add_takeoff_climb_rate(float climb_rate_cms, float dt);

    /// set_alt_target_to_current_alt - set altitude target to current altitude
    void set_alt_target_to_current_alt() { _pos_target.z = _inav.get_altitude(); }

    /// shift altitude target (positive means move altitude up)
    void shift_alt_target(float z_cm);

    /// relax_alt_hold_controllers - set all desired and targets to measured
    void relax_alt_hold_controllers(float throttle_setting);

    /// get_alt_target, get_desired_alt - get desired altitude (in cm above home) from loiter or wp controller which should be fed into throttle controller
    /// To-Do: remove one of the two functions below
    float get_alt_target() const { return _pos_target.z; }

    /// get_alt_error - returns altitude error in cm
    float get_alt_error() const;

    // returns horizontal error in cm
    float get_horizontal_error() const;

    /// set_target_to_stopping_point_z - sets altitude target to reasonable stopping altitude in cm above home
    void set_target_to_stopping_point_z();

    /// get_stopping_point_z - calculates stopping point based on current position, velocity, vehicle acceleration
    void get_stopping_point_z(Vector3f& stopping_point) const;

    /// init_takeoff - initialises target altitude if we are taking off
    void init_takeoff();

    // is_active - returns true if the z-axis position controller has been run very recently
    bool is_active_z() const;

    /// update_z_controller - fly to altitude in cm above home
    void update_z_controller();

    // get_leash_down_z, get_leash_up_z - returns vertical leash lengths in cm
    float get_leash_down_z() const { return _leash_down_z; }
    float get_leash_up_z() const { return _leash_up_z; }

    ///
    /// xy position controller
    ///

    /// get_lean_angle_max_cd - returns the maximum lean angle the autopilot may request
    float get_lean_angle_max_cd() const;

    /// init_xy_controller - initialise the xy controller
    ///     sets target roll angle, pitch angle and I terms based on vehicle current lean angles
    ///     should be called once whenever significant changes to the position target are made
    ///     this does not update the xy target
    void init_xy_controller(bool reset_I = true);

    /// set_accel_xy - set horizontal acceleration in cm/s/s
    ///     leash length will be recalculated the next time update_xy_controller() is called
    void set_accel_xy(float accel_cmss);
    float get_accel_xy() const { return _accel_cms; }

    /// set_speed_xy - set horizontal speed maximum in cm/s
    ///     leash length will be recalculated the next time update_xy_controller() is called
    void set_speed_xy(float speed_cms);
    float get_speed_xy() const { return _speed_cms; }

    /// set_limit_accel_xy - mark that accel has been limited
    ///     this prevents integrator buildup
    void set_limit_accel_xy(void) { _limit.accel_xy = true; }

    /// calc_leash_length - calculates the horizontal leash length given a maximum speed, acceleration
    ///     should be called whenever the speed, acceleration or position kP is modified
    void calc_leash_length_xy();

    /// set the horizontal leash length
    void set_leash_length_xy(float leash) { _leash = leash; _flags.recalc_leash_xy = false; }

    /// get_pos_target - get target as position vector (from home in cm)
    const Vector3f& get_pos_target() const { return _pos_target; }

    /// set_pos_target in cm from home
    void set_pos_target(const Vector3f& position);

    /// set_xy_target in cm from home
    void set_xy_target(float x, float y);

    /// shift position target target in x, y axis
    void shift_pos_xy_target(float x_cm, float y_cm);

    /// get_desired_velocity - returns xy desired velocity (i.e. feed forward) in cm/s in lat and lon direction
    const Vector3f& get_desired_velocity() { return _vel_desired; }

    /// set_desired_velocity_z - sets desired velocity in cm/s in z axis
    void set_desired_velocity_z(float vel_z_cms) {_vel_desired.z = vel_z_cms;}

    // clear desired velocity feed-forward in z axis
    void clear_desired_velocity_ff_z() { _flags.use_desvel_ff_z = false; }

    // set desired acceleration in cm/s in xy axis
    void set_desired_accel_xy(float accel_lat_cms, float accel_lon_cms) { _accel_desired.x = accel_lat_cms; _accel_desired.y = accel_lon_cms; }

    /// set_desired_velocity_xy - sets desired velocity in cm/s in lat and lon directions
    ///     when update_xy_controller is next called the position target is moved based on the desired velocity and
    ///     the desired velocities are fed forward into the rate_to_accel step
    void set_desired_velocity_xy(float vel_lat_cms, float vel_lon_cms) {_vel_desired.x = vel_lat_cms; _vel_desired.y = vel_lon_cms; }

    /// set_desired_velocity - sets desired velocity in cm/s in all 3 axis
    ///     when update_vel_controller_xyz is next called the position target is moved based on the desired velocity
    void set_desired_velocity(const Vector3f &des_vel) { _vel_desired = des_vel; }

    // overrides the velocity process variable for one timestep
    void override_vehicle_velocity_xy(const Vector2f& vel_xy) { _vehicle_horiz_vel = vel_xy; _flags.vehicle_horiz_vel_override = true; }

    /// freeze_ff_z - used to stop the feed forward being calculated during a known discontinuity
    void freeze_ff_z() { _flags.freeze_ff_z = true; }

    // is_active_xy - returns true if the xy position controller has been run very recently
    bool is_active_xy() const;

    /// update_xy_controller - run the horizontal position controller - should be called at 100hz or higher
    ///     when use_desired_velocity is true the desired velocity (i.e. feed forward) is incorporated at the pos_to_rate step
    void update_xy_controller(float ekfNavVelGainScaler);

    /// set_target_to_stopping_point_xy - sets horizontal target to reasonable stopping position in cm from home
    void set_target_to_stopping_point_xy();

    /// get_stopping_point_xy - calculates stopping point based on current position, velocity, vehicle acceleration
    ///     distance_max allows limiting distance to stopping point
    ///     results placed in stopping_position vector
    ///     set_accel_xy() should be called before this method to set vehicle acceleration
    ///     set_leash_length() should have been called before this method
    void get_stopping_point_xy(Vector3f &stopping_point) const;

    /// get_distance_to_target - get horizontal distance to position target in cm (used for reporting)
    float get_distance_to_target() const;

    /// get_bearing_to_target - get bearing to target position in centi-degrees
    int32_t get_bearing_to_target() const;

    /// xyz velocity controller

    /// init_vel_controller_xyz - initialise the velocity controller - should be called once before the caller attempts to use the controller
    void init_vel_controller_xyz();

    /// update_velocity_controller_xy - run the XY velocity controller - should be called at 100hz or higher
    ///     velocity targets should we set using set_desired_velocity_xy() method
    ///     callers should use get_roll() and get_pitch() methods and sent to the attitude controller
    ///     throttle targets will be sent directly to the motors
    void update_vel_controller_xy(float ekfNavVelGainScaler);

    /// update_velocity_controller_xyz - run the velocity controller - should be called at 100hz or higher
    ///     velocity targets should we set using set_desired_velocity_xyz() method
    ///     callers should use get_roll() and get_pitch() methods and sent to the attitude controller
    ///     throttle targets will be sent directly to the motors
    void update_vel_controller_xyz(float ekfNavVelGainScaler);

    /// get desired roll, pitch which should be fed into stabilize controllers
    float get_roll() const { return _roll_target; }
    float get_pitch() const { return _pitch_target; }

    // get_leash_xy - returns horizontal leash length in cm
    float get_leash_xy() const { return _leash; }

    /// get pid controllers
    AC_P& get_pos_z_p() { return _p_pos_z; }
    AC_P& get_vel_z_p() { return _p_vel_z; }
    AC_PID& get_accel_z_pid() { return _pid_accel_z; }
    AC_P& get_pos_xy_p() { return _p_pos_xy; }
    AC_PID_2D& get_vel_xy_pid() { return _pid_vel_xy; }

    /// accessors for reporting
    const Vector3f& get_vel_target() const { return _vel_target; }
    const Vector3f& get_accel_target() const { return _accel_target; }

    // lean_angles_to_accel - convert roll, pitch lean angles to lat/lon frame accelerations in cm/s/s
    void accel_to_lean_angles(float accel_x_cmss, float accel_y_cmss, float& roll_target, float& pitch_target) const;

    // lean_angles_to_accel - convert roll, pitch lean angles to lat/lon frame accelerations in cm/s/s
    void lean_angles_to_accel(float& accel_x_cmss, float& accel_y_cmss) const;

    // time_since_last_xy_update - returns time in seconds since the horizontal position controller was last run
    float time_since_last_xy_update() const;

    // write log to dataflash
    void write_log();

    static const struct AP_Param::GroupInfo var_info[];

protected:

    // general purpose flags
    struct poscontrol_flags {
            uint16_t recalc_leash_z     : 1;    // 1 if we should recalculate the z axis leash length
            uint16_t recalc_leash_xy    : 1;    // 1 if we should recalculate the xy axis leash length
            uint16_t reset_desired_vel_to_pos   : 1;    // 1 if we should reset the rate_to_accel_xy step
            uint16_t reset_accel_to_lean_xy     : 1;    // 1 if we should reset the accel to lean angle step
            uint16_t reset_rate_to_accel_z      : 1;    // 1 if we should reset the rate_to_accel_z step
            uint16_t reset_accel_to_throttle    : 1;    // 1 if we should reset the accel_to_throttle step of the z-axis controller
            uint16_t freeze_ff_z        : 1;    // 1 used to freeze velocity to accel feed forward for one iteration
            uint16_t use_desvel_ff_z    : 1;    // 1 to use z-axis desired velocity as feed forward into velocity step
            uint16_t vehicle_horiz_vel_override : 1; // 1 if we should use _vehicle_horiz_vel as our velocity process variable for one timestep
    } _flags;

    // limit flags structure
    struct poscontrol_limit_flags {
        uint8_t pos_up      : 1;    // 1 if we have hit the vertical position leash limit while going up
        uint8_t pos_down    : 1;    // 1 if we have hit the vertical position leash limit while going down
        uint8_t vel_up      : 1;    // 1 if we have hit the vertical velocity limit going up
        uint8_t vel_down    : 1;    // 1 if we have hit the vertical velocity limit going down
        uint8_t accel_xy    : 1;    // 1 if we have hit the horizontal accel limit
    } _limit;

    ///
    /// z controller private methods
    ///

    // run position control for Z axis
    // target altitude should be set with one of these functions
    //          set_alt_target
    //          set_target_to_stopping_point_z
    //          init_takeoff
    void run_z_controller();

    ///
    /// xy controller private methods
    ///

    /// move velocity target using desired acceleration
    void desired_accel_to_vel(float nav_dt);

    /// desired_vel_to_pos - move position target using desired velocities
    void desired_vel_to_pos(float nav_dt);

    /// run horizontal position controller correcting position and velocity
    ///     converts position (_pos_target) to target velocity (_vel_target)
    ///     desired velocity (_vel_desired) is combined into final target velocity
    ///     converts desired velocities in lat/lon directions to accelerations in lat/lon frame
    ///     converts desired accelerations provided in lat/lon frame to roll/pitch angles
    void run_xy_controller(float dt, float ekfNavVelGainScaler);

    /// calc_leash_length - calculates the horizontal leash length given a maximum speed, acceleration and position kP gain
    float calc_leash_length(float speed_cms, float accel_cms, float kP) const;

    /// limit vector to a given length, returns true if vector was limited
    static bool limit_vector_length(float& vector_x, float& vector_y, float max_length);

    /// Proportional controller with piecewise sqrt sections to constrain second derivative
    static Vector3f sqrt_controller(const Vector3f& error, float p, float second_ord_lim);

    /// initialise and check for ekf position resets
    void init_ekf_xy_reset();
    void check_for_ekf_xy_reset();
    void init_ekf_z_reset();
    void check_for_ekf_z_reset();

    // references to inertial nav and ahrs libraries
    const AP_AHRS_View &        _ahrs;
    const AP_InertialNav&       _inav;
    const AP_Motors&            _motors;
    AC_AttitudeControl&         _attitude_control;

    // parameters
    AP_Float    _accel_xy_filt_hz;      // XY acceleration filter cutoff frequency
    AP_Float    _lean_angle_max;        // Maximum autopilot commanded angle (in degrees). Set to zero for Angle Max
    AC_P        _p_pos_z;
    AC_P        _p_vel_z;
    AC_PID      _pid_accel_z;
    AC_P        _p_pos_xy;
    AC_PID_2D   _pid_vel_xy;

    // internal variables
    float       _dt;                    // time difference (in seconds) between calls from the main program
    uint32_t    _last_update_xy_ms;     // system time of last update_xy_controller call
    uint32_t    _last_update_z_ms;      // system time of last update_z_controller call
    float       _speed_down_cms;        // max descent rate in cm/s
    float       _speed_up_cms;          // max climb rate in cm/s
    float       _speed_cms;             // max horizontal speed in cm/s
    float       _accel_z_cms;           // max vertical acceleration in cm/s/s
    float       _accel_last_z_cms;      // max vertical acceleration in cm/s/s
    float       _accel_cms;             // max horizontal acceleration in cm/s/s
    float       _leash;                 // horizontal leash length in cm.  target will never be further than this distance from the vehicle
    float       _leash_down_z;          // vertical leash down in cm.  target will never be further than this distance below the vehicle
    float       _leash_up_z;            // vertical leash up in cm.  target will never be further than this distance above the vehicle

    // output from controller
    float       _roll_target;           // desired roll angle in centi-degrees calculated by position controller
    float       _pitch_target;          // desired roll pitch in centi-degrees calculated by position controller

    // position controller internal variables
    Vector3f    _pos_target;            // target location in cm from home
    Vector3f    _pos_error;             // error between desired and actual position in cm
    Vector3f    _vel_desired;           // desired velocity in cm/s
    Vector3f    _vel_target;            // velocity target in cm/s calculated by pos_to_rate step
    Vector3f    _vel_error;             // error between desired and actual acceleration in cm/s
    Vector3f    _vel_last;              // previous iterations velocity in cm/s
    Vector3f    _accel_desired;         // desired acceleration in cm/s/s (feed forward)
    Vector3f    _accel_target;          // acceleration target in cm/s/s
    Vector3f    _accel_error;           // acceleration error in cm/s/s
    Vector2f    _vehicle_horiz_vel;     // velocity to use if _flags.vehicle_horiz_vel_override is set
    float       _distance_to_target;    // distance to position target - for reporting only
    LowPassFilterFloat _vel_error_filter;   // low-pass-filter on z-axis velocity error

    LowPassFilterVector2f _accel_target_filter; // acceleration target filter

    // ekf reset handling
    uint32_t    _ekf_xy_reset_ms;      // system time of last recorded ekf xy position reset
    uint32_t    _ekf_z_reset_ms;       // system time of last recorded ekf altitude reset
};
      #   with: #include "AC_PosControl_Sub.h"

AC_PosControl_Sub::AC_PosControl_Sub(const AP_AHRS_View& ahrs, const AP_InertialNav& inav,
                                     const AP_Motors& motors, AC_AttitudeControl& attitude_control) :
    AC_PosControl(ahrs, inav, motors, attitude_control),
    _alt_max(0.0f),
    _alt_min(0.0f)
{}

/// set_alt_target_from_climb_rate - adjusts target up or down using a climb rate in cm/s
///     should be called continuously (with dt set to be the expected time between calls)
///     actual position target will be moved no faster than the speed_down and speed_up
///     target will also be stopped if the motors hit their limits or leash length is exceeded
void AC_PosControl_Sub::set_alt_target_from_climb_rate(float climb_rate_cms, float dt, bool force_descend)
{
    // adjust desired alt if motors have not hit their limits
    // To-Do: add check of _limit.pos_down?
    if ((climb_rate_cms<0 && (!_motors.limit.throttle_lower || force_descend)) || (climb_rate_cms>0 && !_motors.limit.throttle_upper && !_limit.pos_up)) {
        _pos_target.z += climb_rate_cms * dt;
    }

    // do not let target alt get above limit
    if (_alt_max < 100 && _pos_target.z > _alt_max) {
        _pos_target.z = _alt_max;
        _limit.pos_up = true;
    }

    // do not let target alt get below limit
    if (_alt_min < 0 && _alt_min < _alt_max && _pos_target.z < _alt_min) {
        _pos_target.z = _alt_min;
        _limit.pos_down = true;
    }

    // do not use z-axis desired velocity feed forward
    // vel_desired set to desired climb rate for reporting and land-detector
    _flags.use_desvel_ff_z = false;
    _vel_desired.z = climb_rate_cms;
}

/// set_alt_target_from_climb_rate_ff - adjusts target up or down using a climb rate in cm/s using feed-forward
///     should be called continuously (with dt set to be the expected time between calls)
///     actual position target will be moved no faster than the speed_down and speed_up
///     target will also be stopped if the motors hit their limits or leash length is exceeded
///     set force_descend to true during landing to allow target to move low enough to slow the motors
void AC_PosControl_Sub::set_alt_target_from_climb_rate_ff(float climb_rate_cms, float dt, bool force_descend)
{
    // calculated increased maximum acceleration if over speed
    float accel_z_cms = _accel_z_cms;
    if (_vel_desired.z < _speed_down_cms && !is_zero(_speed_down_cms)) {
        accel_z_cms *= POSCONTROL_OVERSPEED_GAIN_Z * _vel_desired.z / _speed_down_cms;
    }
    if (_vel_desired.z > _speed_up_cms && !is_zero(_speed_up_cms)) {
        accel_z_cms *= POSCONTROL_OVERSPEED_GAIN_Z * _vel_desired.z / _speed_up_cms;
    }
    accel_z_cms = constrain_float(accel_z_cms, 0.0f, 750.0f);

    // jerk_z is calculated to reach full acceleration in 1000ms.
    float jerk_z = accel_z_cms * POSCONTROL_JERK_RATIO;

    float accel_z_max = MIN(accel_z_cms, safe_sqrt(2.0f*fabsf(_vel_desired.z - climb_rate_cms)*jerk_z));

    _accel_last_z_cms += jerk_z * dt;
    _accel_last_z_cms = MIN(accel_z_max, _accel_last_z_cms);

    float vel_change_limit = _accel_last_z_cms * dt;
    _vel_desired.z = constrain_float(climb_rate_cms, _vel_desired.z-vel_change_limit, _vel_desired.z+vel_change_limit);
    _flags.use_desvel_ff_z = true;

    // adjust desired alt if motors have not hit their limits
    // To-Do: add check of _limit.pos_down?
    if ((_vel_desired.z<0 && (!_motors.limit.throttle_lower || force_descend)) || (_vel_desired.z>0 && !_motors.limit.throttle_upper && !_limit.pos_up)) {
        _pos_target.z += _vel_desired.z * dt;
    }

    // do not let target alt get above limit
    if (_alt_max < 100 && _pos_target.z > _alt_max) {
        _pos_target.z = _alt_max;
        _limit.pos_up = true;
        // decelerate feed forward to zero
        _vel_desired.z = constrain_float(0.0f, _vel_desired.z-vel_change_limit, _vel_desired.z+vel_change_limit);
    }

    // do not let target alt get below limit
    if (_alt_min < 0 && _alt_min < _alt_max && _pos_target.z < _alt_min) {
        _pos_target.z = _alt_min;
        _limit.pos_down = true;
        // decelerate feed forward to zero
        _vel_desired.z = constrain_float(0.0f, _vel_desired.z-vel_change_limit, _vel_desired.z+vel_change_limit);
    }
}
      #     name: #pragma once

#include "AC_PosControl.h"

class AC_PosControl_Sub : public AC_PosControl {
public:
    AC_PosControl_Sub(const AP_AHRS_View & ahrs, const AP_InertialNav& inav,
                      const AP_Motors& motors, AC_AttitudeControl& attitude_control);

    /// set_alt_max - sets maximum altitude above the ekf origin in cm
    ///   only enforced when set_alt_target_from_climb_rate is used
    ///   set to zero to disable limit
    void set_alt_max(float alt) { _alt_max = alt; }

    /// set_alt_min - sets the minimum altitude (maximum depth) in cm
    ///   only enforced when set_alt_target_from_climb_rate is used
    ///   set to zero to disable limit
    void set_alt_min(float alt) { _alt_min = alt; }

    /// set_alt_target_from_climb_rate - adjusts target up or down using a climb rate in cm/s
    ///     should be called continuously (with dt set to be the expected time between calls)
    ///     actual position target will be moved no faster than the speed_down and speed_up
    ///     target will also be stopped if the motors hit their limits or leash length is exceeded
    ///     set force_descend to true during landing to allow target to move low enough to slow the motors
    void set_alt_target_from_climb_rate(float climb_rate_cms, float dt, bool force_descend) override;

    /// set_alt_target_from_climb_rate_ff - adjusts target up or down using a climb rate in cm/s using feed-forward
    ///     should be called continuously (with dt set to be the expected time between calls)
    ///     actual position target will be moved no faster than the speed_down and speed_up
    ///     target will also be stopped if the motors hit their limits or leash length is exceeded
    ///     set force_descend to true during landing to allow target to move low enough to slow the motors
    void set_alt_target_from_climb_rate_ff(float climb_rate_cms, float dt, bool force_descend) override;

private:
    float       _alt_max; // max altitude - should be updated from the main code with altitude limit from fence
    float       _alt_min; // min altitude - should be updated from the main code with altitude limit from fence
};
      #     path: #include "AC_AttitudeControl.h"
#include <AP_HAL/AP_HAL.h>
#include <AP_Math/AP_Math.h>

/*
  code to monitor and report on the rate controllers, allowing for
  notification of controller oscillation
 */


/*
  update a RMS estimate of controller state
 */
void AC_AttitudeControl::control_monitor_filter_pid(float value, float &rms)
{
    const float filter_constant = 0.99f;
    // we don't do the sqrt() here as it is quite expensive. That is
    // done when reporting a result
    rms = filter_constant * rms + (1.0f - filter_constant) * sq(value);
}

/*
  update state in _control_monitor
 */
void AC_AttitudeControl::control_monitor_update(void)
{
    const DataFlash_Class::PID_Info &iroll  = get_rate_roll_pid().get_pid_info();
    control_monitor_filter_pid(iroll.P + iroll.FF,  _control_monitor.rms_roll_P);
    control_monitor_filter_pid(iroll.D,             _control_monitor.rms_roll_D);

    const DataFlash_Class::PID_Info &ipitch = get_rate_pitch_pid().get_pid_info();
    control_monitor_filter_pid(ipitch.P + iroll.FF,  _control_monitor.rms_pitch_P);
    control_monitor_filter_pid(ipitch.D,             _control_monitor.rms_pitch_D);

    const DataFlash_Class::PID_Info &iyaw   = get_rate_yaw_pid().get_pid_info();
    control_monitor_filter_pid(iyaw.P + iyaw.D + iyaw.FF,  _control_monitor.rms_yaw);
}

/*
  log a CRTL message
 */
void AC_AttitudeControl::control_monitor_log(void)
{
    DataFlash_Class::instance()->Log_Write("CTRL", "TimeUS,RMSRollP,RMSRollD,RMSPitchP,RMSPitchD,RMSYaw", "Qfffff",
                                           AP_HAL::micros64(),
                                           (double)sqrtf(_control_monitor.rms_roll_P),
                                           (double)sqrtf(_control_monitor.rms_roll_D),
                                           (double)sqrtf(_control_monitor.rms_pitch_P),
                                           (double)sqrtf(_control_monitor.rms_pitch_D),
                                           (double)sqrtf(_control_monitor.rms_yaw));

}

/*
  return current controller RMS filter value for roll
 */
float AC_AttitudeControl::control_monitor_rms_output_roll(void) const
{
    return sqrtf(_control_monitor.rms_roll_P + _control_monitor.rms_roll_D);
}

/*
  return current controller RMS filter value for roll_P
 */
float AC_AttitudeControl::control_monitor_rms_output_roll_P(void) const
{
    return sqrtf(_control_monitor.rms_roll_P);
}

/*
  return current controller RMS filter value for roll_D
 */
float AC_AttitudeControl::control_monitor_rms_output_roll_D(void) const
{
    return sqrtf(_control_monitor.rms_roll_D);
}

/*
  return current controller RMS filter value for pitch
 */
float AC_AttitudeControl::control_monitor_rms_output_pitch(void) const
{
    return sqrtf(_control_monitor.rms_pitch_P + _control_monitor.rms_pitch_D);
}

/*
  return current controller RMS filter value for pitch_P
 */
float AC_AttitudeControl::control_monitor_rms_output_pitch_P(void) const
{
    return sqrtf(_control_monitor.rms_pitch_P);
}

/*
  return current controller RMS filter value for pitch_D
 */
float AC_AttitudeControl::control_monitor_rms_output_pitch_D(void) const
{
    return sqrtf(_control_monitor.rms_pitch_D);
}

/*
  return current controller RMS filter value for yaw
 */
float AC_AttitudeControl::control_monitor_rms_output_yaw(void) const
{
    return sqrtf(_control_monitor.rms_yaw);
}
