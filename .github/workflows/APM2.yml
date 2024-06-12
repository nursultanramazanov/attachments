# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
#
# Find more information at:
# https://github.com/microsoft/msvc-code-analysis-action

name: Microsoft C++ Code Analysis

on: #include "Rover.h"

/*
    work out if we are going to use pivot steering
*/
bool Rover::use_pivot_steering(float yaw_error_cd)
{
    // check cases where we clearly cannot use pivot steering
    if (!g2.motors.have_skid_steering() || g.pivot_turn_angle <= 0) {
        pivot_steering_active = false;
        return false;
    }

    // calc bearing error
    const float yaw_error = fabsf(yaw_error_cd) * 0.01f;

    // if error is larger than pivot_turn_angle start pivot steering
    if (yaw_error > g.pivot_turn_angle) {
        pivot_steering_active = true;
        return true;
    }

    // if within 10 degrees of the target heading, exit pivot steering
    if (yaw_error < 10.0f) {
        pivot_steering_active = false;
        return false;
    }

    // by default stay in
    return pivot_steering_active;
}

/*****************************************
    Set the flight control servos based on the current calculated values
*****************************************/
void Rover::set_servos(void)
{
    // send output signals to motors
    if (motor_test) {
        motor_test_output();
    } else {
        g2.motors.output(arming.is_armed() && hal.util->get_soft_armed(), G_Dt);
    }
}
  push: /*
  Rover specific AP_AdvancedFailsafe class
 */

#include "Rover.h"

#if ADVANCED_FAILSAFE == ENABLED

// Constructor
AP_AdvancedFailsafe_Rover::AP_AdvancedFailsafe_Rover(AP_Mission &_mission, const AP_GPS &_gps, const RCMapper &_rcmap) :
    AP_AdvancedFailsafe(_mission, _gps, _rcmap)
{}


/*
  Setup radio_out values for all channels to termination values
 */
void AP_AdvancedFailsafe_Rover::terminate_vehicle(void)
{
    // disarm as well
    rover.disarm_motors();

    // Set to HOLD mode
    rover.set_mode(rover.mode_hold, MODE_REASON_CRASH_FAILSAFE);
}

/*
  Return an AFS_MODE for current control mode
 */
AP_AdvancedFailsafe::control_mode AP_AdvancedFailsafe_Rover::afs_mode(void)
{
    if (rover.control_mode->is_autopilot_mode()) {
        return AP_AdvancedFailsafe::AFS_AUTO;
    }
    return AP_AdvancedFailsafe::AFS_STABILIZED;
}

#endif  // ADVANCED_FAILSAFE
    branches: [ "main" ]
  pull_request: /*
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
#pragma once

/*
  advanced failsafe support for rover
 */

#if ADVANCED_FAILSAFE == ENABLED
#include <AP_AdvancedFailsafe/AP_AdvancedFailsafe.h>

/*
  a rover specific AP_AdvancedFailsafe class
 */
class AP_AdvancedFailsafe_Rover : public AP_AdvancedFailsafe
{
public:
    AP_AdvancedFailsafe_Rover(AP_Mission &_mission, const AP_GPS &_gps, const RCMapper &_rcmap);

    // called to set all outputs to termination state
    void terminate_vehicle(void) override;

protected:
    // setup failsafe values - this is handled by motors library
    void setup_IO_failsafe(void) override {}

    // return the AFS mapped control mode
    enum control_mode afs_mode(void) override;
};

#endif  // ADVANCED_FAILSAFE
    branches: [ "main" ]
  schedule: #include "Rover.h"

void Rover::init_capabilities(void)
{
    hal.util->set_capabilities(MAV_PROTOCOL_CAPABILITY_MISSION_FLOAT |
                               MAV_PROTOCOL_CAPABILITY_PARAM_FLOAT |
                               MAV_PROTOCOL_CAPABILITY_MISSION_INT |
                               MAV_PROTOCOL_CAPABILITY_COMMAND_INT |
                               MAV_PROTOCOL_CAPABILITY_SET_POSITION_TARGET_LOCAL_NED |
                               MAV_PROTOCOL_CAPABILITY_SET_POSITION_TARGET_GLOBAL_INT |
                               MAV_PROTOCOL_CAPABILITY_COMPASS_CALIBRATION);
}
    - cron: '20 0 * * 1'

env: #include "Rover.h"

// checks if we should update ahrs home position from the EKF's position
void Rover::update_home_from_EKF()
{
    // exit immediately if home already set
    if (ahrs.home_is_set()) {
        return;
    }

    // move home to current ekf location (this will set home_state to HOME_SET)
    set_home_to_current_location(false);
}

// set ahrs home to current location from EKF reported location or GPS
bool Rover::set_home_to_current_location(bool lock)
{
    // use position from EKF if available otherwise use GPS
    Location temp_loc;
    if (ahrs.have_inertial_nav() && ahrs.get_position(temp_loc)) {
        if (!set_home(temp_loc, lock)) {
            return false;
        }
        // we have successfully set AHRS home, set it for SmartRTL
        g2.smart_rtl.set_home(true);
        return true;
    }
    return false;
}

// sets ahrs home to specified location
//  returns true if home location set successfully
bool Rover::set_home(const Location& loc, bool lock)
{
    // check location is valid
    if (loc.lat == 0 && loc.lng == 0 && loc.alt == 0) {
        return false;
    }
    if (!check_latlng(loc)) {
        return false;
    }

    // check if EKF origin has been set
    Location ekf_origin;
    if (!ahrs.get_origin(ekf_origin)) {
        return false;
    }

    // set ahrs home
    ahrs.set_home(loc);

    // init compass declination
    if (!ahrs.home_is_set()) {
        // record home is set
        ahrs.set_home_status(HOME_SET_NOT_LOCKED);

        // log new home position which mission library will pull from ahrs
        if (should_log(MASK_LOG_CMD)) {
            AP_Mission::Mission_Command temp_cmd;
            if (mission.read_cmd_from_storage(0, temp_cmd)) {
                DataFlash.Log_Write_Mission_Cmd(mission, temp_cmd);
            }
        }
    }

    // lock home position
    if (lock) {
        ahrs.set_home_status(HOME_SET_AND_LOCKED);
    }

    // Save Home to EEPROM
    mission.write_home_to_storage();

    // log ahrs home and ekf origin dataflash
    Log_Write_Home_And_Origin();

    // send new home and ekf origin to GCS
    gcs().send_home(loc);
    gcs().send_ekf_origin(loc);

    // send text of home position to ground stations
    gcs().send_text(MAV_SEVERITY_INFO, "Set HOME to %.6f %.6f at %.2fm",
            static_cast<double>(loc.lat * 1.0e-7f),
            static_cast<double>(loc.lng * 1.0e-7f),
            static_cast<double>(loc.alt * 0.01f));

    // return success
    return true;
}

// sets ekf_origin if it has not been set.
//  should only be used when there is no GPS to provide an absolute position
void Rover::set_ekf_origin(const Location& loc)
{
    // check location is valid
    if (!check_latlng(loc)) {
        return;
    }

    // check if EKF origin has already been set
    Location ekf_origin;
    if (ahrs.get_origin(ekf_origin)) {
        return;
    }

    if (!ahrs.set_origin(loc)) {
        return;
    }

    // log ahrs home and ekf origin dataflash
    Log_Write_Home_And_Origin();

    // send ekf origin to GCS
    gcs().send_ekf_origin(loc);
}

// checks if we should update ahrs/RTL home position from GPS
void Rover::set_system_time_from_GPS()
{
    // exit immediately if system time already set
    if (system_time_set) {
        return;
    }

    // if we have a 3d lock and valid location
    if (gps.status() >= AP_GPS::GPS_OK_FIX_3D) {
        // set system clock for log timestamps
        const uint64_t gps_timestamp = gps.time_epoch_usec();

        hal.util->set_system_clock(gps_timestamp);

        // update signing timestamp
        GCS_MAVLINK::update_signing_timestamp(gps_timestamp);

        system_time_set = true;
    }
}

