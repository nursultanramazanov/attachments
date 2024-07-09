# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
#
# Find more information at:
# https://github.com/microsoft/msvc-code-analysis-action

name: Microsoft C++ Code Analysis

on: // This file is just a placeholder for your configuration file.  If you wish to change any of the setup parameters from
// their default values, place the appropriate #define statements here.

  push: /*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
   This is the APMrover2 firmware. It was originally derived from
   ArduPlane by Jean-Louis Naudin (JLN), and then rewritten after the
   AP_HAL merge by Andrew Tridgell

   Maintainer: Grant Morphett

   Authors:    Doug Weibel, Jose Julio, Jordi Munoz, Jason Short, Andrew Tridgell, Randy Mackay, Pat Hickey, John Arne Birkeland, Olivier Adler, Jean-Louis Naudin, Grant Morphett

   Thanks to:  Chris Anderson, Michael Oborne, Paul Mather, Bill Premerlani, James Cohen, JB from rotorFX, Automatik, Fefenin, Peter Meister, Remzibi, Yury Smirnov, Sandro Benigno, Max Levine, Roberto Navoni, Lorenz Meier 

   APMrover alpha version tester: Franco Borasio, Daniel Chapelat...

   Please contribute your ideas! See http://dev.ardupilot.org for details
*/

#include "Rover.h"

const AP_HAL::HAL& hal = AP_HAL::get_HAL();

Rover rover;

#define SCHED_TASK(func, _interval_ticks, _max_time_micros) SCHED_TASK_CLASS(Rover, &rover, func, _interval_ticks, _max_time_micros)

/*
  scheduler table - all regular tasks are listed here, along with how
  often they should be called (in Hz) and the maximum time
  they are expected to take (in microseconds)
 */
const AP_Scheduler::Task Rover::scheduler_tasks[] = {
    //         Function name,          Hz,     us,
    SCHED_TASK(read_radio,             50,    200),
    SCHED_TASK(ahrs_update,            50,   1500),
    SCHED_TASK(read_rangefinders,      50,    200),
    SCHED_TASK(update_current_mode,    50,    200),
    SCHED_TASK(set_servos,             50,    200),
    SCHED_TASK_CLASS(AP_GPS,              &rover.gps,              update,         50,  300),
    SCHED_TASK(update_GPS_10Hz,        10,    300),
    SCHED_TASK_CLASS(AP_Baro,             &rover.barometer,        update,         10,  200),
    SCHED_TASK_CLASS(AP_Beacon,           &rover.g2.beacon,        update,         50,  200),
    SCHED_TASK_CLASS(AP_Proximity,        &rover.g2.proximity,     update,         50,  200),
    SCHED_TASK(update_visual_odom,     50,    200),
    SCHED_TASK(update_wheel_encoder,   20,    200),
    SCHED_TASK(update_compass,         10,    200),
    SCHED_TASK(update_mission,         50,    200),
    SCHED_TASK(update_logging1,        10,    200),
    SCHED_TASK(update_logging2,        10,    200),
    SCHED_TASK(gcs_retry_deferred,     50,   1000),
    SCHED_TASK(gcs_update,             50,   1000),
    SCHED_TASK(gcs_data_stream_send,   50,   3000),
    SCHED_TASK(read_control_switch,     7,    200),
    SCHED_TASK(read_aux_switch,        10,    200),
    SCHED_TASK_CLASS(AP_BattMonitor,      &rover.battery,          read,           10,  300),
    SCHED_TASK(read_receiver_rssi,     10,    200),
    SCHED_TASK_CLASS(AP_ServoRelayEvents, &rover.ServoRelayEvents, update_events,  50,  200),
    SCHED_TASK(check_usb_mux,           3,    200),
#if MOUNT == ENABLED
    SCHED_TASK_CLASS(AP_Mount,            &rover.camera_mount,     update,         50,  200),
#endif
#if CAMERA == ENABLED
    SCHED_TASK_CLASS(AP_Camera,           &rover.camera,           update_trigger, 50,  200),
#endif
    SCHED_TASK(gcs_failsafe_check,     10,    200),
    SCHED_TASK(fence_check,            10,    200),
    SCHED_TASK(compass_accumulate,     50,    200),
    SCHED_TASK_CLASS(ModeSmartRTL,        &rover.mode_smartrtl,    save_position,   3,  200),
    SCHED_TASK_CLASS(AP_Notify,           &rover.notify,           update,         50,  300),
    SCHED_TASK(one_second_loop,         1,   1500),
    SCHED_TASK(compass_cal_update,     50,    200),
    SCHED_TASK(accel_cal_update,       10,    200),
    SCHED_TASK_CLASS(DataFlash_Class,     &rover.DataFlash,        periodic_tasks, 50,  300),
    SCHED_TASK_CLASS(AP_InertialSensor,   &rover.ins,              periodic,       50,  200),
    SCHED_TASK_CLASS(AP_Scheduler,        &rover.scheduler,        update_logging, 0.1, 200),
    SCHED_TASK_CLASS(AP_Button,           &rover.button,           update,          5,  200),
#if STATS_ENABLED == ENABLED
    SCHED_TASK(stats_update,            1,    200),
#endif
    SCHED_TASK(crash_check,            10,    200),
    SCHED_TASK(cruise_learn_update,    50,    200),
#if ADVANCED_FAILSAFE == ENABLED
    SCHED_TASK(afs_fs_check,           10,    200),
#endif
};

constexpr int8_t Rover::_failsafe_priorities[7];

#if STATS_ENABLED == ENABLED
/*
  update AP_Stats
*/
void Rover::stats_update(void)
{
    g2.stats.set_flying(motor_active());
    g2.stats.update();
}
#endif

/*
  setup is called when the sketch starts
 */
void Rover::setup()
{
    // load the default values of variables listed in var_info[]
    AP_Param::setup_sketch_defaults();

    init_ardupilot();

    // initialise the main loop scheduler
    scheduler.init(&scheduler_tasks[0], ARRAY_SIZE(scheduler_tasks), MASK_LOG_PM);
}

/*
  loop() is called rapidly while the sketch is running
 */
void Rover::loop()
{
    scheduler.loop();
    G_Dt = scheduler.get_last_loop_time_s();
}

void Rover::update_soft_armed()
{
    hal.util->set_soft_armed(arming.is_armed() &&
                             hal.util->safety_switch_state() != AP_HAL::Util::SAFETY_DISARMED);
    DataFlash.set_vehicle_armed(hal.util->get_soft_armed());
}

// update AHRS system
void Rover::ahrs_update()
{
    update_soft_armed();

#if HIL_MODE != HIL_MODE_DISABLED
    // update hil before AHRS update
    gcs_update();
#endif

    // when in reverse we need to tell AHRS not to assume we are a
    // 'fly forward' vehicle, otherwise it will see a large
    // discrepancy between the mag and the GPS heading and will try to
    // correct for it, leading to a large yaw error
    ahrs.set_fly_forward(!in_reverse);

    ahrs.update();

    // update home from EKF if necessary
    update_home_from_EKF();

    // if using the EKF get a speed update now (from accelerometers)
    Vector3f velocity;
    if (ahrs.get_velocity_NED(velocity)) {
        ground_speed = norm(velocity.x, velocity.y);
    } else if (gps.status() >= AP_GPS::GPS_OK_FIX_3D) {
        ground_speed = ahrs.groundspeed();
    }

    if (should_log(MASK_LOG_ATTITUDE_FAST)) {
        Log_Write_Attitude();
    }

    if (should_log(MASK_LOG_IMU)) {
        DataFlash.Log_Write_IMU();
    }
}

/*
  check for GCS failsafe - 10Hz
 */
void Rover::gcs_failsafe_check(void)
{
    if (g.fs_gcs_enabled) {
        failsafe_trigger(FAILSAFE_EVENT_GCS, last_heartbeat_ms != 0 && (millis() - last_heartbeat_ms) > 2000);
    }
}

/*
  check for new compass data - 10Hz
 */
void Rover::update_compass(void)
{
    if (g.compass_enabled && compass.read()) {
        ahrs.set_compass(&compass);
        // update offsets
        if (should_log(MASK_LOG_COMPASS)) {
            DataFlash.Log_Write_Compass(compass);
        }
    }
}

/*
  log some key data - 10Hz
 */
void Rover::update_logging1(void)
{
    if (should_log(MASK_LOG_ATTITUDE_MED) && !should_log(MASK_LOG_ATTITUDE_FAST)) {
        Log_Write_Attitude();
    }

    if (should_log(MASK_LOG_THR)) {
        Log_Write_Throttle();
        DataFlash.Log_Write_Beacon(g2.beacon);
        Log_Write_Proximity();
    }

    if (should_log(MASK_LOG_NTUN)) {
        Log_Write_Nav_Tuning();
    }
}

/*
  log some key data - 10Hz
 */
void Rover::update_logging2(void)
{
    if (should_log(MASK_LOG_STEERING)) {
        Log_Write_Steering();
    }

    if (should_log(MASK_LOG_RC)) {
        Log_Write_RC();
        Log_Write_WheelEncoder();
    }

    if (should_log(MASK_LOG_IMU)) {
        DataFlash.Log_Write_Vibration();
    }
}


/*
  update aux servo mappings
 */
void Rover::update_aux(void)
{
    SRV_Channels::enable_aux_servos();
}

/*
  once a second events
 */
void Rover::one_second_loop(void)
{
    // send a heartbeat
    gcs().send_message(MSG_HEARTBEAT);

    // allow orientation change at runtime to aid config
    ahrs.set_orientation();

    set_control_channels();

    // cope with changes to aux functions
    update_aux();

    // update notify flags
    AP_Notify::flags.pre_arm_check = arming.pre_arm_checks(false);
    AP_Notify::flags.pre_arm_gps_check = true;
    AP_Notify::flags.armed = arming.is_armed() || arming.arming_required() == AP_Arming::NO;

    // cope with changes to mavlink system ID
    mavlink_system.sysid = g.sysid_this_mav;

    static uint8_t counter;

    counter++;

    // save compass offsets once a minute
    if (counter >= 60) {
        if (g.compass_enabled) {
            compass.save_offsets();
        }
        counter = 0;
    }

    // update home position if not soft armed and gps position has
    // changed. Update every 1s at most
    if (!hal.util->get_soft_armed() &&
        gps.status() >= AP_GPS::GPS_OK_FIX_3D) {
        update_home();
    }

    // update error mask of sensors and subsystems. The mask uses the
    // MAV_SYS_STATUS_* values from mavlink. If a bit is set then it
    // indicates that the sensor or subsystem is present but not
    // functioning correctly
    update_sensor_status_flags();
}

void Rover::update_GPS_10Hz(void)
{
    have_position = ahrs.get_position(current_loc);

    if (gps.last_message_time_ms() != last_gps_msg_ms) {
        last_gps_msg_ms = gps.last_message_time_ms();

        // set system time if necessary
        set_system_time_from_GPS();
#if CAMERA == ENABLED
        camera.update();
#endif
    }
}

void Rover::update_current_mode(void)
{
    control_mode->update();
}

AP_HAL_MAIN_CALLBACKS(&rover);
    branches: [ "main" ]
  pull_request: #include "AP_Arming.h"
#include "Rover.h"

// perform pre_arm_rc_checks checks
bool AP_Arming_Rover::pre_arm_rc_checks(const bool display_failure)
{
    // set rc-checks to success if RC checks are disabled
    if ((checks_to_perform != ARMING_CHECK_ALL) && !(checks_to_perform & ARMING_CHECK_RC)) {
        return true;
    }

    const RC_Channel *channels[] = {
            rover.channel_steer,
            rover.channel_throttle,
    };
    const char *channel_names[] = {"Steer", "Throttle"};

    for (uint8_t i= 0 ; i < ARRAY_SIZE(channels); i++) {
        const RC_Channel *channel = channels[i];
        const char *channel_name = channel_names[i];
        // check if radio has been calibrated
        if (!channel->min_max_configured()) {
            if (display_failure) {
                gcs().send_text(MAV_SEVERITY_CRITICAL, "PreArm: RC %s not configured", channel_name);
            }
            return false;
        }
        if (channel->get_radio_min() > 1300) {
            if (display_failure) {
                gcs().send_text(MAV_SEVERITY_CRITICAL, "PreArm: %s radio min too high", channel_name);
            }
            return false;
        }
        if (channel->get_radio_max() < 1700) {
            if (display_failure) {
                gcs().send_text(MAV_SEVERITY_CRITICAL, "PreArm: %s radio max too low", channel_name);
            }
            return false;
        }
        if (channel->get_radio_trim() < channel->get_radio_min()) {
            if (display_failure) {
                gcs().send_text(MAV_SEVERITY_CRITICAL, "PreArm: %s radio trim below min", channel_name);
            }
            return false;
        }
        if (channel->get_radio_trim() > channel->get_radio_max()) {
            if (display_failure) {
                gcs().send_text(MAV_SEVERITY_CRITICAL, "PreArm: %s radio trim above max", channel_name);
            }
            return false;
        }
    }
    return true;
}

// performs pre_arm gps related checks and returns true if passed
bool AP_Arming_Rover::gps_checks(bool display_failure)
{
    if (!rover.control_mode->requires_position() && !rover.control_mode->requires_velocity()) {
        // we don't care!
        return true;
    }

    // call parent gps checks
    return AP_Arming::gps_checks(display_failure);
}

bool AP_Arming_Rover::pre_arm_checks(bool report)
{
    return (AP_Arming::pre_arm_checks(report)
            & rover.g2.motors.pre_arm_check(report)
            & fence_checks(report)
            & proximity_check(report));
}

bool AP_Arming_Rover::fence_checks(bool report)
{
    // check fence is initialised
    const char *fail_msg = nullptr;
    if (!_fence.pre_arm_check(fail_msg)) {
        if (report && fail_msg != nullptr) {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "PreArm: Fence : %s", fail_msg);
        }
        return false;
    }
    return true;
}

// check nothing is too close to vehicle
bool AP_Arming_Rover::proximity_check(bool report)
{
    // return true immediately if no sensor present
    if (rover.g2.proximity.get_status() == AP_Proximity::Proximity_NotConnected) {
        return true;
    }

    // return false if proximity sensor unhealthy
    if (rover.g2.proximity.get_status() < AP_Proximity::Proximity_Good) {
        if (report) {
            gcs().send_text(MAV_SEVERITY_CRITICAL,"PreArm: check proximity sensor");
        }
        return false;
    }

    // get closest object if we might use it for avoidance
    float angle_deg, distance;
    if (rover.g2.avoid.proximity_avoidance_enabled() && rover.g2.proximity.get_closest_object(angle_deg, distance)) {
        // display error if something is within 60cm
        if (distance <= 0.6f) {
            if (report) {
                gcs().send_text(MAV_SEVERITY_CRITICAL, "PreArm: Proximity %d deg, %4.2fm", static_cast<int32_t>(angle_deg), static_cast<double>(distance));
            }
            return false;
        }
    }

    return true;
}
    branches: [ "main" ]
  schedule: #pragma once

#include <AP_Arming/AP_Arming.h>
#include <AC_Fence/AC_Fence.h>

/*
  a rover-specific arming class
 */
class AP_Arming_Rover : public AP_Arming
{
public:
    AP_Arming_Rover(const AP_AHRS &ahrs_ref, Compass &compass,
                    const AP_BattMonitor &battery, const AC_Fence &fence)
        : AP_Arming(ahrs_ref, compass, battery),
          _fence(fence)
    {
    }

    /* Do not allow copies */
    AP_Arming_Rover(const AP_Arming_Rover &other) = delete;
    AP_Arming_Rover &operator=(const AP_Arming_Rover&) = delete;

    bool pre_arm_checks(bool report) override;
    bool pre_arm_rc_checks(const bool display_failure);
    bool gps_checks(bool display_failure) override;

protected:
    bool fence_checks(bool report);
    bool proximity_check(bool report);

private:
    const AC_Fence& _fence;
};
    - cron: '25 8 * * 6'

env: /*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <AP_HAL/AP_HAL.h>
#include "SRV_Channel/SRV_Channel.h"
#include "AP_MotorsUGV.h"
#include "Rover.h"

extern const AP_HAL::HAL& hal;

// parameters for the motor class
const AP_Param::GroupInfo AP_MotorsUGV::var_info[] = {
    // @Param: PWM_TYPE
    // @DisplayName: Motor Output PWM type
    // @Description: This selects the output PWM type as regular PWM, OneShot, Brushed motor support using PWM (duty cycle) with separated direction signal, Brushed motor support with separate throttle and direction PWM (duty cyle)
    // @Values: 0:Normal,1:OneShot,2:OneShot125,3:BrushedWithRelay,4:BrushedBiPolar
    // @User: Advanced
    // @RebootRequired: True
    AP_GROUPINFO("PWM_TYPE", 1, AP_MotorsUGV, _pwm_type, PWM_TYPE_NORMAL),

    // @Param: PWM_FREQ
    // @DisplayName: Motor Output PWM freq for brushed motors
    // @Description: Motor Output PWM freq for brushed motors
    // @Units: kHz
    // @Range: 1 20
    // @Increment: 1
    // @User: Advanced
    // @RebootRequired: True
    AP_GROUPINFO("PWM_FREQ", 2, AP_MotorsUGV, _pwm_freq, 16),

    // @Param: SAFE_DISARM
    // @DisplayName: Motor PWM output disabled when disarmed
    // @Description: Disables motor PWM output when disarmed
    // @Values: 0:PWM enabled while disarmed, 1:PWM disabled while disarmed
    // @User: Advanced
    AP_GROUPINFO("SAFE_DISARM", 3, AP_MotorsUGV, _disarm_disable_pwm, 0),

    // @Param: THR_MIN
    // @DisplayName: Throttle minimum
    // @Description: Throttle minimum percentage the autopilot will apply. This is useful for handling a deadzone around low throttle and for preventing internal combustion motors cutting out during missions.
    // @Units: %
    // @Range: 0 20
    // @Increment: 1
    // @User: Standard
    AP_GROUPINFO("THR_MIN", 5, AP_MotorsUGV, _throttle_min, 0),

    // @Param: THR_MAX
    // @DisplayName: Throttle maximum
    // @Description: Throttle maximum percentage the autopilot will apply. This can be used to prevent overheating an ESC or motor on an electric rover
    // @Units: %
    // @Range: 30 100
    // @Increment: 1
    // @User: Standard
    AP_GROUPINFO("THR_MAX", 6, AP_MotorsUGV, _throttle_max, 100),

    // @Param: SLEWRATE
    // @DisplayName: Throttle slew rate
    // @Description: Throttle slew rate as a percentage of total range per second. A value of 100 allows the motor to change over its full range in one second.  A value of zero disables the limit.  Note some NiMH powered rovers require a lower setting of 40 to reduce current demand to avoid brownouts.
    // @Units: %/s
    // @Range: 0 1000
    // @Increment: 1
    // @User: Standard
    AP_GROUPINFO("SLEWRATE", 8, AP_MotorsUGV, _slew_rate, 100),

    // @Param: THST_EXPO
    // @DisplayName: Thrust Curve Expo
    // @Description: Thrust curve exponent (-1 to +1 with 0 being linear)
    // @Range: -1.0 1.0
    // @User: Advanced
    AP_GROUPINFO("THST_EXPO", 9, AP_MotorsUGV, _thrust_curve_expo, 0.0f),

    // @Param: VEC_THR_BASE
    // @DisplayName: Vector thrust throttle base
    // @Description: Throttle level above which steering is scaled down when using vector thrust.  zero to disable vectored thrust
    // @Units: %
    // @Range: 0 100
    // @User: Advanced
    AP_GROUPINFO("VEC_THR_BASE", 10, AP_MotorsUGV, _vector_throttle_base, 0.0f),

    AP_GROUPEND
};

AP_MotorsUGV::AP_MotorsUGV(AP_ServoRelayEvents &relayEvents) :
        _relayEvents(relayEvents)
{
    AP_Param::setup_object_defaults(this, var_info);
}

void AP_MotorsUGV::init()
{
    // setup servo ouput
    setup_servo_output();

    // setup pwm type
    setup_pwm_type();

    // set safety output
    setup_safety_output();

    // sanity check parameters
    _vector_throttle_base = constrain_float(_vector_throttle_base, 0.0f, 100.0f);
}

// setup output in case of main CPU failure
void AP_MotorsUGV::setup_safety_output()
{
    if (_pwm_type == PWM_TYPE_BRUSHED_WITH_RELAY) {
        // set trim to min to set duty cycle range (0 - 100%) to servo range
        SRV_Channels::set_trim_to_min_for(SRV_Channel::k_throttle);
        SRV_Channels::set_trim_to_min_for(SRV_Channel::k_throttleLeft);
        SRV_Channels::set_trim_to_min_for(SRV_Channel::k_throttleRight);
    }

    if (_disarm_disable_pwm) {
        // throttle channels output zero pwm (i.e. no signal)
        SRV_Channels::set_safety_limit(SRV_Channel::k_throttle, SRV_Channel::SRV_CHANNEL_LIMIT_ZERO_PWM);
        SRV_Channels::set_safety_limit(SRV_Channel::k_throttleLeft, SRV_Channel::SRV_CHANNEL_LIMIT_ZERO_PWM);
        SRV_Channels::set_safety_limit(SRV_Channel::k_throttleRight, SRV_Channel::SRV_CHANNEL_LIMIT_ZERO_PWM);
    } else {
        // throttle channels output trim values (because rovers will go backwards if set to MIN)
        SRV_Channels::set_safety_limit(SRV_Channel::k_throttle, SRV_Channel::SRV_CHANNEL_LIMIT_TRIM);
        SRV_Channels::set_safety_limit(SRV_Channel::k_throttleLeft, SRV_Channel::SRV_CHANNEL_LIMIT_TRIM);
        SRV_Channels::set_safety_limit(SRV_Channel::k_throttleRight, SRV_Channel::SRV_CHANNEL_LIMIT_TRIM);
    }

    // stop sending pwm if main CPU fails
    SRV_Channels::set_failsafe_limit(SRV_Channel::k_throttle, SRV_Channel::SRV_CHANNEL_LIMIT_ZERO_PWM);
    SRV_Channels::set_failsafe_limit(SRV_Channel::k_throttleLeft, SRV_Channel::SRV_CHANNEL_LIMIT_ZERO_PWM);
    SRV_Channels::set_failsafe_limit(SRV_Channel::k_throttleRight, SRV_Channel::SRV_CHANNEL_LIMIT_ZERO_PWM);
}

// setup servo output ranges
void AP_MotorsUGV::setup_servo_output()
{
    // k_steering are limited to -45;45 degree
    SRV_Channels::set_angle(SRV_Channel::k_steering, SERVO_MAX);

    // k_throttle are in power percent so -100 ... 100
    SRV_Channels::set_angle(SRV_Channel::k_throttle, 100);

    // skid steering left/right throttle as -1000 to 1000 values
    SRV_Channels::set_angle(SRV_Channel::k_throttleLeft,  1000);
    SRV_Channels::set_angle(SRV_Channel::k_throttleRight, 1000);
}

// set steering as a value from -4500 to +4500
void AP_MotorsUGV::set_steering(float steering)
{
    _steering = constrain_float(steering, -4500.0f, 4500.0f);
}

// set throttle as a value from -100 to 100
void AP_MotorsUGV::set_throttle(float throttle)
{
    // sanity check throttle min and max
    _throttle_min = constrain_int16(_throttle_min, 0, 20);
    _throttle_max = constrain_int16(_throttle_max, 30, 100);

    // check throttle is between -_throttle_max ~ +_throttle_max but outside -throttle_min ~ +throttle_min
    _throttle = constrain_float(throttle, -_throttle_max, _throttle_max);
}

/*
  work out if skid steering is available
 */
