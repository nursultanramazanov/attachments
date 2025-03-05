# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
#
# Find more information at:
# https://github.com/microsoft/msvc-code-analysis-action

name: Microsoft C++ Code Analysis

on: // This file is just a placeholder for your configuration file.  If
// you wish to change any of the setup parameters from their default
// values, place the appropriate #define statements here.
  push: /*
   Lead developers:
 
   Please contribute your ideas! See http://dev.ardupilot.org for details

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

#include "Tracker.h"

#define FORCE_VERSION_H_INCLUDE
#include "version.h"
#undef FORCE_VERSION_H_INCLUDE

#define SCHED_TASK(func, _interval_ticks, _max_time_micros) SCHED_TASK_CLASS(Tracker, &tracker, func, _interval_ticks, _max_time_micros)

/*
  scheduler table - all regular tasks apart from the fast_loop()
  should be listed here, along with how often they should be called
  (in 20ms units) and the maximum time they are expected to take (in
  microseconds)
 */
const AP_Scheduler::Task Tracker::scheduler_tasks[] = {
    SCHED_TASK(update_ahrs,            50,   1000),
    SCHED_TASK(read_radio,             50,    200),
    SCHED_TASK(update_tracking,        50,   1000),
    SCHED_TASK(update_GPS,             10,   4000),
    SCHED_TASK(update_compass,         10,   1500),
    SCHED_TASK_CLASS(AP_BattMonitor,    &tracker.battery,   read,           10, 1500),
    SCHED_TASK_CLASS(AP_Baro,          &tracker.barometer,  update,         10,   1500),
    SCHED_TASK(gcs_update,             50,   1700),
    SCHED_TASK(gcs_data_stream_send,   50,   3000),
    SCHED_TASK(compass_accumulate,     50,   1500),
    SCHED_TASK_CLASS(AP_Baro,           &tracker.barometer, accumulate,     50,  900),
    SCHED_TASK(ten_hz_logging_loop,    10,    300),
    SCHED_TASK_CLASS(DataFlash_Class,   &tracker.DataFlash, periodic_tasks, 50,  300),
    SCHED_TASK_CLASS(AP_InertialSensor, &tracker.ins,       periodic,       50,   50),
    SCHED_TASK_CLASS(AP_Notify,         &tracker.notify,    update,         50,  100),
    SCHED_TASK(check_usb_mux,          10,    300),
    SCHED_TASK(gcs_retry_deferred,     50,   1000),
    SCHED_TASK(one_second_loop,         1,   3900),
    SCHED_TASK(compass_cal_update,     50,    100),
    SCHED_TASK(accel_cal_update,       10,    100)
};

/**
  setup the sketch - called once on startup
 */
void Tracker::setup() 
{
    // load the default values of variables listed in var_info[]
    AP_Param::setup_sketch_defaults();

    init_tracker();

    // initialise the main loop scheduler
    scheduler.init(&scheduler_tasks[0], ARRAY_SIZE(scheduler_tasks), (uint32_t)-1);
}

/**
   loop() is called continuously 
 */
void Tracker::loop()
{
    // wait for an INS sample
    ins.wait_for_sample();

    // tell the scheduler one tick has passed
    scheduler.tick();

    scheduler.run(19900UL);
}

void Tracker::one_second_loop()
{
    // send a heartbeat
    gcs().send_message(MSG_HEARTBEAT);

    // make it possible to change orientation at runtime
    ahrs.set_orientation();

    // sync MAVLink system ID
    mavlink_system.sysid = g.sysid_this_mav;

    // updated armed/disarmed status LEDs
    update_armed_disarmed();

    one_second_counter++;

    if (one_second_counter >= 60) {
        if (g.compass_enabled) {
            compass.save_offsets();
        }
        one_second_counter = 0;
    }
}

void Tracker::ten_hz_logging_loop()
{
    if (should_log(MASK_LOG_IMU)) {
        DataFlash.Log_Write_IMU();
    }
    if (should_log(MASK_LOG_ATTITUDE)) {
        Log_Write_Attitude();
    }
    if (should_log(MASK_LOG_RCIN)) {
        DataFlash.Log_Write_RCIN();
    }
    if (should_log(MASK_LOG_RCOUT)) {
        DataFlash.Log_Write_RCOUT();
    }
}

const AP_HAL::HAL& hal = AP_HAL::get_HAL();

Tracker::Tracker(void)
    : DataFlash(fwver.fw_string, g.log_bitmask)
{
    memset(&current_loc, 0, sizeof(current_loc));
    memset(&vehicle, 0, sizeof(vehicle));
}

Tracker tracker;

AP_HAL_MAIN_CALLBACKS(&tracker); 
    branches: [ "main" ]
  pull_request: AntennaTracker

2014-03-05

This is the ardupilot AntennaTracker

This software, combined with appropriate hardware, can be used to cause a
high-gain antenna to track a mavlink equipped air or ground vehicle in real
time, providing higher quality radio links over longer distances than a
stationary or omnidirectional antenna.

AntennaTracker can be a stationary installation on the ground, or can be
mobile, mounted on a vehicle, and moving at the same time as the vehicle it is
tracking, such as on a pursuit vehicle etc.

Antenna Tracker Hardware

The software works with:

- Alt-Azimuth mounted antenna tracker, equipped with 2 servos: one for Azimuth
(also called bearing or yaw) -180 to 180 degreees and one for Altitude (also
called elevation, or pitch) -90 degrees (straight down) to 90 degrees
(straight up). I used an Eagle Tree Antenna Pan-Tilt.

- Ardupilot compatible processor such as 
-- Flymaple
-- others tested?
and with a radio or wired link to a source of mavlink messages with the
tracked vehicle's position data. 

A suitable power supply is also needed, possibly battery powered. In the case
of Flymaple, the onboard 5V regulator is not able to drive large servos
without causing large amounts of electrical noise in the processor, so you
would probably need an external 5V regulated supply

Theory of Operation

AntennaTracker receives real-time GPS position reports from the vehicle being
tracked (in the form of GLOBAL_POSITION_INT messages sent by mavlink radio
connection or possibly by hardwired serial connection). AntennaTracker also
knows it own current position. Based on these 2 sets of coordinates, it
calculates the magnetic bearing and elevation to the vehicle. It then moves
the tracker servos until the antenna (and the AntennaTracker processor) are
aligned with the desired bearing and elevation. At this stage the antenna is
pointed straight at the tracked vehicle. As the vehicle (and possibly the
antenna mount itself) move around, the servos are continually move to track
the vehicle.

AntennaTracker currently only supports closed loop feedback to position the
antenna. This requires that AntennaTracker processor (including
accelerometers and compass) be mounted on the moving part of the antenna
tracker hardware. The servos are moved until the yaw and pitch of the processor
(and therfore of the antenna itself) matches the bearing and elevation to the
tracked vehicle.

How does AntennaTracker know where it is on the earth?

- If the tracker is equipped with a GPS and it has had a good 3D fix, the most
recent good GPS fix is used.

- If the tracker is equipped with a GPS and it has lost its fix, the most
recent good GPS fix is used.

- If there is no good GPS fix since startup (either because there is no GPS
equipped or because the GPS has not yet got a fix), the most resently set HOME
location is used.

- If no HOME location has ever been set in the tracker, 0.0 Latitude, 0.0
Longitude, 0 Altitude are used (which is probably not very helpful)

How does AntennaTracker get GLOBAL_POSITION_INT messages from the tracked
vehicle?

mavproxy, when it runs the tracker module, intercepts all GLOBAL_POSITION_INT
messages received from its master vehicle, and sends them to the tracker. It
knows how to connect to the tracker with the set port and set baudrate
commands, for example to connect by a USB-Serial  radio, such as the 3DR Radio
on port /dev/ttyUSB0:

(after starting mavproxy)
module load tracker
tracker set port /dev/ttyUSB0
tracker set baudrate 57600
tracker start

Mounting

The AntennaTracker processor must be mounted on the moving (ie the rotating,
tilting) part of the antenna tracker hardware. Just how you do this will
depend on your exact hardware, but if the processor is mounted in anything
other than the default orientation (ie with the processor default 'forward'
direction pointing in the direction the antenna points, you will have to
configure AHRS_ORIENTATION in the AntennaTracker configuration to suit your
mechanical mounting arrangements.

In my case, I used a Flymaple with AHRS_ORIENTATION set to 1 (Yaw45). The
Flymaple was mounted on the antenna tracker with component side up and the
green pin headers horizontal and perpendicular to the antenna beam (See
pictures???).

Configuration

It is vitally important that your AntennaTracker be properly configured for the
mounting, servos and other hardware in your tracker. Unless you do this IT
WILL PROBABLY NOT WORK AS EXPECTED. CAUTION: It might even oscillate or flap
around. It might even damage itself.

AntennaTracker (like other ardupilot software such as ArduPlane, APMRover2
etc) has configuration values that control and tailor its operation, and which
are stored in EEPROM on the processor. The configuration is restored from
EEPROM every time the processsor starts.

You can use MissionPlanner, mavproxy or apm_planner or other mavlink compatible
software to check and change the configuration of your AntennaTracker.

You will almost certainly have to change the following configuration items to
suit, and possibly others besides:

AHRS_ORIENTATION
RC1_MIN
RC1_MAX
RC1_REV
RC2_MIN
RC2_MAX
RC2_REV
PITCH2SRV_P
PITCH2SRV_I
PITCH2SRV_D
YAW2SRV_P
YAW2SRV_I
YAW2SRV_D

(more info here about how to find the right values when the setup commands are
available)

You can find the complete set of configuration items I used for my Flymaple
tracker mounted on an Eagle Tree tracker WHERE?????

Compass Calibration

AntennaTracker uses its internal magnetic compass to determine where North is
and therefore where to point for each bearing to the target vehicle. (On other
ardupilot vehicles, the yaw is also calibrated by the current GPS track. This
is not possible for AntennaTracker, so the compass is the only possible source
for absolute bearing)

The compass magnetometer is exquisitely sensitive to nearby magnetic fields from
wires, metal fittings, servos, radios etc. Therefore is it VITALLY ESSENTIAL that you
calibrate the compass. Further, it must be calibrated outdoors, while installed
in situ in the tracker, with all other equipment installed, fastened down and
ready to operate.

Failure to calibrate correctly will result in the tracker pointing in the
wrong direction, or the azimuth/yaw and elevation/pitch drifting.

Use Mission Planner to calibrate the tracker:
- Assemble the completed tracker, complete and ready to operate.
- Power up the tracker
- Connect to it with Mission Planner
- On Mission Planner->INITIAL SETUP->Compass, click on Live Calibration
- click on OK
- rotate the complete antenna tracker assembly around, in 3 dimensions, making sure to
rotate the tracker to every possible orientation in the 1 minute you have
available. DON'T just move the yaw and pitch motors around. You will probably have to
pick the whole thing up.
- At the end of the minute, Mission Planner will update AntennaTrackers compass offsets.

Levelling

Since AntennaTracker uses its internal accelerometers (amongst other things)
to determine the current elevation/pitch of the antenna, it is also important
to 'level' the AntennaTracker before use so the accelerometers are
calibrated. Failure to do this will result in the antenna constantly pointing
a bit high or a bit low compared to the tracked vehicle.

You can do this by connecting to the AntennaTracker with mavproxy and issuing
these commands:

disarm
  - manually move the antenna so the antenna is level and pointing at 0 degrees elevation
    i.e. at the horizon
level
  - wait a minute without disturbing the antenna position
arm throttle
  - this rearms the servos, allowing the antenna to move

Operation

<more here>

After powering up AntennaTracker, it may take up to 1 minute while it
automatially calibrates the compass before it points in the expected
direction. Moving the tracker around will speed up this process.

How to set the HOME location

If your AntennaTracker is not equipped with a GPS, you can still use it to
track vehicles. First you will have to tell AntennaTracker where it is on the
ground by setting its HOME position.

Connect to AntennaTracker with mavproxy

module load map
 - the map will appear.
 - Zoom and drag the map until you can see the location where
   your AntennaTracker is located
 - Click the mouse on the location of the antenna
wp sethome

AntennaTracker will now remember this as its HOME location whenever it is
started.

Simulating an antenna tracker
-----------------------------

To run a full antenna tracker simulation with a simulated APM:Plane as
the vehicle, cd to the ArduPlane directory and run this:

  ../Tools/autotest/sim_arduplane.sh -T --aircraft test

The -T flag tells sim_arduplane.sh to start an entenna tracker
simulator and also start a virtual antenna tracker in a window.

To start the antenna tracker running run "tracker start" in the
mavproxy window.

To then connect to your antenna tracker with mavproxy you would run:

  mavproxy.py --master tcp:127.0.0.1:5770

then you can configure it as needed. 
    branches: [ "main" ]
  schedule: #include "GCS_Mavlink.h"

#include "Tracker.h"

// default sensors are present and healthy: gyro, accelerometer, barometer, rate_control, attitude_stabilization, yaw_position, altitude control, x/y position control, motor_control
#define MAVLINK_SENSOR_PRESENT_DEFAULT (MAV_SYS_STATUS_SENSOR_3D_GYRO | MAV_SYS_STATUS_SENSOR_3D_ACCEL | MAV_SYS_STATUS_SENSOR_ABSOLUTE_PRESSURE | MAV_SYS_STATUS_SENSOR_ANGULAR_RATE_CONTROL | MAV_SYS_STATUS_SENSOR_ATTITUDE_STABILIZATION | MAV_SYS_STATUS_SENSOR_YAW_POSITION | MAV_SYS_STATUS_SENSOR_Z_ALTITUDE_CONTROL | MAV_SYS_STATUS_SENSOR_XY_POSITION_CONTROL | MAV_SYS_STATUS_SENSOR_MOTOR_OUTPUTS)

/*
 *  !!NOTE!!
 *
 *  the use of NOINLINE separate functions for each message type avoids
 *  a compiler bug in gcc that would cause it to use far more stack
 *  space than is needed. Without the NOINLINE we use the sum of the
 *  stack needed for each message type. Please be careful to follow the
 *  pattern below when adding any new messages
 */

MAV_TYPE GCS_MAVLINK_Tracker::frame_type() const
{
    return MAV_TYPE_ANTENNA_TRACKER;
}

MAV_MODE GCS_MAVLINK_Tracker::base_mode() const
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
    switch (tracker.control_mode) {
    case MANUAL:
        _base_mode |= MAV_MODE_FLAG_MANUAL_INPUT_ENABLED;
        break;

    case STOP:
        break;

    case SCAN:
    case SERVO_TEST:
    case AUTO:
        _base_mode |= MAV_MODE_FLAG_GUIDED_ENABLED |
            MAV_MODE_FLAG_STABILIZE_ENABLED;
        // note that MAV_MODE_FLAG_AUTO_ENABLED does not match what
        // APM does in any mode, as that is defined as "system finds its own goal
        // positions", which APM does not currently do
        break;

    case INITIALISING:
        break;
    }

    // we are armed if safety switch is not disarmed
    if (hal.util->safety_switch_state() != AP_HAL::Util::SAFETY_DISARMED) {
        _base_mode |= MAV_MODE_FLAG_SAFETY_ARMED;
    }

    return (MAV_MODE)_base_mode;
}

uint32_t GCS_MAVLINK_Tracker::custom_mode() const
{
    return tracker.control_mode;
}

MAV_STATE GCS_MAVLINK_Tracker::system_status() const
{
    if (tracker.control_mode == INITIALISING) {
        return MAV_STATE_CALIBRATING;
    }
    return MAV_STATE_ACTIVE;
}