/*
  update home location from GPS
  this is called as long as we have 3D lock and the arming switch is
  not pushed
*/
void Rover::update_home()
{
    if (ahrs.home_status() == HOME_SET_NOT_LOCKED) {
        Location loc;
        if (ahrs.get_position(loc)) {
            if (get_distance(loc, ahrs.get_home()) > DISTANCE_HOME_MAX) {
                ahrs.set_home(loc);
                Log_Write_Home_And_Origin();
                gcs().send_home(gps.location());
            }
        }
    }
    barometer.update_calibration();
}
  # Path to the CMake build directory.
  build: '${{ #include "Rover.h"

// update mission including starting or stopping commands. called by scheduler at 10Hz
void Rover::update_mission(void)
{
    if (control_mode == &mode_auto) {
        if (ahrs.home_is_set() && mission.num_commands() > 1) {
            mission.update();
        }
    }
}

/********************************************************************************/
// Command Event Handlers
/********************************************************************************/
bool Rover::start_command(const AP_Mission::Mission_Command& cmd)
{
    // log when new commands start
    if (should_log(MASK_LOG_CMD)) {
        DataFlash.Log_Write_Mission_Cmd(mission, cmd);
    }

    gcs().send_text(MAV_SEVERITY_INFO, "Executing %s(ID=%i)",
                    cmd.type(), cmd.id);

    switch (cmd.id) {
    case MAV_CMD_NAV_WAYPOINT:  // Navigate to Waypoint
        do_nav_wp(cmd, false);
        break;

    case MAV_CMD_NAV_RETURN_TO_LAUNCH:
        do_RTL();
        break;

    case MAV_CMD_NAV_LOITER_UNLIM:  // Loiter indefinitely
    case MAV_CMD_NAV_LOITER_TIME:   // Loiter for specified time
        do_nav_wp(cmd, true);
        break;

    case MAV_CMD_NAV_SET_YAW_SPEED:
        do_nav_set_yaw_speed(cmd);
        break;

    // Conditional commands
    case MAV_CMD_CONDITION_DELAY:
        do_wait_delay(cmd);
        break;

    case MAV_CMD_CONDITION_DISTANCE:
        do_within_distance(cmd);
        break;

    // Do commands
    case MAV_CMD_DO_CHANGE_SPEED:
        do_change_speed(cmd);
        break;

    case MAV_CMD_DO_SET_HOME:
        do_set_home(cmd);
        break;

    case MAV_CMD_DO_SET_SERVO:
        ServoRelayEvents.do_set_servo(cmd.content.servo.channel, cmd.content.servo.pwm);
        break;

    case MAV_CMD_DO_SET_RELAY:
        ServoRelayEvents.do_set_relay(cmd.content.relay.num, cmd.content.relay.state);
        break;

    case MAV_CMD_DO_REPEAT_SERVO:
        ServoRelayEvents.do_repeat_servo(cmd.content.repeat_servo.channel, cmd.content.repeat_servo.pwm,
                                         cmd.content.repeat_servo.repeat_count, cmd.content.repeat_servo.cycle_time * 1000.0f);
        break;

    case MAV_CMD_DO_REPEAT_RELAY:
        ServoRelayEvents.do_repeat_relay(cmd.content.repeat_relay.num, cmd.content.repeat_relay.repeat_count,
                                         cmd.content.repeat_relay.cycle_time * 1000.0f);
        break;

#if CAMERA == ENABLED
    case MAV_CMD_DO_CONTROL_VIDEO:                      // Control on-board camera capturing. |Camera ID (-1 for all)| Transmission: 0: disabled, 1: enabled compressed, 2: enabled raw| Transmission mode: 0: video stream, >0: single images every n seconds (decimal)| Recording: 0: disabled, 1: enabled compressed, 2: enabled raw| Empty| Empty| Empty|
        break;

    case MAV_CMD_DO_DIGICAM_CONFIGURE:                  // Mission command to configure an on-board camera controller system. |Modes: P, TV, AV, M, Etc| Shutter speed: Divisor number for one second| Aperture: F stop number| ISO number e.g. 80, 100, 200, Etc| Exposure type enumerator| Command Identity| Main engine cut-off time before camera trigger in seconds/10 (0 means no cut-off)|
        do_digicam_configure(cmd);
        break;

    case MAV_CMD_DO_DIGICAM_CONTROL:                    // Mission command to control an on-board camera controller system. |Session control e.g. show/hide lens| Zoom's absolute position| Zooming step value to offset zoom from the current position| Focus Locking, Unlocking or Re-locking| Shooting Command| Command Identity| Empty|
        do_digicam_control(cmd);
        break;

    case MAV_CMD_DO_SET_CAM_TRIGG_DIST:
        camera.set_trigger_distance(cmd.content.cam_trigg_dist.meters);
        break;
#endif

#if MOUNT == ENABLED
    // Sets the region of interest (ROI) for a sensor set or the
    // vehicle itself. This can then be used by the vehicles control
    // system to control the vehicle attitude and the attitude of various
    // devices such as cameras.
    //    |Region of interest mode. (see MAV_ROI enum)| Waypoint index/ target ID. (see MAV_ROI enum)| ROI index (allows a vehicle to manage multiple cameras etc.)| Empty| x the location of the fixed ROI (see MAV_FRAME)| y| z|
    case MAV_CMD_DO_SET_ROI:
        if (cmd.content.location.alt == 0 && cmd.content.location.lat == 0 && cmd.content.location.lng == 0) {
            // switch off the camera tracking if enabled
            if (camera_mount.get_mode() == MAV_MOUNT_MODE_GPS_POINT) {
                camera_mount.set_mode_to_default();
            }
        } else {
            // send the command to the camera mount
            camera_mount.set_roi_target(cmd.content.location);
        }
        break;
#endif

    case MAV_CMD_DO_SET_REVERSE:
        do_set_reverse(cmd);
        break;

    case MAV_CMD_DO_FENCE_ENABLE:
        if (cmd.p1 == 0) {  //disable
            g2.fence.enable(false);
            gcs().send_text(MAV_SEVERITY_INFO, "Fence Disabled");
        } else {  //enable fence
            g2.fence.enable(true);
            gcs().send_text(MAV_SEVERITY_INFO, "Fence Enabled");
        }
        break;

    default:
        // return false for unhandled commands
        return false;
    }

    // if we got this far we must have been successful
    return true;
}

// exit_mission - callback function called from ap-mission when the mission has completed
void Rover::exit_mission()
{
    gcs().send_text(MAV_SEVERITY_NOTICE, "Mission Complete");
    set_mode(mode_hold, MODE_REASON_MISSION_END);
}

// verify_command_callback - callback function called from ap-mission at 10hz or higher when a command is being run
//      we double check that the flight mode is AUTO to avoid the possibility of ap-mission triggering actions while we're not in AUTO mode
bool Rover::verify_command_callback(const AP_Mission::Mission_Command& cmd)
{
    const bool cmd_complete = verify_command(cmd);

    // send message to GCS
    if (cmd_complete) {
        gcs().send_mission_item_reached_message(cmd.index);
    }

    return cmd_complete;
}

/*******************************************************************************
Verify command Handlers

Each type of mission element has a "verify" operation. The verify
operation returns true when the mission element has completed and we
should move onto the next mission element.
Return true if we do not recognize the command so that we move on to the next command
*******************************************************************************/

