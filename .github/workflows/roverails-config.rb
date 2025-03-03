# This workflow uses actions that are not certified by GitHub.  They are
# provided by a third-party and are governed by separate terms of service,
# privacy policy, and support documentation.
#
# This workflow will install a prebuilt Ruby version, install dependencies, and
# run tests and linters.
name: "Ruby on Rails CI"
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
jobs: #include "Rover.h"

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
  test: #include "Rover.h"

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
    runs-on: APMrover2 Release Notes:
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
    services: #include "Rover.h"

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
      postgres: /*****************************************************************************
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
        image: /*
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
        ports: #pragma once

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
          - "5432:5432"
        env: #!/usr/bin/env python
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
          POSTGRES_DB: #!/usr/bin/env node

'use strict';

/* eslint-disable import/no-dynamic-require */
const config = require('config');
const hoek = require('@hapi/hoek');
const { Bookend } = require('screwdriver-build-bookend');
const logger = require('screwdriver-logger');

/**
 * convert value to Boolean
 * @method convertToBool
 * @param {(Boolean|String)} value
 * @return {Boolean}
 */
function convertToBool(value) {
    if (typeof value === 'boolean') {
        return value;
    }

    // trueList refers to https://yaml.org/type/bool.html
    const trueList = ['on', 'true', 'yes', 'y'];
    const lowerValue = String(value).toLowerCase();

    return trueList.includes(lowerValue);
}

// Setup Authentication
const authConfig = hoek.clone(config.get('auth'));

// Setup HTTPd
const httpdConfig = config.get('httpd');

const shutdownConfig = config.get('shutdown');

// Setup Webhooks
const webhooksConfig = config.get('webhooks');

// Special urls for things like the UI
const ecosystem = hoek.clone(config.get('ecosystem'));

ecosystem.api = httpdConfig.uri;

const release = config.get('release');

// Logging config
const log = config.get('log');

// Notification config
const notificationConfig = config.get('notifications');

// Multiple build cluster feature flag
const multiBuildClusterEnabled = convertToBool(config.get('multiBuildCluster').enabled);

// Unzip artifacts feature flag
const unzipArtifactsEnabled = convertToBool(config.get('unzipArtifacts').enabled);

// Queue Webhook feature flag
const queueWebhookEnabled = convertToBool(config.get('queueWebhook').enabled);

// Default cluster environment variable
const clusterEnvConfig = config.get('build').environment; // readonly
const clusterEnv = { ...clusterEnvConfig };
const artifactsMaxDownloadSize = config.get('build').artifacts.maxDownloadSize;

Object.keys(clusterEnv).forEach(k => {
    clusterEnv[k] = String(clusterEnv[k]);
});

// Flag to throw notifications validation err or not; default true
const notificationsValidationErr = convertToBool(
    hoek.reach(config.get('notifications').options, 'throwValidationErr', { default: true })
);

// Setup Datastore
const datastoreConfig = config.get('datastore');
const DatastorePlugin = require(`screwdriver-datastore-${datastoreConfig.plugin}`);

const datastorePluginConfig = { ...datastoreConfig[datastoreConfig.plugin] };

// Readonly Datastore
const datastoreROConfig = datastorePluginConfig.readOnly;
let datastoreRO;

if (datastoreROConfig && Object.keys(datastoreROConfig).length > 0) {
    datastoreRO = new DatastorePlugin(hoek.applyToDefaults({ ecosystem }, datastoreROConfig));
}
delete datastorePluginConfig.readOnly;

// Default datastore
const datastore = new DatastorePlugin(hoek.applyToDefaults({ ecosystem }, datastorePluginConfig || {}));

// Setup Source Code Plugin(s)
const scmConfig = { scms: config.get('scms') };
const ScmPlugin = require('screwdriver-scm-router');
const scm = new ScmPlugin(scmConfig || {});

authConfig.scm = scm;

// Plugin to upload artifacts to store
const ArtifactPlugin = require('screwdriver-artifact-bookend');
const artifact = new ArtifactPlugin(ecosystem.store);

// Build cache plugin
const CachePlugin = require('screwdriver-cache-bookend');
const cache = new CachePlugin();

// Plugin to upload coverage
const coverageConfig = hoek.clone(config.get('coverage'));
const CoveragePlugin = require('screwdriver-coverage-bookend');
const coverage = new CoveragePlugin(coverageConfig);

// Plugins to run before/after a build
const bookends = config.get('bookends');
const bookend = new Bookend(
    {
        scm,
        'screwdriver-artifact-bookend': artifact,
        'screwdriver-coverage-bookend': coverage,
        'screwdriver-cache-bookend': cache
    },
    bookends
);

// Setup Pipeline Factory for Executor
const Models = require('screwdriver-models');
const server = require('..');
const pipelineFactory = Models.PipelineFactory.getInstance({
    datastore,
    scm,
    externalJoin: true,
    notificationsValidationErr,
    multiBuildClusterEnabled
});

// Setup Executor
const executorConfig = hoek.clone(config.get('executor'));

executorConfig[executorConfig.plugin].options.pipelineFactory = pipelineFactory;
const ExecutorPlugin = require(`screwdriver-executor-${executorConfig.plugin}`);
const executor = new ExecutorPlugin({
    ecosystem: hoek.clone(ecosystem),
    ...executorConfig[executorConfig.plugin].options
});

executor.name = executorConfig.plugin;