void Tracker::send_attitude(mavlink_channel_t chan)
{
    Vector3f omega = ahrs.get_gyro();
    mavlink_msg_attitude_send(
        chan,
        AP_HAL::millis(),
        ahrs.roll,
        ahrs.pitch,
        ahrs.yaw,
        omega.x,
        omega.y,
        omega.z);
}


void Tracker::send_extended_status1(mavlink_channel_t chan)
{
    int16_t battery_current = -1;
    int8_t battery_remaining = -1;

    if (battery.has_current() && battery.healthy()) {
        battery_remaining = battery.capacity_remaining_pct();
        battery_current = battery.current_amps() * 100;
    }

    mavlink_msg_sys_status_send(
        chan,
        0,
        0,
        0,
        static_cast<uint16_t>(scheduler.load_average() * 1000),
        battery.voltage() * 1000,  // mV
        battery_current,        // in 10mA units
        battery_remaining,      // in %
        0,  // comm drops %,
        0,  // comm drops in pkts,
        0, 0, 0, 0);
}

void Tracker::send_location(mavlink_channel_t chan)
{
    uint32_t fix_time;
    if (gps.status() >= AP_GPS::GPS_OK_FIX_2D) {
        fix_time = gps.last_fix_time_ms();
    } else {
        fix_time = AP_HAL::millis();
    }
    const Vector3f &vel = gps.velocity();
    mavlink_msg_global_position_int_send(
        chan,
        fix_time,
        current_loc.lat,                // in 1E7 degrees
        current_loc.lng,                // in 1E7 degrees
        current_loc.alt * 10,        // millimeters above sea level
        0,
        vel.x * 100,  // X speed cm/s (+ve North)
        vel.y * 100,  // Y speed cm/s (+ve East)
        vel.z * 100,  // Z speed cm/s (+ve Down)
        ahrs.yaw_sensor);
}

void Tracker::send_nav_controller_output(mavlink_channel_t chan)
{
	float alt_diff = (g.alt_source == ALT_SOURCE_BARO) ? nav_status.alt_difference_baro : nav_status.alt_difference_gps;

    mavlink_msg_nav_controller_output_send(
        chan,
        0,
        nav_status.pitch,
        nav_status.bearing,
        nav_status.bearing,
        MIN(nav_status.distance, UINT16_MAX),
        alt_diff,
        0,
        0);
}


// report simulator state
void Tracker::send_simstate(mavlink_channel_t chan)
{
#if CONFIG_HAL_BOARD == HAL_BOARD_SITL
    sitl.simstate_send(chan);
#endif
}

bool GCS_MAVLINK_Tracker::handle_guided_request(AP_Mission::Mission_Command&)
{
    // do nothing
    return false;
}

void GCS_MAVLINK_Tracker::handle_change_alt_request(AP_Mission::Mission_Command&)
{
    // do nothing
}