bool Rover::verify_command(const AP_Mission::Mission_Command& cmd)
{
    switch (cmd.id) {
    case MAV_CMD_NAV_WAYPOINT:
        return verify_nav_wp(cmd);

    case MAV_CMD_NAV_RETURN_TO_LAUNCH:
        return verify_RTL();

    case MAV_CMD_NAV_LOITER_UNLIM:
        return verify_loiter_unlimited(cmd);

    case MAV_CMD_NAV_LOITER_TIME:
        return verify_loiter_time(cmd);

    case MAV_CMD_CONDITION_DELAY:
        return verify_wait_delay();

    case MAV_CMD_CONDITION_DISTANCE:
        return verify_within_distance();

    case MAV_CMD_NAV_SET_YAW_SPEED:
        return verify_nav_set_yaw_speed();

    // do commands (always return true)
    case MAV_CMD_DO_CHANGE_SPEED:
    case MAV_CMD_DO_SET_HOME:
    case MAV_CMD_DO_SET_SERVO:
    case MAV_CMD_DO_SET_RELAY:
    case MAV_CMD_DO_REPEAT_SERVO:
    case MAV_CMD_DO_REPEAT_RELAY:
    case MAV_CMD_DO_CONTROL_VIDEO:
    case MAV_CMD_DO_DIGICAM_CONFIGURE:
    case MAV_CMD_DO_DIGICAM_CONTROL:
    case MAV_CMD_DO_SET_CAM_TRIGG_DIST:
    case MAV_CMD_DO_SET_ROI:
    case MAV_CMD_DO_SET_REVERSE:
    case MAV_CMD_DO_FENCE_ENABLE:
        return true;

    default:
        // error message
        gcs().send_text(MAV_SEVERITY_WARNING, "Skipping invalid cmd #%i", cmd.id);
        // return true if we do not recognize the command so that we move on to the next command
        return true;
    }
}

/********************************************************************************/
//  Nav (Must) commands
/********************************************************************************/

void Rover::do_RTL(void)
{
    // start rtl in auto mode
    mode_auto.start_RTL();
}

void Rover::do_nav_wp(const AP_Mission::Mission_Command& cmd, bool always_stop_at_destination)
{
    // just starting so we haven't previously reached the waypoint
    previously_reached_wp = false;

    // this will be used to remember the time in millis after we reach or pass the WP.
    loiter_start_time = 0;

    // this is the delay, stored in seconds
    loiter_duration = cmd.p1;

    // get heading to following waypoint (auto mode reduces speed to allow corning without large overshoot)
    // in case of non-zero loiter duration, we provide heading-unknown to signal we should stop at the point
    float next_leg_bearing_cd = MODE_NEXT_HEADING_UNKNOWN;
    if (!always_stop_at_destination && loiter_duration == 0) {
        next_leg_bearing_cd = mission.get_next_ground_course_cd(MODE_NEXT_HEADING_UNKNOWN);
    }

    // retrieve and sanitize target location
    Location cmdloc = cmd.content.location;
    location_sanitize(current_loc, cmdloc);
    mode_auto.set_desired_location(cmdloc, next_leg_bearing_cd);
}

// do_set_yaw_speed - turn to a specified heading and achieve and given speed
void Rover::do_nav_set_yaw_speed(const AP_Mission::Mission_Command& cmd)
{
    float desired_heading_cd;

    // get final angle, 1 = Relative, 0 = Absolute
    if (cmd.content.set_yaw_speed.relative_angle > 0) {
        // relative angle
        desired_heading_cd = wrap_180_cd(ahrs.yaw_sensor + cmd.content.set_yaw_speed.angle_deg * 100.0f);
    } else {
        // absolute angle
        desired_heading_cd = cmd.content.set_yaw_speed.angle_deg * 100.0f;
    }

    // set auto target
    const float speed_max = control_mode->get_speed_default();
    mode_auto.set_desired_heading_and_speed(desired_heading_cd, constrain_float(cmd.content.set_yaw_speed.speed, -speed_max, speed_max));
}

/********************************************************************************/
//  Verify Nav (Must) commands
/********************************************************************************/
bool Rover::verify_nav_wp(const AP_Mission::Mission_Command& cmd)
{
    // exit immediately if we haven't reached the destination
    if (!mode_auto.reached_destination()) {
        return false;
    }

    // Check if this is the first time we have noticed reaching the waypoint
    if (!previously_reached_wp) {
        previously_reached_wp = true;

        // check if we are loitering at this waypoint - the message sent to the GCS is different
        if (loiter_duration > 0) {
            // send message including loiter time
            gcs().send_text(MAV_SEVERITY_INFO, "Reached waypoint #%u. Loiter for %u seconds",
                    static_cast<uint32_t>(cmd.index),
                    static_cast<uint32_t>(loiter_duration));
            // record the current time i.e. start timer
            loiter_start_time = millis();
        } else {
            // send simpler message to GCS
            gcs().send_text(MAV_SEVERITY_INFO, "Reached waypoint #%u", static_cast<uint32_t>(cmd.index));
        }
    }

    // Check if we have loitered long enough
    if (loiter_duration == 0) {
        return true;
    } else {
        return (((millis() - loiter_start_time) / 1000) >= loiter_duration);
    }
}

bool Rover::verify_RTL()
{
    return mode_auto.reached_destination();
}

bool Rover::verify_loiter_unlimited(const AP_Mission::Mission_Command& cmd)
{
    verify_nav_wp(cmd);
    return false;
}

// verify_loiter_time - check if we have loitered long enough
bool Rover::verify_loiter_time(const AP_Mission::Mission_Command& cmd)
{
    const bool result = verify_nav_wp(cmd);
    if (result) {
        gcs().send_text(MAV_SEVERITY_WARNING, "Finished active loiter");
    }
    return result;
}

// verify_yaw - return true if we have reached the desired heading
bool Rover::verify_nav_set_yaw_speed()
{
    return mode_auto.reached_heading();
}

/********************************************************************************/
//  Condition (May) commands
/********************************************************************************/

void Rover::do_wait_delay(const AP_Mission::Mission_Command& cmd)
{
    condition_start = millis();
    condition_value = static_cast<int32_t>(cmd.content.delay.seconds * 1000);  // convert seconds to milliseconds
}

void Rover::do_within_distance(const AP_Mission::Mission_Command& cmd)
{
    condition_value = cmd.content.distance.meters;
}

/********************************************************************************/
// Verify Condition (May) commands
/********************************************************************************/

bool Rover::verify_wait_delay()
{
    if (static_cast<uint32_t>(millis() - condition_start) > static_cast<uint32_t>(condition_value)) {
        condition_value = 0;
        return true;
    }
    return false;
}

bool Rover::verify_within_distance()
{
    if (mode_auto.get_distance_to_destination() < condition_value) {
        condition_value = 0;
        return true;
    }
    return false;
}


/********************************************************************************/
//  Do (Now) commands
/********************************************************************************/

void Rover::do_change_speed(const AP_Mission::Mission_Command& cmd)
{
    // set speed for active mode
    if ((cmd.content.speed.target_ms >= 0.0f) && (cmd.content.speed.target_ms <= rover.control_mode->get_speed_default())) {
        control_mode->set_desired_speed(cmd.content.speed.target_ms);
        gcs().send_text(MAV_SEVERITY_INFO, "speed: %.1f m/s", static_cast<double>(cmd.content.speed.target_ms));
    }
}

void Rover::do_set_home(const AP_Mission::Mission_Command& cmd)
{
    if (cmd.p1 == 1 && have_position) {
        set_home_to_current_location(false);
    } else {
        set_home(cmd.content.location, false);
    }
}

#if CAMERA == ENABLED

// do_digicam_configure Send Digicam Configure message with the camera library
void Rover::do_digicam_configure(const AP_Mission::Mission_Command& cmd)
{
    camera.configure(cmd.content.digicam_configure.shooting_mode,
                     cmd.content.digicam_configure.shutter_speed,
                     cmd.content.digicam_configure.aperture,
                     cmd.content.digicam_configure.ISO,
                     cmd.content.digicam_configure.exposure_type,
                     cmd.content.digicam_configure.cmd_id,
                     cmd.content.digicam_configure.engine_cutoff_time);
}