// Setup Model Factories
const commandFactory = Models.CommandFactory.getInstance({
    datastore,
    datastoreRO
});
const commandTagFactory = Models.CommandTagFactory.getInstance({
    datastore,
    datastoreRO
});
const templateFactory = Models.TemplateFactory.getInstance({
    datastore,
    datastoreRO
});
const templateTagFactory = Models.TemplateTagFactory.getInstance({
    datastore,
    datastoreRO
});
const jobFactory = Models.JobFactory.getInstance({
    datastore,
    datastoreRO,
    executor
});
const userFactory = Models.UserFactory.getInstance({
    datastore,
    datastoreRO,
    scm,
    password: authConfig.encryptionPassword
});
const buildFactory = Models.BuildFactory.getInstance({
    datastore,
    datastoreRO,
    dockerRegistry: ecosystem.dockerRegistry,
    scm,
    executor,
    bookend,
    uiUri: ecosystem.ui,
    multiBuildClusterEnabled,
    clusterEnv
});
const stepFactory = Models.StepFactory.getInstance({
    datastore,
    datastoreRO
});
const secretFactory = Models.SecretFactory.getInstance({
    datastore,
    datastoreRO,
    password: authConfig.encryptionPassword
});
const tokenFactory = Models.TokenFactory.getInstance({
    datastore,
    datastoreRO,
    password: authConfig.hashingPassword
});
const eventFactory = Models.EventFactory.getInstance({
    datastore,
    datastoreRO,
    scm
});
const bannerFactory = Models.BannerFactory.getInstance({
    datastore,
    datastoreRO,
    scm
});
const collectionFactory = Models.CollectionFactory.getInstance({
    datastore,
    datastoreRO
});
const stageFactory = Models.StageFactory.getInstance({
    datastore,
    datastoreRO
});
const stageBuildFactory = Models.StageBuildFactory.getInstance({
    datastore,
    datastoreRO
});
const triggerFactory = Models.TriggerFactory.getInstance({
    datastore,
    datastoreRO
});
const buildClusterFactory = Models.BuildClusterFactory.getInstance({
    datastore,
    datastoreRO,
    scm
});
const pipelineTemplateFactory = Models.PipelineTemplateFactory.getInstance({
    datastore,
    datastoreRO,
    scm
});
const pipelineTemplateVersionFactory = Models.PipelineTemplateVersionFactory.getInstance({
    datastore,
    datastoreRO,
    scm
});

const pipelineTemplateTagFactory = Models.PipelineTemplateTagFactory.getInstance({
    datastore,
    datastoreRO,
    scm
});

const jobTemplateTagFactory = Models.JobTemplateTagFactory.getInstance({
    datastore,
    datastoreRO,
    scm
});

// @TODO run setup for SCM and Executor
// datastoreConfig.ddlSync => sync datastore schema (ddl) via api (default: true)
datastore.setup(datastoreConfig.ddlSyncEnabled).then(() =>
    server({
        httpd: httpdConfig,
        auth: authConfig,
        webhooks: webhooksConfig,
        notifications: notificationConfig,
        shutdown: shutdownConfig,
        coverage,
        ecosystem,
        commandFactory,
        commandTagFactory,
        templateFactory,
        pipelineTemplateFactory,
        pipelineTemplateVersionFactory,
        pipelineTemplateTagFactory,
        jobTemplateTagFactory,
        templateTagFactory,
        pipelineFactory,
        jobFactory,
        userFactory,
        buildFactory,
        buildClusterFactory,
        stepFactory,
        bannerFactory,
        secretFactory,
        tokenFactory,
        eventFactory,
        collectionFactory,
        stageFactory,
        stageBuildFactory,
        triggerFactory,
        banners: authConfig,
        builds: {
            ecosystem,
            authConfig
        },
        stats: {
            executor,
            scm
        },
        release,
        log,
        validator: {
            externalJoin: true,
            notificationsValidationErr
        },
        queueWebhook: {
            executor,
            queueWebhookEnabled
        },
        unzipArtifactsEnabled,
        artifactsMaxDownloadSize
    })
        .then(instance => logger.info('Server running at %s', instance.info.uri))
        .catch(err => {
            logger.error(err);
            process.exit(1);
        })
);    
          POSTGRES_USER: # All values in this document are the ENVIRONMENT variable names that can override the defaults
# from `default.yaml`
---
auth:
  # A private key uses for signing jwt tokens
  # Easily generate one by running
  # $ openssl genrsa -out jwt.pem 2048
  jwtPrivateKey: SECRET_JWT_PRIVATE_KEY
  # The public key used for verifying the signature
  # Generate one by running
  # $ openssl rsa -in jwt.pem -pubout -out jwt.pub
  jwtPublicKey: SECRET_JWT_PUBLIC_KEY
  # The public key for queue service
  jwtQueueServicePublicKey: SECRET_JWT_QUEUE_SVC_PUBLIC_KEY
  # Environment of the JWTs. For example: 'prod' or 'beta'
  jwtEnvironment: JWT_ENVIRONMENT
  # A password used for encrypting session data.
  # **Needs to be minimum 32 characters**
  cookiePassword: SECRET_COOKIE_PASSWORD
  # A password used for encrypting stored pipeline secrets and user Oauth token.
  # **Needs to be minimum 32 characters**
  encryptionPassword: SECRET_PASSWORD
  # A password used for hashing user/pipeline access tokens.
  # **Needs to be minimum 32 characters**
  hashingPassword: SECRET_HASHING_PASSWORD
  # A flag to set if the server is running over https.
  # Used as a flag for the OAuth flow
  https: IS_HTTPS
  # A flag to set if you want guests to browse your pipelines
  allowGuestAccess: AUTH_GUEST_ACCESS
  whitelist:
    __name: SECRET_WHITELIST
    __format: json
  allowList:
    __name: SECRET_ALLOW_LIST
    __format: json
  admins:
    __name: SECRET_ADMINS
    __format: json
  sdAdmins:
    __name: SECRET_SD_ADMINS
    __format: json
  authCheckById: AUTH_CHECK_BY_ID
  # Default session timeout (in minutes)
  sessionTimeout: SESSION_TIMEOUT
  # Oauth redirect uri, configure this if your app is not running at root under the host
  oauthRedirectUri: OAUTH_REDIRECT_URI
  # SameSite Cookie Option
  sameSite: COOKIE_SAME_SITE
  # cookie path to access the cookie https://github.com/hapijs/cookie/issues/209
  path: COOKIE_PATH_VALUE

shutdown:
  terminationGracePeriod: TERMINATION_GRACE_PERIOD

httpd:
  # Port to listen on
  port: PORT
  # Host to listen on (set to 0.0.0.0 to accept all connections)
  host: HOST
  # Externally routable URI (usually your load balancer or CNAME)
  uri: URI
  # TLS configuration (key, cert, etc.)
  # https://nodejs.org/api/tls.html#tls_tls_createserver_options_secureconnectionlistener
  tls:
    __name: HTTPD_TLS
    __format: json