bool AP_MotorsUGV::have_skid_steering() const
{
    if (SRV_Channels::function_assigned(SRV_Channel::k_throttleLeft) &&
        SRV_Channels::function_assigned(SRV_Channel::k_throttleRight)) {
        return true;
    }
    return false;
}

void AP_MotorsUGV::output(bool armed, float dt)
{
    // soft-armed overrides passed in armed status
    if (!hal.util->get_soft_armed()) {
        armed = false;
    }

    // clear and set limits based on input (limit flags may be set again by output_regular or output_skid_steering methods)
    set_limits_from_input(armed, _steering, _throttle);

    // slew limit throttle
    slew_limit_throttle(dt);

    // output for regular steering/throttle style frames
    output_regular(armed, _steering, _throttle);

    // output for skid steering style frames
    output_skid_steering(armed, _steering, _throttle);

    // send values to the PWM timers for output
    SRV_Channels::calc_pwm();
    SRV_Channels::cork();
    SRV_Channels::output_ch_all();
    SRV_Channels::push();
}

// test steering or throttle output as a percentage of the total (range -100 to +100)
// used in response to DO_MOTOR_TEST mavlink command
bool AP_MotorsUGV::output_test_pct(motor_test_order motor_seq, float pct)
{
    // check if the motor_seq is valid
    if (motor_seq > MOTOR_TEST_THROTTLE_RIGHT) {
        return false;
    }
    pct = constrain_float(pct, -100.0f, 100.0f);

    switch (motor_seq) {
        case MOTOR_TEST_THROTTLE: {
            if (!SRV_Channels::function_assigned(SRV_Channel::k_throttle)) {
                return false;
            }
            output_throttle(SRV_Channel::k_throttle, pct);
            break;
        }
        case MOTOR_TEST_STEERING: {
            if (!SRV_Channels::function_assigned(SRV_Channel::k_steering)) {
                return false;
            }
            SRV_Channels::set_output_scaled(SRV_Channel::k_steering, pct * 45.0f);
            break;
        }
        case MOTOR_TEST_THROTTLE_LEFT: {
            if (!SRV_Channels::function_assigned(SRV_Channel::k_throttleLeft)) {
                return false;
            }
            output_throttle(SRV_Channel::k_throttleLeft, pct);
            break;
        }
        case MOTOR_TEST_THROTTLE_RIGHT: {
            if (!SRV_Channels::function_assigned(SRV_Channel::k_throttleRight)) {
                return false;
            }
            output_throttle(SRV_Channel::k_throttleRight, pct);
            break;
        }
        default:
            return false;
    }
    SRV_Channels::calc_pwm();
    SRV_Channels::cork();
    SRV_Channels::output_ch_all();
    SRV_Channels::push();
    return true;
}

// test steering or throttle output using a pwm value
bool AP_MotorsUGV::output_test_pwm(motor_test_order motor_seq, float pwm)
{
    // check if the motor_seq is valid
    if (motor_seq > MOTOR_TEST_THROTTLE_RIGHT) {
        return false;
    }
    switch (motor_seq) {
        case MOTOR_TEST_THROTTLE: {
            if (!SRV_Channels::function_assigned(SRV_Channel::k_throttle)) {
                return false;
            }
            SRV_Channels::set_output_pwm(SRV_Channel::k_throttle, pwm);
            break;
        }
        case MOTOR_TEST_STEERING: {
            if (!SRV_Channels::function_assigned(SRV_Channel::k_steering)) {
                return false;
            }
            SRV_Channels::set_output_pwm(SRV_Channel::k_steering, pwm);
            break;
        }
        case MOTOR_TEST_THROTTLE_LEFT: {
            if (!SRV_Channels::function_assigned(SRV_Channel::k_throttleLeft)) {
                return false;
            }
            SRV_Channels::set_output_pwm(SRV_Channel::k_throttleLeft, pwm);
            break;
        }
        case MOTOR_TEST_THROTTLE_RIGHT: {
            if (!SRV_Channels::function_assigned(SRV_Channel::k_throttleRight)) {
                return false;
            }
            SRV_Channels::set_output_pwm(SRV_Channel::k_throttleRight, pwm);
            break;
        }
        default:
            return false;
    }
    SRV_Channels::calc_pwm();
    SRV_Channels::cork();
    SRV_Channels::output_ch_all();
    SRV_Channels::push();
    return true;
}

//  returns true if checks pass, false if they fail.  report should be true to send text messages to GCS
bool AP_MotorsUGV::pre_arm_check(bool report) const
{
    // check if both regular and skid steering functions have been defined
    if (SRV_Channels::function_assigned(SRV_Channel::k_throttleLeft) &&
        SRV_Channels::function_assigned(SRV_Channel::k_throttleRight) &&
        SRV_Channels::function_assigned(SRV_Channel::k_throttle) &&
        SRV_Channels::function_assigned(SRV_Channel::k_steering)) {
        if (report) {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "PreArm: regular AND skid steering configured");
        }
        return false;
    }
    // check if only one of skid-steering output has been configured
    if (SRV_Channels::function_assigned(SRV_Channel::k_throttleLeft) != SRV_Channels::function_assigned(SRV_Channel::k_throttleRight)) {
        if (report) {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "PreArm: check skid steering config");
        }
        return false;
    }
    // check if only one of throttle or steering outputs has been configured
    if (SRV_Channels::function_assigned(SRV_Channel::k_throttle) != SRV_Channels::function_assigned(SRV_Channel::k_steering)) {
        if (report) {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "PreArm: check steering and throttle config");
        }
        return false;
    }
    return true;
}

// setup pwm output type
void AP_MotorsUGV::setup_pwm_type()
{
    switch (_pwm_type) {
    case PWM_TYPE_ONESHOT:
        hal.rcout->set_output_mode(0xFFFF, AP_HAL::RCOutput::MODE_PWM_ONESHOT);
        break;
    case PWM_TYPE_ONESHOT125:
        hal.rcout->set_output_mode(0xFFFF, AP_HAL::RCOutput::MODE_PWM_ONESHOT125);
        break;
    case PWM_TYPE_BRUSHED_WITH_RELAY:
    case PWM_TYPE_BRUSHED_BIPOLAR:
        hal.rcout->set_output_mode(0xFFFF, AP_HAL::RCOutput::MODE_PWM_BRUSHED);
        /*
         * Group 0: channels 0 1
         * Group 1: channels 4 5 6 7
         * Group 2: channels 2 3
         */
        // TODO : See if we can seperate frequency between groups
        hal.rcout->set_freq((1UL << 0), static_cast<uint16_t>(_pwm_freq * 1000));  // Steering group
        hal.rcout->set_freq((1UL << 2), static_cast<uint16_t>(_pwm_freq * 1000));  // Throttle group
        break;
    default:
        // do nothing
        break;
    }
}

// output to regular steering and throttle channels
void AP_MotorsUGV::output_regular(bool armed, float steering, float throttle)
{
    // output to throttle channels
    if (armed) {
        // vectored thrust handling
        if (have_vectored_thrust() && (fabsf(throttle) > _vector_throttle_base)) {
            // scale steering down linearly as throttle increases above _vector_throttle_base
            const float steering_scalar = constrain_float(_vector_throttle_base / fabsf(throttle), 0.0f, 1.0f);
            steering *= steering_scalar;
        }
        output_throttle(SRV_Channel::k_throttle, throttle);
    } else {
        // handle disarmed case
        if (_disarm_disable_pwm) {
            SRV_Channels::set_output_limit(SRV_Channel::k_throttle, SRV_Channel::SRV_CHANNEL_LIMIT_ZERO_PWM);
        } else {
            SRV_Channels::set_output_limit(SRV_Channel::k_throttle, SRV_Channel::SRV_CHANNEL_LIMIT_TRIM);
        }
    }

    // always allow steering to move
    SRV_Channels::set_output_scaled(SRV_Channel::k_steering, steering);
}

// output to skid steering channels
void AP_MotorsUGV::output_skid_steering(bool armed, float steering, float throttle)
{
    if (!have_skid_steering()) {
        return;
    }

    // handle simpler disarmed case
    if (!armed) {
        if (_disarm_disable_pwm) {
            SRV_Channels::set_output_limit(SRV_Channel::k_throttleLeft, SRV_Channel::SRV_CHANNEL_LIMIT_ZERO_PWM);
            SRV_Channels::set_output_limit(SRV_Channel::k_throttleRight, SRV_Channel::SRV_CHANNEL_LIMIT_ZERO_PWM);
        } else {
            SRV_Channels::set_output_limit(SRV_Channel::k_throttleLeft, SRV_Channel::SRV_CHANNEL_LIMIT_TRIM);
            SRV_Channels::set_output_limit(SRV_Channel::k_throttleRight, SRV_Channel::SRV_CHANNEL_LIMIT_TRIM);
        }
        return;
    }

    // skid steering mixer
    float steering_scaled = steering / 4500.0f; // steering scaled -1 to +1
    float throttle_scaled = throttle / 100.0f;  // throttle scaled -1 to +1

    // apply constraints
    steering_scaled = constrain_float(steering_scaled, -1.0f, 1.0f);
    throttle_scaled = constrain_float(throttle_scaled, -1.0f, 1.0f);

    // check for saturation and scale back throttle and steering proportionally
    const float saturation_value = fabsf(steering_scaled) + fabsf(throttle_scaled);
    if (saturation_value > 1.0f) {
        steering_scaled = steering_scaled / saturation_value;
        throttle_scaled = throttle_scaled / saturation_value;
    }

    // reverse steering direction if throttle is negative to mimic regular rovers
    const float steering_dir = is_negative(throttle_scaled) ? -1.0f : 1.0f;

    // add in throttle and steering
    const float motor_left = throttle_scaled + (steering_dir * steering_scaled);
    const float motor_right = throttle_scaled - (steering_dir * steering_scaled);

    // send pwm value to each motor
    output_throttle(SRV_Channel::k_throttleLeft, 100.0f * motor_left);
    output_throttle(SRV_Channel::k_throttleRight, 100.0f * motor_right);
}

// output throttle value to main throttle channel, left throttle or right throttle.  throttle should be scaled from -100 to 100
void AP_MotorsUGV::output_throttle(SRV_Channel::Aux_servo_function_t function, float throttle)
{
    // sanity check servo function
    if (function != SRV_Channel::k_throttle && function != SRV_Channel::k_throttleLeft && function != SRV_Channel::k_throttleRight) {
        return;
    }

    // constrain and scale output
    throttle = get_scaled_throttle(throttle);

    // set relay if necessary
    if (_pwm_type == PWM_TYPE_BRUSHED_WITH_RELAY) {
        // find the output channel, if not found return
        const SRV_Channel *out_chan = SRV_Channels::get_channel_for(function);
        if (out_chan == nullptr) {
            return;
        }
        const int8_t reverse_multiplier = out_chan->get_reversed() ? -1 : 1;
        bool relay_high = is_negative(reverse_multiplier * throttle);

        switch (function) {
            case SRV_Channel::k_throttle:
            case SRV_Channel::k_throttleLeft:
                _relayEvents.do_set_relay(0, relay_high);
                break;
            case SRV_Channel::k_throttleRight:
                _relayEvents.do_set_relay(1, relay_high);
                break;
            default:
                // do nothing
                break;
        }
        // invert the output to always have positive value calculated by calc_pwm
        throttle = reverse_multiplier * fabsf(throttle);
    }

    // output to servo channel
    switch (function) {
        case SRV_Channel::k_throttle:
            SRV_Channels::set_output_scaled(function,  throttle);
            break;
        case SRV_Channel::k_throttleLeft:
        case SRV_Channel::k_throttleRight:
            SRV_Channels::set_output_scaled(function,  throttle * 10.0f);
            break;
        default:
            // do nothing
            break;
    }
}

// slew limit throttle for one iteration
void AP_MotorsUGV::slew_limit_throttle(float dt)
{
    if (_slew_rate > 0) {
        // slew throttle
        const float throttle_change_max = MAX(1.0f, (float)_slew_rate * dt);
        if (_throttle > _throttle_prev + throttle_change_max) {
            _throttle = _throttle_prev + throttle_change_max;
            limit.throttle_upper = true;
        } else if (_throttle < _throttle_prev - throttle_change_max) {
            _throttle = _throttle_prev - throttle_change_max;
            limit.throttle_lower = true;
        }
    }
    _throttle_prev = _throttle;
}

// set limits based on steering and throttle input
void AP_MotorsUGV::set_limits_from_input(bool armed, float steering, float throttle)
{
    // set limits based on inputs
    limit.steer_left = !armed || (steering <= -4500.0f);
    limit.steer_right = !armed || (steering >= 4500.0f);
    limit.throttle_lower = !armed || (throttle <= -_throttle_max);
    limit.throttle_upper = !armed || (throttle >= _throttle_max);
}

// scale a throttle using the _throttle_min and _thrust_curve_expo parameters.  throttle should be in the range -100 to +100
float AP_MotorsUGV::get_scaled_throttle(float throttle) const
{
    // exit immediately if throttle is zero
    if (is_zero(throttle)) {
        return throttle;
    }

    // scale using throttle_min
    if (_throttle_min > 0) {
        if (is_negative(throttle)) {
            throttle = -_throttle_min + (throttle * ((100.0f - _throttle_min) / 100.0f));
        } else {
            throttle = _throttle_min + (throttle * ((100.0f - _throttle_min) / 100.0f));
        }
    }

    // skip further scaling if thrust curve disabled or invalid
    if (is_zero(_thrust_curve_expo) || (_thrust_curve_expo > 1.0f) || (_thrust_curve_expo < -1.0f)) {
        return throttle;
    }

    // calculate scaler
    const float sign = (throttle < 0.0f) ? -1.0f : 1.0f;
    const float throttle_pct = constrain_float(throttle, -100.0f, 100.0f) / 100.0f;
    return 100.0f * sign * ((_thrust_curve_expo - 1.0f) + safe_sqrt((1.0f - _thrust_curve_expo) * (1.0f - _thrust_curve_expo) + 4.0f * _thrust_curve_expo * fabsf(throttle_pct))) / (2.0f * _thrust_curve_expo);
}
  # Path to the CMake build directory.
  build: '${{ #pragma once

#include "defines.h"
#include "AP_Arming.h"
#include <AP_ServoRelayEvents/AP_ServoRelayEvents.h>

class AP_MotorsUGV {
public:

    // Constructor
    AP_MotorsUGV(AP_ServoRelayEvents &relayEvents);

    enum pwm_type {
        PWM_TYPE_NORMAL = 0,
        PWM_TYPE_ONESHOT = 1,
        PWM_TYPE_ONESHOT125 = 2,
        PWM_TYPE_BRUSHED_WITH_RELAY = 3,
        PWM_TYPE_BRUSHED_BIPOLAR = 4,
     };

    enum motor_test_order {
        MOTOR_TEST_THROTTLE = 1,
        MOTOR_TEST_STEERING = 2,
        MOTOR_TEST_THROTTLE_LEFT = 3,
        MOTOR_TEST_THROTTLE_RIGHT = 4,
    };

    // initialise motors
    void init();

    // setup output in case of main CPU failure
    void setup_safety_output();

    // setup servo output ranges
    void setup_servo_output();

    // get or set steering as a value from -4500 to +4500
    float get_steering() const { return _steering; }
    void set_steering(float steering);

    // get or set throttle as a value from -100 to 100
    float get_throttle() const { return _throttle; }
    void set_throttle(float throttle);

    // true if vehicle is capable of skid steering
    bool have_skid_steering() const;

    // true if vehicle has vectored thrust (i.e. boat with motor on steering servo)
    bool have_vectored_thrust() const { return is_positive(_vector_throttle_base); }

    // output to motors and steering servos
    void output(bool armed, float dt);

    // test steering or throttle output as a percentage of the total (range -100 to +100)
    // used in response to DO_MOTOR_TEST mavlink command
    bool output_test_pct(motor_test_order motor_seq, float pct);

    // test steering or throttle output using a pwm value
    bool output_test_pwm(motor_test_order motor_seq, float pwm);

    //  returns true if checks pass, false if they fail.  display_failure argument should be true to send text messages to GCS
    bool pre_arm_check(bool report) const;

    // structure for holding motor limit flags
    struct AP_MotorsUGV_limit {
        uint8_t steer_left      : 1; // we have reached the steering controller's left most limit
        uint8_t steer_right     : 1; // we have reached the steering controller's right most limit
        uint8_t throttle_lower  : 1; // we have reached throttle's lower limit
        uint8_t throttle_upper  : 1; // we have reached throttle's upper limit
    } limit;

    // var_info for holding Parameter information
    static const struct AP_Param::GroupInfo var_info[];

protected:

    // setup pwm output type
    void setup_pwm_type();

    // output to regular steering and throttle channels
    void output_regular(bool armed, float steering, float throttle);

    // output to skid steering channels
    void output_skid_steering(bool armed, float steering, float throttle);

    // output throttle (-100 ~ +100) to a throttle channel.  Sets relays if required
    void output_throttle(SRV_Channel::Aux_servo_function_t function, float throttle);

    // slew limit throttle for one iteration
    void slew_limit_throttle(float dt);

    // set limits based on steering and throttle input
    void set_limits_from_input(bool armed, float steering, float throttle);

    // scale a throttle using the _thrust_curve_expo parameter.  throttle should be in the range -100 to +100
    float get_scaled_throttle(float throttle) const;

    // external references
    AP_ServoRelayEvents &_relayEvents;

    // parameters
    AP_Int8 _pwm_type;  // PWM output type
    AP_Int8 _pwm_freq;  // PWM output freq for brushed motors
    AP_Int8 _disarm_disable_pwm;    // disable PWM output while disarmed
    AP_Int16 _slew_rate; // slew rate expressed as a percentage / second
    AP_Int8 _throttle_min; // throttle minimum percentage
    AP_Int8 _throttle_max; // throttle maximum percentage
    AP_Float _thrust_curve_expo; // thrust curve exponent from -1 to +1 with 0 being linear
    AP_Float _vector_throttle_base;  // throttle level above which steering is scaled down when using vector thrust.  zero to disable vectored thrust

    // internal variables
    float   _steering;  // requested steering as a value from -4500 to +4500
    float   _throttle;  // requested throttle as a value from -100 to 100
    float   _throttle_prev; // throttle input from previous iteration
}; }}/build'

permissions: #include "Rover.h"

#include "GCS_Mavlink.h"

#include <AP_RangeFinder/RangeFinder_Backend.h>

MAV_TYPE GCS_MAVLINK_Rover::frame_type() const
{
    if (rover.is_boat()) {
        return MAV_TYPE_SURFACE_BOAT;
    }
    return MAV_TYPE_GROUND_ROVER;
}

MAV_MODE GCS_MAVLINK_Rover::base_mode() const
{
    uint8_t _base_mode = MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;

    // work out the base_mode. This value is not very useful
    // for APM, but we calculate it as best we can so a generic
    // MAVLink enabled ground station can work out something about
    // what the MAV is up to. The actual bit values are highly
    // ambiguous for most of the APM flight modes. In practice, you
    // only get useful information from the custom_mode, which maps to
    // the APM flight mode and has a well defined meaning in the
    // ArduPlane documentation
    if (rover.control_mode->has_manual_input()) {
        _base_mode |= MAV_MODE_FLAG_MANUAL_INPUT_ENABLED;
    }

    if (rover.control_mode->is_autopilot_mode()) {
        _base_mode |= MAV_MODE_FLAG_GUIDED_ENABLED;
    }

#if defined(ENABLE_STICK_MIXING) && (ENABLE_STICK_MIXING == ENABLED) // TODO ???? Remove !
    if (control_mode->stick_mixing_enabled()) {
        // all modes except INITIALISING have some form of manual
        // override if stick mixing is enabled
        _base_mode |= MAV_MODE_FLAG_MANUAL_INPUT_ENABLED;
    }
#endif

#if HIL_MODE != HIL_MODE_DISABLED
    _base_mode |= MAV_MODE_FLAG_HIL_ENABLED;
#endif

    // we are armed if we are not initialising
    if (rover.control_mode != &rover.mode_initializing && rover.arming.is_armed()) {
        _base_mode |= MAV_MODE_FLAG_SAFETY_ARMED;
    }

    // indicate we have set a custom mode
    _base_mode |= MAV_MODE_FLAG_CUSTOM_MODE_ENABLED;

return (MAV_MODE)_base_mode;
}

uint32_t GCS_MAVLINK_Rover::custom_mode() const
{
    return rover.control_mode->mode_number();
}

MAV_STATE GCS_MAVLINK_Rover::system_status() const
{
    if (rover.failsafe.triggered != 0) {
        return MAV_STATE_CRITICAL;
    }
    if (rover.control_mode == &rover.mode_initializing) {
        return MAV_STATE_CALIBRATING;
    }
    if (rover.control_mode == &rover.mode_hold) {
        return MAV_STATE_STANDBY;
    }

    return MAV_STATE_ACTIVE;
}