// try to send a message, return false if it won't fit in the serial tx buffer
bool GCS_MAVLINK_Tracker::try_send_message(enum ap_message id)
{
    switch (id) {
    case MSG_HEARTBEAT:
        CHECK_PAYLOAD_SIZE(HEARTBEAT);
        last_heartbeat_time = AP_HAL::millis();
        send_heartbeat();
        return true;

    case MSG_ATTITUDE:
        CHECK_PAYLOAD_SIZE(ATTITUDE);
        tracker.send_attitude(chan);
        break;

    case MSG_LOCATION:
        CHECK_PAYLOAD_SIZE(GLOBAL_POSITION_INT);
        tracker.send_location(chan);
        break;

    case MSG_NAV_CONTROLLER_OUTPUT:
        CHECK_PAYLOAD_SIZE(NAV_CONTROLLER_OUTPUT);
        tracker.send_nav_controller_output(chan);
        break;

    case MSG_RADIO_IN:
        CHECK_PAYLOAD_SIZE(RC_CHANNELS);
        send_radio_in(0);
        break;

    case MSG_SERVO_OUTPUT_RAW:
        CHECK_PAYLOAD_SIZE(SERVO_OUTPUT_RAW);
        send_servo_output_raw(false);
        break;

    case MSG_RAW_IMU1:
        CHECK_PAYLOAD_SIZE(RAW_IMU);
        send_raw_imu(tracker.ins, tracker.compass);
        break;

    case MSG_RAW_IMU2:
        CHECK_PAYLOAD_SIZE(SCALED_PRESSURE);
        send_scaled_pressure();
        break;

    case MSG_RAW_IMU3:
        CHECK_PAYLOAD_SIZE(SENSOR_OFFSETS);
        send_sensor_offsets(tracker.ins, tracker.compass);
        break;

    case MSG_SIMSTATE:
        CHECK_PAYLOAD_SIZE(SIMSTATE);
        tracker.send_simstate(chan);
        break;

    case MSG_EXTENDED_STATUS1:
        CHECK_PAYLOAD_SIZE(SYS_STATUS);
        tracker.send_extended_status1(chan);
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
GCS_MAVLINK_Tracker::data_stream_send(void)
{
    send_queued_parameters();

    if (tracker.in_mavlink_delay) {
        // don't send any other stream types while in the delay callback
        return;
    }

    if (stream_trigger(STREAM_RAW_SENSORS)) {
        send_message(MSG_RAW_IMU1);
        send_message(MSG_RAW_IMU2);
        send_message(MSG_RAW_IMU3);
    }

    if (stream_trigger(STREAM_EXTENDED_STATUS)) {
        send_message(MSG_EXTENDED_STATUS1);
        send_message(MSG_EXTENDED_STATUS2);
        send_message(MSG_NAV_CONTROLLER_OUTPUT);
        send_message(MSG_GPS_RAW);
        send_message(MSG_GPS_RTK);
        send_message(MSG_GPS2_RAW);
        send_message(MSG_GPS2_RTK);
    }

    if (stream_trigger(STREAM_POSITION)) {
        send_message(MSG_LOCATION);
        send_message(MSG_LOCAL_POSITION);
    }

    if (stream_trigger(STREAM_RAW_CONTROLLER)) {
        send_message(MSG_SERVO_OUTPUT_RAW);
    }

    if (stream_trigger(STREAM_RC_CHANNELS)) {
        send_message(MSG_RADIO_IN);
        send_message(MSG_SERVO_OUTPUT_RAW);
    }

    if (stream_trigger(STREAM_EXTRA1)) {
        send_message(MSG_ATTITUDE);
    }

    if (stream_trigger(STREAM_EXTRA3)) {
        send_message(MSG_AHRS);
        send_message(MSG_HWSTATUS);
        send_message(MSG_SIMSTATE);
        send_message(MSG_MAG_CAL_REPORT);
        send_message(MSG_MAG_CAL_PROGRESS);
    }
}

/*
  We eavesdrop on MAVLINK_MSG_ID_GLOBAL_POSITION_INT and
  MAVLINK_MSG_ID_SCALED_PRESSUREs
*/
void GCS_MAVLINK_Tracker::packetReceived(const mavlink_status_t &status,
                                         mavlink_message_t &msg)
{
    // return immediately if sysid doesn't match our target sysid
    if ((tracker.g.sysid_target != 0) && (tracker.g.sysid_target != msg.sysid)) {
        return;
    }

    switch (msg.msgid) {
    case MAVLINK_MSG_ID_HEARTBEAT:
    {
        mavlink_check_target(msg);
        break;
    }

    case MAVLINK_MSG_ID_GLOBAL_POSITION_INT:
    {
        // decode
        mavlink_global_position_int_t packet;
        mavlink_msg_global_position_int_decode(&msg, &packet);
        tracker.tracking_update_position(packet);
        break;
    }
    
    case MAVLINK_MSG_ID_SCALED_PRESSURE:
    {
        // decode
        mavlink_scaled_pressure_t packet;
        mavlink_msg_scaled_pressure_decode(&msg, &packet);
        tracker.tracking_update_pressure(packet);
        break;
    }
    }
    GCS_MAVLINK::packetReceived(status, msg);
}

// locks onto a particular target sysid and sets it's position data stream to at least 1hz
void GCS_MAVLINK_Tracker::mavlink_check_target(const mavlink_message_t &msg)
{
    // exit immediately if the target has already been set
    if (tracker.target_set) {
        return;
    }

    // decode
    mavlink_heartbeat_t packet;
    mavlink_msg_heartbeat_decode(&msg, &packet);

    // exit immediately if this is not a vehicle we would track
    if ((packet.type == MAV_TYPE_ANTENNA_TRACKER) ||
        (packet.type == MAV_TYPE_GCS) ||
        (packet.type == MAV_TYPE_ONBOARD_CONTROLLER) ||
        (packet.type == MAV_TYPE_GIMBAL)) {
        return;
    }

    // set our sysid to the target, this ensures we lock onto a single vehicle
    if (tracker.g.sysid_target == 0) {
        tracker.g.sysid_target = msg.sysid;
    }

    // send data stream request to target on all channels
    //  Note: this doesn't check success for all sends meaning it's not guaranteed the vehicle's positions will be sent at 1hz
    tracker.gcs().request_datastream_position(msg.sysid, msg.compid);
    tracker.gcs().request_datastream_airpressure(msg.sysid, msg.compid);

    // flag target has been set
    tracker.target_set = true;
}

uint8_t GCS_MAVLINK_Tracker::sysid_my_gcs() const
{
    return tracker.g.sysid_my_gcs;
}

MAV_RESULT GCS_MAVLINK_Tracker::_handle_command_preflight_calibration_baro()
{
    MAV_RESULT ret = GCS_MAVLINK::_handle_command_preflight_calibration_baro();
    if (ret == MAV_RESULT_ACCEPTED) {
        // zero the altitude difference on next baro update
        tracker.nav_status.need_altitude_calibration = true;
    }
    return ret;
}

void GCS_MAVLINK_Tracker::handleMessage(mavlink_message_t* msg)
{
    switch (msg->msgid) {

    // If we are currently operating as a proxy for a remote, 
    // alas we have to look inside each packet to see if it's for us or for the remote
    case MAVLINK_MSG_ID_REQUEST_DATA_STREAM:
    {
        handle_request_data_stream(msg, false);
        break;
    }


    case MAVLINK_MSG_ID_HEARTBEAT:
        break;

    case MAVLINK_MSG_ID_COMMAND_LONG:
    {
        // decode
        mavlink_command_long_t packet;
        mavlink_msg_command_long_decode(msg, &packet);
        
        MAV_RESULT result = MAV_RESULT_UNSUPPORTED;
        
        // do command
        send_text(MAV_SEVERITY_INFO,"Command received: ");
        
        switch(packet.command) {

            case MAV_CMD_COMPONENT_ARM_DISARM:
                if (packet.target_component == MAV_COMP_ID_SYSTEM_CONTROL) {
                    if (is_equal(packet.param1,1.0f)) {
                        tracker.arm_servos();
                        result = MAV_RESULT_ACCEPTED;
                    } else if (is_zero(packet.param1))  {
                        tracker.disarm_servos();
                        result = MAV_RESULT_ACCEPTED;
                    } else {
                        result = MAV_RESULT_UNSUPPORTED;
                    }
                } else {
                    result = MAV_RESULT_UNSUPPORTED;
                }
            break;

            case MAV_CMD_GET_HOME_POSITION:
                send_home(tracker.ahrs.get_home());
                Location ekf_origin;
                if (tracker.ahrs.get_origin(ekf_origin)) {
                    send_ekf_origin(ekf_origin);
                }
                result = MAV_RESULT_ACCEPTED;
                break;

            case MAV_CMD_DO_SET_SERVO:
                if (tracker.servo_test_set_servo(packet.param1, packet.param2)) {
                    result = MAV_RESULT_ACCEPTED;
                }
                break;

                // mavproxy/mavutil sends this when auto command is entered 
            case MAV_CMD_MISSION_START:
                tracker.set_mode(AUTO, MODE_REASON_GCS_COMMAND);
                result = MAV_RESULT_ACCEPTED;
                break;

            case MAV_CMD_PREFLIGHT_REBOOT_SHUTDOWN:
            {
                if (is_equal(packet.param1,1.0f) || is_equal(packet.param1,3.0f)) {
                    // when packet.param1 == 3 we reboot to hold in bootloader
                    hal.scheduler->reboot(is_equal(packet.param1,3.0f));
                    result = MAV_RESULT_ACCEPTED;
                }
                break;
            }

            case MAV_CMD_ACCELCAL_VEHICLE_POS:
                result = MAV_RESULT_FAILED;

                if (tracker.ins.get_acal()->gcs_vehicle_position(packet.param1)) {
                    result = MAV_RESULT_ACCEPTED;
                }
                break;

            default:
                result = handle_command_long_message(packet);
                break;
        }
        mavlink_msg_command_ack_send(
            chan,
            packet.command,
            result);
        
        break;
    }
         
    // When mavproxy 'wp sethome' 
    case MAVLINK_MSG_ID_MISSION_WRITE_PARTIAL_LIST:
    {
        // decode
        mavlink_mission_write_partial_list_t packet;
        mavlink_msg_mission_write_partial_list_decode(msg, &packet);
        if (packet.start_index == 0)
        {
            // New home at wp index 0. Ask for it
            waypoint_receiving = true;
            waypoint_request_i = 0;
            waypoint_request_last = 0;
            send_message(MSG_NEXT_WAYPOINT);
        }
        break;
    }

    // XXX receive a WP from GCS and store in EEPROM if it is HOME
    case MAVLINK_MSG_ID_MISSION_ITEM:
    {
        // decode
        mavlink_mission_item_t packet;
        MAV_MISSION_RESULT result = MAV_MISSION_ACCEPTED;

        mavlink_msg_mission_item_decode(msg, &packet);

        struct Location tell_command = {};

        switch (packet.frame)
        {
        case MAV_FRAME_MISSION:
        case MAV_FRAME_GLOBAL:
        {
            tell_command.lat = 1.0e7f*packet.x;                                     // in as DD converted to * t7
            tell_command.lng = 1.0e7f*packet.y;                                     // in as DD converted to * t7
            tell_command.alt = packet.z*1.0e2f;                                     // in as m converted to cm
            tell_command.options = 0;                                     // absolute altitude
            break;
        }

#ifdef MAV_FRAME_LOCAL_NED
        case MAV_FRAME_LOCAL_NED:                         // local (relative to home position)
        {
            tell_command.lat = 1.0e7f*ToDeg(packet.x/
                                           (RADIUS_OF_EARTH*cosf(ToRad(home.lat/1.0e7f)))) + home.lat;
            tell_command.lng = 1.0e7f*ToDeg(packet.y/RADIUS_OF_EARTH) + home.lng;
            tell_command.alt = -packet.z*1.0e2f;
            tell_command.options = MASK_OPTIONS_RELATIVE_ALT;
            break;
        }
#endif

#ifdef MAV_FRAME_LOCAL
        case MAV_FRAME_LOCAL:                         // local (relative to home position)
        {
            tell_command.lat = 1.0e7f*ToDeg(packet.x/
                                           (RADIUS_OF_EARTH*cosf(ToRad(home.lat/1.0e7f)))) + home.lat;
            tell_command.lng = 1.0e7f*ToDeg(packet.y/RADIUS_OF_EARTH) + home.lng;
            tell_command.alt = packet.z*1.0e2f;
            tell_command.options = MASK_OPTIONS_RELATIVE_ALT;
            break;
        }
#endif

        case MAV_FRAME_GLOBAL_RELATIVE_ALT:                         // absolute lat/lng, relative altitude
        {
            tell_command.lat = 1.0e7f * packet.x;                                     // in as DD converted to * t7
            tell_command.lng = 1.0e7f * packet.y;                                     // in as DD converted to * t7
            tell_command.alt = packet.z * 1.0e2f;
            tell_command.options = MASK_OPTIONS_RELATIVE_ALT;                                     // store altitude relative!! Always!!
            break;
        }

        default:
            result = MAV_MISSION_UNSUPPORTED_FRAME;
            break;
        }

        if (result != MAV_MISSION_ACCEPTED) goto mission_failed;

        // Check if receiving waypoints (mission upload expected)
        if (!waypoint_receiving) {
            result = MAV_MISSION_ERROR;
            goto mission_failed;
        }

        // check if this is the HOME wp
        if (packet.seq == 0) {
            tracker.set_home(tell_command); // New home in EEPROM
            send_text(MAV_SEVERITY_INFO,"New HOME received");
            waypoint_receiving = false;
        }

mission_failed:
        // we are rejecting the mission/waypoint
        mavlink_msg_mission_ack_send(
            chan,
            msg->sysid,
            msg->compid,
            result,
            MAV_MISSION_TYPE_MISSION);
        break;
    }

    case MAVLINK_MSG_ID_MANUAL_CONTROL:
    {
        mavlink_manual_control_t packet;
        mavlink_msg_manual_control_decode(msg, &packet);
        tracker.tracking_manual_control(packet);
        break;
    }

    case MAVLINK_MSG_ID_GLOBAL_POSITION_INT: 
    {
        // decode
        mavlink_global_position_int_t packet;
        mavlink_msg_global_position_int_decode(msg, &packet);
        tracker.tracking_update_position(packet);
        break;
    }

    case MAVLINK_MSG_ID_SCALED_PRESSURE: 
    {
        // decode
        mavlink_scaled_pressure_t packet;
        mavlink_msg_scaled_pressure_decode(msg, &packet);
        tracker.tracking_update_pressure(packet);
        break;
    }

    default:
        handle_common_message(msg);
        break;
    } // end switch
} // end handle mavlink


/*
 *  a delay() callback that processes MAVLink packets. We set this as the
 *  callback in long running library initialisation routines to allow
 *  MAVLink to process packets while waiting for the initialisation to
 *  complete
 */
void Tracker::mavlink_delay_cb()
{
    static uint32_t last_1hz, last_50hz, last_5s;
    if (!gcs().chan(0).initialised) {
        return;
    }

    tracker.in_mavlink_delay = true;
    DataFlash.EnableWrites(false);

    uint32_t tnow = AP_HAL::millis();
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
    DataFlash.EnableWrites(true);
    tracker.in_mavlink_delay = false;
}

/*
 *  send data streams in the given rate range on both links
 */
void Tracker::gcs_data_stream_send(void)
{
    gcs().data_stream_send();
}

/*
 *  look for incoming commands on the GCS links
 */
void Tracker::gcs_update(void)
{
    gcs().update();
}

/**
   retry any deferred messages
 */
void Tracker::gcs_retry_deferred(void)
{
    gcs().retry_deferred();
}

Compass *GCS_MAVLINK_Tracker::get_compass() const
{
    return &tracker.compass;
}

/*
  set_mode() wrapper for MAVLink SET_MODE
 */
bool GCS_MAVLINK_Tracker::set_mode(uint8_t mode)
{
    switch (mode) {
    case AUTO:
    case MANUAL:
    case SCAN:
    case SERVO_TEST:
    case STOP:
        tracker.set_mode((enum ControlMode)mode, MODE_REASON_GCS_COMMAND);
        return true;
    }
    return false;
}

const AP_FWVersion &GCS_MAVLINK_Tracker::get_fwver() const
{
    return tracker.fwver;
}

void GCS_MAVLINK_Tracker::set_ekf_origin(const Location& loc)
{
    tracker.set_ekf_origin(loc);
}

/* dummy methods to avoid having to link against AP_Camera */
void AP_Camera::control_msg(mavlink_message_t const*) {}
void AP_Camera::configure(float, float, float, float, float, float, float) {}
void AP_Camera::control(float, float, float, float, float, float) {}
void AP_Camera::send_feedback(mavlink_channel_t chan) {}
/* end dummy methods to avoid having to link against AP_Camera */

// dummy method to avoid linking AFS
bool AP_AdvancedFailsafe::gcs_terminate(bool should_terminate, const char *reason) {return false;} 
    - cron: #pragma once

#include <GCS_MAVLink/GCS.h>

class GCS_MAVLINK_Tracker : public GCS_MAVLINK
{

public:

    void data_stream_send(void) override;

protected:

    // telem_delay is not used by Tracker but is pure virtual, thus
    // this implementaiton.  it probably *should* be used by Tracker,
    // as currently Tracker may brick XBees
    uint32_t telem_delay() const override { return 0; }

    Compass *get_compass() const override;
    AP_Mission *get_mission() override { return nullptr; };
    AP_Rally *get_rally() const override { return nullptr; };
    AP_Camera *get_camera() const override { return nullptr; };
    const AP_FWVersion &get_fwver() const override;
    void set_ekf_origin(const Location& loc) override;

    uint8_t sysid_my_gcs() const override;

    bool set_mode(uint8_t mode) override;

    MAV_RESULT _handle_command_preflight_calibration_baro() override;

private:

    void packetReceived(const mavlink_status_t &status, mavlink_message_t &msg) override;
    void mavlink_check_target(const mavlink_message_t &msg);
    void handleMessage(mavlink_message_t * msg) override;
    bool handle_guided_request(AP_Mission::Mission_Command &cmd) override;
    void handle_change_alt_request(AP_Mission::Mission_Command &cmd) override;
    bool try_send_message(enum ap_message id) override;

    MAV_TYPE frame_type() const override;
    MAV_MODE base_mode() const override;
    uint32_t custom_mode() const override;
    MAV_STATE system_status() const override;
};


env: #include "GCS_Tracker.h"
#include "Tracker.h"

void GCS_Tracker::request_datastream_position(const uint8_t sysid, const uint8_t compid)
{
    for (uint8_t i=0; i < num_gcs(); i++) {
        if (gcs().chan(i).initialised) {
            // request position
            if (HAVE_PAYLOAD_SPACE((mavlink_channel_t)i, DATA_STREAM)) {
                mavlink_msg_request_data_stream_send(
                    (mavlink_channel_t)i,
                    sysid,
                    compid,
                    MAV_DATA_STREAM_POSITION,
                    tracker.g.mavlink_update_rate,
                    1); // start streaming
            }
        }
    }
}

void GCS_Tracker::request_datastream_airpressure(const uint8_t sysid, const uint8_t compid)
{
    for (uint8_t i=0; i < num_gcs(); i++) {
        if (gcs().chan(i).initialised) {
            // request air pressure
            if (HAVE_PAYLOAD_SPACE((mavlink_channel_t)i, DATA_STREAM)) {
                mavlink_msg_request_data_stream_send(
                    (mavlink_channel_t)i,
                    sysid,
                    compid,
                    MAV_DATA_STREAM_RAW_SENSORS,
                    tracker.g.mavlink_update_rate,
                    1); // start streaming
            }
        }
    }
} 
  # Path to the CMake build directory.
  build: #pragma once

#include <GCS_MAVLink/GCS.h>
#include "GCS_Mavlink.h"

class GCS_Tracker : public GCS
{
    friend class Tracker; // for access to _chan in parameter declarations
    friend class GCS_MAVLINK_Tracker;

public:

    // return the number of valid GCS objects
    uint8_t num_gcs() const override { return ARRAY_SIZE(_chan); };

    // return GCS link at offset ofs
    GCS_MAVLINK_Tracker &chan(const uint8_t ofs) override { return _chan[ofs]; };
    const GCS_MAVLINK_Tracker &chan(const uint8_t ofs) const override { return _chan[ofs]; };

private:

    void request_datastream_position(uint8_t sysid, uint8_t compid);
    void request_datastream_airpressure(uint8_t sysid, uint8_t compid);

    GCS_MAVLINK_Tracker _chan[MAVLINK_COMM_NUM_BUFFERS];

}; 

permissions: #include "Tracker.h"

#if LOGGING_ENABLED == ENABLED

// Code to Write and Read packets from DataFlash log memory

// Write an attitude packet
void Tracker::Log_Write_Attitude()
{
    Vector3f targets;
    targets.y = nav_status.pitch * 100.0f;
    targets.z = wrap_360_cd(nav_status.bearing * 100.0f);
    DataFlash.Log_Write_Attitude(ahrs, targets);
    DataFlash.Log_Write_EKF(ahrs);
    DataFlash.Log_Write_AHRS2(ahrs);
#if CONFIG_HAL_BOARD == HAL_BOARD_SITL
    sitl.Log_Write_SIMSTATE(&DataFlash);
#endif
    DataFlash.Log_Write_POS(ahrs);
}

struct PACKED log_Vehicle_Baro {
    LOG_PACKET_HEADER;
    uint64_t time_us;
    float    press;
    float    alt_diff;
};

// Write a vehicle baro packet
void Tracker::Log_Write_Vehicle_Baro(float pressure, float altitude)
{
    struct log_Vehicle_Baro pkt = {
        LOG_PACKET_HEADER_INIT(LOG_V_BAR_MSG),
        time_us         : AP_HAL::micros64(),
        press           : pressure,
        alt_diff        : altitude
    };
    DataFlash.WriteBlock(&pkt, sizeof(pkt));
}

struct PACKED log_Vehicle_Pos {
    LOG_PACKET_HEADER;
    uint64_t time_us;
    int32_t vehicle_lat;
    int32_t vehicle_lng;
    int32_t vehicle_alt;
    float vehicle_vel_x;
    float vehicle_vel_y;
    float vehicle_vel_z;
};

// Write a vehicle pos packet
void Tracker::Log_Write_Vehicle_Pos(int32_t lat, int32_t lng, int32_t alt, const Vector3f& vel)
{
    struct log_Vehicle_Pos pkt = {
        LOG_PACKET_HEADER_INIT(LOG_V_POS_MSG),
        time_us         : AP_HAL::micros64(),
        vehicle_lat     : lat,
        vehicle_lng     : lng,
        vehicle_alt     : alt,
        vehicle_vel_x   : vel.x,
        vehicle_vel_y   : vel.y,
        vehicle_vel_z   : vel.z,
    };
    DataFlash.WriteBlock(&pkt, sizeof(pkt));
}

// type and unit information can be found in
// libraries/DataFlash/Logstructure.h; search for "log_Units" for
// units and "Format characters" for field type information
const struct LogStructure Tracker::log_structure[] = {
    LOG_COMMON_STRUCTURES,
    {LOG_V_BAR_MSG, sizeof(log_Vehicle_Baro),
        "VBAR", "Qff", "TimeUS,Press,AltDiff", "sPm", "F00" },
    {LOG_V_POS_MSG, sizeof(log_Vehicle_Pos),
        "VPOS", "QLLefff", "TimeUS,Lat,Lng,Alt,VelX,VelY,VelZ", "sddmnnn", "FGGB000" }
};

void Tracker::Log_Write_Vehicle_Startup_Messages()
{
    DataFlash.Log_Write_Mode(control_mode, MODE_REASON_INITIALISED);
    gps.Write_DataFlash_Log_Startup_messages();
}

void Tracker::log_init(void)
{
    DataFlash.Init(log_structure, ARRAY_SIZE(log_structure));
}

#else // LOGGING_ENABLED

void Tracker::Log_Write_Attitude(void) {}

void Tracker::log_init(void) {}
void Tracker::Log_Write_Vehicle_Pos(int32_t lat, int32_t lng, int32_t alt, const Vector3f& vel) {}
void Tracker::Log_Write_Vehicle_Baro(float pressure, float altitude) {}

#endif // LOGGING_ENABLED 
  contents: include ../mk/apm.mk 

jobs: #include "Tracker.h"

/*
 *  AntennaTracker parameter definitions
 *
 */

#define GSCALAR(v, name, def) { tracker.g.v.vtype, name, Parameters::k_param_ ## v, &tracker.g.v, {def_value : def} }
#define ASCALAR(v, name, def) { tracker.aparm.v.vtype, name, Parameters::k_param_ ## v, (const void *)&tracker.aparm.v, {def_value : def} }
#define GGROUP(v, name, class) { AP_PARAM_GROUP, name, Parameters::k_param_ ## v, &tracker.g.v, {group_info : class::var_info} }
#define GOBJECT(v, name, class) { AP_PARAM_GROUP, name, Parameters::k_param_ ## v, (const void *)&tracker.v, {group_info : class::var_info} }
#define GOBJECTN(v, pname, name, class) { AP_PARAM_GROUP, name, Parameters::k_param_ ## pname, (const void *)&tracker.v, {group_info : class::var_info} }

const AP_Param::Info Tracker::var_info[] = {
    // @Param: FORMAT_VERSION
    // @DisplayName: Eeprom format version number
    // @Description: This value is incremented when changes are made to the eeprom format
    // @User: Advanced
    GSCALAR(format_version,         "FORMAT_VERSION", 0),

    // @Param: SYSID_SW_TYPE
    // @DisplayName: Software Type
    // @Description: This is used by the ground station to recognise the software type (eg ArduPlane vs ArduCopter)
    // @Values: 0:ArduPlane,4:AntennaTracker,10:Copter,20:Rover,40:ArduSub
    // @User: Advanced
    // @ReadOnly: True
    GSCALAR(software_type,          "SYSID_SW_TYPE",  Parameters::k_software_type),

    // @Param: SYSID_THISMAV
    // @DisplayName: MAVLink system ID of this vehicle
    // @Description: Allows setting an individual system id for this vehicle to distinguish it from others on the same network
    // @Range: 1 255
    // @User: Advanced
    GSCALAR(sysid_this_mav,         "SYSID_THISMAV",  MAV_SYSTEM_ID),

    // @Param: SYSID_MYGCS
    // @DisplayName: Ground station MAVLink system ID
    // @Description: The identifier of the ground station in the MAVLink protocol. Don't change this unless you also modify the ground station to match.
    // @Range: 1 255
    // @User: Advanced
    GSCALAR(sysid_my_gcs,           "SYSID_MYGCS",    255),

    // @Param: SYSID_TARGET
    // @DisplayName: Target vehicle's MAVLink system ID
    // @Description: The identifier of the vehicle being tracked. This should be zero (to auto detect) or be the same as the SYSID_THISMAV parameter of the vehicle being tracked.
    // @Range: 1 255
    // @User: Advanced
    GSCALAR(sysid_target,           "SYSID_TARGET",    0),

    // @Param: MAG_ENABLE
    // @DisplayName: Enable Compass
    // @Description: Setting this to Enabled(1) will enable the compass. Setting this to Disabled(0) will disable the compass. Note that this is separate from COMPASS_USE. This will enable the low level senor, and will enable logging of magnetometer data. To use the compass for navigation you must also set COMPASS_USE to 1.
    // @Values: 0:Disabled,1:Enabled
    // @User: Standard
    GSCALAR(compass_enabled,        "MAG_ENABLE",     1),

    // @Param: YAW_SLEW_TIME
    // @DisplayName: Time for yaw to slew through its full range
    // @Description: This controls how rapidly the tracker will change the servo output for yaw. It is set as the number of seconds to do a full rotation. You can use this parameter to slow the trackers movements, which may help with some types of trackers. A value of zero will allow for unlimited servo movement per update.
    // @Units: s
    // @Increment: 0.1
    // @Range: 0 20
    // @User: Standard
    GSCALAR(yaw_slew_time,          "YAW_SLEW_TIME",    2),

    // @Param: PITCH_SLEW_TIME
    // @DisplayName: Time for pitch to slew through its full range
    // @Description: This controls how rapidly the tracker will change the servo output for pitch. It is set as the number of seconds to do a full range of pitch movement. You can use this parameter to slow the trackers movements, which may help with some types of trackers. A value of zero will allow for unlimited servo movement per update.
    // @Units: s
    // @Increment: 0.1
    // @Range: 0 20
    // @User: Standard
    GSCALAR(pitch_slew_time,        "PITCH_SLEW_TIME",  2),

    // @Param: SCAN_SPEED
    // @DisplayName: Speed at which to rotate in scan mode
    // @Description: This controls how rapidly the tracker will move the servos in SCAN mode
    // @Units: deg/s
    // @Increment: 1
    // @Range: 0 100
    // @User: Standard
    GSCALAR(scan_speed,             "SCAN_SPEED",      5),

    // @Param: MIN_REVERSE_TIME
    // @DisplayName: Minimum time to apply a yaw reversal
    // @Description: When the tracker detects it has reached the limit of servo movement in yaw it will reverse and try moving to the other extreme of yaw. This parameter controls the minimum time it should reverse for. It is used to cope with trackers that have a significant lag in movement to ensure they do move all the way around.
    // @Units: s
    // @Increment: 1
    // @Range: 0 20
    // @User: Standard
    GSCALAR(min_reverse_time,       "MIN_REVERSE_TIME",  1),

    // @Param: START_LATITUDE
    // @DisplayName: Initial Latitude before GPS lock
    // @Description: Combined with START_LONGITUDE this parameter allows for an initial position of the tracker to be set. This position will be used until the GPS gets lock. It can also be used to run a stationary tracker with no GPS attached.
    // @Units: deg
    // @Increment: 0.000001
    // @Range: -90 90
    // @User: Standard
    GSCALAR(start_latitude,         "START_LATITUDE",   0),

    // @Param: START_LONGITUDE
    // @DisplayName: Initial Longitude before GPS lock
    // @Description: Combined with START_LATITUDE this parameter allows for an initial position of the tracker to be set. This position will be used until the GPS gets lock. It can also be used to run a stationary tracker with no GPS attached.
    // @Units: deg
    // @Increment: 0.000001
    // @Range: -180 180
    // @User: Standard
    GSCALAR(start_longitude,        "START_LONGITUDE",  0),

    // @Param: STARTUP_DELAY
    // @DisplayName: Delay before first servo movement from trim
    // @Description: This parameter can be used to force the servos to their trim value for a time on startup. This can help with some servo types
    // @Units: s
    // @Increment: 0.1
    // @Range: 0 10
    // @User: Standard
    GSCALAR(startup_delay,          "STARTUP_DELAY",   0),

    // @Param: SERVO_PITCH_TYPE
    // @DisplayName: Type of servo system being used for pitch
    // @Description: This allows selection of position servos or on/off servos for pitch
    // @Values: 0:Position,1:OnOff,2:ContinuousRotation
    // @User: Standard
    GSCALAR(servo_pitch_type,          "SERVO_PITCH_TYPE",   SERVO_TYPE_POSITION),

    // @Param: SERVO_YAW_TYPE
    // @DisplayName: Type of servo system being used for yaw
    // @Description: This allows selection of position servos or on/off servos for yaw
    // @Values: 0:Position,1:OnOff,2:ContinuousRotation
    // @User: Standard
    GSCALAR(servo_yaw_type,          "SERVO_YAW_TYPE",   SERVO_TYPE_POSITION),

    // @Param: ONOFF_YAW_RATE
    // @DisplayName: Yaw rate for on/off servos
    // @Description: Rate of change of yaw in degrees/second for on/off servos
    // @Units: deg/s
    // @Increment: 0.1
    // @Range: 0 50
    // @User: Standard
    GSCALAR(onoff_yaw_rate,      "ONOFF_YAW_RATE", 9.0f),

    // @Param: ONOFF_PITCH_RATE
    // @DisplayName: Pitch rate for on/off servos
    // @Description: Rate of change of pitch in degrees/second for on/off servos
    // @Units: deg/s
    // @Increment: 0.1
    // @Range: 0 50
    // @User: Standard
    GSCALAR(onoff_pitch_rate,      "ONOFF_PITCH_RATE", 1.0f),

    // @Param: ONOFF_YAW_MINT
    // @DisplayName: Yaw minimum movement time
    // @Description: Minimum amount of time in seconds to move in yaw
    // @Units: s
    // @Increment: 0.01
    // @Range: 0 2
    // @User: Standard
    GSCALAR(onoff_yaw_mintime,     "ONOFF_YAW_MINT", 0.1f),

    // @Param: ONOFF_PITCH_MINT
    // @DisplayName: Pitch minimum movement time
    // @Description: Minimim amount of time in seconds to move in pitch
    // @Units: s
    // @Increment: 0.01
    // @Range: 0 2
    // @User: Standard
    GSCALAR(onoff_pitch_mintime,   "ONOFF_PITCH_MINT", 0.1f),

    // @Param: YAW_TRIM
    // @DisplayName: Yaw trim
    // @Description: Amount of extra yaw to add when tracking. This allows for small adjustments for an out of trim compass.
    // @Units: deg
    // @Increment: 0.1
    // @Range: -10 10
    // @User: Standard
    GSCALAR(yaw_trim,              "YAW_TRIM", 0),

    // @Param: PITCH_TRIM
    // @DisplayName: Pitch trim
    // @Description: Amount of extra pitch to add when tracking. This allows for small adjustments for a badly calibrated barometer.
    // @Units: deg
    // @Increment: 0.1
    // @Range: -10 10
    // @User: Standard
    GSCALAR(pitch_trim,              "PITCH_TRIM", 0),

    // @Param: YAW_RANGE
    // @DisplayName: Yaw Angle Range
    // @Description: Yaw axis total range of motion in degrees
    // @Units: deg
    // @Increment: 0.1
    // @Range: 0 360
    // @User: Standard
    GSCALAR(yaw_range,              "YAW_RANGE", YAW_RANGE_DEFAULT),

    // @Param: DISTANCE_MIN
    // @DisplayName: Distance minimum to target
    // @Description: Tracker will track targets at least this distance away
    // @Units: m
    // @Increment: 1
    // @Range: 0 100
    // @User: Standard
    GSCALAR(distance_min,           "DISTANCE_MIN", DISTANCE_MIN_DEFAULT),

    // @Param: ALT_SOURCE
    // @DisplayName: Altitude Source
    // @Description: What provides altitude information for vehicle. Vehicle only assumes tracker has same altitude as vehicle's home
    // @Values: 0:Barometer,1:GPS,2:GPS vehicle only
    // @User: Standard
    GSCALAR(alt_source,				"ALT_SOURCE",	0),

    // @Param: MAV_UPDATE_RATE
    // @DisplayName: Mavlink Update Rate
    // @Description: The rate at which Mavlink updates position and baro data
    // @Units: Hz
    // @Increment: 1
    // @Range: 1 10
    // @User: Standard
    GSCALAR(mavlink_update_rate,	"MAV_UPDATE_RATE",	1),

    // @Param: PITCH_MIN
    // @DisplayName: Minimum Pitch Angle
    // @Description: The lowest angle the pitch can reach
    // @Units: deg
    // @Increment: 1
    // @Range: -90 0
    // @User: Standard
    GSCALAR(pitch_min,               "PITCH_MIN",	PITCH_MIN_DEFAULT),

    // @Param: PITCH_MAX
    // @DisplayName: Maximum Pitch Angle
    // @Description: The highest angle the pitch can reach
    // @Units: deg
    // @Increment: 1
    // @Range: 0 90
    // @User: Standard
    GSCALAR(pitch_max,               "PITCH_MAX",	PITCH_MAX_DEFAULT),

    // barometer ground calibration. The GND_ prefix is chosen for
    // compatibility with previous releases of ArduPlane
    // @Group: GND_
    // @Path: ../libraries/AP_Baro/AP_Baro.cpp
    GOBJECT(barometer, "GND_", AP_Baro),

    // @Group: COMPASS_
    // @Path: ../libraries/AP_Compass/AP_Compass.cpp
    GOBJECT(compass,                "COMPASS_",     Compass),

    // @Group: SCHED_
    // @Path: ../libraries/AP_Scheduler/AP_Scheduler.cpp
    GOBJECT(scheduler, "SCHED_", AP_Scheduler),

    // @Group: SR0_
    // @Path: GCS_Mavlink.cpp
    GOBJECTN(gcs().chan(0), gcs0,        "SR0_",     GCS_MAVLINK),

    // @Group: SR1_
    // @Path: GCS_Mavlink.cpp
    GOBJECTN(gcs().chan(1),  gcs1,       "SR1_",     GCS_MAVLINK),

    // @Group: SR2_
    // @Path: GCS_Mavlink.cpp
    GOBJECTN(gcs().chan(2),  gcs2,       "SR2_",     GCS_MAVLINK),

    // @Group: SR3_
    // @Path: GCS_Mavlink.cpp
    GOBJECTN(gcs().chan(3),  gcs3,       "SR3_",     GCS_MAVLINK),

    // @Param: LOG_BITMASK
    // @DisplayName: Log bitmask
    // @Description: 4 byte bitmap of log types to enable
    // @Values: 127:Default,0:Disabled
    // @Bitmask: 0:ATTITUDE,1:GPS,2:RCIN,3:IMU,4:RCOUT,5:COMPASS,6:Battery
    // @User: Standard
    GSCALAR(log_bitmask, "LOG_BITMASK", DEFAULT_LOG_BITMASK),

    // @Group: INS_
    // @Path: ../libraries/AP_InertialSensor/AP_InertialSensor.cpp
    GOBJECT(ins,                    "INS_", AP_InertialSensor),

    // @Group: AHRS_
    // @Path: ../libraries/AP_AHRS/AP_AHRS.cpp
    GOBJECT(ahrs,                   "AHRS_",    AP_AHRS),

#if CONFIG_HAL_BOARD == HAL_BOARD_SITL
    // @Group: SIM_
    // @Path: ../libraries/SITL/SITL.cpp
    GOBJECT(sitl, "SIM_", SITL::SITL),
#endif

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

    // @Group: NTF_
    // @Path: ../libraries/AP_Notify/AP_Notify.cpp
    GOBJECT(notify, "NTF_",  AP_Notify),

    // @Path: ../libraries/RC_Channel/RC_Channels.cpp
    GOBJECT(rc_channels,     "RC", RC_Channels),

    // @Path: ../libraries/SRV_Channel/SRV_Channels.cpp
    GOBJECT(servo_channels,     "SERVO", SRV_Channels),
    
    // @Group: SERIAL
    // @Path: ../libraries/AP_SerialManager/AP_SerialManager.cpp
    GOBJECT(serial_manager,    "SERIAL",   AP_SerialManager),

    // @Param: PITCH2SRV_P
    // @DisplayName: Pitch axis controller P gain
    // @Description: Pitch axis controller P gain.  Converts the difference between desired pitch angle and actual pitch angle into a pitch servo pwm change
    // @Range: 0.0 3.0
    // @Increment: 0.01
    // @User: Standard

    // @Param: PITCH2SRV_I
    // @DisplayName: Pitch axis controller I gain
    // @Description: Pitch axis controller I gain.  Corrects long-term difference in desired pitch angle vs actual pitch angle
    // @Range: 0.0 3.0
    // @Increment: 0.01
    // @User: Standard

    // @Param: PITCH2SRV_IMAX
    // @DisplayName: Pitch axis controller I gain maximum
    // @Description: Pitch axis controller I gain maximum.  Constrains the maximum pwm change that the I gain will output
    // @Range: 0 4000
    // @Increment: 10
    // @Units: d%
    // @User: Standard

    // @Param: PITCH2SRV_D
    // @DisplayName: Pitch axis controller D gain
    // @Description: Pitch axis controller D gain.  Compensates for short-term change in desired pitch angle vs actual pitch angle
    // @Range: 0.001 0.1
    // @Increment: 0.001
    // @User: Standard
	GGROUP(pidPitch2Srv,       "PITCH2SRV_", AC_PID),

    // @Param: YAW2SRV_P
    // @DisplayName: Yaw axis controller P gain
    // @Description: Yaw axis controller P gain.  Converts the difference between desired yaw angle (heading) and actual yaw angle into a yaw servo pwm change
    // @Range: 0.0 3.0
    // @Increment: 0.01
    // @User: Standard

    // @Param: YAW2SRV_I
    // @DisplayName: Yaw axis controller I gain
    // @Description: Yaw axis controller I gain.  Corrects long-term difference in desired yaw angle (heading) vs actual yaw angle
    // @Range: 0.0 3.0
    // @Increment: 0.01
    // @User: Standard

    // @Param: YAW2SRV_IMAX
    // @DisplayName: Yaw axis controller I gain maximum
    // @Description: Yaw axis controller I gain maximum.  Constrains the maximum pwm change that the I gain will output
    // @Range: 0 4000
    // @Increment: 10
    // @Units: d%
    // @User: Standard

    // @Param: YAW2SRV_D
    // @DisplayName: Yaw axis controller D gain
    // @Description: Yaw axis controller D gain.  Compensates for short-term change in desired yaw angle (heading) vs actual yaw angle
    // @Range: 0.001 0.1
    // @Increment: 0.001
    // @User: Standard
	GGROUP(pidYaw2Srv,         "YAW2SRV_", AC_PID),

    // @Param: CMD_TOTAL
    // @DisplayName: Number of loaded mission items
    // @Description: Set to 1 if HOME location has been loaded by the ground station. Do not change this manually.
    // @Range: 1 255
    // @User: Advanced
    GSCALAR(command_total,          "CMD_TOTAL",      0),

    // @Group: BATT
    // @Path: ../libraries/AP_BattMonitor/AP_BattMonitor.cpp
    GOBJECT(battery,                "BATT", AP_BattMonitor),

    AP_VAREND
};


void Tracker::load_parameters(void)
{
    if (!g.format_version.load() ||
        g.format_version != Parameters::k_format_version) {

        // erase all parameters
        hal.console->printf("Firmware change: erasing EEPROM...\n");
        AP_Param::erase_all();

        // save the current format version
        g.format_version.set_and_save(Parameters::k_format_version);
        hal.console->printf("done.\n");
    }

    uint32_t before = AP_HAL::micros();
    // Load all auto-loaded EEPROM variables
    AP_Param::load_all();
    hal.console->printf("load_all took %luus\n", (unsigned long)(AP_HAL::micros() - before));
} 
  analyze: #pragma once

#include <AP_Common/AP_Common.h>

// Global parameter class.
//
class Parameters {
public:

    /*
     *  The value of k_format_version determines whether the existing
     *  eeprom data is considered valid. You should only change this
     *  value under the following circumstances:
     *
     *  1) the meaning of an existing eeprom parameter changes
     *
     *  2) the value of an existing k_param_* enum value changes
     *
     *  Adding a new parameter should _not_ require a change to
     *  k_format_version except under special circumstances. If you
     *  change it anyway then all ArduPlane users will need to reload all
     *  their parameters. We want that to be an extremely rare
     *  thing. Please do not just change it "just in case".
     *
     *  To determine if a k_param_* value has changed, use the rules of
     *  C++ enums to work out the value of the neighboring enum
     *  values. If you don't know the C++ enum rules then please ask for
     *  help.
     */

    //////////////////////////////////////////////////////////////////
    // STOP!!! DO NOT CHANGE THIS VALUE UNTIL YOU FULLY UNDERSTAND THE
    // COMMENTS ABOVE. IF UNSURE, ASK ANOTHER DEVELOPER!!!
    static const uint16_t k_format_version = 1;
    //////////////////////////////////////////////////////////////////


    // The parameter software_type is set up solely for ground station use
    // and identifies the software type (eg ArduPilotMega versus ArduCopterMega)
    // GCS will interpret values 0-9 as ArduPilotMega.  Developers may use
    // values within that range to identify different branches.
    //
    static const uint16_t k_software_type = 4;

    enum {
        // Layout version number, always key zero.
        //
        k_param_format_version = 0,
        k_param_software_type,

        k_param_gcs0 = 100,         // stream rates for uartA
        k_param_gcs1,               // stream rates for uartC
        k_param_sysid_this_mav,
        k_param_sysid_my_gcs,
        k_param_serial0_baud,       // deprecated
        k_param_serial1_baud,       // deprecated
        k_param_imu,
        k_param_compass_enabled,
        k_param_compass,
        k_param_ahrs,  // AHRS group
        k_param_barometer,
        k_param_scheduler,
        k_param_ins,
        k_param_sitl,
        k_param_pidPitch_old,   // deprecated
        k_param_pidYaw_old,     // deprecated
        k_param_gcs2,               // stream rates for uartD
        k_param_serial2_baud,       // deprecated

        k_param_yaw_slew_time,
        k_param_pitch_slew_time,
        k_param_min_reverse_time,

        k_param_start_latitude,
        k_param_start_longitude,
        k_param_startup_delay,
        k_param_BoardConfig,
        k_param_gps,
        k_param_scan_speed,
        k_param_proxy_mode_unused, // deprecated
        k_param_servo_pitch_type,
        k_param_onoff_yaw_rate,
        k_param_onoff_pitch_rate,
        k_param_onoff_yaw_mintime,
        k_param_onoff_pitch_mintime,
        k_param_yaw_trim,
        k_param_pitch_trim,
        k_param_yaw_range,
        k_param_pitch_range,	//deprecated
        k_param_distance_min,
        k_param_sysid_target,       // 138
        k_param_gcs3,               // stream rates for fourth MAVLink port
        k_param_log_bitmask,        // 140
        k_param_notify,
        k_param_BoardConfig_CAN,
        k_param_battery,

        //
        // 150: Telemetry control
        //
        k_param_serial_manager,     // serial manager library
        k_param_servo_yaw_type,
        k_param_alt_source,
        k_param_mavlink_update_rate,
        k_param_pitch_min,
        k_param_pitch_max,

        //
        // 200 : Radio settings
        //
        k_param_channel_yaw_old = 200,
        k_param_channel_pitch_old,
        k_param_pidPitch2Srv,
        k_param_pidYaw2Srv,
        k_param_rc_channels,
        k_param_servo_channels,

        //
        // 220: Waypoint data
        //
        k_param_command_total = 220,

        // 254,255: reserved
    };

    AP_Int16 format_version;
    AP_Int8 software_type;

    // Telemetry control
    //
    AP_Int16 sysid_this_mav;
    AP_Int16 sysid_my_gcs;
    AP_Int16 sysid_target;

    AP_Int8 compass_enabled;

    AP_Float yaw_slew_time;
    AP_Float pitch_slew_time;
    AP_Float min_reverse_time;
    AP_Float scan_speed;

    AP_Float start_latitude;
    AP_Float start_longitude;

    AP_Float startup_delay;
    AP_Int8  servo_pitch_type;
    AP_Int8  servo_yaw_type;
    AP_Int8  alt_source;
    AP_Int8  mavlink_update_rate;
    AP_Float onoff_yaw_rate;
    AP_Float onoff_pitch_rate;
    AP_Float onoff_yaw_mintime;
    AP_Float onoff_pitch_mintime;
    AP_Float yaw_trim;
    AP_Float pitch_trim;
    AP_Int16 yaw_range;             // yaw axis total range of motion in degrees
    AP_Int16 distance_min;          // target's must be at least this distance from tracker to be tracked
    AP_Int16 pitch_min;
    AP_Int16 pitch_max;

    // Waypoints
    //
    AP_Int8 command_total; // 1 if HOME is set

    AP_Int32 log_bitmask;

    // AC_PID controllers
    AC_PID         pidPitch2Srv;
    AC_PID         pidYaw2Srv;

    Parameters() :
        pidPitch2Srv(0.2, 0, 0.05f, 4000.0f, 0.1, 0.02f),
        pidYaw2Srv  (0.2, 0, 0.05f, 4000.0f, 0.1, 0.02f)
        {}
};

extern const AP_Param::Info var_info[]; 
    permissions: Antenna Tracker Release Notes:
------------------------------------------------------------------
AntennaTracker 1.0.0 28-Jul-2016
Changes from 0.8.0
1) Compensate for tracker tilt by converting earth-frame angle targets to body-frame servo outputs
2) Tracker location provided by EKF (still falls back to raw GPS if EKF is unable to provide position estimate)
3) PITCH_MIN, PITCH_MAX parameters replace PITCH_RANGE
------------------------------------------------------------------
AntennaTracker 0.8.0 22-Jun-2016
Changes from 0.7.8
1) Added PITCH2SRV_FILT, YAW2SRV_FILT added to smooth input to controllers.  Lower values lead to more delay but smoother motion.
2) Estimate vehicle's altitude between telemetry updates when using ALT_SOURCE = GPS
3) Bug fix to vehicle position estimate (was using vehicle's heading instead of 3D velocity vector)
4) Added MAV_UPDATE_RATE parameter to allow more easily setting the rate the vehicle sends position data
------------------------------------------------------------------
AntennaTracker 0.7.8 10-Jun-2016
Changes from 0.7.7
1) Bug fix to VBAR dataflash logging
2) VPOS dataflash logging message captures vehicle position
------------------------------------------------------------------
AntennaTracker 0.7.7 31-May-2016
Changes from 0.7.6
1) SERVO_TYPE parameter split into SERVO_PITCH_TYPE, SERVO_YAW_TYPE to allow different servo types for each axis
2) ALT_SOURCE parameter added to allow selecting vehicle's GPS as an altitude source instead of vehicle's baro
3) VBAR dataflash log message added to capture vehicle barometer data
------------------------------------------------------------------
AntennaTracker 0.7.6 08-Feb-2016
Changes from 0.7.5
1) Fix logging of attitude and mode to dataflash 
------------------------------------------------------------------
AntennaTracker 0.7.5 27-Dec-2015
Changes from 0.7.4
1) Add dataflash logging
------------------------------------------------------------------
AntennaTracker 0.7.4 23-Dec-2015
Changes from 0.7.2
1) Request baro pressure from vehicle at 1hz
------------------------------------------------------------------
AntennaTracker 0.7.2 1-Aug-2015
Changes from 0.7.1
1) Fixed Pitch
------------------------------------------------------------------
AntennaTracker 0.7.1 29-May-2015
Changes from 0.7
1) Added support for continuous rotation (CR) servos
------------------------------------------------------------------
AntennaTracker 0.7 17-May-2015
Changes from 0.5 (skipped 0.6 to avoid confusion after the wrong version number was communicated on diydrones.com)
1) added support for 4th MAVLink channel
------------------------------------------------------------------
AntennaTracker 0.5 29-Apr-2015
Changes from 0.4
1) add SERVO_TEST mode which moves the servos to the position specified by a do-set-servo command (normally from MP's extended tuning page)
2) add DISTANCE_MIN parameter (default to 5m).  Vehicles must be at least this distance away for the tracker to track them.
3) add SYSID_TARGET parameter to specify which vehicle to follow which is useful if multiple vehicles are sharing the same network.  This param defaults to zero which means track the first vehicle.  Set to "0" to track the first vehicle found.
4) send request to vehicle for position updates at 1hz.  Improves reliability when used with copter which won't sent position data by default.
5) Listens for vehicle position updates on all channels
6) bug fix for LED on pixhawk so it flashes blue or green (instead of yellow) when waiting for he vehicle's position
------------------------------------------------------------------
AntennaTracker 0.4 31-Jan-2015
Changes from 0.3
1) Init Telem2 port so it can be used to communicate with GCS
------------------------------------------------------------------
AntennaTracker 0.3 28-Jan-2015
Changes from 0.2
1) MAVLink routing replaces Proxy mode
------------------------------------------------------------------ 
      contents: /*
   Lead developers:

   Please contribute your ideas! See http://dev.ardupilot.org for details

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

////////////////////////////////////////////////////////////////////////////////
// Header includes
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <stdarg.h>
#include <stdio.h>

#include <AP_Common/AP_Common.h>
#include <AP_HAL/AP_HAL.h>
#include <AP_Param/AP_Param.h>
#include <StorageManager/StorageManager.h>
#include <AP_GPS/AP_GPS.h>         // ArduPilot GPS library
#include <AP_Baro/AP_Baro.h>        // ArduPilot barometer library
#include <AP_Compass/AP_Compass.h>     // ArduPilot Mega Magnetometer Library
#include <AP_Math/AP_Math.h>        // ArduPilot Mega Vector/Matrix math Library
#include <AP_ADC/AP_ADC.h>         // ArduPilot Mega Analog to Digital Converter Library
#include <AP_InertialSensor/AP_InertialSensor.h> // Inertial Sensor Library
#include <AP_AccelCal/AP_AccelCal.h>                // interface and maths for accelerometer calibration
#include <AP_AHRS/AP_AHRS.h>         // ArduPilot Mega DCM Library
#include <Filter/Filter.h>                     // Filter library
#include <AP_Buffer/AP_Buffer.h>      // APM FIFO Buffer

#include <AP_SerialManager/AP_SerialManager.h>   // Serial manager library
#include <AP_Declination/AP_Declination.h> // ArduPilot Mega Declination Helper Library
#include <DataFlash/DataFlash.h>
#include <AC_PID/AC_PID.h>
#include <AP_Scheduler/AP_Scheduler.h>       // main loop scheduler
#include <AP_NavEKF2/AP_NavEKF2.h>
#include <AP_NavEKF3/AP_NavEKF3.h>

#include <AP_Vehicle/AP_Vehicle.h>
#include <AP_Mission/AP_Mission.h>
#include <AP_Terrain/AP_Terrain.h>
#include <AP_Rally/AP_Rally.h>
#include <AP_Notify/AP_Notify.h>      // Notify library
#include <AP_BattMonitor/AP_BattMonitor.h> // Battery monitor library
#include <AP_Airspeed/AP_Airspeed.h>
#include <RC_Channel/RC_Channel.h>
#include <AP_BoardConfig/AP_BoardConfig.h>
#include <AP_BoardConfig/AP_BoardConfig_CAN.h>
#include <AP_OpticalFlow/AP_OpticalFlow.h>
#include <AP_RangeFinder/AP_RangeFinder.h>
#include <AP_Beacon/AP_Beacon.h>

// Configuration
#include "config.h"
#include "defines.h"

#include "Parameters.h"
#include "GCS_Mavlink.h"
#include "GCS_Tracker.h"

#if CONFIG_HAL_BOARD == HAL_BOARD_SITL
#include <SITL/SITL.h>
#endif

class Tracker : public AP_HAL::HAL::Callbacks {
public:
    friend class GCS_MAVLINK_Tracker;
    friend class GCS_Tracker;
    friend class Parameters;

    Tracker(void);

    static const AP_FWVersion fwver;

    // HAL::Callbacks implementation.
    void setup() override;
    void loop() override;

private:
    Parameters g;

    // main loop scheduler
    AP_Scheduler scheduler;

    // notification object for LEDs, buzzers etc
    AP_Notify notify;

    uint32_t start_time_ms = 0;

    bool usb_connected = false;

    DataFlash_Class DataFlash;

    AP_GPS gps;

    AP_Baro barometer;

    Compass compass;

    AP_InertialSensor ins;

    RangeFinder rng{serial_manager, ROTATION_NONE};

// Inertial Navigation EKF
#if AP_AHRS_NAVEKF_AVAILABLE
    NavEKF2 EKF2{&ahrs, rng};
    NavEKF3 EKF3{&ahrs, rng};
    AP_AHRS_NavEKF ahrs{EKF2, EKF3};
#else
    AP_AHRS_DCM ahrs;
#endif

#if CONFIG_HAL_BOARD == HAL_BOARD_SITL
    SITL::SITL sitl;
#endif
    
    /**
       antenna control channels
    */
    RC_Channels rc_channels;
    SRV_Channels servo_channels;

    LowPassFilterFloat yaw_servo_out_filt;
    LowPassFilterFloat pitch_servo_out_filt;

    bool yaw_servo_out_filt_init = false;
    bool pitch_servo_out_filt_init = false;

    AP_SerialManager serial_manager;
    GCS_Tracker _gcs; // avoid using this; use gcs()
    GCS_Tracker &gcs() { return _gcs; }

    AP_BoardConfig BoardConfig;