datastore:
  plugin: DATASTORE_PLUGIN
  ddlSyncEnabled: DATASTORE_DDL_SYNC_ENABLED
  sequelize:
    # Type of server to talk to
    dialect: DATASTORE_SEQUELIZE_DIALECT
    # Database name
    database: DATASTORE_SEQUELIZE_DATABASE
    # Username/Password
    username: DATASTORE_SEQUELIZE_USERNAME
    password: DATASTORE_SEQUELIZE_PASSWORD
    # Storage location for sqlite
    storage: DATASTORE_SEQUELIZE_STORAGE
    # Network settings
    host: DATASTORE_SEQUELIZE_HOST
    port: DATASTORE_SEQUELIZE_PORT
    # Prefix to the table names
    prefix: DATASTORE_SEQUELIZE_PREFIX
    # Configure SSL/TLS connection settings
    ssl:
      __name: DATASTORE_SEQUELIZE_SSL
      __format: json
    # Connection pool config. See http://docs.sequelizejs.com/class/lib/sequelize.js~Sequelize.html#instance-constructor-constructor
    pool:
      __name: DATASTORE_SEQUELIZE_POOL
      __format: json
    retry:
      __name: DATASTORE_SEQUELIZE_RETRY
      __format: json
    buildMetricsEnabled: DATASTORE_SEQUELIZE_CAPTURE_METRICS_ENABLED
    readOnly:
      __name: DATASTORE_SEQUELIZE_RO
      __format: json
      # dialect: sequelize
      # database:
      # username:
      # password:
      # host:
      # port:
      # More arguments here:
      # http://docs.sequelizejs.com/en/latest/api/sequelize/

executor:
  plugin: EXECUTOR_PLUGIN
  # The NPM module object(s) for the executor plugin(s)
  k8s:
    enabled: EXECUTOR_K8S_ENABLED
    options:
      kubernetes:
        # The host or IP of the kubernetes cluster
        host: K8S_HOST
        # The jwt token used for authenticating kubernetes requests
        token: K8S_TOKEN
        jobsNamespace: K8S_JOBS_NAMESPACE
        # Resources for build pod
        resources:
          # Number of cpu cores
          cpu:
            micro: K8S_CPU_MICRO
            low: K8S_CPU_LOW
            high: K8S_CPU_HIGH
          # Memory in GB
          memory:
            micro: K8S_MEMORY_MICRO
            low: K8S_MEMORY_LOW
            high: K8S_MEMORY_HIGH
        # Default build timeout for all builds in this cluster
        buildTimeout: K8S_BUILD_TIMEOUT
        # Default max build timeout
        maxBuildTimeout: K8S_MAX_BUILD_TIMEOUT
        # k8s node selectors for build pod scheduling.
        # Value is Object of format { label: 'value' } See
        # https://kubernetes.io/docs/concepts/configuration/assign-pod-node/#step-one-attach-label-to-the-node
        # Eg: { dedicated: 'screwdriver' } to schedule pods on nodes having
        # label-value of dedicated=screwdriver
        nodeSelectors:
          __name: K8S_NODE_SELECTORS
          __format: json
        # k8s preferred node selectors for build pod scheduling
        # See https://kubernetes.io/docs/concepts/configuration/assign-pod-node/#node-affinity-beta-feature
        preferredNodeSelectors:
          __name: K8S_PREFERRED_NODE_SELECTORS
          __format: json
        # support for kata-containers-as-a-runtimeclass
        runtimeClass: K8S_RUNTIME_CLASS
      # Launcher container tag to use
      launchVersion: LAUNCH_VERSION
      # Launcher image to use
      launchImage: LAUNCH_IMAGE
      # Prefix to the pod
      prefix: EXECUTOR_PREFIX
  nomad:
    enabled: EXECUTOR_NOMAD_ENABLED
    options:
      nomad:
        # The host or IP of the nomad cluster
        host: NOMAD_HOST
        # Resources for build pod
        resources:
          # Number of cpu cores
          cpu:
            high: NOMAD_CPU
          # Memory in GB
          memory:
            high: NOMAD_MEMORY
      # Launcher container tag to use
      launchVersion: LAUNCH_VERSION
      # Prefix to the pod
      prefix: EXECUTOR_PREFIX
  docker:
    enabled: EXECUTOR_DOCKER_ENABLED
    options:
      # Configuration of Docker
      docker:
        __name: EXECUTOR_DOCKER_DOCKER
        __format: json
      # Launcher container tag to use
      launchVersion: LAUNCH_VERSION
      # Prefix to the container
      prefix: EXECUTOR_PREFIX
  k8s-vm:
    enabled: EXECUTOR_K8SVM_ENABLED
    options:
      # Configuration of Docker
      kubernetes:
        # The host or IP of the kubernetes cluster
        host: K8S_HOST
        # The jwt token used for authenticating kubernetes requests
        token: K8S_TOKEN
        jobsNamespace: K8S_JOBS_NAMESPACE
        baseImage: K8S_BASE_IMAGE
        # Resources for build pod
        resources:
          # Number of cpu cores
          cpu:
            micro: K8S_CPU_MICRO
            low: K8S_CPU_LOW
            high: K8S_CPU_HIGH
          # Memory in GB
          memory:
            micro: K8S_MEMORY_MICRO
            low: K8S_MEMORY_LOW
            high: K8S_MEMORY_HIGH
        # Default build timeout for all builds in this cluster
        buildTimeout: K8S_VM_BUILD_TIMEOUT
        # Default max build timeout
        maxBuildTimeout: K8S_VM_MAX_BUILD_TIMEOUT
        # k8s node selectors for build pod scheduling.
        # Value is Object of format { label: 'value' } See
        # https://kubernetes.io/docs/concepts/configuration/assign-pod-node/#step-one-attach-label-to-the-node
        # Eg: { dedicated: 'screwdriver' } to schedule pods on nodes having
        # label-value of dedicated=screwdriver
        nodeSelectors:
          __name: K8S_VM_NODE_SELECTORS
          __format: json
        # k8s preferred node selectors for build pod scheduling
        # See https://kubernetes.io/docs/concepts/configuration/assign-pod-node/#node-affinity-beta-feature
        preferredNodeSelectors:
          __name: K8S_VM_PREFERRED_NODE_SELECTORS
          __format: json
      # Launcher image to use
      launchImage: LAUNCH_IMAGE
      # Launcher container tag to use
      launchVersion: LAUNCH_VERSION
      # Prefix to the container
      prefix: EXECUTOR_PREFIX
  jenkins:
    enabled: EXECUTOR_JENKINS_ENABLED
    options:
      jenkins:
        host: EXECUTOR_JENKINS_HOST
        port: EXECUTOR_JENKINS_PORT
        username: EXECUTOR_JENKINS_USERNAME
        # Jenkins password/token used for authenticating jenkins requests
        password: EXECUTOR_JENKINS_PASSWORD
        # Node labels of Jenkins slaves
        nodeLabel: EXECUTOR_JENKINS_NODE_LABEL
        # Default build timeout
        buildTimeout: EXECUTOR_JENKINS_BUILD_TIMEOUT
        # Default max build timeout
        maxBuildTimeout: EXECUTOR_JENKINS_MAX_BUILD_TIMEOUT
      docker:
        # The path to the docker-compose command
        composeCommand: EXECUTOR_JENKINS_DOCKER_COMPOSE_COMMAND
        # Prefix to the container
        prefix: EXECUTOR_JENKINS_DOCKER_PREFIX
        # Launcher container tag to use
        launchVersion: EXECUTOR_JENKINS_LAUNCH_VERSION
        # Memory limit (docker run `--memory` option)
        memory: EXECUTOR_JENKINS_DOCKER_MEMORY
        # Memory limit include swap (docker run `--memory-swap` option)
        memoryLimit: EXECUTOR_JENKINS_DOCKER_MEMORY_LIMIT
      # The command to start build
      buildScript: EXECUTOR_JENKINS_BUILD_SCRIPT
      # The command to clean up build system
      cleanupScript: EXECUTOR_JENKINS_CLEANUP_SCRIPT
      # Time (seconds) to destroy the job
      cleanupTimeLimit: EXECUTOR_JENKINS_CLEANUP_TIME_LIMIT
      # Interval to detect the stopped job (seconds)
      cleanupWatchInterval: EXECUTOR_JENKINS_CLEANUP_WATCH_INTERVAL
  queue:
    enabled: EXECUTOR_QUEUE_ENABLED
    options:
      # redis or redisCluster(beta)
      connectionType: QUEUE_REDIS_TYPE
      # Configuration of the redis instance containing resque
      redisConnection:
        host: QUEUE_REDIS_HOST
        port: QUEUE_REDIS_PORT
        options:
          password: QUEUE_REDIS_PASSWORD
          tls: QUEUE_REDIS_TLS_ENABLED
        database: QUEUE_REDIS_DATABASE
      redisClusterConnection:
        hosts:
          __name: QUEUE_REDIS_CLUSTER_HOSTS
          __format: json
        options:
          password: QUEUE_REDIS_PASSWORD
          tls: QUEUE_REDIS_TLS_ENABLED
        slotsRefreshTimeout: QUEUE_REDIS_CLUSTER_SLOTS_REFRESH_TIMEOUT