// do_digicam_control Send Digicam Control message with the camera library
void Rover::do_digicam_control(const AP_Mission::Mission_Command& cmd)
{
    camera.control(cmd.content.digicam_control.session,
                   cmd.content.digicam_control.zoom_pos,
                   cmd.content.digicam_control.zoom_step,
                   cmd.content.digicam_control.focus_lock,
                   cmd.content.digicam_control.shooting_cmd,
                   cmd.content.digicam_control.cmd_id);
}

#endif

void Rover::do_set_reverse(const AP_Mission::Mission_Command& cmd)
{
    mode_auto.set_reversed(cmd.p1 == 1);
    set_reverse(cmd.p1 == 1);
} }}/build'

permissions: #include "Rover.h"

void Rover::delay(uint32_t ms)
{
    hal.scheduler->delay(ms);
}
  contents: read

jobs: //
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
//
//  DO NOT EDIT this file to adjust your configuration.  Create your own
//  APM_Config.h and use APM_Config.h.example as a reference.
//
// WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING
///
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
// Default and automatic configuration details.
//
// Notes for maintainers:
//
// - Try to keep this file organised in the same order as APM_Config.h.example
//
#pragma once

#include "defines.h"

///
/// DO NOT EDIT THIS INCLUDE - if you want to make a local change, make that
/// change in your local copy of APM_Config.h.
///
#include "APM_Config.h"  // <== THIS INCLUDE, DO NOT EDIT IT. EVER.
///
/// DO NOT EDIT THIS INCLUDE - if you want to make a local change, make that
/// change in your local copy of APM_Config.h.
///

//////////////////////////////////////////////////////////////////////////////
// sensor types

//////////////////////////////////////////////////////////////////////////////
// HIL_MODE                                 OPTIONAL

#ifndef HIL_MODE
  #define HIL_MODE HIL_MODE_DISABLED
#endif

#if CONFIG_HAL_BOARD == HAL_BOARD_SITL
  #define BATTERY_PIN_1     1
  #define CURRENT_PIN_1     2
#elif CONFIG_HAL_BOARD == HAL_BOARD_PX4
  #define BATTERY_PIN_1    -1
  #define CURRENT_PIN_1    -1
#elif CONFIG_HAL_BOARD == HAL_BOARD_LINUX
  #define BATTERY_PIN_1    -1
  #define CURRENT_PIN_1    -1
#elif CONFIG_HAL_BOARD == HAL_BOARD_VRBRAIN
  #define BATTERY_PIN_1    -1
  #define CURRENT_PIN_1    -1
#endif

#ifndef MAV_SYSTEM_ID
  #define MAV_SYSTEM_ID    1
#endif

#ifndef ARM_DELAY_MS
  #define ARM_DELAY_MS  2000
#endif

//////////////////////////////////////////////////////////////////////////////
// FrSky telemetry support
//

#ifndef FRSKY_TELEM_ENABLED
  #define FRSKY_TELEM_ENABLED ENABLED
#endif


#ifndef CH7_OPTION
  #define CH7_OPTION CH7_SAVE_WP
#endif

//////////////////////////////////////////////////////////////////////////////
//  MAGNETOMETER
#ifndef MAGNETOMETER
  #define MAGNETOMETER ENABLED
#endif

//////////////////////////////////////////////////////////////////////////////
// MODE
// MODE_CHANNEL
//
#ifndef MODE_CHANNEL
  #define MODE_CHANNEL    8
#endif
#if (MODE_CHANNEL != 5) && (MODE_CHANNEL != 6) && (MODE_CHANNEL != 7) && (MODE_CHANNEL != 8)
  #error XXX
  #error XXX You must set MODE_CHANNEL to 5, 6, 7 or 8
  #error XXX
#endif


//////////////////////////////////////////////////////////////////////////////
//  VISUAL ODOMETRY
#ifndef VISUAL_ODOMETRY_ENABLED
# define VISUAL_ODOMETRY_ENABLED ENABLED
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
// STARTUP BEHAVIOUR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
// GROUND_START_DELAY
//
#ifndef GROUND_START_DELAY
  #define GROUND_START_DELAY    0
#endif

//////////////////////////////////////////////////////////////////////////////
// MOUNT (ANTENNA OR CAMERA)
//
#ifndef MOUNT
  #define MOUNT ENABLED
#endif

//////////////////////////////////////////////////////////////////////////////
// CAMERA control
//
#ifndef CAMERA
  #define CAMERA ENABLED
#endif

//////////////////////////////////////////////////////////////////////////////
// NAVL1
//
#ifndef NAVL1
  #define NAVL1_PERIOD    8
#endif

//////////////////////////////////////////////////////////////////////////////
// CRUISE_SPEED default
//
#ifndef CRUISE_SPEED
  #define CRUISE_SPEED    2  // in m/s
#endif

#ifndef TURN_GAIN
  #define TURN_GAIN       5
#endif

//////////////////////////////////////////////////////////////////////////////
// Dataflash logging control
//
#ifndef LOGGING_ENABLED
  #define LOGGING_ENABLED ENABLED
#endif

#define DEFAULT_LOG_BITMASK    0xffff


//////////////////////////////////////////////////////////////////////////////
// Developer Items
//

// if RESET_SWITCH_CH is not zero, then this is the PWM value on
// that channel where we reset the control mode to the current switch
// position (to for example return to switched mode after failsafe or
// fence breach)
#ifndef RESET_SWITCH_CHAN_PWM
  #define RESET_SWITCH_CHAN_PWM    1750
#endif

#ifndef ADVANCED_FAILSAFE
  #define ADVANCED_FAILSAFE DISABLED
#endif

#ifndef STATS_ENABLED
 # define STATS_ENABLED ENABLED
#endif

#ifndef DEVO_TELEM_ENABLED
#if HAL_MINIMIZE_FEATURES
 #define DEVO_TELEM_ENABLED DISABLED
#else
 #define DEVO_TELEM_ENABLED ENABLED
#endif
#endif
  analyze: #include "Rover.h"

static const int16_t CH_7_PWM_TRIGGER = 1800;

Mode *Rover::mode_from_mode_num(const enum mode num)
{
    Mode *ret = nullptr;
    switch (num) {
    case MANUAL:
        ret = &mode_manual;
        break;
    case ACRO:
        ret = &mode_acro;
        break;
    case STEERING:
        ret = &mode_steering;
        break;
    case HOLD:
        ret = &mode_hold;
        break;
    case AUTO:
        ret = &mode_auto;
        break;
    case RTL:
        ret = &mode_rtl;
        break;
    case SMART_RTL:
        ret = &mode_smartrtl;
        break;
    case GUIDED:
       ret = &mode_guided;
        break;
    case INITIALISING:
        ret = &mode_initializing;
        break;
    default:
        break;
    }
    return ret;
}

void Rover::read_control_switch()
{
    static bool switch_debouncer;
    const uint8_t switchPosition = readSwitch();

    // If switchPosition = 255 this indicates that the mode control channel input was out of range
    // If we get this value we do not want to change modes.
    if (switchPosition == 255) {
        return;
    }

    if (AP_HAL::millis() - failsafe.last_valid_rc_ms > 100) {
        // only use signals that are less than 0.1s old.
        return;
    }

    // we look for changes in the switch position. If the
    // RST_SWITCH_CH parameter is set, then it is a switch that can be
    // used to force re-reading of the control switch. This is useful
    // when returning to the previous mode after a failsafe or fence
    // breach. This channel is best used on a momentary switch (such
    // as a spring loaded trainer switch).
    if (oldSwitchPosition != switchPosition ||
        (g.reset_switch_chan != 0 &&
         RC_Channels::get_radio_in(g.reset_switch_chan-1) > RESET_SWITCH_CHAN_PWM)) {
        if (switch_debouncer == false) {
            // this ensures that mode switches only happen if the
            // switch changes for 2 reads. This prevents momentary
            // spikes in the mode control channel from causing a mode
            // switch
            switch_debouncer = true;
            return;
        }

        Mode *new_mode = mode_from_mode_num((enum mode)modes[switchPosition].get());
        if (new_mode != nullptr) {
            set_mode(*new_mode, MODE_REASON_TX_COMMAND);
        }

        oldSwitchPosition = switchPosition;
    }

    switch_debouncer = false;
}