void Rover::send_attitude(mavlink_channel_t chan)
{
    const Vector3f omega = ahrs.get_gyro();
    mavlink_msg_attitude_send(
        chan,
        millis(),
        ahrs.roll,
        ahrs.pitch,
        ahrs.yaw,
        omega.x,
        omega.y,
        omega.z);
}

void Rover::send_extended_status1(mavlink_channel_t chan)
{
    int16_t battery_current = -1;
    int8_t battery_remaining = -1;

    if (battery.has_current() && battery.healthy()) {
        battery_remaining = battery.capacity_remaining_pct();
        battery_current = battery.current_amps() * 100;
    }

    update_sensor_status_flags();

    mavlink_msg_sys_status_send(
        chan,
        control_sensors_present,
        control_sensors_enabled,
        control_sensors_health,
        static_cast<uint16_t>(scheduler.load_average() * 1000),
        battery.voltage() * 1000,  // mV
        battery_current,        // in 10mA units
        battery_remaining,      // in %
        0,  // comm drops %,
        0,  // comm drops in pkts,
        0, 0, 0, 0);
}

void Rover::send_location(mavlink_channel_t chan)
{
    const uint32_t now = AP_HAL::millis();
    Vector3f vel;
    ahrs.get_velocity_NED(vel);
    mavlink_msg_global_position_int_send(
        chan,
        now,
        current_loc.lat,                    // in 1E7 degrees
        current_loc.lng,                    // in 1E7 degrees
        current_loc.alt * 10UL,             // millimeters above sea level
        (current_loc.alt - home.alt) * 10,  // millimeters above home
        vel.x * 100,   // X speed cm/s (+ve North)
        vel.y * 100,   // Y speed cm/s (+ve East)
        vel.z * 100,   // Z speed cm/s (+ve Down)
        ahrs.yaw_sensor);
}

void Rover::send_nav_controller_output(mavlink_channel_t chan)
{
    mavlink_msg_nav_controller_output_send(
        chan,
        g2.attitude_control.get_desired_lat_accel(),
        ahrs.groundspeed() * ins.get_gyro().z,  // use nav_pitch to hold actual Y accel
        nav_controller->nav_bearing_cd() * 0.01f,
        nav_controller->target_bearing_cd() * 0.01f,
        MIN(control_mode->get_distance_to_destination(), UINT16_MAX),
        0,
        control_mode->speed_error(),
        nav_controller->crosstrack_error());
}

void Rover::send_servo_out(mavlink_channel_t chan)
{
    float motor1, motor3;
    if (g2.motors.have_skid_steering()) {
        motor1 = 10000 * (SRV_Channels::get_output_scaled(SRV_Channel::k_throttleLeft) / 1000.0f);
        motor3 = 10000 * (SRV_Channels::get_output_scaled(SRV_Channel::k_throttleRight) / 1000.0f);
    } else {
        motor1 = 10000 * (SRV_Channels::get_output_scaled(SRV_Channel::k_steering) / 4500.0f);
        motor3 = 10000 * (SRV_Channels::get_output_scaled(SRV_Channel::k_throttle) / 100.0f);
    }
    mavlink_msg_rc_channels_scaled_send(
        chan,
        millis(),
        0,  // port 0
        motor1,
        0,
        motor3,
        0,
        0,
        0,
        0,
        0,
        receiver_rssi);
}

void Rover::send_vfr_hud(mavlink_channel_t chan)
{
    mavlink_msg_vfr_hud_send(
        chan,
        gps.ground_speed(),
        ahrs.groundspeed(),
        (ahrs.yaw_sensor / 100) % 360,
        g2.motors.get_throttle(),
        current_loc.alt / 100.0f,
        0);
}

// report simulator state
void Rover::send_simstate(mavlink_channel_t chan)
{
#if CONFIG_HAL_BOARD == HAL_BOARD_SITL
    sitl.simstate_send(chan);
#endif
}

void Rover::send_rangefinder(mavlink_channel_t chan)
{
    float distance_cm;
    float voltage;
    bool got_one = false;

    // report smaller distance of all rangefinders
    for (uint8_t i=0; i<rangefinder.num_sensors(); i++) {
        AP_RangeFinder_Backend *s = rangefinder.get_backend(i);
        if (s == nullptr) {
            continue;
        }
        if (!got_one ||
            s->distance_cm() < distance_cm) {
            distance_cm = s->distance_cm();
            voltage = s->voltage_mv();
            got_one = true;
        }
    }
    if (!got_one) {
        // no relevant data found
        return;
    }

    mavlink_msg_rangefinder_send(
        chan,
        distance_cm * 0.01f,
        voltage);
}

/*
  send PID tuning message
 */
void Rover::send_pid_tuning(mavlink_channel_t chan)
{
    const DataFlash_Class::PID_Info *pid_info;
    // steering PID
    if (g.gcs_pid_mask & 1) {
        pid_info = &g2.attitude_control.get_steering_rate_pid().get_pid_info();
        mavlink_msg_pid_tuning_send(chan, PID_TUNING_STEER,
                                    degrees(pid_info->desired),
                                    degrees(ahrs.get_yaw_rate_earth()),
                                    pid_info->FF,
                                    pid_info->P,
                                    pid_info->I,
                                    pid_info->D);
        if (!HAVE_PAYLOAD_SPACE(chan, PID_TUNING)) {
            return;
        }
    }
    // speed to throttle PID
    if (g.gcs_pid_mask & 2) {
        pid_info = &g2.attitude_control.get_throttle_speed_pid().get_pid_info();
        float speed = 0.0f;
        g2.attitude_control.get_forward_speed(speed);
        mavlink_msg_pid_tuning_send(chan, PID_TUNING_ACCZ,
                                    pid_info->desired,
                                    speed,
                                    0,
                                    pid_info->P,
                                    pid_info->I,
                                    pid_info->D);
        if (!HAVE_PAYLOAD_SPACE(chan, PID_TUNING)) {
            return;
        }
    }
}

void Rover::send_fence_status(mavlink_channel_t chan)
{
    fence_send_mavlink_status(chan);
}

void Rover::send_wheel_encoder(mavlink_channel_t chan)
{
    // send wheel encoder data using rpm message
    if (g2.wheel_encoder.enabled(0) || g2.wheel_encoder.enabled(1)) {
        mavlink_msg_rpm_send(chan, wheel_encoder_rpm[0], wheel_encoder_rpm[1]);
    }
}

uint8_t GCS_MAVLINK_Rover::sysid_my_gcs() const
{
    return rover.g.sysid_my_gcs;
}

uint32_t GCS_MAVLINK_Rover::telem_delay() const
{
    return static_cast<uint32_t>(rover.g.telem_delay);
}

// try to send a message, return false if it won't fit in the serial tx buffer
bool GCS_MAVLINK_Rover::try_send_message(enum ap_message id)
{
    if (telemetry_delayed()) {
        return false;
    }

    // if we don't have at least 1ms remaining before the main loop
    // wants to fire then don't send a mavlink message. We want to
    // prioritise the main flight control loop over communications
    if (!rover.in_mavlink_delay && rover.scheduler.time_available_usec() < 1200) {
        gcs().set_out_of_time(true);
        return false;
    }

    switch (id) {
    case MSG_HEARTBEAT:
        CHECK_PAYLOAD_SIZE(HEARTBEAT);
        last_heartbeat_time = AP_HAL::millis();
        send_heartbeat();
        return true;

    case MSG_EXTENDED_STATUS1:
        // send extended status only once vehicle has been initialised
        // to avoid unnecessary errors being reported to user
        if (initialised) {
            CHECK_PAYLOAD_SIZE(SYS_STATUS);
            rover.send_extended_status1(chan);
            CHECK_PAYLOAD_SIZE(POWER_STATUS);
            send_power_status();
        }
        break;

    case MSG_ATTITUDE:
        CHECK_PAYLOAD_SIZE(ATTITUDE);
        rover.send_attitude(chan);
        break;

    case MSG_LOCATION:
        CHECK_PAYLOAD_SIZE(GLOBAL_POSITION_INT);
        rover.send_location(chan);
        break;

    case MSG_NAV_CONTROLLER_OUTPUT:
        if (rover.control_mode->is_autopilot_mode()) {
            CHECK_PAYLOAD_SIZE(NAV_CONTROLLER_OUTPUT);
            rover.send_nav_controller_output(chan);
        }
        break;

    case MSG_SERVO_OUT:
        CHECK_PAYLOAD_SIZE(RC_CHANNELS_SCALED);
        rover.send_servo_out(chan);
        break;

    case MSG_RADIO_IN:
        CHECK_PAYLOAD_SIZE(RC_CHANNELS);
        send_radio_in(rover.receiver_rssi);
        break;

    case MSG_SERVO_OUTPUT_RAW:
        CHECK_PAYLOAD_SIZE(SERVO_OUTPUT_RAW);
        send_servo_output_raw(false);
        break;

    case MSG_VFR_HUD:
        CHECK_PAYLOAD_SIZE(VFR_HUD);
        rover.send_vfr_hud(chan);
        break;

    case MSG_RAW_IMU1:
        CHECK_PAYLOAD_SIZE(RAW_IMU);
        send_raw_imu(rover.ins, rover.compass);
        break;

    case MSG_RAW_IMU2:
        CHECK_PAYLOAD_SIZE(SCALED_PRESSURE);
        send_scaled_pressure();
        break;

    case MSG_RAW_IMU3:
        CHECK_PAYLOAD_SIZE(SENSOR_OFFSETS);
        send_sensor_offsets(rover.ins, rover.compass);
        break;

    case MSG_SIMSTATE:
        CHECK_PAYLOAD_SIZE(SIMSTATE);
        rover.send_simstate(chan);
        break;

    case MSG_RANGEFINDER:
        CHECK_PAYLOAD_SIZE(RANGEFINDER);
        rover.send_rangefinder(chan);
        send_distance_sensor(rover.rangefinder);
        send_proximity(rover.g2.proximity);
        break;

    case MSG_RPM:
        CHECK_PAYLOAD_SIZE(RPM);
        rover.send_wheel_encoder(chan);
        break;

    case MSG_MOUNT_STATUS:
#if MOUNT == ENABLED
        CHECK_PAYLOAD_SIZE(MOUNT_STATUS);
        rover.camera_mount.status_msg(chan);
#endif  // MOUNT == ENABLED
        break;

    case MSG_FENCE_STATUS:
        CHECK_PAYLOAD_SIZE(FENCE_STATUS);
        rover.send_fence_status(chan);
        break;

    case MSG_VIBRATION:
        CHECK_PAYLOAD_SIZE(VIBRATION);
        send_vibration(rover.ins);
        break;

    case MSG_BATTERY2:
        CHECK_PAYLOAD_SIZE(BATTERY2);
        send_battery2(rover.battery);
        break;

    case MSG_EKF_STATUS_REPORT:
#if AP_AHRS_NAVEKF_AVAILABLE
        CHECK_PAYLOAD_SIZE(EKF_STATUS_REPORT);
        rover.ahrs.send_ekf_status_report(chan);
#endif
        break;

    case MSG_PID_TUNING:
        CHECK_PAYLOAD_SIZE(PID_TUNING);
        rover.send_pid_tuning(chan);
        break;

    case MSG_BATTERY_STATUS:
        send_battery_status(rover.battery);
        break;

    default:
        return GCS_MAVLINK::try_send_message(id);
    }
    return true;
}

/*
  default stream rates to 1Hz
 */
const AP_Param::GroupInfo GCS_MAVLINK::var_info[] = {
    // @Param: RAW_SENS
    // @DisplayName: Raw sensor stream rate
    // @Description: Raw sensor stream rate to ground station
    // @Units: Hz
    // @Range: 0 10
    // @Increment: 1
    // @User: Advanced
    AP_GROUPINFO("RAW_SENS", 0, GCS_MAVLINK, streamRates[0],  1),

    // @Param: EXT_STAT
    // @DisplayName: Extended status stream rate to ground station
    // @Description: Extended status stream rate to ground station
    // @Units: Hz
    // @Range: 0 10
    // @Increment: 1
    // @User: Advanced
    AP_GROUPINFO("EXT_STAT", 1, GCS_MAVLINK, streamRates[1],  1),

    // @Param: RC_CHAN
    // @DisplayName: RC Channel stream rate to ground station
    // @Description: RC Channel stream rate to ground station
    // @Units: Hz
    // @Range: 0 10
    // @Increment: 1
    // @User: Advanced
    AP_GROUPINFO("RC_CHAN",  2, GCS_MAVLINK, streamRates[2],  1),

    // @Param: RAW_CTRL
    // @DisplayName: Raw Control stream rate to ground station
    // @Description: Raw Control stream rate to ground station
    // @Units: Hz
    // @Range: 0 10
    // @Increment: 1
    // @User: Advanced
    AP_GROUPINFO("RAW_CTRL", 3, GCS_MAVLINK, streamRates[3],  1),

    // @Param: POSITION
    // @DisplayName: Position stream rate to ground station
    // @Description: Position stream rate to ground station
    // @Units: Hz
    // @Range: 0 10
    // @Increment: 1
    // @User: Advanced
    AP_GROUPINFO("POSITION", 4, GCS_MAVLINK, streamRates[4],  1),

    // @Param: EXTRA1
    // @DisplayName: Extra data type 1 stream rate to ground station
    // @Description: Extra data type 1 stream rate to ground station
    // @Units: Hz
    // @Range: 0 10
    // @Increment: 1
    // @User: Advanced
    AP_GROUPINFO("EXTRA1",   5, GCS_MAVLINK, streamRates[5],  1),

    // @Param: EXTRA2
    // @DisplayName: Extra data type 2 stream rate to ground station
    // @Description: Extra data type 2 stream rate to ground station
    // @Units: Hz
    // @Range: 0 10
    // @Increment: 1
    // @User: Advanced
    AP_GROUPINFO("EXTRA2",   6, GCS_MAVLINK, streamRates[6],  1),

    // @Param: EXTRA3
    // @DisplayName: Extra data type 3 stream rate to ground station
    // @Description: Extra data type 3 stream rate to ground station
    // @Units: Hz
    // @Range: 0 10
    // @Increment: 1
    // @User: Advanced
    AP_GROUPINFO("EXTRA3",   7, GCS_MAVLINK, streamRates[7],  1),

    // @Param: PARAMS
    // @DisplayName: Parameter stream rate to ground station
    // @Description: Parameter stream rate to ground station
    // @Units: Hz
    // @Range: 0 10
    // @Increment: 1
    // @User: Advanced
    AP_GROUPINFO("PARAMS",   8, GCS_MAVLINK, streamRates[8],  10),
    AP_GROUPEND
};

void
GCS_MAVLINK_Rover::data_stream_send(void)
{
    gcs().set_out_of_time(false);

    send_queued_parameters();

    if (gcs().out_of_time()) {
      return;
    }

    if (rover.in_mavlink_delay) {
#if HIL_MODE != HIL_MODE_DISABLED
        // in HIL we need to keep sending servo values to ensure
        // the simulator doesn't pause, otherwise our sensor
        // calibration could stall
        if (stream_trigger(STREAM_RAW_CONTROLLER)) {
            send_message(MSG_SERVO_OUT);
        }
        if (stream_trigger(STREAM_RC_CHANNELS)) {
            send_message(MSG_SERVO_OUTPUT_RAW);
        }
#endif
        // don't send any other stream types while in the delay callback
        return;
    }

    if (gcs().out_of_time()) {
      return;
    }

    if (stream_trigger(STREAM_RAW_SENSORS)) {
        send_message(MSG_RAW_IMU1);
        send_message(MSG_RAW_IMU2);
        send_message(MSG_RAW_IMU3);
    }

    if (gcs().out_of_time()) {
      return;
    }

    if (stream_trigger(STREAM_EXTENDED_STATUS)) {
        send_message(MSG_EXTENDED_STATUS1);
        send_message(MSG_EXTENDED_STATUS2);
        send_message(MSG_CURRENT_WAYPOINT);
        send_message(MSG_GPS_RAW);
        send_message(MSG_GPS_RTK);
        send_message(MSG_GPS2_RAW);
        send_message(MSG_GPS2_RTK);
        send_message(MSG_NAV_CONTROLLER_OUTPUT);
        send_message(MSG_FENCE_STATUS);
    }

    if (gcs().out_of_time()) {
      return;
    }

    if (stream_trigger(STREAM_POSITION)) {
        // sent with GPS read
        send_message(MSG_LOCATION);
        send_message(MSG_LOCAL_POSITION);
    }

    if (gcs().out_of_time()) {
      return;
    }

    if (stream_trigger(STREAM_RAW_CONTROLLER)) {
        send_message(MSG_SERVO_OUT);
    }

    if (gcs().out_of_time()) {
      return;
    }

    if (stream_trigger(STREAM_RC_CHANNELS)) {
        send_message(MSG_SERVO_OUTPUT_RAW);
        send_message(MSG_RADIO_IN);
    }

    if (gcs().out_of_time()) {
      return;
    }

    if (stream_trigger(STREAM_EXTRA1)) {
        send_message(MSG_ATTITUDE);
        send_message(MSG_SIMSTATE);
        send_message(MSG_PID_TUNING);
    }

    if (gcs().out_of_time()) {
      return;
    }

    if (stream_trigger(STREAM_EXTRA2)) {
        send_message(MSG_VFR_HUD);
    }

    if (gcs().out_of_time()) {
      return;
    }

    if (stream_trigger(STREAM_EXTRA3)) {
        send_message(MSG_AHRS);
        send_message(MSG_HWSTATUS);
        send_message(MSG_RANGEFINDER);
        send_message(MSG_SYSTEM_TIME);
        send_message(MSG_BATTERY2);
        send_message(MSG_BATTERY_STATUS);
        send_message(MSG_MAG_CAL_REPORT);
        send_message(MSG_MAG_CAL_PROGRESS);
        send_message(MSG_MOUNT_STATUS);
        send_message(MSG_EKF_STATUS_REPORT);
        send_message(MSG_VIBRATION);
        send_message(MSG_RPM);
    }
}



bool GCS_MAVLINK_Rover::handle_guided_request(AP_Mission::Mission_Command &cmd)
{
    if (rover.control_mode != &rover.mode_guided) {
        // only accept position updates when in GUIDED mode
        return false;
    }

    // make any new wp uploaded instant (in case we are already in Guided mode)
    rover.mode_guided.set_desired_location(cmd.content.location);
    return true;
}

void GCS_MAVLINK_Rover::handle_change_alt_request(AP_Mission::Mission_Command &cmd)
{
    // nothing to do
}

MAV_RESULT GCS_MAVLINK_Rover::_handle_command_preflight_calibration(const mavlink_command_long_t &packet)
{
    if (is_equal(packet.param4, 1.0f)) {
        rover.trim_radio();
        return MAV_RESULT_ACCEPTED;
    }

    return GCS_MAVLINK::_handle_command_preflight_calibration(packet);
}