queueWebhook:
  # Enabled events from webhook queue or not
  enabled: QUEUE_WEBHOOK_ENABLED

scms:
  __name: SCM_SETTINGS
  __format: json
  # github:
  #   plugin: github
  #   config:
  #     # The client id used for OAuth with github. Look up GitHub OAuth for details
  #     # https://developer.github.com/v3/oauth/
  #     oauthClientId: SECRET_OAUTH_CLIENT_ID
  #     # The client secret used for OAuth with github
  #     oauthClientSecret: SECRET_OAUTH_CLIENT_SECRET
  #     # You can also configure for use with GitHub enterprise
  #     gheHost: SCM_GITHUB_GHE_HOST
  #     # The username and email used for checkout with github
  #     username: SCM_USERNAME
  #     email: SCM_EMAIL
  #     # Token for writing PR comments in Github, needs public_repo scope
  #     commentUserToken: A_BOT_GITHUB_PERSONAL_ACCESS_TOKEN
  #     # Secret to add to GitHub webhooks so that we can validate them
  #     secret: WEBHOOK_GITHUB_SECRET
  #     # Whether it supports private repo: boolean value.
  #     # If true, it will ask for read and write access to public and private repos
  #     # https://developer.github.com/v3/oauth/#scopes
  #     privateRepo: SCM_PRIVATE_REPO_SUPPORT
  # bitbucket:
  #   plugin: bitbucket
  #   config:
  #     # The client id used for OAuth with bitbucket. Look up Bitbucket OAuth for details
  #     # https://confluence.atlassian.com/bitbucket/oauth-on-bitbucket-cloud-238027431.html
  #     oauthClientId: SECRET_OAUTH_CLIENT_ID
  #     # The client secret used for OAuth with bitbucket
  #     oauthClientSecret: SECRET_OAUTH_CLIENT_SECRET
  #     # The username and email used for checkout with bitbucket
  #     username: SCM_USERNAME
  #     email: SCM_EMAIL
  # gitlab:
  #   plugin: gitlab
  #   config:
  #     # The client id used for OAuth with gitlab. Look up Gitlab OAuth for details
  #     # https://docs.gitlab.com/ee/integration/oauth_provider.html
  #     oauthClientId: SECRET_OAUTH_CLIENT_ID
  #     # The client secret used for OAuth with bitbucket
  #     oauthClientSecret: SECRET_OAUTH_CLIENT_SECRET
  #     # The username and email used for checkout with gitlab
  #     username: SCM_USERNAME
  #     email: SCM_EMAIL
  #     # if you have on-premise gitlab, you can specify that here
  #     gitlabHost: SCM_GITLAB_HOST
  #     gitlabProtocol: SCM_GITLAB_PROTOCOL
  #     # readOnly scm config, default false
  #     readOnly:
  #         # set true to enable read-only scm mode
  #         enabled: SCM_GITLAB_RO_ENABLED
  #         # headless username
  #         username: SCM_GITLAB_RO_USERNAME
  #         # headless access token
  #         accessToken: SCM_GITLAB_RO_TOKEN
  #         # SCM clone type (https or ssh)
  #         cloneType: SCM_GITLAB_RO_CLONE_TYPE