#if HAL_WITH_UAVCAN
    // board specific config for CAN bus
    AP_BoardConfig_CAN BoardConfig_CAN;
#endif

    // Battery Sensors
    AP_BattMonitor battery{MASK_LOG_CURRENT,
                           FUNCTOR_BIND_MEMBER(&Tracker::handle_battery_failsafe, void, const char*, const int8_t),
                           nullptr};

    struct Location current_loc;

    enum ControlMode control_mode  = INITIALISING;

    // Vehicle state
    struct {
        bool location_valid;    // true if we have a valid location for the vehicle
        Location location;      // lat, long in degrees * 10^7; alt in meters * 100
        Location location_estimate; // lat, long in degrees * 10^7; alt in meters * 100
        uint32_t last_update_us;    // last position update in microseconds
        uint32_t last_update_ms;    // last position update in milliseconds
        Vector3f vel;           // the vehicle's velocity in m/s
        int32_t relative_alt;	// the vehicle's relative altitude in meters * 100
    } vehicle;

    // Navigation controller state
    struct {
        float bearing;                  // bearing to vehicle in centi-degrees
        float distance;                 // distance to vehicle in meters
        float pitch;                    // pitch to vehicle in degrees (positive means vehicle is above tracker, negative means below)
        float angle_error_pitch;        // angle error between target and current pitch in centi-degrees
        float angle_error_yaw;          // angle error between target and current yaw in centi-degrees
        float alt_difference_baro;      // altitude difference between tracker and vehicle in meters according to the barometer.  positive value means vehicle is above tracker
        float alt_difference_gps;       // altitude difference between tracker and vehicle in meters according to the gps.  positive value means vehicle is above tracker
        float altitude_offset;          // offset in meters which is added to tracker altitude to align altitude measurements with vehicle's barometer
        bool manual_control_yaw         : 1;// true if tracker yaw is under manual control
        bool manual_control_pitch       : 1;// true if tracker pitch is manually controlled
        bool need_altitude_calibration  : 1;// true if tracker altitude has not been determined (true after startup)
        bool scan_reverse_pitch         : 1;// controls direction of pitch movement in SCAN mode
        bool scan_reverse_yaw           : 1;// controls direction of yaw movement in SCAN mode
    } nav_status = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false, false, true, false, false};

    // setup the var_info table
    AP_Param param_loader{var_info};

    uint8_t one_second_counter = 0;
    bool target_set = false;

    // use this to prevent recursion during sensor init
    bool in_mavlink_delay = false;

    static const AP_Scheduler::Task scheduler_tasks[];
    static const AP_Param::Info var_info[];
    static const struct LogStructure log_structure[];

    void one_second_loop();
    void ten_hz_logging_loop();
    void send_attitude(mavlink_channel_t chan);
    void send_extended_status1(mavlink_channel_t chan);
    void send_location(mavlink_channel_t chan);
    void send_nav_controller_output(mavlink_channel_t chan);
    void send_simstate(mavlink_channel_t chan);
    void gcs_data_stream_send(void);
    void gcs_update(void);
    void gcs_retry_deferred(void);
    void load_parameters(void);
    void update_auto(void);
    void calc_angle_error(float pitch, float yaw, bool direction_reversed);
    void convert_ef_to_bf(float pitch, float yaw, float& bf_pitch, float& bf_yaw);
    bool convert_bf_to_ef(float pitch, float yaw, float& ef_pitch, float& ef_yaw);
    bool get_ef_yaw_direction();
    void update_manual(void);
    void update_scan(void);
    bool servo_test_set_servo(uint8_t servo_num, uint16_t pwm);
    void read_radio();
    void update_ahrs();
    void update_compass(void);
    void compass_accumulate(void);
    void accel_cal_update(void);
    void update_GPS(void);
    void init_servos();
    void update_pitch_servo(float pitch);
    void update_pitch_position_servo(void);
    void update_pitch_cr_servo(float pitch);
    void update_pitch_onoff_servo(float pitch);
    void update_yaw_servo(float yaw);
    void update_yaw_position_servo(void);
    void update_yaw_cr_servo(float yaw);
    void update_yaw_onoff_servo(float yaw);
    void init_tracker();
    bool get_home_eeprom(struct Location &loc);
    void set_home_eeprom(struct Location temp);
    void set_home(struct Location temp);
    void set_ekf_origin(const Location& loc);
    void arm_servos();
    void disarm_servos();
    void prepare_servos();
    void set_mode(enum ControlMode mode, mode_reason_t reason);
    void check_usb_mux(void);
    void update_vehicle_pos_estimate();
    void update_tracker_position();
    void update_bearing_and_distance();
    void update_tracking(void);
    void tracking_update_position(const mavlink_global_position_int_t &msg);
    void tracking_update_pressure(const mavlink_scaled_pressure_t &msg);
    void tracking_manual_control(const mavlink_manual_control_t &msg);
    void update_armed_disarmed();
    void init_capabilities(void);
    void compass_cal_update();
    void Log_Write_Attitude();
    void Log_Write_Vehicle_Pos(int32_t lat,int32_t lng,int32_t alt, const Vector3f& vel);
    void Log_Write_Vehicle_Baro(float pressure, float altitude);
    void Log_Write_Vehicle_Startup_Messages();
    void log_init(void);
    bool should_log(uint32_t mask);
    void handle_battery_failsafe(const char* type_str, const int8_t action);