uint8_t Rover::readSwitch(void) {
    const uint16_t pulsewidth = RC_Channels::get_radio_in(g.mode_channel - 1);
    if (pulsewidth <= 900 || pulsewidth >= 2200) {
        return 255;  // This is an error condition
    }
    if (pulsewidth <= 1230) {
        return 0;
    }
    if (pulsewidth <= 1360) {
        return 1;
    }
    if (pulsewidth <= 1490) {
        return 2;
    }
    if (pulsewidth <= 1620) {
        return 3;
    }
    if (pulsewidth <= 1749) {
        return 4;  // Software Manual
    }
    return 5;  // Hardware Manual
}

void Rover::reset_control_switch()
{
    oldSwitchPosition = 254;
    read_control_switch();
}

// ready auxiliary switch's position
aux_switch_pos Rover::read_aux_switch_pos()
{
    const uint16_t radio_in = channel_aux->get_radio_in();
    if (radio_in < AUX_SWITCH_PWM_TRIGGER_LOW) return AUX_SWITCH_LOW;
    if (radio_in > AUX_SWITCH_PWM_TRIGGER_HIGH) return AUX_SWITCH_HIGH;
    return AUX_SWITCH_MIDDLE;
}

// initialise position of auxiliary switch
void Rover::init_aux_switch()
{
    aux_ch7 = read_aux_switch_pos();
}

// read ch7 aux switch
void Rover::read_aux_switch()
{
    // do not consume input during rc or throttle failsafe
    if ((failsafe.bits & FAILSAFE_EVENT_THROTTLE) || (failsafe.bits & FAILSAFE_EVENT_RC)) {
        return;
    }

    // get ch7's current position
    aux_switch_pos aux_ch7_pos = read_aux_switch_pos();

    // return if no change to switch position
    if (aux_ch7_pos == aux_ch7) {
        return;
    }
    aux_ch7 = aux_ch7_pos;

    switch ((enum ch7_option)g.ch7_option.get()) {
    case CH7_DO_NOTHING:
        break;
    case CH7_SAVE_WP:
        if (aux_ch7 == AUX_SWITCH_HIGH) {
            // do nothing if in AUTO mode
            if (control_mode == &mode_auto) {
                return;
            }

            // if disarmed clear mission and set home to current location
            if (!arming.is_armed()) {
                mission.clear();
                set_home_to_current_location(false);
                return;
            }

            // record the waypoint if not in auto mode
            if (control_mode != &mode_auto) {
                // create new mission command
                AP_Mission::Mission_Command cmd = {};

                // set new waypoint to current location
                cmd.content.location = current_loc;

                // make the new command to a waypoint
                cmd.id = MAV_CMD_NAV_WAYPOINT;

                // save command
                if (mission.add_cmd(cmd)) {
                    hal.console->printf("Added waypoint %u", static_cast<uint32_t>(mission.num_commands()));
                }
            }
        }
        break;

    // learn cruise speed and throttle
    case CH7_LEARN_CRUISE:
        if (aux_ch7 == AUX_SWITCH_HIGH) {
            cruise_learn_start();
        } else if (aux_ch7 == AUX_SWITCH_LOW) {
            cruise_learn_complete();
        }
        break;

    // arm or disarm the motors
    case CH7_ARM_DISARM:
        if (aux_ch7 == AUX_SWITCH_HIGH) {
            arm_motors(AP_Arming::RUDDER);
        } else if (aux_ch7 == AUX_SWITCH_LOW) {
            disarm_motors();
        }
        break;

    // set mode to Manual
    case CH7_MANUAL:
        if (aux_ch7 == AUX_SWITCH_HIGH) {
            set_mode(mode_manual, MODE_REASON_TX_COMMAND);
        } else if ((aux_ch7 == AUX_SWITCH_LOW) && (control_mode == &mode_manual)) {
            reset_control_switch();
        }
        break;

    // set mode to Acro
    case CH7_ACRO:
        if (aux_ch7 == AUX_SWITCH_HIGH) {
            set_mode(mode_acro, MODE_REASON_TX_COMMAND);
        } else if ((aux_ch7 == AUX_SWITCH_LOW) && (control_mode == &mode_acro)) {
            reset_control_switch();
        }
        break;

    // set mode to Steering
    case CH7_STEERING:
        if (aux_ch7 == AUX_SWITCH_HIGH) {
            set_mode(mode_steering, MODE_REASON_TX_COMMAND);
        } else if ((aux_ch7 == AUX_SWITCH_LOW) && (control_mode == &mode_steering)) {
            reset_control_switch();
        }
        break;

    // set mode to Hold
    case CH7_HOLD:
        if (aux_ch7 == AUX_SWITCH_HIGH) {
            set_mode(mode_hold, MODE_REASON_TX_COMMAND);
        } else if ((aux_ch7 == AUX_SWITCH_LOW) && (control_mode == &mode_hold)) {
            reset_control_switch();
        }
        break;

    // set mode to Auto
    case CH7_AUTO:
        if (aux_ch7 == AUX_SWITCH_HIGH) {
            set_mode(mode_auto, MODE_REASON_TX_COMMAND);
        } else if ((aux_ch7 == AUX_SWITCH_LOW) && (control_mode == &mode_auto)) {
            reset_control_switch();
        }
        break;

    // set mode to RTL
    case CH7_RTL:
        if (aux_ch7 == AUX_SWITCH_HIGH) {
            set_mode(mode_rtl, MODE_REASON_TX_COMMAND);
        } else if ((aux_ch7 == AUX_SWITCH_LOW) && (control_mode == &mode_rtl)) {
            reset_control_switch();
        }
        break;

    // set mode to SmartRTL
    case CH7_SMART_RTL:
        if (aux_ch7 == AUX_SWITCH_HIGH) {
            set_mode(mode_smartrtl, MODE_REASON_TX_COMMAND);
        } else if ((aux_ch7 == AUX_SWITCH_LOW) && (control_mode == &mode_smartrtl)) {
            reset_control_switch();
        }
        break;

    // set mode to Guided
    case CH7_GUIDED:
        if (aux_ch7 == AUX_SWITCH_HIGH) {
            set_mode(mode_guided, MODE_REASON_TX_COMMAND);
        } else if ((aux_ch7 == AUX_SWITCH_LOW) && (control_mode == &mode_guided)) {
            reset_control_switch();
        }
        break;
    }
}

// return true if motors are moving
bool Rover::motor_active()
{
    // if soft disarmed, motors not active
    if (!hal.util->get_soft_armed()) {
        return false;
    }

    // check throttle is active
    if (!is_zero(g2.motors.get_throttle())) {
        return true;
    }

    // skid-steering vehicles active when steering
    if (g2.motors.have_skid_steering() && !is_zero(g2.motors.get_steering())) {
        return true;
    }

    return false;
}
    permissions: #include "Rover.h"

// Code to detect a crash or block
static const uint16_t CRASH_CHECK_TRIGGER_SEC = 2;   // 2 seconds blocked indicates a crash
static const float CRASH_CHECK_THROTTLE_MIN = 5.0f;  // vehicle must have a throttle greater that 5% to be considered crashed
static const float CRASH_CHECK_VEL_MIN = 0.08f;      // vehicle must have a velocity under 0.08 m/s or rad/s to be considered crashed