void GCS_MAVLINK_Rover::handleMessage(mavlink_message_t* msg)
{
    switch (msg->msgid) {

    case MAVLINK_MSG_ID_REQUEST_DATA_STREAM:
        {
            handle_request_data_stream(msg, true);
            break;
        }

    case MAVLINK_MSG_ID_COMMAND_INT: {
        // decode packet
        mavlink_command_int_t packet;
        mavlink_msg_command_int_decode(msg, &packet);
        MAV_RESULT result = MAV_RESULT_UNSUPPORTED;

        switch (packet.command) {

        case MAV_CMD_DO_SET_HOME: {
            // assume failure
            result = MAV_RESULT_FAILED;
            if (is_equal(packet.param1, 1.0f)) {
                // if param1 is 1, use current location
                if (rover.set_home_to_current_location(true)) {
                    result = MAV_RESULT_ACCEPTED;
                }
                break;
            }
            // ensure param1 is zero
            if (!is_zero(packet.param1)) {
                break;
            }
            // check frame type is supported
            if (packet.frame != MAV_FRAME_GLOBAL &&
                packet.frame != MAV_FRAME_GLOBAL_INT &&
                packet.frame != MAV_FRAME_GLOBAL_RELATIVE_ALT &&
                packet.frame != MAV_FRAME_GLOBAL_RELATIVE_ALT_INT) {
                break;
            }
            // sanity check location
            if (!check_latlng(packet.x, packet.y)) {
                break;
            }
            Location new_home_loc {};
            new_home_loc.lat = packet.x;
            new_home_loc.lng = packet.y;
            new_home_loc.alt = packet.z * 100;
            // handle relative altitude
            if (packet.frame == MAV_FRAME_GLOBAL_RELATIVE_ALT || packet.frame == MAV_FRAME_GLOBAL_RELATIVE_ALT_INT) {
                if (!rover.ahrs.home_is_set()) {
                    // cannot use relative altitude if home is not set
                    break;
                }
                new_home_loc.alt += rover.ahrs.get_home().alt;
            }
            if (rover.set_home(new_home_loc, true)) {
                result = MAV_RESULT_ACCEPTED;
            }
            break;
        }

#if MOUNT == ENABLED
        case MAV_CMD_DO_SET_ROI: {
            // param1 : /* Region of interest mode (not used)*/
            // param2 : /* MISSION index/ target ID (not used)*/
            // param3 : /* ROI index (not used)*/
            // param4 : /* empty */
            // x : lat
            // y : lon
            // z : alt
            // sanity check location
            if (!check_latlng(packet.x, packet.y)) {
                break;
            }
            Location roi_loc;
            roi_loc.lat = packet.x;
            roi_loc.lng = packet.y;
            roi_loc.alt = (int32_t)(packet.z * 100.0f);
            if (roi_loc.lat == 0 && roi_loc.lng == 0 && roi_loc.alt == 0) {
                // switch off the camera tracking if enabled
                if (rover.camera_mount.get_mode() == MAV_MOUNT_MODE_GPS_POINT) {
                    rover.camera_mount.set_mode_to_default();
                }
            } else {
                // send the command to the camera mount
                rover.camera_mount.set_roi_target(roi_loc);
            }
            result = MAV_RESULT_ACCEPTED;
            break;
        }
#endif

        default:
            result = MAV_RESULT_UNSUPPORTED;
            break;
        }

        // send ACK or NAK
        mavlink_msg_command_ack_send_buf(msg, chan, packet.command, result);
        break;
    }

    case MAVLINK_MSG_ID_COMMAND_LONG:
        {
            // decode
            mavlink_command_long_t packet;
            mavlink_msg_command_long_decode(msg, &packet);

            MAV_RESULT result = MAV_RESULT_UNSUPPORTED;

            // do command

            switch (packet.command) {

            case MAV_CMD_NAV_RETURN_TO_LAUNCH:
                rover.set_mode(rover.mode_rtl, MODE_REASON_GCS_COMMAND);
                result = MAV_RESULT_ACCEPTED;
                break;

#if MOUNT == ENABLED
            // Sets the region of interest (ROI) for the camera
            case MAV_CMD_DO_SET_ROI:
                // sanity check location
                if (!check_latlng(packet.param5, packet.param6)) {
                    break;
                }
                Location roi_loc;
                roi_loc.lat = (int32_t)(packet.param5 * 1.0e7f);
                roi_loc.lng = (int32_t)(packet.param6 * 1.0e7f);
                roi_loc.alt = (int32_t)(packet.param7 * 100.0f);
                if (roi_loc.lat == 0 && roi_loc.lng == 0 && roi_loc.alt == 0) {
                    // switch off the camera tracking if enabled
                    if (rover.camera_mount.get_mode() == MAV_MOUNT_MODE_GPS_POINT) {
                        rover.camera_mount.set_mode_to_default();
                    }
                } else {
                    // send the command to the camera mount
                    rover.camera_mount.set_roi_target(roi_loc);
                }
                result = MAV_RESULT_ACCEPTED;
                break;
#endif

            case MAV_CMD_DO_MOUNT_CONTROL:
#if MOUNT == ENABLED
                rover.camera_mount.control(packet.param1, packet.param2, packet.param3, (MAV_MOUNT_MODE) packet.param7);
                result = MAV_RESULT_ACCEPTED;
#endif
                break;

            case MAV_CMD_MISSION_START:
                rover.set_mode(rover.mode_auto, MODE_REASON_GCS_COMMAND);
                result = MAV_RESULT_ACCEPTED;
                break;

        case MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN:
            if (is_equal(packet.param1, 1.0f) || is_equal(packet.param1, 3.0f)) {
                // when packet.param1 == 3 we reboot to hold in bootloader
                hal.scheduler->reboot(is_equal(packet.param1, 3.0f));
                result = MAV_RESULT_ACCEPTED;
            }
            break;

        case MAV_CMD_COMPONENT_ARM_DISARM:
            if (is_equal(packet.param1, 1.0f)) {
                // run pre_arm_checks and arm_checks and display failures
                if (rover.arm_motors(AP_Arming::MAVLINK)) {
                    result = MAV_RESULT_ACCEPTED;
                } else {
                    result = MAV_RESULT_FAILED;
                }
            } else if (is_zero(packet.param1))  {
                if (rover.disarm_motors()) {
                    result = MAV_RESULT_ACCEPTED;
                } else {
                    result = MAV_RESULT_FAILED;
                }
            } else {
                result = MAV_RESULT_UNSUPPORTED;
            }
            break;

        case MAV_CMD_DO_FENCE_ENABLE:
            result = MAV_RESULT_ACCEPTED;
            switch ((uint16_t)packet.param1) {
                case 0:
                    rover.g2.fence.enable(false);
                    break;
                case 1:
                    rover.g2.fence.enable(true);
                    break;
                default:
                    result = MAV_RESULT_FAILED;
                    break;
            }
            break;

        case MAV_CMD_DO_SET_HOME:
        {
            // param1 : use current (1=use current location, 0=use specified location)
            // param5 : latitude
            // param6 : longitude
            // param7 : altitude
            result = MAV_RESULT_FAILED;  // assume failure
            if (is_equal(packet.param1, 1.0f)) {
                if (rover.set_home_to_current_location(true)) {
                    result = MAV_RESULT_ACCEPTED;
                }
            } else {
                // ensure param1 is zero
                if (!is_zero(packet.param1)) {
                    break;
                }
                Location new_home_loc {};
                new_home_loc.lat = static_cast<int32_t>(packet.param5 * 1.0e7f);
                new_home_loc.lng = static_cast<int32_t>(packet.param6 * 1.0e7f);
                new_home_loc.alt = static_cast<int32_t>(packet.param7 * 100.0f);
                if (rover.set_home(new_home_loc, true)) {
                    result = MAV_RESULT_ACCEPTED;
                }
            }
            break;
        }

        case MAV_CMD_NAV_SET_YAW_SPEED:
        {
            // param1 : yaw angle to adjust direction by in centidegress
            // param2 : Speed - normalized to 0 .. 1

            // exit if vehicle is not in Guided mode
            if (rover.control_mode != &rover.mode_guided) {
                break;
            }

            // send yaw change and target speed to guided mode controller
            const float speed_max = rover.control_mode->get_speed_default();
            const float target_speed = constrain_float(packet.param2 * speed_max, -speed_max, speed_max);
            rover.mode_guided.set_desired_heading_delta_and_speed(packet.param1, target_speed);
            result = MAV_RESULT_ACCEPTED;
            break;
        }

        case MAV_CMD_ACCELCAL_VEHICLE_POS:
            result = MAV_RESULT_FAILED;

            if (rover.ins.get_acal()->gcs_vehicle_position(packet.param1)) {
                result = MAV_RESULT_ACCEPTED;
            }
            break;

        case MAV_CMD_DO_MOTOR_TEST:
            // param1 : motor sequence number (a number from 1 to max number of motors on the vehicle)
            // param2 : throttle type (0=throttle percentage, 1=PWM, 2=pilot throttle channel pass-through. See MOTOR_TEST_THROTTLE_TYPE enum)
            // param3 : throttle (range depends upon param2)
            // param4 : timeout (in seconds)
            result = rover.mavlink_motor_test_start(chan, static_cast<uint8_t>(packet.param1),
                                                    static_cast<uint8_t>(packet.param2),
                                                    static_cast<int16_t>(packet.param3),
                                                    packet.param4);
            break;

        default:
            result = handle_command_long_message(packet);
                break;
            }

            mavlink_msg_command_ack_send_buf(
                msg,
                chan,
                packet.command,
                result);

            break;
        }

    case MAVLINK_MSG_ID_RC_CHANNELS_OVERRIDE:
    {
        // allow override of RC channel values for HIL
        // or for complete GCS control of switch position
        // and RC PWM values.
        if (msg->sysid != rover.g.sysid_my_gcs) {  // Only accept control from our gcs
            break;
        }

        mavlink_rc_channels_override_t packet;
        mavlink_msg_rc_channels_override_decode(msg, &packet);

        RC_Channels::set_override(0, packet.chan1_raw);
        RC_Channels::set_override(1, packet.chan2_raw);
        RC_Channels::set_override(2, packet.chan3_raw);
        RC_Channels::set_override(3, packet.chan4_raw);
        RC_Channels::set_override(4, packet.chan5_raw);
        RC_Channels::set_override(5, packet.chan6_raw);
        RC_Channels::set_override(6, packet.chan7_raw);
        RC_Channels::set_override(7, packet.chan8_raw);

        rover.failsafe.rc_override_timer = AP_HAL::millis();
        rover.failsafe_trigger(FAILSAFE_EVENT_RC, false);
        break;
    }

    case MAVLINK_MSG_ID_MANUAL_CONTROL:
    {
        if (msg->sysid != rover.g.sysid_my_gcs) {  // Only accept control from our gcs
            break;
        }

        mavlink_manual_control_t packet;
        mavlink_msg_manual_control_decode(msg, &packet);

        if (packet.target != rover.g.sysid_this_mav) {
            break; // only accept control aimed at us
        }

        const int16_t roll = (packet.y == INT16_MAX) ? 0 : rover.channel_steer->get_radio_min() + (rover.channel_steer->get_radio_max() - rover.channel_steer->get_radio_min()) * (packet.y + 1000) / 2000.0f;
        const int16_t throttle = (packet.z == INT16_MAX) ? 0 : rover.channel_throttle->get_radio_min() + (rover.channel_throttle->get_radio_max() - rover.channel_throttle->get_radio_min()) * (packet.z + 1000) / 2000.0f;
        RC_Channels::set_override(uint8_t(rover.rcmap.roll() - 1), roll);
        RC_Channels::set_override(uint8_t(rover.rcmap.throttle() - 1), throttle);

        rover.failsafe.rc_override_timer = AP_HAL::millis();
        rover.failsafe_trigger(FAILSAFE_EVENT_RC, false);
        break;
    }

    case MAVLINK_MSG_ID_HEARTBEAT:
        {
            // We keep track of the last time we received a heartbeat from our GCS for failsafe purposes
            if (msg->sysid != rover.g.sysid_my_gcs) {
                break;
            }

            rover.last_heartbeat_ms = rover.failsafe.rc_override_timer = AP_HAL::millis();
            rover.failsafe_trigger(FAILSAFE_EVENT_GCS, false);
            break;
        }

    case MAVLINK_MSG_ID_SET_ATTITUDE_TARGET:   // MAV ID: 82
        {
            // decode packet
            mavlink_set_attitude_target_t packet;
            mavlink_msg_set_attitude_target_decode(msg, &packet);

            // exit if vehicle is not in Guided mode
            if (rover.control_mode != &rover.mode_guided) {
                break;
            }

            // ensure type_mask specifies to use thrust
            if ((packet.type_mask & MAVLINK_SET_ATT_TYPE_MASK_THROTTLE_IGNORE) != 0) {
                break;
            }

            // convert thrust to ground speed
            packet.thrust = constrain_float(packet.thrust, -1.0f, 1.0f);
            const float target_speed = rover.control_mode->get_speed_default() * packet.thrust;

            // if the body_yaw_rate field is ignored, convert quaternion to heading
            if ((packet.type_mask & MAVLINK_SET_ATT_TYPE_MASK_YAW_RATE_IGNORE) != 0) {
                // convert quaternion to heading
                float target_heading_cd = degrees(Quaternion(packet.q[0], packet.q[1], packet.q[2], packet.q[3]).get_euler_yaw()) * 100.0f;
                rover.mode_guided.set_desired_heading_and_speed(target_heading_cd, target_speed);
            } else {
                // use body_yaw_rate field
                rover.mode_guided.set_desired_turn_rate_and_speed((RAD_TO_DEG * packet.body_yaw_rate) * 100.0f, target_speed);
            }
            break;
        }

    case MAVLINK_MSG_ID_SET_POSITION_TARGET_LOCAL_NED:     // MAV ID: 84
        {
            // decode packet
            mavlink_set_position_target_local_ned_t packet;
            mavlink_msg_set_position_target_local_ned_decode(msg, &packet);

            // exit if vehicle is not in Guided mode
            if (rover.control_mode != &rover.mode_guided) {
                break;
            }

            // check for supported coordinate frames
            if (packet.coordinate_frame != MAV_FRAME_LOCAL_NED &&
                packet.coordinate_frame != MAV_FRAME_LOCAL_OFFSET_NED &&
                packet.coordinate_frame != MAV_FRAME_BODY_NED &&
                packet.coordinate_frame != MAV_FRAME_BODY_OFFSET_NED) {
                break;
            }

            bool pos_ignore = packet.type_mask & MAVLINK_SET_POS_TYPE_MASK_POS_IGNORE;
            bool vel_ignore = packet.type_mask & MAVLINK_SET_POS_TYPE_MASK_VEL_IGNORE;
            bool acc_ignore = packet.type_mask & MAVLINK_SET_POS_TYPE_MASK_ACC_IGNORE;
            bool yaw_ignore = packet.type_mask & MAVLINK_SET_POS_TYPE_MASK_YAW_IGNORE;
            bool yaw_rate_ignore = packet.type_mask & MAVLINK_SET_POS_TYPE_MASK_YAW_RATE_IGNORE;

            // prepare target position
            Location target_loc = rover.current_loc;
            if (!pos_ignore) {
                switch (packet.coordinate_frame) {
                case MAV_FRAME_BODY_NED:
                case MAV_FRAME_BODY_OFFSET_NED: {
                    // rotate from body-frame to NE frame
                    const float ne_x = packet.x * rover.ahrs.cos_yaw() - packet.y * rover.ahrs.sin_yaw();
                    const float ne_y = packet.x * rover.ahrs.sin_yaw() + packet.y * rover.ahrs.cos_yaw();
                    // add offset to current location
                    location_offset(target_loc, ne_x, ne_y);
                    }
                    break;

                case MAV_FRAME_LOCAL_OFFSET_NED:
                    // add offset to current location
                    location_offset(target_loc, packet.x, packet.y);
                    break;

                default:
                    // MAV_FRAME_LOCAL_NED interpret as an offset from home
                    target_loc = rover.ahrs.get_home();
                    location_offset(target_loc, packet.x, packet.y);
                    break;
                }
            }

            float target_speed = 0.0f;
            float target_yaw_cd = 0.0f;

            // consume velocity and convert to target speed and heading
            if (!vel_ignore) {
                const float speed_max = rover.control_mode->get_speed_default();
                // convert vector length into a speed
                target_speed = constrain_float(safe_sqrt(sq(packet.vx) + sq(packet.vy)), -speed_max, speed_max);
                // convert vector direction to target yaw
                target_yaw_cd = degrees(atan2f(packet.vy, packet.vx)) * 100.0f;

                // rotate target yaw if provided in body-frame
                if (packet.coordinate_frame == MAV_FRAME_BODY_NED || packet.coordinate_frame == MAV_FRAME_BODY_OFFSET_NED) {
                    target_yaw_cd = wrap_180_cd(target_yaw_cd + rover.ahrs.yaw_sensor);
                }
            }

            // consume yaw heading
            if (!yaw_ignore) {
                target_yaw_cd = ToDeg(packet.yaw) * 100.0f;
                // rotate target yaw if provided in body-frame
                if (packet.coordinate_frame == MAV_FRAME_BODY_NED || packet.coordinate_frame == MAV_FRAME_BODY_OFFSET_NED) {
                    target_yaw_cd = wrap_180_cd(target_yaw_cd + rover.ahrs.yaw_sensor);
                }
            }
            // consume yaw rate
            float target_turn_rate_cds = 0.0f;
            if (!yaw_rate_ignore) {
                target_turn_rate_cds = ToDeg(packet.yaw_rate) * 100.0f;
            }

            // handling case when both velocity and either yaw or yaw-rate are provided
            // by default, we consider that the rover will drive forward
            float speed_dir = 1.0f;
            if (!vel_ignore && (!yaw_ignore || !yaw_rate_ignore)) {
                // Note: we are using the x-axis velocity to determine direction even though
                // the frame may have been provided in MAV_FRAME_LOCAL_OFFSET_NED or MAV_FRAME_LOCAL_NED
                if (is_negative(packet.vx)) {
                    speed_dir = -1.0f;
                }
            }

            // set guided mode targets
            if (!pos_ignore && vel_ignore && acc_ignore && yaw_ignore && yaw_rate_ignore) {
                // consume position target
                rover.mode_guided.set_desired_location(target_loc);
            } else if (pos_ignore && !vel_ignore && acc_ignore && yaw_ignore && yaw_rate_ignore) {
                // consume velocity
                rover.mode_guided.set_desired_heading_and_speed(target_yaw_cd, speed_dir * target_speed);
            } else if (pos_ignore && !vel_ignore && acc_ignore && yaw_ignore && !yaw_rate_ignore) {
                // consume velocity and turn rate
                rover.mode_guided.set_desired_turn_rate_and_speed(target_turn_rate_cds, speed_dir * target_speed);
            } else if (pos_ignore && !vel_ignore && acc_ignore && !yaw_ignore && yaw_rate_ignore) {
                // consume velocity
                rover.mode_guided.set_desired_heading_and_speed(target_yaw_cd, speed_dir * target_speed);
            } else if (pos_ignore && vel_ignore && acc_ignore && !yaw_ignore && yaw_rate_ignore) {
                // consume just target heading (probably only skid steering vehicles can do this)
                rover.mode_guided.set_desired_heading_and_speed(target_yaw_cd, 0.0f);
            } else if (pos_ignore && vel_ignore && acc_ignore && yaw_ignore && !yaw_rate_ignore) {
                // consume just turn rate(probably only skid steering vehicles can do this)
                rover.mode_guided.set_desired_turn_rate_and_speed(target_turn_rate_cds, 0.0f);
            }
            break;
        }

    case MAVLINK_MSG_ID_SET_POSITION_TARGET_GLOBAL_INT:    // MAV ID: 86
        {
            // decode packet
            mavlink_set_position_target_global_int_t packet;
            mavlink_msg_set_position_target_global_int_decode(msg, &packet);

            // exit if vehicle is not in Guided mode
            if (rover.control_mode != &rover.mode_guided) {
                break;
            }
            // check for supported coordinate frames
            if (packet.coordinate_frame != MAV_FRAME_GLOBAL &&
                packet.coordinate_frame != MAV_FRAME_GLOBAL_INT &&
                packet.coordinate_frame != MAV_FRAME_GLOBAL_RELATIVE_ALT &&
                packet.coordinate_frame != MAV_FRAME_GLOBAL_RELATIVE_ALT_INT &&
                packet.coordinate_frame != MAV_FRAME_GLOBAL_TERRAIN_ALT &&
                packet.coordinate_frame != MAV_FRAME_GLOBAL_TERRAIN_ALT_INT) {
                break;
            }
            bool pos_ignore = packet.type_mask & MAVLINK_SET_POS_TYPE_MASK_POS_IGNORE;
            bool vel_ignore = packet.type_mask & MAVLINK_SET_POS_TYPE_MASK_VEL_IGNORE;
            bool acc_ignore = packet.type_mask & MAVLINK_SET_POS_TYPE_MASK_ACC_IGNORE;
            bool yaw_ignore = packet.type_mask & MAVLINK_SET_POS_TYPE_MASK_YAW_IGNORE;
            bool yaw_rate_ignore = packet.type_mask & MAVLINK_SET_POS_TYPE_MASK_YAW_RATE_IGNORE;

            // prepare target position
            Location target_loc = rover.current_loc;
            if (!pos_ignore) {
                // sanity check location
                if (!check_latlng(packet.lat_int, packet.lon_int)) {
                    // result = MAV_RESULT_FAILED;
                    break;
                }
                target_loc.lat = packet.lat_int;
                target_loc.lng = packet.lon_int;
            }

            float target_speed = 0.0f;
            float target_yaw_cd = 0.0f;

            // consume velocity and convert to target speed and heading
            if (!vel_ignore) {
                const float speed_max = rover.control_mode->get_speed_default();
                // convert vector length into a speed
                target_speed = constrain_float(safe_sqrt(sq(packet.vx) + sq(packet.vy)), -speed_max, speed_max);
                // convert vector direction to target yaw
                target_yaw_cd = degrees(atan2f(packet.vy, packet.vx)) * 100.0f;

                // rotate target yaw if provided in body-frame
                if (packet.coordinate_frame == MAV_FRAME_BODY_NED || packet.coordinate_frame == MAV_FRAME_BODY_OFFSET_NED) {
                    target_yaw_cd = wrap_180_cd(target_yaw_cd + rover.ahrs.yaw_sensor);
                }
            }

            // consume yaw heading
            if (!yaw_ignore) {
                target_yaw_cd = ToDeg(packet.yaw) * 100.0f;
                // rotate target yaw if provided in body-frame
                if (packet.coordinate_frame == MAV_FRAME_BODY_NED || packet.coordinate_frame == MAV_FRAME_BODY_OFFSET_NED) {
                    target_yaw_cd = wrap_180_cd(target_yaw_cd + rover.ahrs.yaw_sensor);
                }
            }
            // consume yaw rate
            float target_turn_rate_cds = 0.0f;
            if (!yaw_rate_ignore) {
                target_turn_rate_cds = ToDeg(packet.yaw_rate) * 100.0f;
            }

            // handling case when both velocity and either yaw or yaw-rate are provided
            // by default, we consider that the rover will drive forward
            float speed_dir = 1.0f;
            if (!vel_ignore && (!yaw_ignore || !yaw_rate_ignore)) {
                // Note: we are using the x-axis velocity to determine direction even though
                // the frame is provided in MAV_FRAME_GLOBAL_xxx
                if (is_negative(packet.vx)) {
                    speed_dir = -1.0f;
                }
            }

            // set guided mode targets
            if (!pos_ignore && vel_ignore && acc_ignore && yaw_ignore && yaw_rate_ignore) {
                // consume position target
                rover.mode_guided.set_desired_location(target_loc);
            } else if (pos_ignore && !vel_ignore && acc_ignore && yaw_ignore && yaw_rate_ignore) {
                // consume velocity
                rover.mode_guided.set_desired_heading_and_speed(target_yaw_cd, speed_dir * target_speed);
            } else if (pos_ignore && !vel_ignore && acc_ignore && yaw_ignore && !yaw_rate_ignore) {
                // consume velocity and turn rate
                rover.mode_guided.set_desired_turn_rate_and_speed(target_turn_rate_cds, speed_dir * target_speed);
            } else if (pos_ignore && !vel_ignore && acc_ignore && !yaw_ignore && yaw_rate_ignore) {
                // consume velocity
                rover.mode_guided.set_desired_heading_and_speed(target_yaw_cd, speed_dir * target_speed);
            } else if (pos_ignore && vel_ignore && acc_ignore && !yaw_ignore && yaw_rate_ignore) {
                // consume just target heading (probably only skid steering vehicles can do this)
                rover.mode_guided.set_desired_heading_and_speed(target_yaw_cd, 0.0f);
            } else if (pos_ignore && vel_ignore && acc_ignore && yaw_ignore && !yaw_rate_ignore) {
                // consume just turn rate(probably only skid steering vehicles can do this)
                rover.mode_guided.set_desired_turn_rate_and_speed(target_turn_rate_cds, 0.0f);
            }
            break;
        }

#if HIL_MODE != HIL_MODE_DISABLED
    case MAVLINK_MSG_ID_HIL_STATE:
        {
            mavlink_hil_state_t packet;
            mavlink_msg_hil_state_decode(msg, &packet);

            // sanity check location
            if (!check_latlng(packet.lat, packet.lon)) {
                break;
            }

            // set gps hil sensor
            Location loc;
            loc.lat = packet.lat;
            loc.lng = packet.lon;
            loc.alt = packet.alt/10;
            Vector3f vel(packet.vx, packet.vy, packet.vz);
            vel *= 0.01f;

            gps.setHIL(0, AP_GPS::GPS_OK_FIX_3D,
                       packet.time_usec/1000,
                       loc, vel, 10, 0);

            // rad/sec
            Vector3f gyros;
            gyros.x = packet.rollspeed;
            gyros.y = packet.pitchspeed;
            gyros.z = packet.yawspeed;

            // m/s/s
            Vector3f accels;
            accels.x = packet.xacc * (GRAVITY_MSS/1000.0f);
            accels.y = packet.yacc * (GRAVITY_MSS/1000.0f);
            accels.z = packet.zacc * (GRAVITY_MSS/1000.0f);

            ins.set_gyro(0, gyros);

            ins.set_accel(0, accels);
            compass.setHIL(0, packet.roll, packet.pitch, packet.yaw);
            compass.setHIL(1, packet.roll, packet.pitch, packet.yaw);
            break;
        }
#endif  // HIL_MODE

#if MOUNT == ENABLED
    // deprecated. Use MAV_CMD_DO_MOUNT_CONFIGURE
    case MAVLINK_MSG_ID_MOUNT_CONFIGURE:
        {
            rover.camera_mount.configure_msg(msg);
            break;
        }

    // deprecated. Use MAV_CMD_DO_MOUNT_CONTROL
    case MAVLINK_MSG_ID_MOUNT_CONTROL:
        {
            rover.camera_mount.control_msg(msg);
            break;
        }
#endif  // MOUNT == ENABLED

    case MAVLINK_MSG_ID_RADIO:
    case MAVLINK_MSG_ID_RADIO_STATUS:
        {
            handle_radio_status(msg, rover.DataFlash, rover.should_log(MASK_LOG_PM));
            break;
        }

    // send or receive fence points with GCS
    case MAVLINK_MSG_ID_FENCE_POINT:  // MAV ID: 160
    case MAVLINK_MSG_ID_FENCE_FETCH_POINT:
        rover.g2.fence.handle_msg(*this, msg);
        break;

    case MAVLINK_MSG_ID_DISTANCE_SENSOR:
        rover.rangefinder.handle_msg(msg);
        rover.g2.proximity.handle_msg(msg);
        break;

    default:
        handle_common_message(msg);
        break;
    }  // end switch
}  // end handle mavlink