public:
    void mavlink_delay_cb();
};

extern const AP_HAL::HAL& hal;
extern Tracker tracker; 
      security-events: #include "Tracker.h"

void Tracker::init_capabilities(void)
{
    hal.util->set_capabilities(MAV_PROTOCOL_CAPABILITY_PARAM_FLOAT |
                               MAV_PROTOCOL_CAPABILITY_COMPASS_CALIBRATION);
} 
      actions: //
#pragma once

#include "defines.h"

#include "APM_Config.h" // <== THIS INCLUDE, DO NOT EDIT IT. EVER.

///
/// DO NOT EDIT THIS INCLUDE - if you want to make a local change, make that
/// change in your local copy of APM_Config.h.
///

// Just so that it's completely clear...
#define ENABLED                 1
#define DISABLED                0

// this avoids a very common config error
#define ENABLE ENABLED
#define DISABLE DISABLED

#ifndef MAV_SYSTEM_ID
 // use 2 for antenna tracker by default
 # define MAV_SYSTEM_ID          2
#endif


//////////////////////////////////////////////////////////////////////////////
// RC Channel definitions
//
#ifndef CH_YAW
 # define CH_YAW        CH_1    // RC input/output for yaw on channel 1
#endif
#ifndef CH_PITCH
 # define CH_PITCH      CH_2    // RC input/output for pitch on channel 2