// crash_check - disarms motors if a crash or block has been detected
// crashes are detected by the vehicle being static (no speed) for more than CRASH_CHECK_TRIGGER_SEC and motor are running
// called at 10Hz
void Rover::crash_check()
{
  static uint16_t crash_counter;  // number of iterations vehicle may have been crashed

  // return immediately if disarmed, crash checking is disabled or vehicle is Hold, Manual or Acro mode
  if (!arming.is_armed() || g.fs_crash_check == FS_CRASH_DISABLE || (!control_mode->is_autopilot_mode())) {
    crash_counter = 0;
    return;
  }

  // TODO : Check if min vel can be calculated
  // min_vel = ( CRASH_CHECK_THROTTLE_MIN * g.speed_cruise) / g.throttle_cruise;

  if ((ahrs.groundspeed() >= CRASH_CHECK_VEL_MIN) ||        // Check velocity
      (fabsf(ahrs.get_gyro().z) >= CRASH_CHECK_VEL_MIN) ||  // Check turn speed
      (fabsf(g2.motors.get_throttle()) < CRASH_CHECK_THROTTLE_MIN)) {
    crash_counter = 0;
    return;
  }

  // we may be crashing
  crash_counter++;

  // check if crashing for 2 seconds
  if (crash_counter >= (CRASH_CHECK_TRIGGER_SEC * 10)) {
    // log an error in the dataflash
    Log_Write_Error(ERROR_SUBSYSTEM_CRASH_CHECK, ERROR_CODE_CRASH_CHECK_CRASH);
    // send message to gcs
    gcs().send_text(MAV_SEVERITY_EMERGENCY, "Crash: Going to HOLD");
    // change mode to hold and disarm
    set_mode(mode_hold, MODE_REASON_CRASH_FAILSAFE);
    if (g.fs_crash_check == FS_CRASH_HOLD_AND_DISARM) {
      disarm_motors();
    }
  }
}
      contents: read # for actions/checkout to fetch code
      security-events: write # for github/codeql-action/upload-sarif to upload SARIF results
      actions: read # only required for a private repository by github/codeql-action/upload-sarif to get the Action run status
    name: Analyze
    runs-on: windows-latest

    steps: #!/bin/bash
#" Autocompletion enabled vim for arduino pde's