/*
 *  a delay() callback that processes MAVLink packets. We set this as the
 *  callback in long running library initialisation routines to allow
 *  MAVLink to process packets while waiting for the initialisation to
 *  complete
 */
void Rover::mavlink_delay_cb()
{
    static uint32_t last_1hz, last_50hz, last_5s;
    if (!gcs().chan(0).initialised || in_mavlink_delay) {
        return;
    }

    in_mavlink_delay = true;
    // don't allow potentially expensive logging calls:
    DataFlash.EnableWrites(false);

    const uint32_t tnow = millis();
    if (tnow - last_1hz > 1000) {
        last_1hz = tnow;
        gcs().send_message(MSG_HEARTBEAT);
        gcs().send_message(MSG_EXTENDED_STATUS1);
    }
    if (tnow - last_50hz > 20) {
        last_50hz = tnow;
        gcs_update();
        gcs_data_stream_send();
        notify.update();
    }
    if (tnow - last_5s > 5000) {
        last_5s = tnow;
        gcs().send_text(MAV_SEVERITY_INFO, "Initialising APM");
    }
    check_usb_mux();

    DataFlash.EnableWrites(true);
    in_mavlink_delay = false;
}

/*
 *  send data streams in the given rate range on both links
 */
void Rover::gcs_data_stream_send(void)
{
    gcs().data_stream_send();
}

/*
 *  look for incoming commands on the GCS links
 */
void Rover::gcs_update(void)
{
    gcs().update();
}

/**
   retry any deferred messages
 */
void Rover::gcs_retry_deferred(void)
{
    gcs().retry_deferred();
}

/*
  return true if we will accept this packet. Used to implement SYSID_ENFORCE
 */
bool GCS_MAVLINK_Rover::accept_packet(const mavlink_status_t &status, mavlink_message_t &msg)
{
    if (!rover.g2.sysid_enforce) {
      return true;
    }
    if (msg.msgid == MAVLINK_MSG_ID_RADIO || msg.msgid == MAVLINK_MSG_ID_RADIO_STATUS) {
        return true;
    }
    return (msg.sysid == rover.g.sysid_my_gcs);
}

AP_Camera *GCS_MAVLINK_Rover::get_camera() const
{
#if CAMERA == ENABLED
    return &rover.camera;
#else
    return nullptr;
#endif
}

AP_AdvancedFailsafe *GCS_MAVLINK_Rover::get_advanced_failsafe() const
{
#if ADVANCED_FAILSAFE == ENABLED
    return &rover.g2.afs;
#else
    return nullptr;
#endif
}

AP_VisualOdom *GCS_MAVLINK_Rover::get_visual_odom() const
{
#if VISUAL_ODOMETRY_ENABLED == ENABLED
    return &rover.g2.visual_odom;
#else
    return nullptr;
#endif
}

Compass *GCS_MAVLINK_Rover::get_compass() const
{
    return &rover.compass;
}

AP_Mission *GCS_MAVLINK_Rover::get_mission()
{
    return &rover.mission;
}

bool GCS_MAVLINK_Rover::set_mode(const uint8_t mode)
{
    Mode *new_mode = rover.mode_from_mode_num((enum mode)mode);
    if (new_mode == nullptr) {
        return false;
    }
    return rover.set_mode(*new_mode, MODE_REASON_GCS_COMMAND);
}

const AP_FWVersion &GCS_MAVLINK_Rover::get_fwver() const
{
    return rover.fwver;
}

void GCS_MAVLINK_Rover::set_ekf_origin(const Location& loc)
{
    rover.set_ekf_origin(loc);
}
  contents: read

jobs: #pragma once

#include <GCS_MAVLink/GCS.h>

// default sensors are present and healthy: gyro, accelerometer, rate_control, attitude_stabilization, yaw_position, altitude control, x/y position control, motor_control
#define MAVLINK_SENSOR_PRESENT_DEFAULT (MAV_SYS_STATUS_SENSOR_3D_GYRO | MAV_SYS_STATUS_SENSOR_3D_ACCEL | MAV_SYS_STATUS_SENSOR_ANGULAR_RATE_CONTROL | MAV_SYS_STATUS_SENSOR_ATTITUDE_STABILIZATION | MAV_SYS_STATUS_SENSOR_YAW_POSITION | MAV_SYS_STATUS_SENSOR_XY_POSITION_CONTROL | MAV_SYS_STATUS_SENSOR_MOTOR_OUTPUTS | MAV_SYS_STATUS_AHRS | MAV_SYS_STATUS_SENSOR_BATTERY)

class GCS_MAVLINK_Rover : public GCS_MAVLINK
{
public:

    void data_stream_send(void) override;

protected:

    uint32_t telem_delay() const override;
    bool accept_packet(const mavlink_status_t &status, mavlink_message_t &msg) override;

    Compass *get_compass() const override;
    AP_Mission *get_mission() override;
    AP_Rally *get_rally() const override { return nullptr; };
    AP_Camera *get_camera() const override;
    AP_AdvancedFailsafe *get_advanced_failsafe() const override;
    AP_VisualOdom *get_visual_odom() const override;
    const AP_FWVersion &get_fwver() const override;
    void set_ekf_origin(const Location& loc) override;

    uint8_t sysid_my_gcs() const override;

    bool set_mode(uint8_t mode) override;

    MAV_RESULT _handle_command_preflight_calibration(const mavlink_command_long_t &packet) override;

private:

    void handleMessage(mavlink_message_t * msg) override;
    bool handle_guided_request(AP_Mission::Mission_Command &cmd) override;
    void handle_change_alt_request(AP_Mission::Mission_Command &cmd) override;
    bool try_send_message(enum ap_message id) override;

    MAV_TYPE frame_type() const override;
    MAV_MODE base_mode() const override;
    uint32_t custom_mode() const override;
    MAV_STATE system_status() const override;
};
  analyze: #pragma once

#include <GCS_MAVLink/GCS.h>
#include "GCS_Mavlink.h"

class GCS_Rover : public GCS
{
    friend class Rover; // for access to _chan in parameter declarations

public:

    // return the number of valid GCS objects
    uint8_t num_gcs() const override { return ARRAY_SIZE(_chan); };

    // return GCS link at offset ofs
    GCS_MAVLINK_Rover &chan(const uint8_t ofs) override { return _chan[ofs]; };
    // return GCS link at offset ofs
    const GCS_MAVLINK_Rover &chan(const uint8_t ofs) const override { return _chan[ofs]; };

private:

    GCS_MAVLINK_Rover _chan[MAVLINK_COMM_NUM_BUFFERS];

};
    permissions: #include "Rover.h"

#include <AP_RangeFinder/RangeFinder_Backend.h>

#if LOGGING_ENABLED == ENABLED

struct PACKED log_Steering {
    LOG_PACKET_HEADER;
    uint64_t time_us;
    int16_t steering_in;
    float steering_out;
    float desired_lat_accel;
    float lat_accel;
    float desired_turn_rate;
    float turn_rate;
};

// Write a steering packet
void Rover::Log_Write_Steering()
{
    float lat_accel = DataFlash.quiet_nanf();
    g2.attitude_control.get_lat_accel(lat_accel);
    struct log_Steering pkt = {
        LOG_PACKET_HEADER_INIT(LOG_STEERING_MSG),
        time_us        : AP_HAL::micros64(),
        steering_in        : channel_steer->get_control_in(),
        steering_out       : g2.motors.get_steering(),
        desired_lat_accel  : g2.attitude_control.get_desired_lat_accel(),
        lat_accel          : lat_accel,
        desired_turn_rate  : degrees(g2.attitude_control.get_desired_turn_rate()),
        turn_rate          : degrees(ahrs.get_yaw_rate_earth())
    };
    DataFlash.WriteBlock(&pkt, sizeof(pkt));
}

struct PACKED log_Startup {
    LOG_PACKET_HEADER;
    uint64_t time_us;
    uint8_t startup_type;
    uint16_t command_total;
};

void Rover::Log_Write_Startup(uint8_t type)
{
    struct log_Startup pkt = {
        LOG_PACKET_HEADER_INIT(LOG_STARTUP_MSG),
        time_us         : AP_HAL::micros64(),
        startup_type    : type,
        command_total   : mission.num_commands()
    };
    DataFlash.WriteBlock(&pkt, sizeof(pkt));
}

struct PACKED log_Throttle {
    LOG_PACKET_HEADER;
    uint64_t time_us;
    int16_t throttle_in;
    float throttle_out;
    float desired_speed;
    float speed;
    float accel_y;
};

// Write a throttle control packet
void Rover::Log_Write_Throttle()
{
    const Vector3f accel = ins.get_accel();
    float speed = DataFlash.quiet_nanf();
    g2.attitude_control.get_forward_speed(speed);
    struct log_Throttle pkt = {
        LOG_PACKET_HEADER_INIT(LOG_THR_MSG),
        time_us         : AP_HAL::micros64(),
        throttle_in     : channel_throttle->get_control_in(),
        throttle_out    : g2.motors.get_throttle(),
        desired_speed   : g2.attitude_control.get_desired_speed(),
        speed           : speed,
        accel_y         : accel.y
    };
    DataFlash.WriteBlock(&pkt, sizeof(pkt));
}

struct PACKED log_Nav_Tuning {
    LOG_PACKET_HEADER;
    uint64_t time_us;
    uint16_t yaw;
    float    wp_distance;
    uint16_t target_bearing_cd;
    uint16_t nav_bearing_cd;
    float xtrack_error;
};

// Write a navigation tuning packet
void Rover::Log_Write_Nav_Tuning()
{
    struct log_Nav_Tuning pkt = {
        LOG_PACKET_HEADER_INIT(LOG_NTUN_MSG),
        time_us             : AP_HAL::micros64(),
        yaw                 : static_cast<uint16_t>(ahrs.yaw_sensor),
        wp_distance         : control_mode->get_distance_to_destination(),
        target_bearing_cd   : static_cast<uint16_t>(abs(nav_controller->target_bearing_cd())),
        nav_bearing_cd      : static_cast<uint16_t>(abs(nav_controller->nav_bearing_cd())),
        xtrack_error        : nav_controller->crosstrack_error()
    };
    DataFlash.WriteBlock(&pkt, sizeof(pkt));
}

// Write an attitude packet
void Rover::Log_Write_Attitude()
{
    const Vector3f targets(0.0f, 0.0f, 0.0f);  // Rover does not have attitude targets, use place-holder for commonality with Dataflash Log_Write_Attitude message

    DataFlash.Log_Write_Attitude(ahrs, targets);

#if AP_AHRS_NAVEKF_AVAILABLE
    DataFlash.Log_Write_EKF(ahrs);
    DataFlash.Log_Write_AHRS2(ahrs);
#endif
    DataFlash.Log_Write_POS(ahrs);

    // log steering rate controller
    DataFlash.Log_Write_PID(LOG_PIDS_MSG, g2.attitude_control.get_steering_rate_pid().get_pid_info());
    DataFlash.Log_Write_PID(LOG_PIDA_MSG, g2.attitude_control.get_throttle_speed_pid().get_pid_info());
}

struct PACKED log_Rangefinder {
    LOG_PACKET_HEADER;
    uint64_t time_us;
    float    lateral_accel;
    uint16_t rangefinder1_distance;
    uint16_t rangefinder2_distance;
    uint16_t detected_count;
    int8_t   turn_angle;
    uint16_t turn_time;
    uint16_t ground_speed;
    int8_t   throttle;
};

// Write a rangefinder packet
void Rover::Log_Write_Rangefinder()
{
    uint16_t turn_time = 0;
    if (!is_zero(obstacle.turn_angle)) {
        turn_time = AP_HAL::millis() - obstacle.detected_time_ms;
    }
    AP_RangeFinder_Backend *s0 = rangefinder.get_backend(0);
    AP_RangeFinder_Backend *s1 = rangefinder.get_backend(1);
    struct log_Rangefinder pkt = {
        LOG_PACKET_HEADER_INIT(LOG_RANGEFINDER_MSG),
        time_us               : AP_HAL::micros64(),
        lateral_accel         : g2.attitude_control.get_desired_lat_accel(),
        rangefinder1_distance : s0 ? s0->distance_cm() : (uint16_t)0,
        rangefinder2_distance : s1 ? s1->distance_cm() : (uint16_t)0,
        detected_count        : obstacle.detected_count,
        turn_angle            : static_cast<int8_t>(obstacle.turn_angle),
        turn_time             : turn_time,
        ground_speed          : static_cast<uint16_t>(fabsf(ground_speed * 100.0f)),
        throttle              : int8_t(SRV_Channels::get_output_scaled(SRV_Channel::k_throttle))
    };
    DataFlash.WriteBlock(&pkt, sizeof(pkt));
}

struct PACKED log_Arm_Disarm {
    LOG_PACKET_HEADER;
    uint64_t time_us;
    uint8_t  arm_state;
    uint16_t arm_checks;
};

void Rover::Log_Arm_Disarm() {
    struct log_Arm_Disarm pkt = {
        LOG_PACKET_HEADER_INIT(LOG_ARM_DISARM_MSG),
        time_us                 : AP_HAL::micros64(),
        arm_state               : arming.is_armed(),
        arm_checks              : arming.get_enabled_checks()
    };
    DataFlash.WriteBlock(&pkt, sizeof(pkt));
}

void Rover::Log_Write_RC(void)
{
    DataFlash.Log_Write_RCIN();
    DataFlash.Log_Write_RCOUT();
    if (rssi.enabled()) {
        DataFlash.Log_Write_RSSI(rssi);
    }
}

struct PACKED log_Error {
  LOG_PACKET_HEADER;
  uint64_t time_us;
  uint8_t sub_system;
  uint8_t error_code;
};

// Write an error packet
void Rover::Log_Write_Error(uint8_t sub_system, uint8_t error_code)
{
  struct log_Error pkt = {
      LOG_PACKET_HEADER_INIT(LOG_ERROR_MSG),
      time_us       : AP_HAL::micros64(),
      sub_system    : sub_system,
      error_code    : error_code,
  };
  DataFlash.WriteBlock(&pkt, sizeof(pkt));
}

// log ahrs home and EKF origin to dataflash
void Rover::Log_Write_Home_And_Origin()
{
#if AP_AHRS_NAVEKF_AVAILABLE
    // log ekf origin if set
    Location ekf_orig;
    if (ahrs.get_origin(ekf_orig)) {
        DataFlash.Log_Write_Origin(LogOriginType::ekf_origin, ekf_orig);
    }
#endif

    // log ahrs home if set
    if (ahrs.home_is_set()) {
        DataFlash.Log_Write_Origin(LogOriginType::ahrs_home, ahrs.get_home());
    }
}

// guided mode logging
struct PACKED log_GuidedTarget {
    LOG_PACKET_HEADER;
    uint64_t time_us;
    uint8_t type;
    float pos_target_x;
    float pos_target_y;
    float pos_target_z;
    float vel_target_x;
    float vel_target_y;
    float vel_target_z;
};

// Write a Guided mode target
void Rover::Log_Write_GuidedTarget(uint8_t target_type, const Vector3f& pos_target, const Vector3f& vel_target)
{
    struct log_GuidedTarget pkt = {
        LOG_PACKET_HEADER_INIT(LOG_GUIDEDTARGET_MSG),
        time_us         : AP_HAL::micros64(),
        type            : target_type,
        pos_target_x    : pos_target.x,
        pos_target_y    : pos_target.y,
        pos_target_z    : pos_target.z,
        vel_target_x    : vel_target.x,
        vel_target_y    : vel_target.y,
        vel_target_z    : vel_target.z
    };
    DataFlash.WriteBlock(&pkt, sizeof(pkt));
}

// wheel encoder packet
struct PACKED log_WheelEncoder {
    LOG_PACKET_HEADER;
    uint64_t time_us;
    float distance_0;
    uint8_t quality_0;
    float rpm_0;
    float distance_1;
    uint8_t quality_1;
    float rpm_1;
};

// log wheel encoder information
void Rover::Log_Write_WheelEncoder()
{
    // return immediately if no wheel encoders are enabled
    if (!g2.wheel_encoder.enabled(0) && !g2.wheel_encoder.enabled(1)) {
        return;
    }
    struct log_WheelEncoder pkt = {
        LOG_PACKET_HEADER_INIT(LOG_WHEELENCODER_MSG),
        time_us     : AP_HAL::micros64(),
        distance_0  : g2.wheel_encoder.get_distance(0),
        quality_0   : (uint8_t)constrain_float(g2.wheel_encoder.get_signal_quality(0), 0.0f, 100.0f),
        rpm_0       : wheel_encoder_rpm[0],
        distance_1  : g2.wheel_encoder.get_distance(1),
        quality_1   : (uint8_t)constrain_float(g2.wheel_encoder.get_signal_quality(1), 0.0f, 100.0f),
        rpm_1       : wheel_encoder_rpm[1]
    };
    DataFlash.WriteBlock(&pkt, sizeof(pkt));
}

// Write proximity sensor distances
void Rover::Log_Write_Proximity()
{
    DataFlash.Log_Write_Proximity(g2.proximity);
}

// type and unit information can be found in
// libraries/DataFlash/Logstructure.h; search for "log_Units" for
// units and "Format characters" for field type information
const LogStructure Rover::log_structure[] = {
    LOG_COMMON_STRUCTURES,
    { LOG_STARTUP_MSG, sizeof(log_Startup),
      "STRT", "QBH",        "TimeUS,SType,CTot", "s--", "F--" },
    { LOG_THR_MSG, sizeof(log_Throttle),
      "THR", "Qhffff", "TimeUS,ThrIn,ThrOut,DesSpeed,Speed,AccY", "s--nno", "F--000" },
    { LOG_NTUN_MSG, sizeof(log_Nav_Tuning),
      "NTUN", "QHfHHf", "TimeUS,Yaw,WpDist,TargBrg,NavBrg,XT", "sdmddm", "FB0BB0" },
    { LOG_RANGEFINDER_MSG, sizeof(log_Rangefinder),
      "RGFD", "QfHHHbHCb",  "TimeUS,LatAcc,R1Dist,R2Dist,DCnt,TAng,TTim,Spd,Thr", "somm-hsm-", "F0BB-0CB-" },
    { LOG_ARM_DISARM_MSG, sizeof(log_Arm_Disarm),
      "ARM", "QBH", "TimeUS,ArmState,ArmChecks", "s--", "F--" },
    { LOG_STEERING_MSG, sizeof(log_Steering),
      "STER", "Qhfffff",   "TimeUS,SteerIn,SteerOut,DesLatAcc,LatAcc,DesTurnRate,TurnRate", "s--ookk", "F--0000" },
    { LOG_GUIDEDTARGET_MSG, sizeof(log_GuidedTarget),
      "GUID",  "QBffffff",    "TimeUS,Type,pX,pY,pZ,vX,vY,vZ", "s-mmmnnn", "F-000000" },
    { LOG_ERROR_MSG, sizeof(log_Error),
      "ERR",   "QBB",         "TimeUS,Subsys,ECode", "s--", "F--" },
    { LOG_WHEELENCODER_MSG, sizeof(log_WheelEncoder),
      "WENC",  "Qfbffbf", "TimeUS,Dist0,Qual0,RPM0,Dist1,Qual1,RPM1", "sm-qm-q", "F0--0--" },
};

void Rover::log_init(void)
{
    DataFlash.Init(log_structure, ARRAY_SIZE(log_structure));
}

void Rover::Log_Write_Vehicle_Startup_Messages()
{
    // only 200(?) bytes are guaranteed by DataFlash
    Log_Write_Startup(TYPE_GROUNDSTART_MSG);
    DataFlash.Log_Write_Mode(control_mode->mode_number(), control_mode_reason);
    Log_Write_Home_And_Origin();
    gps.Write_DataFlash_Log_Startup_messages();
}

#else  // LOGGING_ENABLED