#endif


//////////////////////////////////////////////////////////////////////////////
// yaw and pitch axis angle range defaults
//
#ifndef YAW_RANGE_DEFAULT
 # define YAW_RANGE_DEFAULT 360
#endif
#ifndef PITCH_MIN_DEFAULT
 # define PITCH_MIN_DEFAULT -90
#endif
#ifndef PITCH_MAX_DEFAULT
 # define PITCH_MAX_DEFAULT 90
#endif

//////////////////////////////////////////////////////////////////////////////
// Tracking definitions
//
#ifndef TRACKING_TIMEOUT_MS
 # define TRACKING_TIMEOUT_MS               5000    // consider we've lost track of vehicle after 5 seconds with no position update.  Used to update armed/disarmed status leds
#endif
#ifndef TRACKING_TIMEOUT_SEC
 # define TRACKING_TIMEOUT_SEC              5.0f    // consider we've lost track of vehicle after 5 seconds with no position update.
#endif
#ifndef DISTANCE_MIN_DEFAULT
 # define DISTANCE_MIN_DEFAULT              5.0f    // do not track targets within 5 meters
#endif

//
// Dataflash logging control
//
#ifndef LOGGING_ENABLED
# define LOGGING_ENABLED        ENABLED
#endif

// Default logging bitmask
#ifndef DEFAULT_LOG_BITMASK
 # define DEFAULT_LOG_BITMASK \
    MASK_LOG_ATTITUDE | \
    MASK_LOG_GPS | \
    MASK_LOG_RCIN | \
    MASK_LOG_IMU | \
    MASK_LOG_RCOUT | \
    MASK_LOG_COMPASS | \
    MASK_LOG_CURRENT
#endif 
    name: #include "Tracker.h"

/*
 * Auto control mode
 */

/*
 * update_auto - runs the auto controller
 *  called at 50hz while control_mode is 'AUTO'
 */
void Tracker::update_auto(void)
{
    // exit immediately if we do not have a valid vehicle position
    if (!vehicle.location_valid) {
        return;
    }

    float yaw = wrap_180_cd((nav_status.bearing+g.yaw_trim)*100); // target yaw in centidegrees
    float pitch = constrain_float(nav_status.pitch+g.pitch_trim, -90, 90) * 100; // target pitch in centidegrees

    bool direction_reversed = get_ef_yaw_direction();

    calc_angle_error(pitch, yaw, direction_reversed);

    float bf_pitch;
    float bf_yaw;
    convert_ef_to_bf(pitch, yaw, bf_pitch, bf_yaw);

    // only move servos if target is at least distance_min away
    if ((g.distance_min <= 0) || (nav_status.distance >= g.distance_min)) {
        update_pitch_servo(bf_pitch);
        update_yaw_servo(bf_yaw);
    }
}

void Tracker::calc_angle_error(float pitch, float yaw, bool direction_reversed)
{
    // Pitch angle error in centidegrees
    // Positive error means the target is above current pitch
    // Negative error means the target is below current pitch
    float ahrs_pitch = ahrs.pitch_sensor;
    int32_t ef_pitch_angle_error = pitch - ahrs_pitch;

    // Yaw angle error in centidegrees
    // Positive error means the target is right of current yaw
    // Negative error means the target is left of current yaw
    int32_t ahrs_yaw_cd = wrap_180_cd(ahrs.yaw_sensor);
    int32_t ef_yaw_angle_error = wrap_180_cd(yaw - ahrs_yaw_cd);
    if (direction_reversed) {
        if (ef_yaw_angle_error > 0) {
            ef_yaw_angle_error = (yaw - ahrs_yaw_cd) - 36000;
        } else {
            ef_yaw_angle_error = 36000 + (yaw - ahrs_yaw_cd);
        }
    }

    // earth frame to body frame angle error conversion
    float bf_pitch_err;
    float bf_yaw_err;
    convert_ef_to_bf(ef_pitch_angle_error, ef_yaw_angle_error, bf_pitch_err, bf_yaw_err);
    nav_status.angle_error_pitch = bf_pitch_err;
    nav_status.angle_error_yaw = bf_yaw_err;
}

void Tracker::convert_ef_to_bf(float pitch, float yaw, float& bf_pitch, float& bf_yaw)
{
	// earth frame to body frame pitch and yaw conversion
    bf_pitch = ahrs.cos_roll() * pitch + ahrs.sin_roll() * ahrs.cos_pitch() * yaw;
    bf_yaw = -ahrs.sin_roll() * pitch + ahrs.cos_pitch() * ahrs.cos_roll() * yaw;
}

bool Tracker::convert_bf_to_ef(float pitch, float yaw, float& ef_pitch, float& ef_yaw)
{
    // avoid divide by zero
    if (is_zero(ahrs.cos_pitch())) {
        return false;
    }
    // convert earth frame angle or rates to body frame
    ef_pitch = ahrs.cos_roll() * pitch - ahrs.sin_roll() * yaw;
    ef_yaw = (ahrs.sin_roll() / ahrs.cos_pitch()) * pitch + (ahrs.cos_roll() / ahrs.cos_pitch()) * yaw;
    return true;
}

// return value is true if taking the long road to the target, false if normal, shortest direction should be used
bool Tracker::get_ef_yaw_direction()
{
    // calculating distances from current pitch/yaw to lower and upper limits in centi-degrees
    float yaw_angle_limit_lower =   (-g.yaw_range * 100.0f / 2.0f) - yaw_servo_out_filt.get();
    float yaw_angle_limit_upper =   (g.yaw_range * 100.0f / 2.0f) - yaw_servo_out_filt.get();
    float pitch_angle_limit_lower = (g.pitch_min * 100.0f) - pitch_servo_out_filt.get();
    float pitch_angle_limit_upper = (g.pitch_max * 100.0f) - pitch_servo_out_filt.get();

    // distances to earthframe angle limits in centi-degrees
    float ef_yaw_limit_lower = yaw_angle_limit_lower;
    float ef_yaw_limit_upper = yaw_angle_limit_upper;
    float ef_pitch_limit_lower = pitch_angle_limit_lower;
    float ef_pitch_limit_upper = pitch_angle_limit_upper;
    convert_bf_to_ef(pitch_angle_limit_lower, yaw_angle_limit_lower, ef_pitch_limit_lower, ef_yaw_limit_lower);
    convert_bf_to_ef(pitch_angle_limit_upper, yaw_angle_limit_upper, ef_pitch_limit_upper, ef_yaw_limit_upper);

    float ef_yaw_current = wrap_180_cd(ahrs.yaw_sensor);
    float ef_yaw_target = wrap_180_cd((nav_status.bearing+g.yaw_trim)*100);
    float ef_yaw_angle_error = wrap_180_cd(ef_yaw_target - ef_yaw_current);

    // calculate angle error to target in both directions (i.e. moving up/right or lower/left)
    float yaw_angle_error_upper;
    float yaw_angle_error_lower;
    if (ef_yaw_angle_error >= 0) {
        yaw_angle_error_upper = ef_yaw_angle_error;
        yaw_angle_error_lower = ef_yaw_angle_error - 36000;
    } else {
        yaw_angle_error_upper = 36000 + ef_yaw_angle_error;
        yaw_angle_error_lower = ef_yaw_angle_error;
    }

    // checks that the vehicle is outside the tracker's range
    if ((yaw_angle_error_lower < ef_yaw_limit_lower) && (yaw_angle_error_upper > ef_yaw_limit_upper)) {
        // if the tracker is trying to move clockwise to reach the vehicle,
        // but the tracker could get closer to the vehicle by moving counter-clockwise then set direction_reversed to true
        if (ef_yaw_angle_error>0 && ((ef_yaw_limit_lower - yaw_angle_error_lower) < (yaw_angle_error_upper - ef_yaw_limit_upper))) {
            return true;
        }
        // if the tracker is trying to move counter-clockwise to reach the vehicle,
        // but the tracker could get closer to the vehicle by moving then set direction_reversed to true
        if (ef_yaw_angle_error<0 && ((ef_yaw_limit_lower - yaw_angle_error_lower) > (yaw_angle_error_upper - ef_yaw_limit_upper))) {
            return true;
        }
    }

    // if we get this far we can use the regular, shortest path to the target
    return false;
} 
    runs-on: #include "Tracker.h"

/*
 * Manual control mode
 */

/*
 * update_manual - runs the manual controller
 *  called at 50hz while control_mode is 'MANUAL'
 */
void Tracker::update_manual(void)
{
    // copy yaw and pitch input to output
    SRV_Channels::set_output_pwm(SRV_Channel::k_tracker_yaw, RC_Channels::rc_channel(CH_YAW)->get_radio_in());
    SRV_Channels::constrain_pwm(SRV_Channel::k_tracker_yaw);
    
    SRV_Channels::set_output_pwm(SRV_Channel::k_tracker_pitch, RC_Channels::rc_channel(CH_PITCH)->get_radio_in());
    SRV_Channels::constrain_pwm(SRV_Channel::k_tracker_pitch);
    
    SRV_Channels::calc_pwm();
    SRV_Channels::output_ch_all();
} 

    steps: #include "Tracker.h"

/*
 * Scan control mode
 */

/*
 * update_scan - runs the scan controller
 *  called at 50hz while control_mode is 'SCAN'
 */
void Tracker::update_scan(void)
{
    if (!nav_status.manual_control_yaw) {
        float yaw_delta = g.scan_speed * 0.02f;
        nav_status.bearing   += yaw_delta   * (nav_status.scan_reverse_yaw?-1:1);
        if (nav_status.bearing < 0 && nav_status.scan_reverse_yaw) {
            nav_status.scan_reverse_yaw = false;
        }
        if (nav_status.bearing > 360 && !nav_status.scan_reverse_yaw) {
            nav_status.scan_reverse_yaw = true;
        }
        nav_status.bearing = constrain_float(nav_status.bearing, 0, 360);
    }

    if (!nav_status.manual_control_pitch) {
        float pitch_delta = g.scan_speed * 0.02f;
        nav_status.pitch += pitch_delta * (nav_status.scan_reverse_pitch?-1:1);
        if (nav_status.pitch < -90 && nav_status.scan_reverse_pitch) {
            nav_status.scan_reverse_pitch = false;
        }
        if (nav_status.pitch > 90 && !nav_status.scan_reverse_pitch) {
            nav_status.scan_reverse_pitch = true;
        }
        nav_status.pitch = constrain_float(nav_status.pitch, -90, 90);
    }

    update_auto();
} 
      - name: #include "Tracker.h"

/*
 * GCS controlled servo test mode
 */

/*
 * servo_test_set_servo - sets the yaw or pitch servo pwm directly
 *  servo_num are 1 for yaw, 2 for pitch
 */
bool Tracker::servo_test_set_servo(uint8_t servo_num, uint16_t pwm)
{
    // convert servo_num from 1~2 to 0~1 range
    servo_num--;

    // exit immediately if servo_num is invalid
    if (servo_num != CH_YAW && servo_num != CH_PITCH) {
        return false;
    }

    // ensure we are in servo test mode
    if (control_mode != SERVO_TEST) {
        set_mode(SERVO_TEST, MODE_REASON_SERVOTEST);
    }

    // set yaw servo pwm and send output to servo
    if (servo_num == CH_YAW) {
        SRV_Channels::set_output_pwm(SRV_Channel::k_tracker_yaw, pwm);
        SRV_Channels::constrain_pwm(SRV_Channel::k_tracker_yaw);
    }

    // set pitch servo pwm and send output to servo
    if (servo_num == CH_PITCH) {
        SRV_Channels::set_output_pwm(SRV_Channel::k_tracker_pitch, pwm);
        SRV_Channels::constrain_pwm(SRV_Channel::k_tracker_pitch);
    }

    SRV_Channels::calc_pwm();
    SRV_Channels::output_ch_all();
    
    // return success
    return true;
} 
        uses: #pragma once

// Command/Waypoint/Location Options Bitmask
//--------------------
#define MASK_OPTIONS_RELATIVE_ALT       (1<<0)          // 1 = Relative
                                                        // altitude