ctags -RV --language-force=C++ --c++-kinds=+p --fields=+iaS --extra=+q  \
        . \
        ../libraries/*

# sample vimrc file
# you'll need to have omnicpp plugin for vim

#"set compatible

#" Vim5 and later versions support syntax highlighting. Uncommenting the next
#" line enables syntax highlighting by default.
#syntax on
#filetype on

#" If using a dark background within the editing area and syntax highlighting
#" turn on this option as well
#"set background=dark

#" Uncomment the following to have Vim jump to the last position when
#" reopening a file
#if has("autocmd")
  #au BufReadPost * if line("'\"") > 1 && line("'\"") <= line("$") | exe "normal! g'\"" | endif
#endif

#" Uncomment the following to have Vim load indentation rules and plugins
#" according to the detected filetype.
#if has("autocmd")
  #filetype plugin indent on
#endif

#" The following are commented out as they cause vim to behave a lot
#" differently from regular Vi. They are highly recommended though.
#set showcmd                " Show (partial) command in status line.
#set showmatch                " Show matching brackets.
#set ignorecase                " Do case insensitive matching
#set smartcase                " Do smart case matching
#set incsearch                " Incremental search
#set autowrite                " Automatically save before commands like :next and :make
#set hidden             " Hide buffers when they are abandoned
#set mouse=a                " Enable mouse usage (all modes)
#set vb

#" build tags of your own project with Ctrl-F12
#map <C-F12> :!ctags -R --sort=yes --c++-kinds=+p --fields=+iaS --extra=+q .<CR>

#" OmniCppComplete
#let OmniCpp_NamespaceSearch = 1
#let OmniCpp_GlobalScopeSearch = 1
#let OmniCpp_ShowAccess = 1
#let OmniCpp_ShowPrototypeInAbbr = 1 " show function parameters
#let OmniCpp_MayCompleteDot = 1 " autocomplete after .
#let OmniCpp_MayCompleteArrow = 1 " autocomplete after ->
#let OmniCpp_MayCompleteScope = 1 " autocomplete after ::
#let OmniCpp_DefaultNamespaces = ["std", "_GLIBCXX_STD"]

#" automatically open and close the popup menu / preview window
#"au CursorMovedI,InsertLeave * if pumvisible() == 0|silent! pclose|endif
#"set completeopt=menuone,menu,longest,preview
#set ts=4
#set sw=4
      - name: Checkout repository
        uses: actions/checkout@v4

      - name: Configure CMake
        run: cmake -B ${{ #include "Rover.h"

// start cruise throttle and speed learning
void Rover::cruise_learn_start()
{
    // if disarmed or no speed available do nothing
    float speed;
    if (!arming.is_armed() || !g2.attitude_control.get_forward_speed(speed)) {
        cruise_learn.learning = false;
        gcs().send_text(MAV_SEVERITY_CRITICAL, "Cruise Learning NOT started");
        return;
    }
    // start learning
    cruise_learn.learning = true;
    cruise_learn.speed_filt.reset(speed);
    cruise_learn.throttle_filt.reset(g2.motors.get_throttle());
    gcs().send_text(MAV_SEVERITY_CRITICAL, "Cruise Learning started");
    // To-Do: add dataflash logging of learning started event
}

// update cruise learning with latest speed and throttle
// should be called at 50hz
void Rover::cruise_learn_update()
{
    float speed;
    if (cruise_learn.learning && g2.attitude_control.get_forward_speed(speed)) {
        // update filters with latest speed and throttle
        cruise_learn.speed_filt.apply(speed, 0.02f);
        cruise_learn.throttle_filt.apply(g2.motors.get_throttle(), 0.02f);
        return;
    }
}

// complete cruise learning and save results
void Rover::cruise_learn_complete()
{
    // when switch is moved low, save learned cruise value
    if (cruise_learn.learning) {
        const float thr = cruise_learn.throttle_filt.get();
        const float speed = cruise_learn.speed_filt.get();
        if (thr >= 10.0f && thr <= 100.0f && is_positive(speed)) {
            g.throttle_cruise.set_and_save(thr);
            g.speed_cruise.set_and_save(speed);
            gcs().send_text(MAV_SEVERITY_CRITICAL, "Cruise Learned: Thr:%d Speed:%3.1f", (int)g.throttle_cruise, (double)g.speed_cruise);
        } else {
            gcs().send_text(MAV_SEVERITY_CRITICAL, "Cruise Learning failed");
        }
        cruise_learn.learning = false;
        // To-Do: add dataflash logging of learning completion event
    }
} }}

      # Build is not required unless generated source files are used
      # - name: Build CMake
      #   run: cmake --build ${{ env.build }}

      - name: Initialize MSVC Code Analysis
        uses: microsoft/msvc-code-analysis-action@04825f6d9e00f87422d6bf04e1a38b1f3ed60d99
        # Provide a unique ID to access the sarif output path
        id: run-analysis
        with: #pragma once

// Internal defines, don't edit and expect things to work
// -------------------------------------------------------

// Just so that it's completely clear...
#define ENABLED  1
#define DISABLED 0

// this avoids a very common config error
#define ENABLE ENABLED
#define DISABLE DISABLED

#define SERVO_MAX 4500  // This value represents 45 degrees and is just an arbitrary representation of servo max travel.

// CH 7 control
enum ch7_option {
    CH7_DO_NOTHING      = 0,
    CH7_SAVE_WP         = 1,
    CH7_LEARN_CRUISE    = 2,
    CH7_ARM_DISARM      = 3,
    CH7_MANUAL          = 4,
    CH7_ACRO            = 5,
    CH7_STEERING        = 6,
    CH7_HOLD            = 7,
    CH7_AUTO            = 8,
    CH7_RTL             = 9,
    CH7_SMART_RTL       = 10,
    CH7_GUIDED          = 11
};

// HIL enumerations
#define HIL_MODE_DISABLED 0
#define HIL_MODE_SENSORS  1

// Auto Pilot modes
// ----------------
enum mode {
    MANUAL       = 0,
    ACRO         = 1,
    STEERING     = 3,
    HOLD         = 4,
    AUTO         = 10,
    RTL          = 11,
    SMART_RTL    = 12,
    GUIDED       = 15,
    INITIALISING = 16
};

// types of failsafe events
#define FAILSAFE_EVENT_THROTTLE (1<<0)
#define FAILSAFE_EVENT_GCS      (1<<1)
#define FAILSAFE_EVENT_RC       (1<<2)

//  Logging parameters
#define LOG_THR_MSG             0x01
#define LOG_NTUN_MSG            0x02
#define LOG_STARTUP_MSG         0x06
#define LOG_RANGEFINDER_MSG     0x07
#define LOG_ARM_DISARM_MSG      0x08
#define LOG_STEERING_MSG        0x0D
#define LOG_GUIDEDTARGET_MSG    0x0E
#define LOG_WHEELENCODER_MSG    0x0F
#define LOG_ERROR_MSG           0x13

#define TYPE_AIRSTART_MSG       0x00
#define TYPE_GROUNDSTART_MSG    0x01

#define MASK_LOG_ATTITUDE_FAST  (1<<0)
#define MASK_LOG_ATTITUDE_MED   (1<<1)
#define MASK_LOG_GPS            (1<<2)
#define MASK_LOG_PM             (1<<3)
#define MASK_LOG_THR            (1<<4)
#define MASK_LOG_NTUN           (1<<5)
//#define MASK_LOG_MODE         (1<<6) // no longer used
#define MASK_LOG_IMU            (1<<7)
#define MASK_LOG_CMD            (1<<8)
#define MASK_LOG_CURRENT        (1<<9)
#define MASK_LOG_RANGEFINDER    (1<<10)
#define MASK_LOG_COMPASS        (1<<11)
#define MASK_LOG_CAMERA         (1<<12)
#define MASK_LOG_STEERING       (1<<13)
#define MASK_LOG_RC             (1<<14)
#define MASK_LOG_ARM_DISARM     (1<<15)
#define MASK_LOG_IMU_RAW        (1UL<<19)

// for mavlink SET_POSITION_TARGET messages
#define MAVLINK_SET_POS_TYPE_MASK_POS_IGNORE      ((1<<0) | (1<<1))
#define MAVLINK_SET_POS_TYPE_MASK_VEL_IGNORE      ((1<<3) | (1<<4))
#define MAVLINK_SET_POS_TYPE_MASK_ACC_IGNORE      ((1<<6) | (1<<7))
#define MAVLINK_SET_POS_TYPE_MASK_FORCE           (1<<9)
#define MAVLINK_SET_POS_TYPE_MASK_YAW_IGNORE      (1<<10)
#define MAVLINK_SET_POS_TYPE_MASK_YAW_RATE_IGNORE (1<<11)

// for mavlink SET_ATTITUDE_TARGET messages
#define MAVLINK_SET_ATT_TYPE_MASK_ROLL_RATE_IGNORE     (1<<0)
#define MAVLINK_SET_ATT_TYPE_MASK_PITCH_RATE_IGNORE    (1<<1)
#define MAVLINK_SET_ATT_TYPE_MASK_YAW_RATE_IGNORE      (1<<2)
#define MAVLINK_SET_ATT_TYPE_MASK_THROTTLE_IGNORE      (1<<6)
#define MAVLINK_SET_ATT_TYPE_MASK_ATTITUDE_IGNORE      (1<<7)

// general error codes
#define ERROR_CODE_ERROR_RESOLVED       0
// Error message sub systems and error codes
#define ERROR_SUBSYSTEM_FAILSAFE_FENCE  9
#define ERROR_SUBSYSTEM_FLIGHT_MODE     10
#define ERROR_SUBSYSTEM_CRASH_CHECK     12
// subsystem specific error codes -- crash checker
#define ERROR_CODE_CRASH_CHECK_CRASH 1

enum fs_crash_action {
  FS_CRASH_DISABLE = 0,
  FS_CRASH_HOLD = 1,
  FS_CRASH_HOLD_AND_DISARM = 2
};

#define DISTANCE_HOME_MAX 0.5f  // Distance max to home location before changing it when disarm

enum mode_reason_t {
    MODE_REASON_INITIALISED = 0,
    MODE_REASON_TX_COMMAND,
    MODE_REASON_GCS_COMMAND,
    MODE_REASON_FAILSAFE,
    MODE_REASON_MISSION_END,
    MODE_REASON_CRASH_FAILSAFE,
    MODE_REASON_MISSION_COMMAND,
    MODE_REASON_FENCE_BREACH,
};

// values used by the ap.ch7_opt and ap.ch8_opt flags
enum aux_switch_pos {
    AUX_SWITCH_LOW,
    AUX_SWITCH_MIDDLE,
    AUX_SWITCH_HIGH
};

enum pilot_steer_type_t {
    PILOT_STEER_TYPE_DEFAULT = 0,
    PILOT_STEER_TYPE_TWO_PADDLES = 1,
    PILOT_STEER_TYPE_DIR_REVERSED_WHEN_REVERSING = 2,
    PILOT_STEER_TYPE_DIR_UNCHANGED_WHEN_REVERSING = 3,
};

// frame class enum used for FRAME_CLASS parameter
enum frame_class {
    FRAME_UNDEFINED = 0,
    FRAME_ROVER = 1,
    FRAME_BOAT = 2
};

#define AUX_SWITCH_PWM_TRIGGER_HIGH 1800   // pwm value above which the ch7 or ch8 option will be invoked
#define AUX_SWITCH_PWM_TRIGGER_LOW  1200   // pwm value below which the ch7 or ch8 option will be disabled
          cmakeBuildDirectory: ${{ #include "Rover.h"

/*
  our failsafe strategy is to detect main loop lockup and switch to
  passing inputs straight from the RC inputs to RC outputs.
 */

/*
  this failsafe_check function is called from the core timer interrupt
  at 1kHz.
 */
void Rover::failsafe_check()
{
    static uint16_t last_ticks;
    static uint32_t last_timestamp;
    const uint32_t tnow = AP_HAL::micros();

    const uint16_t ticks = scheduler.ticks();
    if (ticks != last_ticks) {
        // the main loop is running, all is OK
        last_ticks = ticks;
        last_timestamp = tnow;
        return;
    }

    if (tnow - last_timestamp > 200000) {
        // we have gone at least 0.2 seconds since the main loop
        // ran. That means we're in trouble, or perhaps are in
        // an initialisation routine or log erase. disarm the motors
        // To-Do: log error to dataflash
        if (arming.is_armed()) {
            // disarm motors
            disarm_motors();
        }
    }
}

/*
  called to set/unset a failsafe event.
 */