// dummy functions
void Rover::Log_Write_Startup(uint8_t type) {}
void Rover::Log_Write_Nav_Tuning() {}
void Rover::Log_Write_Performance() {}
void Rover::Log_Write_Throttle() {}
void Rover::Log_Write_Rangefinder() {}
void Rover::Log_Write_Attitude() {}
void Rover::Log_Write_RC(void) {}
void Rover::Log_Write_GuidedTarget(uint8_t target_type, const Vector3f& pos_target, const Vector3f& vel_target) {}
void Rover::Log_Write_Home_And_Origin() {}
void Rover::Log_Arm_Disarm() {}
void Rover::Log_Write_Error(uint8_t sub_system, uint8_t error_code) {}
void Rover::Log_Write_Steering() {}
void Rover::Log_Write_WheelEncoder() {}
void Rover::Log_Write_Proximity() {}

#endif  // LOGGING_ENABLED
      contents: read # for actions/checkout to fetch code
      security-events: write # for github/codeql-action/upload-sarif to upload SARIF results
      actions: read # only required for a private repository by github/codeql-action/upload-sarif to get the Action run status
    name: Analyze
    runs-on: windows-latest

    steps: include ../mk/apm.mk
      - name: Checkout repository
        uses: actions/checkout@v4

      - name: Configure CMake
        run: cmake -B ${{ all:
        @$(MAKE) -C ../ -f Makefile.waf rover }}

      # Build is not required unless generated source files are used
      # - name: Build CMake
      #   run: cmake --build ${{ env.build }}

      - name: Initialize MSVC Code Analysis
        uses: microsoft/msvc-code-analysis-action@04825f6d9e00f87422d6bf04e1a38b1f3ed60d99
        # Provide a unique ID to access the sarif output path
        id: run-analysis
        with: #include "Rover.h"

/*
  APMRover2 parameter definitions
*/

#define GSCALAR(v, name, def) { rover.g.v.vtype, name, Parameters::k_param_ ## v, &rover.g.v, {def_value:def} }
#define GGROUP(v, name, class) { AP_PARAM_GROUP, name, Parameters::k_param_ ## v, &rover.g.v, {group_info:class::var_info} }
#define GOBJECT(v, name, class) { AP_PARAM_GROUP, name, Parameters::k_param_ ## v, &rover.v, {group_info:class::var_info} }
#define GOBJECTN(v, pname, name, class) { AP_PARAM_GROUP, name, Parameters::k_param_ ## pname, &rover.v, {group_info : class::var_info} }

const AP_Param::Info Rover::var_info[] = {
    // @Param: FORMAT_VERSION
    // @DisplayName: Eeprom format version number
    // @Description: This value is incremented when changes are made to the eeprom format
    // @User: Advanced
    GSCALAR(format_version,         "FORMAT_VERSION",   1),

    // @Param: SYSID_SW_TYPE
    // @DisplayName: Software Type
    // @Description: This is used by the ground station to recognise the software type (eg ArduPlane vs ArduCopter)
    // @Values: 0:ArduPlane,4:AntennaTracker,10:Copter,20:Rover,40:ArduSub
    // @User: Advanced
    // @ReadOnly: True
    GSCALAR(software_type,          "SYSID_SW_TYPE",    Parameters::k_software_type),

    // @Param: LOG_BITMASK
    // @DisplayName: Log bitmask
    // @Description: Bitmap of what log types to enable in dataflash. This values is made up of the sum of each of the log types you want to be saved on dataflash. On a PX4 or Pixhawk the large storage size of a microSD card means it is usually best just to enable all log types by setting this to 65535. On APM2 the smaller 4 MByte dataflash means you need to be more selective in your logging or you may run out of log space while flying (in which case it will wrap and overwrite the start of the log). The individual bits are ATTITUDE_FAST=1, ATTITUDE_MEDIUM=2, GPS=4, PerformanceMonitoring=8, ControlTuning=16, NavigationTuning=32, Mode=64, IMU=128, Commands=256, Battery=512, Compass=1024, TECS=2048, Camera=4096, RCandServo=8192, Rangefinder=16384, Arming=32768, FullLogs=65535
    // @Values: 0:Disabled,5190:APM2-Default,65535:PX4/Pixhawk-Default
    // @Bitmask: 0:ATTITUDE_FAST,1:ATTITUDE_MED,2:GPS,3:PM,4:THR,5:NTUN,7:IMU,8:CMD,9:CURRENT,10:RANGEFINDER,11:COMPASS,12:CAMERA,13:STEERING,14:RC,15:ARM/DISARM,19:IMU_RAW
    // @User: Advanced
    GSCALAR(log_bitmask,            "LOG_BITMASK",      DEFAULT_LOG_BITMASK),

    // @Param: RST_SWITCH_CH
    // @DisplayName: Reset Switch Channel
    // @Description: RC channel to use to reset to last flight mode after geofence takeover.
    // @User: Advanced
    GSCALAR(reset_switch_chan,      "RST_SWITCH_CH",    0),

    // @Param: INITIAL_MODE
    // @DisplayName: Initial driving mode
    // @Description: This selects the mode to start in on boot. This is useful for when you want to start in AUTO mode on boot without a receiver. Usually used in combination with when AUTO_TRIGGER_PIN or AUTO_KICKSTART.
    // @Values: 0:MANUAL,1:ACRO,3:STEERING,4:HOLD,10:AUTO,11:RTL,15:GUIDED
    // @User: Advanced
    GSCALAR(initial_mode,        "INITIAL_MODE",     MANUAL),

    // @Param: SYSID_THIS_MAV
    // @DisplayName: MAVLink system ID of this vehicle
    // @Description: Allows setting an individual MAVLink system id for this vehicle to distinguish it from others on the same network
    // @Range: 1 255
    // @User: Advanced
    GSCALAR(sysid_this_mav,         "SYSID_THISMAV",    MAV_SYSTEM_ID),

    // @Param: SYSID_MYGCS
    // @DisplayName: MAVLink ground station ID
    // @Description: The identifier of the ground station in the MAVLink protocol. Don't change this unless you also modify the ground station to match.
    // @Range: 1 255
    // @User: Advanced
    GSCALAR(sysid_my_gcs,           "SYSID_MYGCS",      255),

    // @Param: TELEM_DELAY
    // @DisplayName: Telemetry startup delay
    // @Description: The amount of time (in seconds) to delay radio telemetry to prevent an Xbee bricking on power up
    // @User: Standard
    // @Units: s
    // @Range: 0 30
    // @Increment: 1
    GSCALAR(telem_delay,            "TELEM_DELAY",     0),

    // @Param: GCS_PID_MASK
    // @DisplayName: GCS PID tuning mask
    // @Description: bitmask of PIDs to send MAVLink PID_TUNING messages for
    // @User: Advanced
    // @Values: 0:None,1:Steering,2:Throttle
    // @Bitmask: 0:Steering,1:Throttle
    GSCALAR(gcs_pid_mask,           "GCS_PID_MASK",     0),

    // @Param: MAG_ENABLE
    // @DisplayName: Enable Compass
    // @Description: Setting this to Enabled(1) will enable the compass. Setting this to Disabled(0) will disable the compass. Note that this is separate from COMPASS_USE. This will enable the low level senor, and will enable logging of magnetometer data. To use the compass for navigation you must also set COMPASS_USE to 1.
    // @User: Standard
    // @Values: 0:Disabled,1:Enabled
    GSCALAR(compass_enabled,        "MAG_ENABLE",       MAGNETOMETER),

    // @Param: AUTO_TRIGGER_PIN
    // @DisplayName: Auto mode trigger pin
    // @Description: pin number to use to enable the throttle in auto mode. If set to -1 then don't use a trigger, otherwise this is a pin number which if held low in auto mode will enable the motor to run. If the switch is released while in AUTO then the motor will stop again. This can be used in combination with INITIAL_MODE to give a 'press button to start' rover with no receiver.
    // @Values: -1:Disabled,0:APM TriggerPin0,1:APM TriggerPin1,2:APM TriggerPin2,3:APM TriggerPin3,4:APM TriggerPin4,5:APM TriggerPin5,6:APM TriggerPin6,7:APM TriggerPin7,8:APM TriggerPin8,50:Pixhawk TriggerPin50,51:Pixhawk TriggerPin51,52:Pixhawk TriggerPin52,53:Pixhawk TriggerPin53,54:Pixhawk TriggerPin54,55:Pixhawk TriggerPin55
    // @User: standard
    GSCALAR(auto_trigger_pin,        "AUTO_TRIGGER_PIN", -1),

    // @Param: AUTO_KICKSTART
    // @DisplayName: Auto mode trigger kickstart acceleration
    // @Description: X acceleration in meters/second/second to use to trigger the motor start in auto mode. If set to zero then auto throttle starts immediately when the mode switch happens, otherwise the rover waits for the X acceleration to go above this value before it will start the motor
    // @Units: m/s/s
    // @Range: 0 20
    // @Increment: 0.1
    // @User: standard
    GSCALAR(auto_kickstart,          "AUTO_KICKSTART", 0.0f),

    // @Param: CRUISE_SPEED
    // @DisplayName: Target cruise speed in auto modes
    // @Description: The target speed in auto missions.
    // @Units: m/s
    // @Range: 0 100
    // @Increment: 0.1
    // @User: Standard
    GSCALAR(speed_cruise,        "CRUISE_SPEED",    CRUISE_SPEED),

    // @Param: SPEED_TURN_GAIN
    // @DisplayName: Target speed reduction while turning
    // @Description: The percentage to reduce the throttle to while turning. If this is 100% then the target speed is not reduced while turning. If this is 50% then the target speed is reduced in proportion to the turn rate, with a reduction of 50% when the steering is maximally deflected.
    // @Units: %
    // @Range: 0 100
    // @Increment: 1
    // @User: Standard
    GSCALAR(speed_turn_gain,    "SPEED_TURN_GAIN",  50),

    // @Param: PIVOT_TURN_ANGLE
    // @DisplayName: Pivot turn angle
    // @Description: Navigation angle threshold in degrees to switch to pivot steering when SKID_STEER_OUT is 1. This allows you to setup a skid steering rover to turn on the spot in auto mode when the angle it needs to turn it greater than this angle. An angle of zero means to disable pivot turning. Note that you will probably also want to set a low value for WP_RADIUS to get neat turns.
    // @Units: deg
    // @Range: 0 360
    // @Increment: 1
    // @User: Standard
    GSCALAR(pivot_turn_angle,   "PIVOT_TURN_ANGLE",  60),

    // @Param: CH7_OPTION
    // @DisplayName: Channel 7 option
    // @Description: What to do use channel 7 for
    // @Values: 0:Nothing,1:SaveWaypoint,2:LearnCruiseSpeed,3:ArmDisarm,4:Manual,5:Acro,6:Steering,7:Hold,8:Auto,9:RTL,10:SmartRTL,11:Guided
    // @User: Standard
    GSCALAR(ch7_option,             "CH7_OPTION",          CH7_OPTION),

    // @Param: CRUISE_THROTTLE
    // @DisplayName: Base throttle percentage in auto
    // @Description: The base throttle percentage to use in auto mode. The CRUISE_SPEED parameter controls the target speed, but the rover starts with the CRUISE_THROTTLE setting as the initial estimate for how much throttle is needed to achieve that speed. It then adjusts the throttle based on how fast the rover is actually going.
    // @Units: %
    // @Range: 0 100
    // @Increment: 1
    // @User: Standard
    GSCALAR(throttle_cruise,        "CRUISE_THROTTLE",    50),

    // @Param: PILOT_STEER_TYPE
    // @DisplayName: Pilot input steering type
    // @Description: Set this to 1 for skid steering input rovers (tank track style in RC controller). When enabled, servo1 is used for the left track control, servo3 is used for right track control
    // @Values: 0:Default,1:Two Paddles Input,2:Direction reversed when backing up,3:Direction unchanged when backing up
    // @User: Standard
    GSCALAR(pilot_steer_type, "PILOT_STEER_TYPE", 0),

    // @Param: FS_ACTION
    // @DisplayName: Failsafe Action
    // @Description: What to do on a failsafe event
    // @Values: 0:Nothing,1:RTL,2:Hold,3:SmartRTL or RTL,4:SmartRTL or Hold
    // @User: Standard
    GSCALAR(fs_action,    "FS_ACTION",     2),

    // @Param: FS_TIMEOUT
    // @DisplayName: Failsafe timeout
    // @Description: How long a failsafe event need to happen for before we trigger the failsafe action
    // @Units: s
    // @User: Standard
    GSCALAR(fs_timeout,    "FS_TIMEOUT",     5),

    // @Param: FS_THR_ENABLE
    // @DisplayName: Throttle Failsafe Enable
    // @Description: The throttle failsafe allows you to configure a software failsafe activated by a setting on the throttle input channel to a low value. This can be used to detect the RC transmitter going out of range. Failsafe will be triggered when the throttle channel goes below the FS_THR_VALUE for FS_TIMEOUT seconds.
    // @Values: 0:Disabled,1:Enabled
    // @User: Standard
    GSCALAR(fs_throttle_enabled,    "FS_THR_ENABLE",     1),

    // @Param: FS_THR_VALUE
    // @DisplayName: Throttle Failsafe Value
    // @Description: The PWM level on the throttle channel below which throttle failsafe triggers.
    // @Range: 910 1100
    // @Increment: 1
    // @User: Standard
    GSCALAR(fs_throttle_value,      "FS_THR_VALUE",     910),

    // @Param: FS_GCS_ENABLE
    // @DisplayName: GCS failsafe enable
    // @Description: Enable ground control station telemetry failsafe. When enabled the Rover will execute the FS_ACTION when it fails to receive MAVLink heartbeat packets for FS_TIMEOUT seconds.
    // @Values: 0:Disabled,1:Enabled
    // @User: Standard
    GSCALAR(fs_gcs_enabled, "FS_GCS_ENABLE",   0),

    // @Param: FS_CRASH_CHECK
    // @DisplayName: Crash check action
    // @Description: What to do on a crash event. When enabled the rover will go to hold if a crash is detected.
    // @Values: 0:Disabled,1:Hold,2:HoldAndDisarm
    // @User: Standard
    GSCALAR(fs_crash_check, "FS_CRASH_CHECK",    FS_CRASH_DISABLE),

    // @Param: RNGFND_TRIGGR_CM
    // @DisplayName: Rangefinder trigger distance
    // @Description: The distance from an obstacle in centimeters at which the rangefinder triggers a turn to avoid the obstacle
    // @Units: cm
    // @Range: 0 1000
    // @Increment: 1
    // @User: Standard
    GSCALAR(rangefinder_trigger_cm,   "RNGFND_TRIGGR_CM",    100),

    // @Param: RNGFND_TURN_ANGL
    // @DisplayName: Rangefinder trigger angle
    // @Description: The course deviation in degrees to apply while avoiding an obstacle detected with the rangefinder. A positive number means to turn right, and a negative angle means to turn left.
    // @Units: deg
    // @Range: -45 45
    // @Increment: 1
    // @User: Standard
    GSCALAR(rangefinder_turn_angle,   "RNGFND_TURN_ANGL",    45),

    // @Param: RNGFND_TURN_TIME
    // @DisplayName: Rangefinder turn time
    // @Description: The amount of time in seconds to apply the RNGFND_TURN_ANGL after detecting an obstacle.
    // @Units: s
    // @Range: 0 100
    // @Increment: 0.1
    // @User: Standard
    GSCALAR(rangefinder_turn_time,    "RNGFND_TURN_TIME",     1.0f),

    // @Param: RNGFND_DEBOUNCE
    // @DisplayName: Rangefinder debounce count
    // @Description: The number of 50Hz rangefinder hits needed to trigger an obstacle avoidance event. If you get a lot of false rangefinder events then raise this number, but if you make it too large then it will cause lag in detecting obstacles, which could cause you go hit the obstacle.
    // @Range: 1 100
    // @Increment: 1
    // @User: Standard
    GSCALAR(rangefinder_debounce,   "RNGFND_DEBOUNCE",    2),

    // @Param: AUX_CH
    // @DisplayName: Auxiliary switch channel
    // @Description: RC Channel to use for auxiliary functions including saving waypoints
    // @User: Advanced
    GSCALAR(aux_channel,    "AUX_CH",       7),

    // @Param: MODE_CH
    // @DisplayName: Mode channel
    // @Description: RC Channel to use for driving mode control
    // @User: Advanced
    GSCALAR(mode_channel,    "MODE_CH",       MODE_CHANNEL),

    // @Param: MODE1
    // @DisplayName: Mode1
    // @Values: 0:Manual,1:Acro,3:Steering,4:Hold,10:Auto,11:RTL,12:SmartRTL,15:Guided
    // @User: Standard
    // @Description: Driving mode for switch position 1 (910 to 1230 and above 2049)
    GSCALAR(mode1,           "MODE1",         MANUAL),

    // @Param: MODE2
    // @DisplayName: Mode2
    // @Description: Driving mode for switch position 2 (1231 to 1360)
    // @Values: 0:Manual,1:Acro,3:Steering,4:Hold,10:Auto,11:RTL,12:SmartRTL,15:Guided
    // @User: Standard
    GSCALAR(mode2,           "MODE2",         MANUAL),

    // @Param: MODE3
    // @DisplayName: Mode3
    // @Description: Driving mode for switch position 3 (1361 to 1490)
    // @Values: 0:Manual,1:Acro,3:Steering,4:Hold,10:Auto,11:RTL,12:SmartRTL,15:Guided
    // @User: Standard
    GSCALAR(mode3,           "MODE3",         MANUAL),

    // @Param: MODE4
    // @DisplayName: Mode4
    // @Description: Driving mode for switch position 4 (1491 to 1620)
    // @Values: 0:Manual,1:Acro,3:Steering,4:Hold,10:Auto,11:RTL,12:SmartRTL,15:Guided
    // @User: Standard
    GSCALAR(mode4,           "MODE4",         MANUAL),

    // @Param: MODE5
    // @DisplayName: Mode5
    // @Description: Driving mode for switch position 5 (1621 to 1749)
    // @Values: 0:Manual,1:Acro,3:Steering,4:Hold,10:Auto,11:RTL,12:SmartRTL,15:Guided
    // @User: Standard
    GSCALAR(mode5,           "MODE5",         MANUAL),

    // @Param: MODE6
    // @DisplayName: Mode6
    // @Description: Driving mode for switch position 6 (1750 to 2049)
    // @Values: 0:Manual,1:Acro,3:Steering,4:Hold,10:Auto,11:RTL,12:SmartRTL,15:Guided
    // @User: Standard
    GSCALAR(mode6,           "MODE6",         MANUAL),

    // @Param: WP_RADIUS
    // @DisplayName: Waypoint radius
    // @Description: The distance in meters from a waypoint when we consider the waypoint has been reached. This determines when the rover will turn along the next waypoint path.
    // @Units: m
    // @Range: 0 1000
    // @Increment: 0.1
    // @User: Standard
    GSCALAR(waypoint_radius,        "WP_RADIUS",        2.0f),

    // @Param: WP_OVERSHOOT
    // @DisplayName: Waypoint overshoot maximum
    // @Description: Waypoint overshoot maximum in meters.  The vehicle will attempt to stay within this many meters of the track as it completes one waypoint and moves to the next.
    // @Units: m
    // @Range: 0 10
    // @Increment: 0.1
    // @User: Standard
    GSCALAR(waypoint_overshoot,     "WP_OVERSHOOT", 2.0f),

    // @Param: TURN_MAX_G
    // @DisplayName: Turning maximum G force
    // @Description: The maximum turning acceleration (in units of gravities) that the rover can handle while remaining stable. The navigation code will keep the lateral acceleration below this level to avoid rolling over or slipping the wheels in turns
    // @Units: gravities
    // @Range: 0.1 10
    // @Increment: 0.01
    // @User: Standard
    GSCALAR(turn_max_g,             "TURN_MAX_G",      0.6f),

    // variables not in the g class which contain EEPROM saved variables

    // @Group: COMPASS_
    // @Path: ../libraries/AP_Compass/AP_Compass.cpp
    GOBJECT(compass,                "COMPASS_", Compass),

    // @Group: SCHED_
    // @Path: ../libraries/AP_Scheduler/AP_Scheduler.cpp
    GOBJECT(scheduler, "SCHED_", AP_Scheduler),

    // barometer ground calibration. The GND_ prefix is chosen for
    // compatibility with previous releases of ArduPlane
    // @Group: GND_
    // @Path: ../libraries/AP_Baro/AP_Baro.cpp
    GOBJECT(barometer, "GND_", AP_Baro),

    // @Group: RELAY_
    // @Path: ../libraries/AP_Relay/AP_Relay.cpp
    GOBJECT(relay,                  "RELAY_", AP_Relay),

    // @Group: RCMAP_
    // @Path: ../libraries/AP_RCMapper/AP_RCMapper.cpp
    GOBJECT(rcmap,                 "RCMAP_",         RCMapper),

    // @Group: SR0_
    // @Path: GCS_Mavlink.cpp
    GOBJECTN(_gcs._chan[0], gcs0,        "SR0_",     GCS_MAVLINK),

    // @Group: SR1_
    // @Path: GCS_Mavlink.cpp
    GOBJECTN(_gcs._chan[1],  gcs1,       "SR1_",     GCS_MAVLINK),

    // @Group: SR2_
    // @Path: GCS_Mavlink.cpp
    GOBJECTN(_gcs._chan[2],  gcs2,       "SR2_",     GCS_MAVLINK),

    // @Group: SR3_
    // @Path: GCS_Mavlink.cpp
    GOBJECTN(_gcs._chan[3],  gcs3,       "SR3_",     GCS_MAVLINK),

    // @Group: SERIAL
    // @Path: ../libraries/AP_SerialManager/AP_SerialManager.cpp
    GOBJECT(serial_manager,         "SERIAL",   AP_SerialManager),

    // @Group: NAVL1_
    // @Path: ../libraries/AP_L1_Control/AP_L1_Control.cpp
    GOBJECT(L1_controller,         "NAVL1_",   AP_L1_Control),

    // @Group: RNGFND
    // @Path: ../libraries/AP_RangeFinder/RangeFinder.cpp
    GOBJECT(rangefinder,                 "RNGFND", RangeFinder),

    // @Group: INS_
    // @Path: ../libraries/AP_InertialSensor/AP_InertialSensor.cpp
    GOBJECT(ins,                            "INS_", AP_InertialSensor),

#if CONFIG_HAL_BOARD == HAL_BOARD_SITL
    // @Group: SIM_
    // @Path: ../libraries/SITL/SITL.cpp
    GOBJECT(sitl, "SIM_", SITL::SITL),
#endif

    // @Group: AHRS_
    // @Path: ../libraries/AP_AHRS/AP_AHRS.cpp
    GOBJECT(ahrs,                   "AHRS_",    AP_AHRS),

#if CAMERA == ENABLED
    // @Group: CAM_
    // @Path: ../libraries/AP_Camera/AP_Camera.cpp
    GOBJECT(camera,                  "CAM_", AP_Camera),
#endif

#if MOUNT == ENABLED
    // @Group: MNT
    // @Path: ../libraries/AP_Mount/AP_Mount.cpp
    GOBJECT(camera_mount,           "MNT",  AP_Mount),
#endif

    // @Group: ARMING_
    // @Path: ../libraries/AP_Arming/AP_Arming.cpp
    GOBJECT(arming,                 "ARMING_", AP_Arming),

    // @Group: LOG
    // @Path: ../libraries/DataFlash/DataFlash.cpp
    GOBJECT(DataFlash,           "LOG",  DataFlash_Class),

    // @Group: BATT
    // @Path: ../libraries/AP_BattMonitor/AP_BattMonitor.cpp
    GOBJECT(battery,                "BATT", AP_BattMonitor),

    // @Group: BRD_
    // @Path: ../libraries/AP_BoardConfig/AP_BoardConfig.cpp
    GOBJECT(BoardConfig,            "BRD_",       AP_BoardConfig),

#if HAL_WITH_UAVCAN
    // @Group: CAN_
    // @Path: ../libraries/AP_BoardConfig/AP_BoardConfig_CAN.cpp
    GOBJECT(BoardConfig_CAN,        "CAN_",       AP_BoardConfig_CAN),
#endif

    // GPS driver
    // @Group: GPS_
    // @Path: ../libraries/AP_GPS/AP_GPS.cpp
    GOBJECT(gps, "GPS_", AP_GPS),

#if AP_AHRS_NAVEKF_AVAILABLE
    // @Group: EK2_
    // @Path: ../libraries/AP_NavEKF2/AP_NavEKF2.cpp
    GOBJECTN(EKF2, NavEKF2, "EK2_", NavEKF2),

    // @Group: EK3_
    // @Path: ../libraries/AP_NavEKF3/AP_NavEKF3.cpp
    GOBJECTN(EKF3, NavEKF3, "EK3_", NavEKF3),
#endif

    // @Group: MIS_
    // @Path: ../libraries/AP_Mission/AP_Mission.cpp
    GOBJECT(mission, "MIS_",       AP_Mission),

    // @Group: RSSI_
    // @Path: ../libraries/AP_RSSI/AP_RSSI.cpp
    GOBJECT(rssi, "RSSI_",  AP_RSSI),

    // @Group: NTF_
    // @Path: ../libraries/AP_Notify/AP_Notify.cpp
    GOBJECT(notify, "NTF_",  AP_Notify),

    // @Group: BTN_
    // @Path: ../libraries/AP_Button/AP_Button.cpp
    GOBJECT(button, "BTN_",  AP_Button),

    // @Group:
    // @Path: Parameters.cpp
    GOBJECT(g2, "",  ParametersG2),

    AP_VAREND
};