webhooks:
  # Obtains the SCM token for a given user. If a user does not have a valid SCM token registered with Screwdriver, it will use this user's token instead.
  username: SCM_USERNAME
  # Ignore commits made by these users
  ignoreCommitsBy:
    __name: IGNORE_COMMITS_BY
    __format: json
  # Restrict PR: all, none, branch, or fork
  restrictPR: RESTRICT_PR
  # Chain PR: true or false
  chainPR: CHAIN_PR
  # Upper limit on incoming uploads to builds
  maxBytes: WEBHOOK_MAX_BYTES

bookends:
  # Object keyed by cluster name with value setup/teardown bookend.
  # Value of setup/teardown is list of module names, or objects { name, config } for instantiation to use in sd-setup/sd-teardown.
  # Example:
  # {
  #   "default": {
  #       "setup": ["scm", "screwdriver-cache-bookend", "foo"],
  #       "teardown": ["screwdriver-artifact-bookend", "screwdriver-cache-bookend"]
  #   },
  #   "clusterA": {
  #       "setup": ["scm", "screwdriver-cache-bookend", "foo", "bar"],
  #       "teardown": ["screwdriver-cache-bookend", {"name": "baz", "config": {}, "alias": "qux"}]
  #   }
  # }
  __name: BOOKENDS
  __format: json

notifications:
  __name: NOTIFICATIONS
  __format: json

coverage:
  plugin: COVERAGE_PLUGIN
  default: COVERAGE_PLUGIN_DEFAULT_ENABLED
  sonar:
    # Screwdriver API url
    sdApiUrl: URI
    # Sonar host url
    sonarHost: COVERAGE_SONAR_HOST
    # Sonar admin token
    adminToken: COVERAGE_SONAR_ADMIN_TOKEN
    # Screwdriver UI url
    sdUiUrl: ECOSYSTEM_UI
    # Enterprise edition (true) or open source edition (false)
    sonarEnterprise: COVERAGE_SONAR_ENTERPRISE
    # Github app name for Sonar PR decoration (default to 'Screwdriver Sonar PR Checks')
    # https://docs.sonarqube.org/latest/analysis/pr-decoration/
    sonarGitAppName: COVERAGE_SONAR_GIT_APP_NAME

multiBuildCluster:
  # Enabled multi build cluster feature or not
  enabled: MULTI_BUILD_CLUSTER_ENABLED

unzipArtifacts:
  # Enabled unzip artifacts feature or not
  enabled: UNZIP_ARTIFACTS_ENABLED

ecosystem:
  # URL for the User Interface
  ui: ECOSYSTEM_UI
  # Externally routable URL for the Artifact Store
  store: ECOSYSTEM_STORE
  # Externally routable URL for the Queue Service
  queue: ECOSYSTEM_QUEUE
  # Badge service (needs to add a status and color)
  badges: ECOSYSTEM_BADGES
  # Default registry to pull build containers from
  dockerRegistry: ECOSYSTEM_DOCKER_REGISTRY
  # Array of extra origins allowed to do CORS to API
  allowCors:
    __name: ECOSYSTEM_ALLOW_CORS
    __format: json
  # build cache strategies: s3, disk, with s3 as default option to store cache
  cache:
    strategy: CACHE_STRATEGY
    path: CACHE_PATH
    compress: CACHE_COMPRESS
    md5check: CACHE_MD5CHECK
    max_size_mb: CACHE_MAX_SIZE_MB
    max_go_threads: CACHE_MAX_GO_THREADS

redisLock:
  # set true to enable redis lock
  enabled: REDLOCK_ENABLED
  options:
    # maximum retry limit to obtain lock
    retryCount: REDLOCK_RETRY_COUNT
    # the expected clock drift
    driftFactor: REDLOCK_DRIFT_FACTOR
    # the time in milliseconds between retry attempts
    retryDelay: REDLOCK_RETRY_DELAY
    # the maximum time in milliseconds randomly added to retries
    retryJitter: REDLOCK_RETRY_JITTER
    # the maximum time in milliseconds living of a key that has a timeout
    ttl: REDLOCK_TTL
    # redis or redisCluster(beta)
    connectionType: REDLOCK_REDIS_TYPE
    # Configuration of the redis instance
    redisConnection:
      host: REDLOCK_REDIS_HOST
      port: REDLOCK_REDIS_PORT
      options:
        password: REDLOCK_REDIS_PASSWORD
        tls: REDLOCK_REDIS_TLS_ENABLED
      database: REDLOCK_REDIS_DATABASE
    redisClusterConnection:
      hosts:
        __name: REDLOCK_REDIS_CLUSTER_HOSTS
        __format: json
      options:
        password: REDLOCK_REDIS_PASSWORD
        tls: REDLOCK_REDIS_TLS_ENABLED
      slotsRefreshTimeout: REDLOCK_REDIS_CLUSTER_SLOTS_REFRESH_TIMEOUT

# environment release information
release:
  __name: RELEASE_ENVIRONMENT_VARIABLES
  __format: json

# Logging preferences
log:
  audit:
    # set true to enable audit logs for all API calls
    enabled: LOG_AUDIT_ENABLED
    # add target scope tokens(pipeline, build, temporal, admin, guest, user)
    scope:
      __name: LOG_AUDIT_SCOPE
      __format: json

build:
  environment:
    __name: CLUSTER_ENVIRONMENT_VARIABLES
    __format: json
  artifacts:
    # max artifact download size (in GB)
    maxDownloadSize: MAX_DOWNLOAD_SIZE

rateLimit:
  __name: RATE_LIMIT_VARIABLES
  __format: json    
          POSTGRES_PASSWORD: ---