// Controller modes
// ----------------

enum ControlMode {
    MANUAL=0,
    STOP=1,
    SCAN=2,
    SERVO_TEST=3,
    AUTO=10,
    INITIALISING=16
};

enum ServoType {
    SERVO_TYPE_POSITION=0,
    SERVO_TYPE_ONOFF=1,
    SERVO_TYPE_CR=2
};

enum AltSource {
	ALT_SOURCE_BARO=0,
	ALT_SOURCE_GPS=1,
	ALT_SOURCE_GPS_VEH_ONLY=2
};

enum mode_reason_t {
    MODE_REASON_INITIALISED = 0,
    MODE_REASON_STARTUP,
    MODE_REASON_SERVOTEST,
    MODE_REASON_GCS_COMMAND,
};

//  Filter
#define SERVO_OUT_FILT_HZ               0.1f
#define G_Dt                            0.02f

//  Logging parameters
#define MASK_LOG_ATTITUDE               (1<<0)
#define MASK_LOG_GPS                    (1<<1)
#define MASK_LOG_RCIN                   (1<<2)
#define MASK_LOG_IMU                    (1<<3)
#define MASK_LOG_RCOUT                  (1<<4)
#define MASK_LOG_COMPASS                (1<<5)
#define MASK_LOG_CURRENT                (1<<6)
#define MASK_LOG_ANY                    0xFFFF

//  Logging messages
#define LOG_V_BAR_MSG                   0x04
#define LOG_V_POS_MSG                   0x05 

      - name: LIBRARIES += AP_Common
LIBRARIES += AP_Param
LIBRARIES += StorageManager
LIBRARIES += AP_GPS
LIBRARIES += AP_Baro
LIBRARIES += AP_Compass
LIBRARIES += AP_Math
LIBRARIES += AP_AccelCal
LIBRARIES += AP_InertialSensor
LIBRARIES += AP_AHRS
LIBRARIES += Filter
LIBRARIES += AP_Buffer
LIBRARIES += GCS_MAVLink
LIBRARIES += AP_SerialManager
LIBRARIES += AP_Declination
LIBRARIES += DataFlash
LIBRARIES += AC_PID
LIBRARIES += AP_Scheduler
LIBRARIES += AP_NavEKF2
LIBRARIES += AP_NavEKF3
LIBRARIES += AP_Vehicle
LIBRARIES += AP_Mission
LIBRARIES += AP_Terrain
LIBRARIES += AP_Rally
LIBRARIES += AP_Notify
LIBRARIES += AP_BattMonitor
LIBRARIES += RC_Channel
LIBRARIES += AP_BoardConfig
LIBRARIES += AP_OpticalFlow
LIBRARIES += AP_RangeFinder
LIBRARIES += AP_Mount
LIBRARIES += AP_RSSI
LIBRARIES += AP_RPM
LIBRARIES += AP_Frsky_Telem
LIBRARIES += AP_Beacon 
        run: #include "Tracker.h"

// Functions to read the RC radio input

void Tracker::read_radio()
{
    RC_Channels::read_input();
} 

      # Build is not required unless generated source files are used
      # - name: #include "Tracker.h"

/*
  update INS and attitude
 */
void Tracker::update_ahrs()
{
    ahrs.update();
}


/*
  read and update compass
 */
void Tracker::update_compass(void)
{
    if (g.compass_enabled && compass.read()) {
        ahrs.set_compass(&compass);
        if (should_log(MASK_LOG_COMPASS)) {
            DataFlash.Log_Write_Compass(compass);
        }
    }
}

/*
  if the compass is enabled then try to accumulate a reading
 */
void Tracker::compass_accumulate(void)
{
    if (g.compass_enabled) {
        compass.accumulate();
    }    
}

/*
 calibrate compass
*/
void Tracker::compass_cal_update() {
    if (!hal.util->get_soft_armed()) {
        compass.compass_cal_update();
    }
}

/*
    Accel calibration
*/
void Tracker::accel_cal_update() {
    if (hal.util->get_soft_armed()) {
        return;
    }
    ins.acal_update();
    float trim_roll, trim_pitch;
    if (ins.get_new_trim(trim_roll, trim_pitch)) {
        ahrs.set_trim(Vector3f(trim_roll, trim_pitch, 0));
    }
}

/*
  read the GPS
 */
void Tracker::update_GPS(void)
{
    gps.update();

    static uint32_t last_gps_msg_ms;
    static uint8_t ground_start_count = 5;
    if (gps.last_message_time_ms() != last_gps_msg_ms && 
        gps.status() >= AP_GPS::GPS_OK_FIX_3D) {
        last_gps_msg_ms = gps.last_message_time_ms();
        
        if (ground_start_count > 1) {
            ground_start_count--;
        } else if (ground_start_count == 1) {
            // We countdown N number of good GPS fixes
            // so that the altitude is more accurate
            // -------------------------------------
            if (current_loc.lat == 0 && current_loc.lng == 0) {
                ground_start_count = 5;

            } else {
                // Now have an initial GPS position
                // use it as the HOME position in future startups
                current_loc = gps.location();
                set_home(current_loc);

                // set system clock for log timestamps
                uint64_t gps_timestamp = gps.time_epoch_usec();
                
                hal.util->set_system_clock(gps_timestamp);
                
                // update signing timestamp
                GCS_MAVLINK::update_signing_timestamp(gps_timestamp);

                if (g.compass_enabled) {
                    // Set compass declination automatically
                    compass.set_initial_location(gps.location().lat, gps.location().lng);
                }
                ground_start_count = 0;
            }
        }
    }
}

void Tracker::handle_battery_failsafe(const char* type_str, const int8_t action)
{
    // NOP
    // useful failsafes in the future would include actually recalling the vehicle
    // that is tracked before the tracker loses power to continue tracking it
}
 
      #   run: #include "Tracker.h"

/*
 * Code to move pitch and yaw servos to attain a target heading or pitch
 */

// init_servos - initialises the servos
void Tracker::init_servos()
{
    SRV_Channels::set_default_function(CH_YAW, SRV_Channel::k_tracker_yaw);
    SRV_Channels::set_default_function(CH_PITCH, SRV_Channel::k_tracker_pitch);

    // yaw range is +/- (YAW_RANGE parameter/2) converted to centi-degrees
    SRV_Channels::set_angle(SRV_Channel::k_tracker_yaw, g.yaw_range * 100/2);        

    // pitch range is +/- (PITCH_MIN/MAX parameters/2) converted to centi-degrees
    SRV_Channels::set_angle(SRV_Channel::k_tracker_pitch, (-g.pitch_min+g.pitch_max) * 100/2);

    SRV_Channels::output_trim_all();
    SRV_Channels::calc_pwm();
    SRV_Channels::output_ch_all();
    
    yaw_servo_out_filt.set_cutoff_frequency(SERVO_OUT_FILT_HZ);
    pitch_servo_out_filt.set_cutoff_frequency(SERVO_OUT_FILT_HZ);
}

/**
   update the pitch (elevation) servo. The aim is to drive the boards ahrs pitch to the
   requested pitch, so the board (and therefore the antenna) will be pointing at the target
 */
void Tracker::update_pitch_servo(float pitch)
{
    switch ((enum ServoType)g.servo_pitch_type.get()) {
    case SERVO_TYPE_ONOFF:
        update_pitch_onoff_servo(pitch);
        break;

    case SERVO_TYPE_CR:
        update_pitch_cr_servo(pitch);
        break;

    case SERVO_TYPE_POSITION:
    default:
        update_pitch_position_servo();
        break;
    }

    // convert servo_out to radio_out and send to servo
    SRV_Channels::calc_pwm();
    SRV_Channels::output_ch_all();
}

/**
   update the pitch (elevation) servo. The aim is to drive the boards ahrs pitch to the
   requested pitch, so the board (and therefore the antenna) will be pointing at the target
 */
void Tracker::update_pitch_position_servo()
{
    int32_t pitch_min_cd = g.pitch_min*100;
    int32_t pitch_max_cd = g.pitch_max*100;
    // Need to configure your servo so that increasing servo_out causes increase in pitch/elevation (ie pointing higher into the sky,
    // above the horizon. On my antenna tracker this requires the pitch/elevation servo to be reversed
    // param set RC2_REV -1
    //
    // The pitch servo (RC channel 2) is configured for servo_out of -9000-0-9000 servo_out,
    // which will drive the servo from RC2_MIN to RC2_MAX usec pulse width.
    // Therefore, you must set RC2_MIN and RC2_MAX so that your servo drives the antenna altitude between -90 to 90 exactly
    // To drive my HS-645MG servos through their full 180 degrees of rotational range, I have to set:
    // param set RC2_MAX 2540
    // param set RC2_MIN 640
    //
    // You will also need to tune the pitch PID to suit your antenna and servos. I use:
    // PITCH2SRV_P      0.100000
    // PITCH2SRV_I      0.020000
    // PITCH2SRV_D      0.000000
    // PITCH2SRV_IMAX   4000.000000

    // calculate new servo position
    g.pidPitch2Srv.set_input_filter_all(nav_status.angle_error_pitch);
    int32_t new_servo_out = SRV_Channels::get_output_scaled(SRV_Channel::k_tracker_pitch) + g.pidPitch2Srv.get_pid();

    // position limit pitch servo
    if (new_servo_out <= pitch_min_cd) {
        new_servo_out = pitch_min_cd;
        g.pidPitch2Srv.reset_I();
    }
    if (new_servo_out >= pitch_max_cd) {
        new_servo_out = pitch_max_cd;
        g.pidPitch2Srv.reset_I();
    }
    // rate limit pitch servo
    SRV_Channels::set_output_scaled(SRV_Channel::k_tracker_pitch, new_servo_out);

    if (pitch_servo_out_filt_init) {
        pitch_servo_out_filt.apply(new_servo_out, G_Dt);
    } else {
        pitch_servo_out_filt.reset(new_servo_out);
        pitch_servo_out_filt_init = true;
    }
}


/**
   update the pitch (elevation) servo. The aim is to drive the boards ahrs pitch to the
   requested pitch, so the board (and therefore the antenna) will be pointing at the target
 */
void Tracker::update_pitch_onoff_servo(float pitch)
{
    int32_t pitch_min_cd = g.pitch_min*100;
    int32_t pitch_max_cd = g.pitch_max*100;

    float acceptable_error = g.onoff_pitch_rate * g.onoff_pitch_mintime;
    if (fabsf(nav_status.angle_error_pitch) < acceptable_error) {
        SRV_Channels::set_output_scaled(SRV_Channel::k_tracker_pitch, 0);
    } else if ((nav_status.angle_error_pitch > 0) && (pitch*100>pitch_min_cd)) {
        // positive error means we are pointing too low, so push the
        // servo up
        SRV_Channels::set_output_scaled(SRV_Channel::k_tracker_pitch, -9000);
    } else if (pitch*100<pitch_max_cd) {
        // negative error means we are pointing too high, so push the
        // servo down
        SRV_Channels::set_output_scaled(SRV_Channel::k_tracker_pitch, 9000);
    }
}

/**
   update the pitch for continuous rotation servo
*/
void Tracker::update_pitch_cr_servo(float pitch)
{
    int32_t pitch_min_cd = g.pitch_min*100;
    int32_t pitch_max_cd = g.pitch_max*100;
    if ((pitch>pitch_min_cd) && (pitch<pitch_max_cd)) {
        g.pidPitch2Srv.set_input_filter_all(nav_status.angle_error_pitch);
        SRV_Channels::set_output_scaled(SRV_Channel::k_tracker_pitch, g.pidPitch2Srv.get_pid());
    }
}

/**
   update the yaw (azimuth) servo.
 */
void Tracker::update_yaw_servo(float yaw)
{
	switch ((enum ServoType)g.servo_yaw_type.get()) {
    case SERVO_TYPE_ONOFF:
        update_yaw_onoff_servo(yaw);
        break;

    case SERVO_TYPE_CR:
        update_yaw_cr_servo(yaw);
        break;

    case SERVO_TYPE_POSITION:
    default:
        update_yaw_position_servo();
        break;
    }

    // convert servo_out to radio_out and send to servo
    SRV_Channels::calc_pwm();
    SRV_Channels::output_ch_all();
}

/**
   update the yaw (azimuth) servo. The aim is to drive the boards ahrs
   yaw to the requested yaw, so the board (and therefore the antenna)
   will be pointing at the target
 */
void Tracker::update_yaw_position_servo()
{
    int32_t yaw_limit_cd = g.yaw_range*100/2;

    // Antenna as Ballerina. Use with antenna that do not have continuously rotating servos, ie at some point in rotation
    // the servo limits are reached and the servo has to slew 360 degrees to the 'other side' to keep tracking.
    //
    // This algorithm accounts for the fact that the antenna mount may not be aligned with North
    // (in fact, any alignment is permissible), and that the alignment may change (possibly rapidly) over time
    // (as when the antenna is mounted on a moving, turning vehicle)
    //
    // With my antenna mount, large pwm output drives the antenna anticlockise, so need:
    // param set RC1_REV -1
    // to reverse the servo. Yours may be different
    //
    // You MUST set RC1_MIN and RC1_MAX so that your servo drives the antenna azimuth from -180 to 180 relative
    // to the mount.
    // To drive my HS-645MG servos through their full 180 degrees of rotational range and therefore the
    // antenna through a full 360 degrees, I have to set:
    // param set RC1_MAX 2380
    // param set RC1_MIN 680
    // According to the specs at https://www.servocity.com/html/hs-645mg_ultra_torque.html,
    // that should be 600 through 2400, but the azimuth gearing in my antenna pointer is not exactly 2:1

    /*
      a positive error means that we need to rotate clockwise
      a negative error means that we need to rotate counter-clockwise

      Use our current yawspeed to determine if we are moving in the
      right direction
     */

    g.pidYaw2Srv.set_input_filter_all(nav_status.angle_error_yaw);
    float servo_change = g.pidYaw2Srv.get_pid();
    servo_change = constrain_float(servo_change, -18000, 18000);
    float new_servo_out = constrain_float(SRV_Channels::get_output_scaled(SRV_Channel::k_tracker_yaw) + servo_change, -18000, 18000);

    // position limit yaw servo
    if (new_servo_out <= -yaw_limit_cd) {
        new_servo_out = -yaw_limit_cd;
        g.pidYaw2Srv.reset_I();
    }
    if (new_servo_out >= yaw_limit_cd) {
        new_servo_out = yaw_limit_cd;
        g.pidYaw2Srv.reset_I();
    }

    SRV_Channels::set_output_scaled(SRV_Channel::k_tracker_yaw, new_servo_out);

    if (yaw_servo_out_filt_init) {
        yaw_servo_out_filt.apply(new_servo_out, G_Dt);
    } else {
        yaw_servo_out_filt.reset(new_servo_out);
        yaw_servo_out_filt_init = true;
    }
}


/**
   update the yaw (azimuth) servo. The aim is to drive the boards ahrs
   yaw to the requested yaw, so the board (and therefore the antenna)
   will be pointing at the target
 */
void Tracker::update_yaw_onoff_servo(float yaw)
{
    float acceptable_error = g.onoff_yaw_rate * g.onoff_yaw_mintime;
    if (fabsf(nav_status.angle_error_yaw * 0.01f) < acceptable_error) {
        SRV_Channels::set_output_scaled(SRV_Channel::k_tracker_yaw, 0);
    } else if (nav_status.angle_error_yaw * 0.01f > 0) {
        // positive error means we are counter-clockwise of the target, so
        // move clockwise
        SRV_Channels::set_output_scaled(SRV_Channel::k_tracker_yaw, 18000);
    } else {
        // negative error means we are clockwise of the target, so
        // move counter-clockwise
        SRV_Channels::set_output_scaled(SRV_Channel::k_tracker_yaw, -18000);
    }
}