/*
  2nd group of parameters
 */
const AP_Param::GroupInfo ParametersG2::var_info[] = {
#if STATS_ENABLED == ENABLED
    // @Group: STAT
    // @Path: ../libraries/AP_Stats/AP_Stats.cpp
    AP_SUBGROUPINFO(stats, "STAT", 1, ParametersG2, AP_Stats),
#endif
    // @Param: SYSID_ENFORCE
    // @DisplayName: GCS sysid enforcement
    // @Description: This controls whether packets from other than the expected GCS system ID will be accepted
    // @Values: 0:NotEnforced,1:Enforced
    // @User: Advanced
    AP_GROUPINFO("SYSID_ENFORCE", 2, ParametersG2, sysid_enforce, 0),

    // @Group: SERVO
    // @Path: ../libraries/SRV_Channel/SRV_Channels.cpp
    AP_SUBGROUPINFO(servo_channels, "SERVO", 3, ParametersG2, SRV_Channels),

    // @Group: RC
    // @Path: ../libraries/RC_Channel/RC_Channels.cpp
    AP_SUBGROUPINFO(rc_channels, "RC", 4, ParametersG2, RC_Channels),

#if ADVANCED_FAILSAFE == ENABLED
    // @Group: AFS_
    // @Path: ../libraries/AP_AdvancedFailsafe/AP_AdvancedFailsafe.cpp
    AP_SUBGROUPINFO(afs, "AFS_", 5, ParametersG2, AP_AdvancedFailsafe),
#endif

    // @Group: BCN
    // @Path: ../libraries/AP_Beacon/AP_Beacon.cpp
    AP_SUBGROUPINFO(beacon, "BCN", 6, ParametersG2, AP_Beacon),

    // @Group: VISO
    // @Path: ../libraries/AP_VisualOdom/AP_VisualOdom.cpp
    AP_SUBGROUPINFO(visual_odom, "VISO", 7, ParametersG2, AP_VisualOdom),

    // @Group: MOT_
    // @Path: AP_MotorsUGV.cpp
    AP_SUBGROUPINFO(motors, "MOT_", 8, ParametersG2, AP_MotorsUGV),

    // @Group: WENC
    // @Path: ../libraries/AP_WheelEncoder/AP_WheelEncoder.cpp
    AP_SUBGROUPINFO(wheel_encoder, "WENC", 9, ParametersG2, AP_WheelEncoder),

    // @Group: ATC
    // @Path: ../libraries/APM_Control/AR_AttitudeControl.cpp
    AP_SUBGROUPINFO(attitude_control, "ATC", 10, ParametersG2, AR_AttitudeControl),

    // @Param: TURN_RADIUS
    // @DisplayName: Turn radius of vehicle
    // @Description: Turn radius of vehicle in meters while at low speeds.  Lower values produce tighter turns in steering mode
    // @Units: m
    // @Range: 0 10
    // @Increment: 0.1
    // @User: Standard
    AP_GROUPINFO("TURN_RADIUS", 11, ParametersG2, turn_radius, 0.9),

    // @Param: ACRO_TURN_RATE
    // @DisplayName: Acro mode turn rate maximum
    // @Description: Acro mode turn rate maximum
    // @Units: deg/s
    // @Range: 0 360
    // @Increment: 1
    // @User: Standard
    AP_GROUPINFO("ACRO_TURN_RATE", 12, ParametersG2, acro_turn_rate, 180.0f),

    // @Group: SRTL_
    // @Path: ../libraries/AP_SmartRTL/AP_SmartRTL.cpp
    AP_SUBGROUPINFO(smart_rtl, "SRTL_", 13, ParametersG2, AP_SmartRTL),

    // @Param: WP_SPEED
    // @DisplayName: Waypoint speed default
    // @Description: Waypoint speed default.  If zero use CRUISE_SPEED.
    // @Units: m/s
    // @Range: 0 100
    // @Increment: 0.1
    // @User: Standard
    AP_GROUPINFO("WP_SPEED", 14, ParametersG2, wp_speed, 0.0f),

    // @Param: RTL_SPEED
    // @DisplayName: Return-to-Launch speed default
    // @Description: Return-to-Launch speed default.  If zero use WP_SPEED or CRUISE_SPEED.
    // @Units: m/s
    // @Range: 0 100
    // @Increment: 0.1
    // @User: Standard
    AP_GROUPINFO("RTL_SPEED", 15, ParametersG2, rtl_speed, 0.0f),

    // @Param: FRAME_CLASS
    // @DisplayName: Frame Class
    // @Description: Frame Class
    // @Values: 0:Undefined,1:Rover,2:Boat
    // @User: Standard
    AP_GROUPINFO("FRAME_CLASS", 16, ParametersG2, frame_class, 1),

    // @Group: FENCE_
    // @Path: ../libraries/AC_Fence/AC_Fence.cpp
    AP_SUBGROUPINFO(fence, "FENCE_", 17, ParametersG2, AC_Fence),

    // @Group: PRX
    // @Path: ../libraries/AP_Proximity/AP_Proximity.cpp
    AP_SUBGROUPINFO(proximity, "PRX", 18, ParametersG2, AP_Proximity),

    // @Group: AVOID_
    // @Path: ../libraries/AC_Avoidance/AC_Avoid.cpp
    AP_SUBGROUPINFO(avoid, "AVOID_", 19, ParametersG2, AC_Avoid),

    AP_GROUPEND
};


ParametersG2::ParametersG2(void)
    :
#if ADVANCED_FAILSAFE == ENABLED
    afs(rover.mission, rover.barometer, rover.gps, rover.rcmap),
#endif
    beacon(rover.serial_manager),
    motors(rover.ServoRelayEvents),
    attitude_control(rover.ahrs),
    smart_rtl(),
    fence(rover.ahrs),
    proximity(rover.serial_manager),
    avoid(rover.ahrs, fence, rover.g2.proximity, &rover.g2.beacon)
{
    AP_Param::setup_object_defaults(this, var_info);
}


/*
  This is a conversion table from old parameter values to new
  parameter names. The startup code looks for saved values of the old
  parameters and will copy them across to the new parameters if the
  new parameter does not yet have a saved value. It then saves the new
  value.

  Note that this works even if the old parameter has been removed. It
  relies on the old k_param index not being removed

  The second column below is the index in the var_info[] table for the
  old object. This should be zero for top level parameters.
 */
const AP_Param::ConversionInfo conversion_table[] = {
    { Parameters::k_param_battery_monitoring, 0,      AP_PARAM_INT8,  "BATT_MONITOR" },
    { Parameters::k_param_battery_volt_pin,   0,      AP_PARAM_INT8,  "BATT_VOLT_PIN" },
    { Parameters::k_param_battery_curr_pin,   0,      AP_PARAM_INT8,  "BATT_CURR_PIN" },
    { Parameters::k_param_volt_div_ratio,     0,      AP_PARAM_FLOAT, "BATT_VOLT_MULT" },
    { Parameters::k_param_curr_amp_per_volt,  0,      AP_PARAM_FLOAT, "BATT_AMP_PERVOLT" },
    { Parameters::k_param_pack_capacity,      0,      AP_PARAM_INT32, "BATT_CAPACITY" },
    { Parameters::k_param_serial0_baud,       0,      AP_PARAM_INT16, "SERIAL0_BAUD" },
    { Parameters::k_param_serial1_baud,       0,      AP_PARAM_INT16, "SERIAL1_BAUD" },
    { Parameters::k_param_serial2_baud,       0,      AP_PARAM_INT16, "SERIAL2_BAUD" },
    { Parameters::k_param_throttle_min_old,   0,      AP_PARAM_INT8,  "MOT_THR_MIN" },
    { Parameters::k_param_throttle_max_old,   0,      AP_PARAM_INT8,  "MOT_THR_MAX" },
};

void Rover::load_parameters(void)
{
    if (!AP_Param::check_var_info()) {
        hal.console->printf("Bad var table\n");
        AP_HAL::panic("Bad var table");
    }

    if (!g.format_version.load() ||
         g.format_version != Parameters::k_format_version) {
        // erase all parameters
        hal.console->printf("Firmware change: erasing EEPROM...\n");
        AP_Param::erase_all();

        // save the current format version
        g.format_version.set_and_save(Parameters::k_format_version);
        hal.console->printf("done.\n");
    }

    const uint32_t before = micros();
    // Load all auto-loaded EEPROM variables
    AP_Param::load_all();
    AP_Param::convert_old_parameters(&conversion_table[0], ARRAY_SIZE(conversion_table));

    AP_Param::set_frame_type_flags(AP_PARAM_FRAME_ROVER);

    SRV_Channels::set_default_function(CH_1, SRV_Channel::k_steering);
    SRV_Channels::set_default_function(CH_3, SRV_Channel::k_throttle);

    const uint8_t old_rc_keys[14] = { Parameters::k_param_rc_1_old,  Parameters::k_param_rc_2_old,
                                      Parameters::k_param_rc_3_old,  Parameters::k_param_rc_4_old,
                                      Parameters::k_param_rc_5_old,  Parameters::k_param_rc_6_old,
                                      Parameters::k_param_rc_7_old,  Parameters::k_param_rc_8_old,
                                      Parameters::k_param_rc_9_old,  Parameters::k_param_rc_10_old,
                                      Parameters::k_param_rc_11_old, Parameters::k_param_rc_12_old,
                                      Parameters::k_param_rc_13_old, Parameters::k_param_rc_14_old };
    const uint16_t old_aux_chan_mask = 0x3FFA;
    SRV_Channels::upgrade_parameters(old_rc_keys, old_aux_chan_mask, &rcmap);
    hal.console->printf("load_all took %uus\n", micros() - before);
    // set a more reasonable default NAVL1_PERIOD for rovers
    L1_controller.set_default_period(NAVL1_PERIOD);
}
          cmakeBuildDirectory: ${{ #pragma once

#include <AP_Common/AP_Common.h>

// Global parameter class.
//
class Parameters {
public:
    // The version of the layout as described by the parameter enum.
    //
    // When changing the parameter enum in an incompatible fashion, this
    // value should be incremented by one.
    //
    // The increment will prevent old parameters from being used incorrectly
    // by newer code.
    //
    static const uint16_t k_format_version = 16;
    static const uint16_t k_software_type = 20;

    enum {
        // Layout version number, always key zero.
        //
        k_param_format_version = 0,
        k_param_software_type,
        k_param_BoardConfig_CAN,

        // Misc
        //
        k_param_log_bitmask_old = 10,  // unused
        k_param_num_resets_old,         // unused
        k_param_reset_switch_chan,
        k_param_initial_mode,
        k_param_scheduler,
        k_param_relay,
        k_param_BoardConfig,
        k_param_pivot_turn_angle,
        k_param_rc_13_old,  // unused
        k_param_rc_14_old,  // unused

        // IO pins
        k_param_rssi_pin = 20,  // unused, replaced by rssi_ library parameters
        k_param_battery_volt_pin,
        k_param_battery_curr_pin,

        // braking
        k_param_braking_percent_old = 30,   // unused
        k_param_braking_speederr_old,       // unused

        // misc2
        k_param_log_bitmask = 40,
        k_param_gps,
        k_param_serial0_baud,   // deprecated, can be deleted
        k_param_serial1_baud,   // deprecated, can be deleted
        k_param_serial2_baud,   // deprecated, can be deleted

        // 97: RSSI
        k_param_rssi = 97,

        // 100: Arming parameters
        k_param_arming = 100,

        // 110: Telemetry control
        //
        k_param_gcs0 = 110,         // stream rates for uartA
        k_param_gcs1,               // stream rates for uartC
        k_param_sysid_this_mav,
        k_param_sysid_my_gcs,
        k_param_serial0_baud_old,   // unused
        k_param_serial1_baud_old,   // unused
        k_param_telem_delay,
        k_param_skip_gyro_cal,      // unused
        k_param_gcs2,               // stream rates for uartD
        k_param_serial2_baud_old,   // unused
        k_param_serial2_protocol,   // deprecated, can be deleted
        k_param_serial_manager,     // serial manager library
        k_param_cli_enabled_old,    // unused
        k_param_gcs3,
        k_param_gcs_pid_mask,

        //
        // 130: Sensor parameters
        //
        k_param_compass_enabled = 130,
        k_param_steering_learn,     // unused
        k_param_NavEKF,             // deprecated - remove
        k_param_mission,            // mission library
        k_param_NavEKF2_old,        // deprecated - remove
        k_param_NavEKF2,
        k_param_g2,                 // 2nd block of parameters
        k_param_NavEKF3,

        // 140: battery controls
        k_param_battery_monitoring = 140,   // deprecated, can be deleted
        k_param_volt_div_ratio,             // deprecated, can be deleted
        k_param_curr_amp_per_volt,          // deprecated, can be deleted
        k_param_input_voltage,              // deprecated, can be deleted
        k_param_pack_capacity,              // deprecated, can be deleted
        k_param_battery,

        //
        // 150: Navigation parameters
        //
        k_param_crosstrack_gain = 150,  // unused
        k_param_crosstrack_entry_angle, // unused
        k_param_speed_cruise,
        k_param_speed_turn_gain,
        k_param_speed_turn_dist,    // unused
        k_param_ch7_option,
        k_param_auto_trigger_pin,
        k_param_auto_kickstart,
        k_param_turn_circle,  // unused
        k_param_turn_max_g,

        //
        // 160: Radio settings
        //
        k_param_rc_1_old = 160, // unused
        k_param_rc_2_old,       // unused
        k_param_rc_3_old,       // unused
        k_param_rc_4_old,       // unused
        k_param_rc_5_old,       // unused
        k_param_rc_6_old,       // unused
        k_param_rc_7_old,       // unused
        k_param_rc_8_old,       // unused

        // throttle control
        k_param_throttle_min_old = 170, // unused
        k_param_throttle_max_old,       // unused
        k_param_throttle_cruise,
        k_param_throttle_slewrate_old,  // unused
        k_param_throttle_reduction,     // unused
        k_param_pilot_steer_type,
        k_param_skid_steer_out_old, // unused

        // failsafe control
        k_param_fs_action = 180,
        k_param_fs_timeout,
        k_param_fs_throttle_enabled,
        k_param_fs_throttle_value,
        k_param_fs_gcs_enabled,
        k_param_fs_crash_check,

        // obstacle control
        k_param_sonar_enabled = 190,  // deprecated, can be removed
        k_param_sonar_old,            // unused
        k_param_rangefinder_trigger_cm,
        k_param_rangefinder_turn_angle,
        k_param_rangefinder_turn_time,
        k_param_sonar2_old,           // unused
        k_param_rangefinder_debounce,
        k_param_rangefinder,          // rangefinder object

        //
        // 210: driving modes
        //
        k_param_mode_channel = 210,
        k_param_mode1,
        k_param_mode2,
        k_param_mode3,
        k_param_mode4,
        k_param_mode5,
        k_param_mode6,
        k_param_aux_channel,

        //
        // 220: Waypoint data
        //
        k_param_command_total = 220,    // unused
        k_param_command_index,          // unused
        k_param_waypoint_radius,
        k_param_waypoint_overshoot,

        //
        // 230: camera control
        //
        k_param_camera,
        k_param_camera_mount,
        k_param_camera_mount2,          // unused

        //
        // 240: PID Controllers
        k_param_pidNavSteer = 230,
        k_param_pidServoSteer,  // unused
        k_param_pidSpeedThrottle_old,   // unused

        // high RC channels
        k_param_rc_9_old = 235, // unused
        k_param_rc_10_old,      // unused
        k_param_rc_11_old,      // unused
        k_param_rc_12_old,      // unusedS

        // other objects
        k_param_sitl = 240,
        k_param_ahrs,
        k_param_ins,
        k_param_compass,
        k_param_rcmap,
        k_param_L1_controller,
        k_param_steerController_old,    // unused
        k_param_barometer,
        k_param_notify,
        k_param_button,

        k_param_DataFlash = 253,  // Logging Group

        // 254,255: reserved
        };

    AP_Int16    format_version;
    AP_Int8     software_type;

    // Misc
    //
    AP_Int32    log_bitmask;
    AP_Int8     reset_switch_chan;
    AP_Int8     initial_mode;

    // Telemetry control
    //
    AP_Int16    sysid_this_mav;
    AP_Int16    sysid_my_gcs;
    AP_Int8     telem_delay;

    // sensor parameters
    AP_Int8     compass_enabled;

    // navigation parameters
    //
    AP_Float    speed_cruise;
    AP_Int8     speed_turn_gain;
    AP_Int8     ch7_option;
    AP_Int8     auto_trigger_pin;
    AP_Float    auto_kickstart;
    AP_Float    turn_max_g;
    AP_Int16    pivot_turn_angle;
    AP_Int16    gcs_pid_mask;

    // Throttle
    //
    AP_Int8     throttle_cruise;
    AP_Int8     pilot_steer_type;

    // failsafe control
    AP_Int8     fs_action;
    AP_Float    fs_timeout;
    AP_Int8     fs_throttle_enabled;
    AP_Int16    fs_throttle_value;
    AP_Int8     fs_gcs_enabled;
    AP_Int8     fs_crash_check;

    // obstacle avoidance control
    AP_Int16    rangefinder_trigger_cm;
    AP_Float    rangefinder_turn_angle;
    AP_Float    rangefinder_turn_time;
    AP_Int8     rangefinder_debounce;


    // driving modes
    //
    AP_Int8     mode_channel;
    AP_Int8     mode1;
    AP_Int8     mode2;
    AP_Int8     mode3;
    AP_Int8     mode4;
    AP_Int8     mode5;
    AP_Int8     mode6;
    AP_Int8     aux_channel;

    // Waypoints
    //
    AP_Float    waypoint_radius;
    AP_Float    waypoint_overshoot;

    Parameters() {}
};

/*
  2nd block of parameters, to avoid going past 256 top level keys
 */
class ParametersG2 {
public:
    ParametersG2(void);

    // var_info for holding Parameter information
    static const struct AP_Param::GroupInfo var_info[];

#if STATS_ENABLED == ENABLED
    // vehicle statistics
    AP_Stats stats;
#endif

    // whether to enforce acceptance of packets only from sysid_my_gcs
    AP_Int8 sysid_enforce;

    // RC input channels
    RC_Channels rc_channels;

    // control over servo output ranges
    SRV_Channels servo_channels;

#if ADVANCED_FAILSAFE == ENABLED
    // advanced failsafe library
    AP_AdvancedFailsafe_Rover afs;
#endif

    AP_Beacon beacon;

    // Visual Odometry camera
    AP_VisualOdom visual_odom;

    // Motor library
    AP_MotorsUGV motors;

    // wheel encoders
    AP_WheelEncoder wheel_encoder;

    // steering and throttle controller
    AR_AttitudeControl attitude_control;

    // turn radius of vehicle (only used in steering mode)
    AP_Float turn_radius;

    // acro mode turn rate maximum
    AP_Float acro_turn_rate;

    // Safe RTL library
    AP_SmartRTL smart_rtl;

    // default speeds for auto, rtl
    AP_Float wp_speed;
    AP_Float rtl_speed;

    // frame class for vehicle
    AP_Int8 frame_class;

    // fence library
    AC_Fence fence;

    // proximity library
    AP_Proximity proximity;

    // avoidance library
    AC_Avoid avoid;
};

extern const AP_Param::Info var_info[]; }}
          # Ruleset file that will determine what checks will be run
          ruleset: NativeRecommendedRules.ruleset

      # Upload SARIF file to GitHub Code Scanning Alerts
      - name: Upload SARIF to GitHub
        uses: github/codeql-action/upload-sarif@v2
        with: /*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/* 
   main Rover class, containing all vehicle specific state
*/