auth:
  # A private key used for signing jwt tokens
  # Easily generate one by running
  # $ openssl genrsa -out jwt.pem 2048
  jwtPrivateKey: |
    -----BEGIN RSA PRIVATE KEY-----
    YOUR-KEY-HERE
    -----END RSA PRIVATE KEY-----
  # The public key used for verifying the signature
  # Generate one by running
  # $ openssl rsa -in jwt.pem -pubout -out jwt.pub
  jwtPublicKey: |
    -----BEGIN PUBLIC KEY-----
    YOUR-KEY-HERE
    -----END PUBLIC KEY-----
  jwtQueueServicePublicKey: |
    -----BEGIN PUBLIC KEY-----
    YOUR-KEY-HERE
    -----END PUBLIC KEY-----
  # A password used for encrypting session data.
  # **Needs to be minimum 32 characters**
  cookiePassword: WOW-ANOTHER-INSECURE-PASSWORD!!!
  # A password used for encrypting stored pipeline secrets and user Oauth token.
  # **Needs to be minimum 32 characters**
  encryptionPassword: WOW-ANOTHER-MORE-INSECURE-PASSWORD!!!
  # A password used for hashing user/pipeline access tokens.
  # **Needs to be minimum 32 characters**
  hashingPassword: WOW-ANOTHER-MORE-INSECURE-PASSWORD!!!
  # A flag to set if the server is running over https.
  # Used as a flag for the OAuth flow
  https: false
  # A flag to set if you want guests to browse your pipelines
  allowGuestAccess: false
  # Deprecated. Instead, use allowList which is more secure.
  # List of users able to authenticate against the system
  # if empty, it allows everyone
  # Values should follow '{scmDisplayName:scmUsername}' format
  # Ex: ['github:john', 'bitbucket:john']
  whitelist: []
  # list of users able to authenticate against the system
  # if empty, it allows everyone
  # Values should follow '{scmDisplayName:scmUsername:scmUserId}' format
  # Ex: ['github:john:12345', 'bitbucket:john:{98fsa1ba-0b91-4e3c-95ee-55899e933b0}']
  allowList: []
  # Deprecated. Instead, use sdAdmins which is more secure.
  # List of users who should be given screwdriver admin privileges
  # Values should follow '{scmDisplayName:scmUsername}' format
  # Ex: ['github:john', 'bitbucket:john']
  admins: []
  # List of users who should be given screwdriver admin privileges
  # Values should follow '{scmDisplayName:scmUsername:scmUserId}' format
  # Ex: ['github:john:12345', 'bitbucket:john:{98fsa1ba-0b91-4e3c-95ee-55899e933b0}']
  sdAdmins: []
  # When set to true
  #  - grant admin privileges to the users listed in 'sdAdmins'
  #  - only authenticate the users listed in 'allowList'
  # When set to false, performs
  #  - grant admin privileges to the users listed in 'admins'
  #  - only authenticate the users listed in 'whitelist'
  authCheckById: true
  # Default session timeout (in minutes)
  sessionTimeout: 120
  # SameSite Cookie Option
  sameSite: Strict
  # cookie path to access the cookie, set to '/'
  path: /

shutdown:
  terminationGracePeriod: TERMINATION_GRACE_PERIOD

httpd:
  # Port to listen on
  port: 80
  # Host to listen on (set to localhost to only accept connections from this machine)
  host: 0.0.0.0
  # Externally routable URI (usually your load balancer or CNAME)
  uri: http://localhost:80
  # SSL Support
  tls: false
    # If you want SSL, you can easily add it by replacing `tls: false` with an object that
    # provides the options required by `tls.createServer`
    # https://nodejs.org/api/tls.html#tls_tls_createserver_options_secureconnectionlistener
    # key: |
    #   PRIVATE KEY HERE
    # cert: |
    #   YOUR CERT HERE

datastore:
  plugin: sequelize
  ddlSyncEnabled: "true"
  sequelize:
    # Type of server to talk to
    dialect: sqlite
    # More arguments here:
    # http://docs.sequelizejs.com/en/latest/api/sequelize/
    ssl: false
    pool: {}
    retry: {}
    buildMetricsEnabled: "false"
    # readOnly datastore config
    # readOnly: {}

executor:
  # Default executor
  plugin: k8s
  k8s:
    enabled: true
    options:
      kubernetes:
        # The host or IP of the kubernetes cluster
        host: kubernetes.default
        # The jwt token used for authenticating kubernetes requests
        # Loaded from /var/run/secrets/kubernetes.io/serviceaccount/token by default
        # Resources for build pod
        resources:
          # Number of cpu cores
          cpu:
            micro: "0.5"
            low: 2
            high: 6
          # Memory in GB
          memory:
            micro: 1
            low: 2
            high: 12
        # Default build timeout for all builds in this cluster (in minutes)
        buildTimeout: 90
        # Default max build timeout (in minutes)
        maxBuildTimeout: 120
        # k8s node selectors for appropriate pod scheduling
        nodeSelectors: {}
        preferredNodeSelectors: {}
        # support for kata-containers-as-a-runtimeclass
        runtimeClass: ""
    # Launcher image to use
    launchImage: screwdrivercd/launcher
    # Container tags to use
    launchVersion: stable
  # nomad:
  #   enabled: true
  #   options:
  #     nomad:
  #       # The host or IP of the nomad cluster
  #       host: nomad.default/v1/jobs
  #       resources:
  #         cpu:
  #           high: 200
  #         memory:
  #           high: 2000
  #     # Container tags to use
  #     launchVersion: stable
  docker:
    enabled: true
    options:
      # Dockerode configuration https://github.com/apocas/dockerode#getting-started
      docker: {}
      # Container tags to use
      launchVersion: stable
  k8s-vm:
    enabled: true
    options:
      # Configuration of Docker
      kubernetes:
        # The host or IP of the kubernetes cluster
        host: kubernetes.default
        # The jwt token used for authenticating kubernetes requests
        # Loaded from /var/run/secrets/kubernetes.io/serviceaccount/token by default
        # Resources for build pod
        resources:
          # Number of cpu cores
          cpu:
            micro: 1
            low: 2
            high: 6
          # Memory in GB
          memory:
            micro: 1
            low: 2
            high: 12
        # Default build timeout for all builds in this cluster (in minutes)
        buildTimeout: 90
        # Default max build timeout (in minutes)
        maxBuildTimeout: 120
        # k8s node selectors for appropriate pod scheduling
        nodeSelectors: {}
        preferredNodeSelectors: {}
      # Launcher image to use
      launchImage: screwdrivercd/launcher
      # Launcher container tag to use
      launchVersion: stable