/**
   update the yaw continuous rotation servo
 */
void Tracker::update_yaw_cr_servo(float yaw)
{
    g.pidYaw2Srv.set_input_filter_all(nav_status.angle_error_yaw);
    SRV_Channels::set_output_scaled(SRV_Channel::k_tracker_yaw, -g.pidYaw2Srv.get_pid());
} 

      - name: #include "Tracker.h"

// mission storage
static const StorageAccess wp_storage(StorageManager::StorageMission);

static void mavlink_delay_cb_static()
{
    tracker.mavlink_delay_cb();
}

void Tracker::init_tracker()
{
    // initialise console serial port
    serial_manager.init_console();

    hal.console->printf("\n\nInit %s\n\nFree RAM: %u\n",
                        fwver.fw_string,
                        hal.util->available_memory());

    // Check the EEPROM format version before loading any parameters from EEPROM
    load_parameters();

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

    // initialise notify
    notify.init(false);
    AP_Notify::flags.pre_arm_check = true;
    AP_Notify::flags.pre_arm_gps_check = true;
    AP_Notify::flags.failsafe_battery = false;

    // init baro before we start the GCS, so that the CLI baro test works
    barometer.set_log_baro_bit(MASK_LOG_IMU);
    barometer.init();

    // we start by assuming USB connected, as we initialed the serial
    // port with SERIAL0_BAUD. check_usb_mux() fixes this if need be.    
    usb_connected = true;
    check_usb_mux();

    // setup telem slots with serial ports
    gcs().setup_uarts(serial_manager);

#if LOGGING_ENABLED == ENABLED
    log_init();
#endif

    if (g.compass_enabled==true) {
        if (!compass.init() || !compass.read()) {
            hal.console->printf("Compass initialisation failed!\n");
            g.compass_enabled = false;
        } else {
            ahrs.set_compass(&compass);
        }
    }

    // GPS Initialization
    gps.set_log_gps_bit(MASK_LOG_GPS);
    gps.init(serial_manager);

    ahrs.init();
    ahrs.set_fly_forward(false);

    ins.init(scheduler.get_loop_rate_hz());
    ahrs.reset();

    barometer.calibrate();

    // initialise DataFlash library
    DataFlash.setVehicle_Startup_Log_Writer(FUNCTOR_BIND(&tracker, &Tracker::Log_Write_Vehicle_Startup_Messages, void));

    // set serial ports non-blocking
    serial_manager.set_blocking_writes_all(false);

    // initialise servos
    init_servos();

    // use given start positions - useful for indoor testing, and
    // while waiting for GPS lock
    // sanity check location
    if (fabsf(g.start_latitude) <= 90.0f && fabsf(g.start_longitude) <= 180.0f) {
        current_loc.lat = g.start_latitude * 1.0e7f;
        current_loc.lng = g.start_longitude * 1.0e7f;
    } else {
        gcs().send_text(MAV_SEVERITY_NOTICE, "Ignoring invalid START_LATITUDE or START_LONGITUDE parameter");
    }

    // see if EEPROM has a default location as well
    if (current_loc.lat == 0 && current_loc.lng == 0) {
        get_home_eeprom(current_loc);
    }

    init_capabilities();

    gcs().send_text(MAV_SEVERITY_INFO,"Ready to track");
    hal.scheduler->delay(1000); // Why????

    set_mode(AUTO, MODE_REASON_STARTUP); // tracking

    if (g.startup_delay > 0) {
        // arm servos with trim value to allow them to start up (required
        // for some servos)
        prepare_servos();
    }

    // disable safety if requested
    BoardConfig.init_safety();    
}

/*
  fetch HOME from EEPROM
*/
bool Tracker::get_home_eeprom(struct Location &loc)
{
    // Find out proper location in memory by using the start_byte position + the index
    // --------------------------------------------------------------------------------
    if (g.command_total.get() == 0) {
        return false;
    }

    // read WP position
    loc.options = wp_storage.read_byte(0);
    loc.alt = wp_storage.read_uint32(1);
    loc.lat = wp_storage.read_uint32(5);
    loc.lng = wp_storage.read_uint32(9);

    return true;
}

void Tracker::set_home_eeprom(struct Location temp)
{
    wp_storage.write_byte(0, temp.options);
    wp_storage.write_uint32(1, temp.alt);
    wp_storage.write_uint32(5, temp.lat);
    wp_storage.write_uint32(9, temp.lng);

    // Now have a home location in EEPROM
    g.command_total.set_and_save(1); // At most 1 entry for HOME
}

void Tracker::set_home(struct Location temp)
{
    set_home_eeprom(temp);
    current_loc = temp;
    gcs().send_home(temp);
    Location ekf_origin;
    if (ahrs.get_origin(ekf_origin)) {
        gcs().send_ekf_origin(ekf_origin);
    }
}

// sets ekf_origin if it has not been set.
//  should only be used when there is no GPS to provide an absolute position
void Tracker::set_ekf_origin(const Location& loc)
{
    // check location is valid
    if (!check_latlng(loc)) {
        return;
    }

    // check EKF origin has already been set
    Location ekf_origin;
    if (ahrs.get_origin(ekf_origin)) {
        return;
    }

    if (!ahrs.set_origin(loc)) {
        return;
    }

    // send ekf origin to GCS
    gcs().send_ekf_origin(loc);
}

void Tracker::arm_servos()
{
    hal.util->set_soft_armed(true);
    DataFlash.set_vehicle_armed(true);
}

void Tracker::disarm_servos()
{
    hal.util->set_soft_armed(false);
    DataFlash.set_vehicle_armed(false);
}

/*
  setup servos to trim value after initialising
 */
void Tracker::prepare_servos()
{
    start_time_ms = AP_HAL::millis();
    SRV_Channels::set_output_limit(SRV_Channel::k_tracker_yaw, SRV_Channel::SRV_CHANNEL_LIMIT_TRIM);
    SRV_Channels::set_output_limit(SRV_Channel::k_tracker_pitch, SRV_Channel::SRV_CHANNEL_LIMIT_TRIM);
    SRV_Channels::calc_pwm();
    SRV_Channels::output_ch_all();
}

void Tracker::set_mode(enum ControlMode mode, mode_reason_t reason)
{
    if (control_mode == mode) {
        // don't switch modes if we are already in the correct mode.
        return;
    }
    control_mode = mode;

	switch (control_mode) {
    case AUTO:
    case MANUAL:
    case SCAN:
    case SERVO_TEST:
        arm_servos();
        break;

    case STOP:
    case INITIALISING:
        disarm_servos();
        break;
    }

	// log mode change
	DataFlash.Log_Write_Mode(control_mode, reason);
}

void Tracker::check_usb_mux(void)
{
    bool usb_check = hal.gpio->usb_connected();
    if (usb_check == usb_connected) {
        return;
    }

    // the user has switched to/from the telemetry port
    usb_connected = usb_check;
}

/*
  should we log a message type now?
 */
bool Tracker::should_log(uint32_t mask)
{
    if (!DataFlash.should_log(mask)) {
        return false;
    }
    return true;
} 
        uses: #include "Tracker.h"

/**
  update_vehicle_position_estimate - updates estimate of vehicle positions
  should be called at 50hz
 */
void Tracker::update_vehicle_pos_estimate()
{
    // calculate time since last actual position update
    float dt = (AP_HAL::micros() - vehicle.last_update_us) * 1.0e-6f;

    // if less than 5 seconds since last position update estimate the position
    if (dt < TRACKING_TIMEOUT_SEC) {
        // project the vehicle position to take account of lost radio packets
        vehicle.location_estimate = vehicle.location;
        float north_offset = vehicle.vel.x * dt;
        float east_offset = vehicle.vel.y * dt;
        location_offset(vehicle.location_estimate, north_offset, east_offset);
    	vehicle.location_estimate.alt += vehicle.vel.z * 100.0f * dt;
        // set valid_location flag
        vehicle.location_valid = true;
    } else {
        // vehicle has been lost, set lost flag
        vehicle.location_valid = false;
    }
}

/**
  update_tracker_position - updates antenna tracker position from GPS location
  should be called at 50hz
 */
void Tracker::update_tracker_position()
{
    // update our position if we have at least a 2D fix
    // REVISIT: what if we lose lock during a mission and the antenna is moving?
    if (!ahrs.get_position(current_loc) && (gps.status() >= AP_GPS::GPS_OK_FIX_2D)) {
        current_loc = gps.location();
    }
}

/**
  update_bearing_and_distance - updates bearing and distance to the vehicle
  should be called at 50hz
 */
void Tracker::update_bearing_and_distance()
{
    // exit immediately if we do not have a valid vehicle position
    if (!vehicle.location_valid) {
        return;
    }

    // calculate bearing to vehicle
    // To-Do: remove need for check of control_mode
    if (control_mode != SCAN && !nav_status.manual_control_yaw) {
        nav_status.bearing  = get_bearing_cd(current_loc, vehicle.location_estimate) * 0.01f;
    }

    // calculate distance to vehicle
    nav_status.distance = get_distance(current_loc, vehicle.location_estimate);

    // calculate altitude difference to vehicle using gps
    if (g.alt_source == ALT_SOURCE_GPS){
        nav_status.alt_difference_gps = (vehicle.location_estimate.alt - current_loc.alt) / 100.0f;
    } else {
        // g.alt_source == ALT_SOURCE_GPS_VEH_ONLY
        nav_status.alt_difference_gps = vehicle.relative_alt / 100.0f;
    }

    // calculate pitch to vehicle
    // To-Do: remove need for check of control_mode
    if (control_mode != SCAN && !nav_status.manual_control_pitch) {
    	if (g.alt_source == ALT_SOURCE_BARO) {
    	    nav_status.pitch = degrees(atan2f(nav_status.alt_difference_baro, nav_status.distance));
    	} else {
            nav_status.pitch = degrees(atan2f(nav_status.alt_difference_gps, nav_status.distance));
    	}
    }
}

/**
  main antenna tracking code, called at 50Hz
 */
void Tracker::update_tracking(void)
{
    // update vehicle position estimate
    update_vehicle_pos_estimate();

    // update antenna tracker position from GPS
    update_tracker_position();

    // update bearing and distance to vehicle
    update_bearing_and_distance();

    // do not perform any servo updates until startup delay has passed
    if (g.startup_delay > 0 &&
        AP_HAL::millis() - start_time_ms < g.startup_delay*1000) {
        return;
    }

    // do not perform updates if safety switch is disarmed (i.e. servos can't be moved)
    if (hal.util->safety_switch_state() == AP_HAL::Util::SAFETY_DISARMED) {
        return;
    }

    switch (control_mode) {
    case AUTO:
        update_auto();
        break;

    case MANUAL:
        update_manual();
        break;

    case SCAN:
        update_scan();
        break;

    case SERVO_TEST:
    case STOP:
    case INITIALISING:
        break;
    }
}

/**
   handle an updated position from the aircraft
 */
void Tracker::tracking_update_position(const mavlink_global_position_int_t &msg)
{
    vehicle.location.lat = msg.lat;
    vehicle.location.lng = msg.lon;
    vehicle.location.alt = msg.alt/10;
    vehicle.relative_alt = msg.relative_alt/10;
    vehicle.vel = Vector3f(msg.vx/100.0f, msg.vy/100.0f, msg.vz/100.0f);
    vehicle.last_update_us = AP_HAL::micros();
    vehicle.last_update_ms = AP_HAL::millis();
    // log vehicle as GPS2
    if (should_log(MASK_LOG_GPS)) {
        Log_Write_Vehicle_Pos(vehicle.location.lat, vehicle.location.lng, vehicle.location.alt, vehicle.vel);
    }
}


/**
   handle an updated pressure reading from the aircraft
 */
void Tracker::tracking_update_pressure(const mavlink_scaled_pressure_t &msg)
{
    float local_pressure = barometer.get_pressure();
    float aircraft_pressure = msg.press_abs*100.0f;

    // calculate altitude difference based on difference in barometric pressure
    float alt_diff = barometer.get_altitude_difference(local_pressure, aircraft_pressure);
    if (!isnan(alt_diff) && !isinf(alt_diff)) {
        nav_status.alt_difference_baro = alt_diff + nav_status.altitude_offset;

		if (nav_status.need_altitude_calibration) {
			// we have done a baro calibration - zero the altitude
			// difference to the aircraft
			nav_status.altitude_offset = -alt_diff;
			nav_status.alt_difference_baro = 0;
			nav_status.need_altitude_calibration = false;
		}
    }

    // log vehicle baro data
    Log_Write_Vehicle_Baro(aircraft_pressure, alt_diff);
}

/**
   handle a manual control message by using the data to command yaw and pitch
 */
void Tracker::tracking_manual_control(const mavlink_manual_control_t &msg)
{
    nav_status.bearing = msg.x;
    nav_status.pitch   = msg.y;
    nav_status.distance = 0.0;
    nav_status.manual_control_yaw   = (msg.x != 0x7FFF);
    nav_status.manual_control_pitch = (msg.y != 0x7FFF);
    // z, r and buttons are not used
}

/**
   update_armed_disarmed - set armed LED if we have received a position update within the last 5 seconds
 */
void Tracker::update_armed_disarmed()
{
    if (vehicle.last_update_ms != 0 && (AP_HAL::millis() - vehicle.last_update_ms) < TRACKING_TIMEOUT_MS) {
        AP_Notify::flags.armed = true;
    } else {
        AP_Notify::flags.armed = false;
    }
} 
        # Provide a unique ID to access the sarif output path
        id: /*
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
#include "Tracker.h"

#define FORCE_VERSION_H_INCLUDE
#include "version.h"
#undef FORCE_VERSION_H_INCLUDE

#include <AP_Common/AP_FWVersion.h>

const AP_FWVersion Tracker::fwver{
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
        with: #pragma once

#ifndef FORCE_VERSION_H_INCLUDE
#error version.h should never be included directly. You probably want to include AP_Common/AP_FWVersion.h
#endif

#include "ap_version.h"

#define THISFIRMWARE "AntennaTracker V1.0.0"

// the following line is parsed by the autotest scripts
#define FIRMWARE_VERSION 1,0,0,FIRMWARE_VERSION_TYPE_BETA

#define FW_MAJOR 1
#define FW_MINOR 0
#define FW_PATCH 0
#define FW_TYPE FIRMWARE_VERSION_TYPE_BETA 
          cmakeBuildDirectory: #!/usr/bin/env python
# encoding: utf-8

def build(bld):
    vehicle = bld.path.name
    bld.ap_stlib(
        name=vehicle + '_libs',
        ap_vehicle=vehicle,
        ap_libraries=bld.ap_common_vehicle_libraries() + [
            'AC_PID',
            'AP_Beacon',
        ],
    )

    bld.ap_program(
        program_name='antennatracker',
        program_groups=['bin', 'antennatracker'],
        use=vehicle + '_libs',
    ) 
          # Ruleset file that will determine what checks will be run
          ruleset: NativeRecommendedRules.ruleset

      # Upload SARIF file to GitHub Code Scanning Alerts
      - name: Upload SARIF to GitHub
        uses: github/codeql-action/upload-sarif@v3
        with:
          sarif_file: ${{ steps.run-analysis.outputs.sarif }}

      # Upload SARIF file as an Artifact to download and view
      # - name: Upload SARIF as an Artifact
      #   uses: actions/upload-artifact@v4
      #   with:
      #     name: sarif-file
      #     path: ${{ steps.run-analysis.outputs.sarif }}