void Rover::failsafe_trigger(uint8_t failsafe_type, bool on)
{
    uint8_t old_bits = failsafe.bits;
    if (on) {
        failsafe.bits |= failsafe_type;
    } else {
        failsafe.bits &= ~failsafe_type;
    }
    if (old_bits == 0 && failsafe.bits != 0) {
        // a failsafe event has started
        failsafe.start_time = millis();
    }
    if (failsafe.triggered != 0 && failsafe.bits == 0) {
        // a failsafe event has ended
        gcs().send_text(MAV_SEVERITY_INFO, "Failsafe ended");
    }

    failsafe.triggered &= failsafe.bits;

    if (failsafe.triggered == 0 &&
        failsafe.bits != 0 &&
        millis() - failsafe.start_time > g.fs_timeout * 1000 &&
        control_mode != &mode_rtl &&
        control_mode != &mode_hold) {
        failsafe.triggered = failsafe.bits;
        gcs().send_text(MAV_SEVERITY_WARNING, "Failsafe trigger 0x%x", static_cast<uint32_t>(failsafe.triggered));
        switch (g.fs_action) {
            case 0:
                break;
            case 1:
                if (!set_mode(mode_rtl, MODE_REASON_FAILSAFE)) {
                    set_mode(mode_hold, MODE_REASON_FAILSAFE);
                }
                break;
            case 2:
                set_mode(mode_hold, MODE_REASON_FAILSAFE);
                break;
            case 3:
                if (!set_mode(mode_smartrtl, MODE_REASON_FAILSAFE)) {
                    if (!set_mode(mode_rtl, MODE_REASON_FAILSAFE)) {
                        set_mode(mode_hold, MODE_REASON_FAILSAFE);
                    }
                }
                break;
            case 4:
                if (!set_mode(mode_smartrtl, MODE_REASON_FAILSAFE)) {
                    set_mode(mode_hold, MODE_REASON_FAILSAFE);
                }
                break;
        }
    }
}

void Rover::handle_battery_failsafe(const char* type_str, const int8_t action)
{
        switch ((Failsafe_Action)action) {
            case Failsafe_Action_None:
                break;
            case Failsafe_Action_SmartRTL:
                if (set_mode(mode_smartrtl, MODE_REASON_FAILSAFE)) {
                    break;
                }
                FALLTHROUGH;
            case Failsafe_Action_RTL:
                if (set_mode(mode_rtl, MODE_REASON_FAILSAFE)) {
                    break;
                }
                FALLTHROUGH;
            case Failsafe_Action_Hold:
                set_mode(mode_hold, MODE_REASON_FAILSAFE);
                break;
            case Failsafe_Action_SmartRTL_Hold:
                if (!set_mode(mode_smartrtl, MODE_REASON_FAILSAFE)) {
                    set_mode(mode_hold, MODE_REASON_FAILSAFE);
                }
                break;
            case Failsafe_Action_Terminate:
#if ADVANCED_FAILSAFE == ENABLED
                char battery_type_str[17];
                snprintf(battery_type_str, 17, "%s battery", type_str);
                afs.gcs_terminate(true, battery_type_str);
#else
                disarm_motors();
#endif // ADVANCED_FAILSAFE == ENABLED
                break;
        }
}

#if ADVANCED_FAILSAFE == ENABLED
/*
   check for AFS failsafe check
 */
void Rover::afs_fs_check(void)
{
    // perform AFS failsafe checks
    g2.afs.check(rover.last_heartbeat_ms, rover.g2.fence.get_breaches() != 0, failsafe.last_valid_rc_ms);
}
#endif }}
          # Ruleset file that will determine what checks will be run
          ruleset: NativeRecommendedRules.ruleset

      # Upload SARIF file to GitHub Code Scanning Alerts
      - name: Upload SARIF to GitHub
        uses: github/codeql-action/upload-sarif@v2
        with: #include "Rover.h"

// fence_check - ask fence library to check for breaches and initiate the response
void Rover::fence_check()
{
    uint8_t new_breaches;  // the type of fence that has been breached
    const uint8_t orig_breaches = g2.fence.get_breaches();

    // check for a breach
    new_breaches = g2.fence.check();

    // return immediately if motors are not armed
    if (!arming.is_armed()) {
        return;
    }

    // if there is a new breach take action
    if (new_breaches) {
        // if the user wants some kind of response and motors are armed
        if (g2.fence.get_action() != AC_FENCE_ACTION_REPORT_ONLY) {
            // if we are within 100m of the fence, RTL
            if (g2.fence.get_breach_distance(new_breaches) <= AC_FENCE_GIVE_UP_DISTANCE) {
                if (!set_mode(mode_rtl, MODE_REASON_FENCE_BREACH)) {
                    set_mode(mode_hold, MODE_REASON_FENCE_BREACH);
                }
            } else {
                // if more than 100m outside the fence just force to HOLD
                set_mode(mode_hold, MODE_REASON_FENCE_BREACH);
            }
        }
        // log an error in the dataflash
        Log_Write_Error(ERROR_SUBSYSTEM_FAILSAFE_FENCE, new_breaches);

    } else if (orig_breaches) {
        // record clearing of breach
        Log_Write_Error(ERROR_SUBSYSTEM_FAILSAFE_FENCE, ERROR_CODE_ERROR_RESOLVED);
    }
}

// fence_send_mavlink_status - send fence status to ground station
void Rover::fence_send_mavlink_status(mavlink_channel_t chan)
{
    if (g2.fence.enabled()) {
        // traslate fence library breach types to mavlink breach types
        uint8_t mavlink_breach_type = FENCE_BREACH_NONE;
        const uint8_t breaches = g2.fence.get_breaches();
        if ((breaches & (AC_FENCE_TYPE_CIRCLE | AC_FENCE_TYPE_POLYGON)) != 0) {
            mavlink_breach_type = FENCE_BREACH_BOUNDARY;
        }

        // send status
        mavlink_msg_fence_status_send(chan,
                                      static_cast<int8_t>(g2.fence.get_breaches() != 0),
                                      g2.fence.get_breach_count(),
                                      mavlink_breach_type,
                                      g2.fence.get_breach_time());
    }
}
          sarif_file: ${{ LIBRARIES = AP_Common
LIBRARIES += AP_Param
LIBRARIES += StorageManager
LIBRARIES += AP_GPS
LIBRARIES += AP_Baro
LIBRARIES += AP_Compass
LIBRARIES += AP_Math
LIBRARIES += AP_InertialSensor
LIBRARIES += AP_AccelCal
LIBRARIES += AP_AHRS
LIBRARIES += AP_NavEKF2
LIBRARIES += AP_NavEKF3
LIBRARIES += AP_Mission
LIBRARIES += AP_Rally
LIBRARIES += AP_Terrain
LIBRARIES += RC_Channel
LIBRARIES += AP_RangeFinder
LIBRARIES += Filter
LIBRARIES += Butter
LIBRARIES += AP_Buffer
LIBRARIES += ModeFilter
LIBRARIES += AverageFilter
LIBRARIES += AP_Mount
LIBRARIES += AP_Camera
LIBRARIES += GCS_MAVLink
LIBRARIES += AP_SerialManager
LIBRARIES += AP_Vehicle
LIBRARIES += DataFlash
LIBRARIES += AP_RCMapper
LIBRARIES += AP_Scheduler
LIBRARIES += AP_Navigation
LIBRARIES += APM_Control
LIBRARIES += AP_L1_Control
LIBRARIES += AP_BoardConfig
LIBRARIES += AP_Frsky_Telem
LIBRARIES += AP_Notify
LIBRARIES += AP_BattMonitor
LIBRARIES += AP_OpticalFlow
LIBRARIES += AP_RSSI
LIBRARIES += AP_Declination
LIBRARIES += AP_RPM
LIBRARIES += AP_Arming
LIBRARIES += AP_Stats
LIBRARIES += AP_Beacon
LIBRARIES += AP_WheelEncoder
LIBRARIES += AP_AdvancedFailsafe
LIBRARIES += AR_AttitudeControl
LIBRARIES += AC_PID
LIBRARIES += AP_Proximity
LIBRARIES += AC_Fence
LIBRARIES += AP_SmartRTL
LIBRARIES += AC_Avoidance
LIBRARIES += AC_AttitudeControl
LIBRARIES += AP_Devo_Telem }}

      # Upload SARIF file as an Artifact to download and view
      # - name: Upload SARIF as an Artifact
      #   uses: actions/upload-artifact@v3
      #   with:
      #     name: sarif-file
      #     path: ${{ steps.run-analysis.outputs.sarif }}