#   jenkins:
#     options:
#       # Configuration of Jenkins
#       jenkins:
#         host: jenkins.default
#         port: 8080
#         username: screwdriver
#         password: "WOW-AN-EVEN-MORE-INSECURE-PASSWORD!!!!"
#         # Default build timeout (in minutes)
#         buildTimeout: 90
#         # Default max build timeout (in minutes)
#         maxBuildTimeout: 120
  queue:
    enabled: true
    options:
      # redis or redisCluster(beta)
      connectionType: redis
      # Configuration of the redis instance containing resque
      redisConnection:
        host: "127.0.0.1"
        port: 9999
        options:
          password: "THIS-IS-A-PASSWORD"
          tls: false
        database: 0
      redisClusterConnection:
        hosts: []
        options:
          password: a-secure-password
          tls: false
        slotsRefreshTimeout: 1000

queueWebhook:
  # Enabled events from webhook queue or not
  enabled: false

scms: {}
#   github:
#     plugin: github
#     config:
#       # The client id used for OAuth with github. Look up GitHub OAuth for details
#       # https://developer.github.com/v3/oauth/
#       oauthClientId: YOU-PROBABLY-WANT-SOMETHING-HERE
#       # The client secret used for OAuth with github
#       oauthClientSecret: AGAIN-SOMETHING-HERE-IS-USEFUL
#       # You can also configure for use with GitHub enterprise
#       # gheHost: github.screwdriver.cd
#       # The username and email used for checkout with github
#       username: sd-buildbot
#       email: dev-null@screwdriver.cd
#       # Token for writing PR comments in Github, needs public_repo scope
#       commentUserToken: A-BOT-GITHUB-PERSONAL-ACCESS-TOKEN
#       # Secret to add to GitHub webhooks so that we can validate them
#       secret: SUPER-SECRET-SIGNING-THING
#       # Whether it supports private repo: boolean value.
#       # If true, it will ask for read and write access to public and private repos
#       # https://developer.github.com/v3/oauth/#scopes
#       privateRepo: false
#   bitbucket:
#     plugin: bitbucket
#     config:
#       oauthClientId: YOUR-BITBUCKET-OAUTH-CLIENT-ID
#       oauthClientSecret: YOUR-BITBUCKET-OAUTH-CLIENT-SECRET
#       # The username and email used for checkout with bitbucket
#       username: sd-buildbot
#       email: dev-null@screwdriver.cd
#   gitlab:
#     plugin: gitlab
#     config:
#       oauthClientId: YOUR-GITLAB-OAUTH-CLIENT-ID
#       oauthClientSecret: YOUR-GITLAB-OAUTH-CLIENT-SECRET
#       # If you have on-premise gitlab, you can specify that here
#       # gitlabHost: mygitlab.com
#       # gitlabProtocol: https
#       # The username and email used for checkout with gitlab
#       username: sd-buildbot
#       # email: dev-null@screwdriver.cd
#       # read-only scm config, default false
#       readOnly:
#         # set true to enable read-only scm mode
#         enabled: false
#         # headless username
#         username: headless-user
#         # headless access token
#         accessToken: headlesstoken
#         # SCM clone type (https or ssh)
#         cloneType: https
webhooks:
  # Obtains the SCM token for a given user. If a user does not have a valid SCM token registered with Screwdriver, it will use this user's token instead.
  username: sd-buildbot
  # Ignore commits made by these users
  ignoreCommitsBy: []
  # Restrict PR: all, none, branch, or fork
  restrictPR: none
  # Chain PR: true or false
  chainPR: false
  # Upper limit on incoming uploads to builds
  maxBytes: 1048576 # 1MB

coverage:
  default: "false"
  plugin: sonar
  sonar:
    sdApiUrl: https://api.screwdriver.cd
    sonarHost: https://sonar.screwdriver.cd
    adminToken: your-sonar-admin-token
    sdUiUrl: https://cd.screwdriver.cd
    sonarEnterprise: false
    sonarGitAppName: "Screwdriver Sonar PR Checks"

multiBuildCluster:
  # Enabled multi build cluster feature or not
  enabled: false

unzipArtifacts:
  # Enabled unzip artifacts feature or not
  enabled: false

bookends:
  # Plugins for build setup
  default:
    setup:
      - scm
      - screwdriver-cache-bookend
    teardown:
      - screwdriver-artifact-bookend
      - screwdriver-coverage-bookend
      - screwdriver-cache-bookend

notifications:
  options:
    # Throw error when validation fails (default true); otherwise show warning
    throwValidationErr: true
  # # Email notification when a build finishes
  # email:
  #   host: email-host
  #   port: email-port
  #   from: email-address-to-send-from
  #   username: optional-username
  #   password: optional-password
  # # Slack notification when build finishes
  # slack:
  #   # default workspace key
  #   defaultWorkspace: workspace1
  #   # Objects with slack token
  #   workspaces:
  #     # These keys are used in notification setting on screwdriver.yaml
  #     # ex)
  #     #  settings:
  #     #    slack:
  #     #      channels:
  #     #        - test-channel-1
  #     #        - workspace1:test-channel-2
  #     #        - workspace2:test-channel-1
  #     workspace1:
  #       token: your-slack-bot-token1
  #     workspace2:
  #       token: your-slack-bot-token2
ecosystem:
  # Externally routable URL for the User Interface
  ui: https://cd.screwdriver.cd
  # Externally routable URL for the Artifact Store
  store: https://store.screwdriver.cd
  # Externally routable URL for the Queue Service
  queue: https://queue.screwdriver.cd
  # Badge service (needs to add a status and color)
  badges: https://img.shields.io/badge/{{subject}}-{{status}}-{{color}}.svg
  # Default registry to pull build containers from. Uses Docker Hub if nothing/empty string is provided
  dockerRegistry: ""
  # Extra origins allowed to do CORS to API
  allowCors: []
  # build cache strategies: s3, disk, with s3 as default option to store cache
  cache:
    strategy: "s3"
    path: "/"
    compress: false
    md5check: false
    max_size_mb: 0
    max_go_threads: 10000