#include "Rover.h"

#define FORCE_VERSION_H_INCLUDE
#include "version.h"
#undef FORCE_VERSION_H_INCLUDE

Rover::Rover(void) :
    param_loader(var_info),
    channel_steer(nullptr),
    channel_throttle(nullptr),
    channel_aux(nullptr),
    DataFlash{fwver.fw_string, g.log_bitmask},
    modes(&g.mode1),
    nav_controller(&L1_controller),
    control_mode(&mode_initializing),
    home(ahrs.get_home()),
    G_Dt(0.02f),
    mode_auto(mode_rtl)
{
}
          sarif_file: ${{ /*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
/*
   main Rover class, containing all vehicle specific state
*/
#pragma once

#include <cmath>
#include <stdarg.h>

// Libraries
#include <AP_Common/AP_Common.h>
#include <AP_HAL/AP_HAL.h>
#include <AC_PID/AC_P.h>
#include <AC_PID/AC_PID.h>
#include <AP_AccelCal/AP_AccelCal.h>                // interface and maths for accelerometer calibration
#include <AP_ADC/AP_ADC.h>                          // ArduPilot Mega Analog to Digital Converter Library
#include <AP_AHRS/AP_AHRS.h>                        // ArduPilot Mega DCM Library
#include <AP_Airspeed/AP_Airspeed.h>                // needed for AHRS build
#include <AP_Baro/AP_Baro.h>
#include <AP_BattMonitor/AP_BattMonitor.h>          // Battery monitor library
#include <AP_Beacon/AP_Beacon.h>
#include <AP_BoardConfig/AP_BoardConfig.h>
#include <AP_BoardConfig/AP_BoardConfig_CAN.h>
#include <AP_Buffer/AP_Buffer.h>                    // FIFO buffer library
#include <AP_Button/AP_Button.h>
#include <AP_Camera/AP_Camera.h>                    // Camera triggering
#include <AP_Compass/AP_Compass.h>                  // ArduPilot Mega Magnetometer Library
#include <AP_Declination/AP_Declination.h>          // Compass declination library
#include <AP_Frsky_Telem/AP_Frsky_Telem.h>
#include <AP_Devo_Telem/AP_Devo_Telem.h>
#include <AP_GPS/AP_GPS.h>                          // ArduPilot GPS library
#include <AP_InertialSensor/AP_InertialSensor.h>    // Inertial Sensor (uncalibated IMU) Library
#include <AP_L1_Control/AP_L1_Control.h>
#include <AP_Math/AP_Math.h>                        // ArduPilot Mega Vector/Matrix math Library
#include <AP_Menu/AP_Menu.h>
#include <AP_Mission/AP_Mission.h>                  // Mission command library
#include <AP_Mount/AP_Mount.h>                      // Camera/Antenna mount
#include <AP_NavEKF2/AP_NavEKF2.h>
#include <AP_NavEKF3/AP_NavEKF3.h>
#include <AP_Navigation/AP_Navigation.h>
#include <AP_Notify/AP_Notify.h>                    // Notify library
#include <AP_OpticalFlow/AP_OpticalFlow.h>          // Optical Flow library
#include <AP_Param/AP_Param.h>
#include <AP_Rally/AP_Rally.h>
#include <AP_RangeFinder/AP_RangeFinder.h>          // Range finder library
#include <AP_RCMapper/AP_RCMapper.h>                // RC input mapping library
#include <AP_Relay/AP_Relay.h>                      // APM relay
#include <AP_RSSI/AP_RSSI.h>                        // RSSI Library
#include <AP_Scheduler/AP_Scheduler.h>              // main loop scheduler
#include <AP_SerialManager/AP_SerialManager.h>      // Serial manager library
#include <AP_ServoRelayEvents/AP_ServoRelayEvents.h>
#include <AP_Stats/AP_Stats.h>                      // statistics library
#include <AP_Terrain/AP_Terrain.h>
#include <AP_Vehicle/AP_Vehicle.h>                  // needed for AHRS build
#include <AP_VisualOdom/AP_VisualOdom.h>
#include <AP_WheelEncoder/AP_WheelEncoder.h>
#include <APM_Control/AR_AttitudeControl.h>
#include <AP_SmartRTL/AP_SmartRTL.h>
#include <DataFlash/DataFlash.h>
#include <Filter/AverageFilter.h>                   // Mode Filter from Filter library
#include <Filter/Butter.h>                          // Filter library - butterworth filter
#include <Filter/Filter.h>                          // Filter library
#include <Filter/LowPassFilter.h>
#include <Filter/ModeFilter.h>                      // Mode Filter from Filter library
#include <RC_Channel/RC_Channel.h>                  // RC Channel Library
#include <StorageManager/StorageManager.h>
#include <AC_Fence/AC_Fence.h>
#include <AP_Proximity/AP_Proximity.h>
#include <AC_Avoidance/AC_Avoid.h>
#if CONFIG_HAL_BOARD == HAL_BOARD_SITL
#include <SITL/SITL.h>
#endif

// Local modules
#include "AP_MotorsUGV.h"
#include "mode.h"
#include "AP_Arming.h"
// Configuration
#include "config.h"
#include "defines.h"
#if ADVANCED_FAILSAFE == ENABLED
#include "afs_rover.h"
#endif
#include "Parameters.h"
#include "GCS_Mavlink.h"
#include "GCS_Rover.h"

class Rover : public AP_HAL::HAL::Callbacks {
public:
    friend class GCS_MAVLINK_Rover;
    friend class Parameters;
    friend class ParametersG2;
    friend class AP_Arming_Rover;
#if ADVANCED_FAILSAFE == ENABLED
    friend class AP_AdvancedFailsafe_Rover;
#endif
    friend class GCS_Rover;
    friend class Mode;
    friend class ModeAcro;
    friend class ModeAuto;
    friend class ModeGuided;
    friend class ModeHold;
    friend class ModeSteering;
    friend class ModeManual;
    friend class ModeRTL;
    friend class ModeSmartRTL;

    Rover(void);

    // HAL::Callbacks implementation.
    void setup(void) override;
    void loop(void) override;

private:
    static const AP_FWVersion fwver;

    // must be the first AP_Param variable declared to ensure its
    // constructor runs before the constructors of the other AP_Param
    // variables
    AP_Param param_loader;

    // all settable parameters
    Parameters g;
    ParametersG2 g2;

    // main loop scheduler
    AP_Scheduler scheduler;

    // mapping between input channels
    RCMapper rcmap;

    // board specific config
    AP_BoardConfig BoardConfig;

#if HAL_WITH_UAVCAN
    // board specific config for CAN bus
    AP_BoardConfig_CAN BoardConfig_CAN;
#endif

    // primary control channels
    RC_Channel *channel_steer;
    RC_Channel *channel_throttle;
    RC_Channel *channel_aux;

    DataFlash_Class DataFlash;

    // sensor drivers
    AP_GPS gps;
    AP_Baro barometer;
    Compass compass;
    AP_InertialSensor ins;
    RangeFinder rangefinder{serial_manager, ROTATION_NONE};
    AP_Button button;

    // flight modes convenience array
    AP_Int8 *modes;

    // Inertial Navigation EKF
#if AP_AHRS_NAVEKF_AVAILABLE
    NavEKF2 EKF2{&ahrs, rangefinder};
    NavEKF3 EKF3{&ahrs, rangefinder};
    AP_AHRS_NavEKF ahrs{EKF2, EKF3};
#else
    AP_AHRS_DCM ahrs;
#endif

    // Arming/Disarming management class
    AP_Arming_Rover arming{ahrs, compass, battery, g2.fence};

    AP_L1_Control L1_controller{ahrs, nullptr};

    // selected navigation controller
    AP_Navigation *nav_controller;

    // Mission library
    AP_Mission mission{ahrs,
            FUNCTOR_BIND_MEMBER(&Rover::start_command, bool, const AP_Mission::Mission_Command&),
            FUNCTOR_BIND_MEMBER(&Rover::verify_command_callback, bool, const AP_Mission::Mission_Command&),
            FUNCTOR_BIND_MEMBER(&Rover::exit_mission, void)};

#if AP_AHRS_NAVEKF_AVAILABLE
    OpticalFlow optflow{ahrs};
#endif

    // RSSI
    AP_RSSI rssi;

#if CONFIG_HAL_BOARD == HAL_BOARD_SITL
    SITL::SITL sitl;
#endif

    AP_SerialManager serial_manager;

    // GCS handling
    GCS_Rover _gcs;  // avoid using this; use gcs()
    GCS_Rover &gcs() { return _gcs; }

    // relay support
    AP_Relay relay;

    AP_ServoRelayEvents ServoRelayEvents{relay};

    // The rover's current location
    struct Location current_loc;

    // Camera
#if CAMERA == ENABLED
    AP_Camera camera{&relay, MASK_LOG_CAMERA, current_loc, ahrs};
#endif

    // Camera/Antenna mount tracking and stabilisation stuff
#if MOUNT == ENABLED
    // current_loc uses the baro/gps solution for altitude rather than gps only.
    AP_Mount camera_mount{ahrs, current_loc};
#endif

    // true if initialisation has completed
    bool initialised;

    // if USB is connected
    bool usb_connected;

    // This is the state of the flight control system
    // There are multiple states defined such as MANUAL, AUTO, ...
    Mode *control_mode;
    mode_reason_t control_mode_reason = MODE_REASON_INITIALISED;

    // Used to maintain the state of the previous control switch position
    // This is set to -1 when we need to re-read the switch
    uint8_t oldSwitchPosition;

    // A flag if GCS joystick control is in use
    bool rc_override_active;

    // Failsafe
    // A tracking variable for type of failsafe active
    // Used for failsafe based on loss of RC signal or GCS signal. See
    // FAILSAFE_EVENT_*
    struct {
        uint8_t bits;
        uint32_t rc_override_timer;
        uint32_t start_time;
        uint8_t triggered;
        uint32_t last_valid_rc_ms;
    } failsafe;

    // notification object for LEDs, buzzers etc (parameter set to false disables external leds)
    AP_Notify notify;

    // true if we have a position estimate from AHRS
    bool have_position;

    // receiver RSSI
    uint8_t receiver_rssi;

    // the time when the last HEARTBEAT message arrived from a GCS
    uint32_t last_heartbeat_ms;

    // obstacle detection information
    struct {
        // have we detected an obstacle?
        uint8_t detected_count;
        float turn_angle;
        uint16_t rangefinder1_distance_cm;
        uint16_t rangefinder2_distance_cm;

        // time when we last detected an obstacle, in milliseconds
        uint32_t detected_time_ms;
    } obstacle;

    // Ground speed
    // The amount current ground speed is below min ground speed.  meters per second
    float ground_speed;

    // CH7 auxiliary switches last known position
    aux_switch_pos aux_ch7;

    // Battery Sensors
    AP_BattMonitor battery{MASK_LOG_CURRENT,
                           FUNCTOR_BIND_MEMBER(&Rover::handle_battery_failsafe, void, const char*, const int8_t),
                           _failsafe_priorities};

#if FRSKY_TELEM_ENABLED == ENABLED
    // FrSky telemetry support
    AP_Frsky_Telem frsky_telemetry{ahrs, battery, rangefinder};
#endif
#if DEVO_TELEM_ENABLED == ENABLED
    AP_DEVO_Telem devo_telemetry{ahrs};
#endif

    uint32_t control_sensors_present;
    uint32_t control_sensors_enabled;
    uint32_t control_sensors_health;

    // Conditional command
    // A value used in condition commands (eg delay, change alt, etc.)
    // For example in a change altitude command, it is the altitude to change to.
    int32_t condition_value;
    // A starting value used to check the status of a conditional command.
    // For example in a delay command the condition_start records that start time for the delay
    int32_t condition_start;

    // 3D Location vectors
    // Location structure defined in AP_Common
    // The home location used for RTL.  The location is set when we first get stable GPS lock
    const struct Location &home;

    // true if the system time has been set from the GPS
    bool system_time_set;

    // true if the compass's initial location has been set
    bool compass_init_location;

    // IMU variables
    // The main loop execution time.  Seconds
    // This is the time between calls to the DCM algorithm and is the Integration time for the gyros.
    float G_Dt;

    // set if we are driving backwards
    bool in_reverse;

    // true if pivoting (set by use_pivot_steering)
    bool pivot_steering_active;

    static const AP_Scheduler::Task scheduler_tasks[];

    // use this to prevent recursion during sensor init
    bool in_mavlink_delay;

    static const AP_Param::Info var_info[];
    static const LogStructure log_structure[];

    // Loiter control
    uint16_t loiter_duration;       // How long we should loiter at the nav_waypoint (time in seconds)
    uint32_t loiter_start_time;     // How long have we been loitering - The start time in millis
    bool previously_reached_wp;     // set to true if we have EVER reached the waypoint

    // time that rudder/steering arming has been running
    uint32_t rudder_arm_timer;

    // Store the time the last GPS message was received.
    uint32_t last_gps_msg_ms{0};

    // last visual odometry update time
    uint32_t visual_odom_last_update_ms;

    // last wheel encoder update times
    float wheel_encoder_last_angle_rad[WHEELENCODER_MAX_INSTANCES];     // distance in radians at time of last update to EKF
    uint32_t wheel_encoder_last_update_ms[WHEELENCODER_MAX_INSTANCES];  // system time of last ping from each encoder
    uint32_t wheel_encoder_last_ekf_update_ms;                          // system time of last encoder data push to EKF
    float wheel_encoder_rpm[WHEELENCODER_MAX_INSTANCES];                // for reporting to GCS

    // True when we are doing motor test
    bool motor_test;

    ModeInitializing mode_initializing;
    ModeHold mode_hold;
    ModeManual mode_manual;
    ModeAcro mode_acro;
    ModeGuided mode_guided;
    ModeAuto mode_auto;
    ModeSteering mode_steering;
    ModeRTL mode_rtl;
    ModeSmartRTL mode_smartrtl;

    // cruise throttle and speed learning
    struct {
        bool learning;
        LowPassFilterFloat speed_filt = LowPassFilterFloat(2.0f);
        LowPassFilterFloat throttle_filt = LowPassFilterFloat(2.0f);
    } cruise_learn;

private:

    // APMrover2.cpp
    void stats_update();
    void ahrs_update();
    void gcs_failsafe_check(void);
    void update_compass(void);
    void update_logging1(void);
    void update_logging2(void);
    void update_aux(void);
    void one_second_loop(void);
    void update_GPS_50Hz(void);
    void update_GPS_10Hz(void);
    void update_current_mode(void);

    // capabilities.cpp
    void init_capabilities(void);

    // commands_logic.cpp
    void update_mission(void);
    bool start_command(const AP_Mission::Mission_Command& cmd);
    void exit_mission();
    bool verify_command_callback(const AP_Mission::Mission_Command& cmd);
    bool verify_command(const AP_Mission::Mission_Command& cmd);
    void do_RTL(void);
    void do_nav_wp(const AP_Mission::Mission_Command& cmd, bool always_stop_at_destination);
    void do_nav_set_yaw_speed(const AP_Mission::Mission_Command& cmd);
    bool verify_nav_wp(const AP_Mission::Mission_Command& cmd);
    bool verify_RTL();
    bool verify_loiter_unlimited(const AP_Mission::Mission_Command& cmd);
    bool verify_loiter_time(const AP_Mission::Mission_Command& cmd);
    bool verify_nav_set_yaw_speed();
    void do_wait_delay(const AP_Mission::Mission_Command& cmd);
    void do_within_distance(const AP_Mission::Mission_Command& cmd);
    bool verify_wait_delay();
    bool verify_within_distance();
    void do_change_speed(const AP_Mission::Mission_Command& cmd);
    void do_set_home(const AP_Mission::Mission_Command& cmd);
#if CAMERA == ENABLED
    void do_digicam_configure(const AP_Mission::Mission_Command& cmd);
    void do_digicam_control(const AP_Mission::Mission_Command& cmd);
#endif
    void do_set_reverse(const AP_Mission::Mission_Command& cmd);

    // commands.cpp
    void update_home_from_EKF();
    bool set_home_to_current_location(bool lock);
    bool set_home(const Location& loc, bool lock);
    void set_ekf_origin(const Location& loc);
    void set_system_time_from_GPS();
    void update_home();

    // compat.cpp
    void delay(uint32_t ms);

    // control_modes.cpp
    Mode *mode_from_mode_num(enum mode num);
    void read_control_switch();
    uint8_t readSwitch(void);
    void reset_control_switch();
    aux_switch_pos read_aux_switch_pos();
    void init_aux_switch();
    void read_aux_switch();
    bool motor_active();

    // crash_check.cpp
    void crash_check();

    // cruise_learn.cpp
    void cruise_learn_start();
    void cruise_learn_update();
    void cruise_learn_complete();

    // failsafe.cpp
    void failsafe_trigger(uint8_t failsafe_type, bool on);
    void handle_battery_failsafe(const char* type_str, const int8_t action);
#if ADVANCED_FAILSAFE == ENABLED
    void afs_fs_check(void);
#endif

    // fence.cpp
    void fence_check();
    void fence_send_mavlink_status(mavlink_channel_t chan);

    // GCS_Mavlink.cpp
    void send_attitude(mavlink_channel_t chan);
    void send_extended_status1(mavlink_channel_t chan);
    void send_location(mavlink_channel_t chan);
    void send_nav_controller_output(mavlink_channel_t chan);
    void send_servo_out(mavlink_channel_t chan);
    void send_vfr_hud(mavlink_channel_t chan);
    void send_simstate(mavlink_channel_t chan);
    void send_rangefinder(mavlink_channel_t chan);
    void send_pid_tuning(mavlink_channel_t chan);
    void send_wheel_encoder(mavlink_channel_t chan);
    void send_fence_status(mavlink_channel_t chan);
    void gcs_data_stream_send(void);
    void gcs_update(void);
    void gcs_retry_deferred(void);

    // Log.cpp
    void Log_Write_Performance();
    void Log_Write_Steering();
    void Log_Write_Startup(uint8_t type);
    void Log_Write_Throttle();
    void Log_Write_Nav_Tuning();
    void Log_Write_Attitude();
    void Log_Write_Rangefinder();
    void Log_Arm_Disarm();
    void Log_Write_RC(void);
    void Log_Write_Error(uint8_t sub_system, uint8_t error_code);
    void Log_Write_Home_And_Origin();
    void Log_Write_GuidedTarget(uint8_t target_type, const Vector3f& pos_target, const Vector3f& vel_target);
    void Log_Write_WheelEncoder();
    void Log_Write_Proximity();
    void Log_Read(uint16_t log_num, uint16_t start_page, uint16_t end_page);
    void log_init(void);
    void Log_Write_Vehicle_Startup_Messages();

    // Parameters.cpp
    void load_parameters(void);

    // radio.cpp
    void set_control_channels(void);
    void init_rc_in();
    void init_rc_out();
    void rudder_arm_disarm_check();
    void read_radio();
    void control_failsafe(uint16_t pwm);
    void trim_control_surfaces();
    void trim_radio();

    // sensors.cpp
    void init_compass(void);
    void compass_accumulate(void);
    void init_rangefinder(void);
    void init_beacon();
    void init_visual_odom();
    void update_visual_odom();
    void update_wheel_encoder();
    void read_receiver_rssi(void);
    void compass_cal_update(void);
    void accel_cal_update(void);
    void read_rangefinders(void);
    void init_proximity();
    void update_sensor_status_flags(void);

    // Steering.cpp
    bool use_pivot_steering(float yaw_error_cd);
    void set_servos(void);

    // system.cpp
    void init_ardupilot();
    void startup_ground(void);
    void set_reverse(bool reverse);
    bool set_mode(Mode &new_mode, mode_reason_t reason);
    bool mavlink_set_mode(uint8_t mode);
    void startup_INS_ground(void);
    void check_usb_mux(void);
    void print_mode(AP_HAL::BetterStream *port, uint8_t mode);
    void notify_mode(const Mode *new_mode);
    uint8_t check_digital_pin(uint8_t pin);
    bool should_log(uint32_t mask);
    void change_arm_state(void);
    bool arm_motors(AP_Arming::ArmingMethod method);
    bool disarm_motors(void);
    bool is_boat() const;

    enum Failsafe_Action {
        Failsafe_Action_None          = 0,
        Failsafe_Action_RTL           = 1,
        Failsafe_Action_Hold          = 2,
        Failsafe_Action_SmartRTL      = 3,
        Failsafe_Action_SmartRTL_Hold = 4,
        Failsafe_Action_Terminate     = 5
    };

    static constexpr int8_t _failsafe_priorities[] = {
                                                       Failsafe_Action_Terminate,
                                                       Failsafe_Action_Hold,
                                                       Failsafe_Action_RTL,
                                                       Failsafe_Action_SmartRTL_Hold,
                                                       Failsafe_Action_SmartRTL,
                                                       Failsafe_Action_None,
                                                       -1 // the priority list must end with a sentinel of -1
                                                      };
    static_assert(_failsafe_priorities[ARRAY_SIZE(_failsafe_priorities) - 1] == -1,
                  "_failsafe_priorities is missing the sentinel");


public:
    void mavlink_delay_cb();
    void failsafe_check();

    void update_soft_armed();
    // Motor test
    void motor_test_output();
    bool mavlink_motor_test_check(mavlink_channel_t chan, bool check_rc, uint8_t motor_seq, uint8_t throttle_type, int16_t throttle_value);
    MAV_RESULT mavlink_motor_test_start(mavlink_channel_t chan, uint8_t motor_seq, uint8_t throttle_type, int16_t throttle_value, float timeout_sec);
    void motor_test_stop();
};

extern const AP_HAL::HAL& hal;
extern Rover rover;

using AP_HAL::millis;
using AP_HAL::micros; }}

      # Upload SARIF file as an Artifact to download and view
      # - name: Upload SARIF as an Artifact
      #   uses: actions/upload-artifact@v3
      #   with:
      #     name: sarif-file
      #     path: ${{ steps.run-analysis.outputs.sarif }}