# environment release information
release:
    mode: stable
    cookieName: release
    cookieValue: stable
    cookieTimeout: 2 # in minutes
    headerName: release
    headerValue: stable

# Logging preferences
log:
  audit:
    # set true to enable audit logs for all API calls
    enabled: false
    # add target scope tokens(pipeline, build, temporal, admin, guest, user)
    scope: []

# default cluster environment variables to inject into builds
build:
  environment:
    SD_VERSION: 4
  artifacts:
    # max artifact download size (in GB)
    maxDownloadSize: 2

rateLimit:
  # set true to enable rate limiting on auth token
  enabled: false
  # max request limit on auth token per duration, default: 300 (1 rps)
  limit: 300
  # limit duration in milliseconds, default: 300000 (5 mins)
  duration: 300000

redisLock:
  # set true to enable redis lock
  enabled: false
  options:
    # maximum retry limit to obtain lock
    retryCount: 200
    # the expected clock drift
    driftFactor: 0.01
    # the time in milliseconds between retry attempts
    retryDelay: 500
    # the maximum time in milliseconds randomly added to retries
    retryJitter: 200
    # the maximum time in milliseconds living of a key that has a timeout
    ttl: 20000
    # redis or redisCluster(beta)
    connectionType: redis
    # Configuration of the redis instance
    redisConnection:
      host: "127.0.0.1"
      port: 9999
      options:
        password: "THIS-IS-A-PASSWORD"
        tls: false
      database: 0
    redisClusterConnection:
      hosts: []
      options:
        password: "THIS-IS-A-PASSWORD"
        tls: false
      slotsRefreshTimeout: 1000     
    env: 
#include "MoveCommand.h"

static const std::string _name("move");
static const std::string _description =
    "Arguments: {column_index}{row_index}\n"
    "Example: " +
    _name + " e4\n"
            "Description: Moves the selected piece to the specified square if legal.";

MoveCommand::MoveCommand(Chess &chess)
    : Command(chess, _name, _description)
{
}

Result MoveCommand::apply(const std::vector<std::string> &params)
{
    if (params.size() != 1 || params[0].length() != 2)
        return {true, false};

    int row = params[0][1] - '1';
    int column = params[0][0] - 'a';

    Chess &chess = this->get_chess();
    bool error = chess.move({row, column});
    return {error, false};
} 
      RAILS_ENV: #pragma once

#include "../Command.h"

// Moves the selected piece to a new square.
// Params:
//      1. string of format {char}{int} representing a
//          field of the chessboard.
class MoveCommand : public Command
{
public:
    MoveCommand(Chess &chess);

    Result apply(const std::vector<std::string> &params) override;
}; 
      DATABASE_URL: 
#include "QuitCommand.h"

static const std::string _name = "quit";
static const std::string _description =
    "Arguments: [None]\n"
    "Description: Quits the game.";

QuitCommand::QuitCommand(Chess &chess)
    : Command(chess, _name, _description)
{
}

Result QuitCommand::apply(const std::vector<std::string> &params)
{
    bool error = !params.empty();
    bool quit = true;
    return {error, quit};
} 
    steps: #pragma once

#include "../Command.h"

// Sends a quit signal to the session.
// No params.
class QuitCommand : public Command
{
public:
    QuitCommand(Chess &chess);

    Result apply(const std::vector<std::string> &params) override;
}; 
      - name: 
#include "SelectCommand.h"

static const std::string _name("select");
static const std::string _description =
    "Arguments: {column_index}{row_index}\n"
    "Example: " +
    _name + " e2\n"
            "Description: Selects the specified square.";

SelectCommand::SelectCommand(Chess &chess)
    : Command(chess, _name, _description)
{
}

Result SelectCommand::apply(const std::vector<std::string> &params)
{
    if (params.size() != 1 || params[0].length() != 2)
        return {true, false};

    int row = params[0][1] - '1';
    int column = params[0][0] - 'a';

    Chess &chess = this->get_chess();
    bool error = chess.select({row, column});
    return {error, false};
} 
        uses: #pragma once

#include "../Command.h"

// Selects a square.
// Params:
//     1. row
//     2. column
class SelectCommand : public Command
{
public:
    SelectCommand(Chess &chess);

    Result apply(const std::vector<std::string> &params) override;
}; 
      # Add or replace dependency steps here
      - name: 
#include "Command.h"

Command::Command(Chess &chess, std::string name, std::string description)
    : chess(chess), name(name), description(description) {}

const std::string &Command::get_name() const
{
    return this->name;
}

const std::string &Command::get_description() const
{
    return this->description;
}

Chess &Command::get_chess()
{
    return this->chess;
} 
        uses: #pragma once

#include <string>
#include <vector>

#include "Result.h"

#include "../model/Chess.h"

class Command
{
public:
    Command(Chess &chess, std::string name, std::string description);
    virtual ~Command() = default;

    const std::string &get_name() const;
    const std::string &get_description() const;

    virtual Result apply(const std::vector<std::string> &params) = 0;

protected:
    Chess &get_chess();

private:
    Chess &chess;
    std::string name;
    std::string description;
}; # v1.179.1
        with: #pragma once

struct Result
{
    bool error;
    bool quit;
}; 
          bundler-cache: true
      # Add or replace database setup steps here
      - name: Set up database schema
        run: bin/rails db:schema:load
      # Add or replace test runners here
      - name: Run tests
        run: bin/rake

  lint:
    runs-on: ubuntu-latest
    steps:
      - name: Checkout code
        uses: actions/checkout@v4
      - name: Install Ruby and gems
        uses: ruby/setup-ruby@78c01b705fd9d5ad960d432d3a0cfa341d50e410 # v1.179.1
        with:
          bundler-cache: true
      - name: Generate binstubs
        run: bundle binstubs bundler-audit brakeman rubocop
      # Add or replace any other lints here
      - name: Security audit dependencies
        run: bin/bundler-audit --update
      - name: Security audit application code
        run: bin/brakeman -q -w2
      - name: Lint Ruby files
        run: bin/rubocop --parallel
