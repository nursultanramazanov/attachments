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

Mode::Mode() :
    ahrs(rover.ahrs),
    g(rover.g),
    g2(rover.g2),
    channel_steer(rover.channel_steer),
    channel_throttle(rover.channel_throttle),
    mission(rover.mission),
    attitude_control(rover.g2.attitude_control)
{ }

void Mode::exit()
{
    // call sub-classes exit
    _exit();
}

bool Mode::enter()
{
    const bool ignore_checks = !hal.util->get_soft_armed();   // allow switching to any mode if disarmed.  We rely on the arming check to perform
    if (!ignore_checks) {

        // get EKF filter status
        nav_filter_status filt_status;
        rover.ahrs.get_filter_status(filt_status);

        // check position estimate.  requires origin and at least one horizontal position flag to be true
        Location origin;
        const bool position_ok = ahrs.get_origin(origin) &&
                                (filt_status.flags.horiz_pos_abs || filt_status.flags.pred_horiz_pos_abs ||
                                 filt_status.flags.horiz_pos_rel || filt_status.flags.pred_horiz_pos_rel);
        if (requires_position() && !position_ok) {
            return false;
        }

        // check velocity estimate (if we have position estimate, we must have velocity estimate)
        if (requires_velocity() && !position_ok && !filt_status.flags.horiz_vel) {
            return false;
        }
    }

    return _enter();
}

void Mode::get_pilot_desired_steering_and_throttle(float &steering_out, float &throttle_out)
{
    // no RC input means no throttle and centered steering
    if (rover.failsafe.bits & FAILSAFE_EVENT_THROTTLE) {
        steering_out = 0;
        throttle_out = 0;
        return;
    }

    // apply RC skid steer mixing
    switch ((enum pilot_steer_type_t)rover.g.pilot_steer_type.get())
    {
        case PILOT_STEER_TYPE_DEFAULT:
        default: {
            // by default regular and skid-steering vehicles reverse their rotation direction when backing up
            // (this is the same as PILOT_STEER_TYPE_DIR_REVERSED_WHEN_REVERSING below)
            throttle_out = rover.channel_throttle->get_control_in();
            steering_out = rover.channel_steer->get_control_in();
            break;
        }

        case PILOT_STEER_TYPE_TWO_PADDLES: {
            // convert the two radio_in values from skid steering values
            // left paddle from steering input channel, right paddle from throttle input channel
            // steering = left-paddle - right-paddle
            // throttle = average(left-paddle, right-paddle)
            const float left_paddle = rover.channel_steer->norm_input();
            const float right_paddle = rover.channel_throttle->norm_input();

            throttle_out = 0.5f * (left_paddle + right_paddle) * 100.0f;

            const float steering_dir = is_negative(throttle_out) ? -1 : 1;
            steering_out = steering_dir * (left_paddle - right_paddle) * 0.5f * 4500.0f;
            break;
        }

        case PILOT_STEER_TYPE_DIR_REVERSED_WHEN_REVERSING:
            throttle_out = rover.channel_throttle->get_control_in();
            steering_out = rover.channel_steer->get_control_in();
            break;

        case PILOT_STEER_TYPE_DIR_UNCHANGED_WHEN_REVERSING: {
            throttle_out = rover.channel_throttle->get_control_in();
            const float steering_dir = is_negative(throttle_out) ? -1 : 1;
            steering_out = steering_dir * rover.channel_steer->get_control_in();
            break;
        }
    }
}

// set desired location
void Mode::set_desired_location(const struct Location& destination, float next_leg_bearing_cd)
{
    // set origin to last destination if waypoint controller active
    if ((AP_HAL::millis() - last_steer_to_wp_ms < 100) && _reached_destination) {
        _origin = _destination;
    } else {
        // otherwise use reasonable stopping point
        calc_stopping_location(_origin);
    }
    _destination = destination;

    // initialise distance
    _distance_to_destination = get_distance(_origin, _destination);
    _reached_destination = false;

    // set final desired speed
    _desired_speed_final = 0.0f;
    if (!is_equal(next_leg_bearing_cd, MODE_NEXT_HEADING_UNKNOWN)) {
        const float curr_leg_bearing_cd = get_bearing_cd(_origin, _destination);
        const float turn_angle_cd = wrap_180_cd(next_leg_bearing_cd - curr_leg_bearing_cd);
        if (is_zero(turn_angle_cd)) {
            // if not turning can continue at full speed
            _desired_speed_final = _desired_speed;
        } else {
            // calculate maximum speed that keeps overshoot within bounds
            const float radius_m = fabsf(g.waypoint_overshoot / (cosf(radians(turn_angle_cd * 0.01f)) - 1.0f));
            _desired_speed_final = MIN(_desired_speed, safe_sqrt(g.turn_max_g * GRAVITY_MSS * radius_m));
        }
    }
}

// set desired location as an offset from the EKF origin in NED frame
bool Mode::set_desired_location_NED(const Vector3f& destination, float next_leg_bearing_cd)
{
    Location destination_ned;
    // initialise destination to ekf origin
    if (!ahrs.get_origin(destination_ned)) {
        return false;
    }
    // apply offset
    location_offset(destination_ned, destination.x, destination.y);
    set_desired_location(destination_ned, next_leg_bearing_cd);
    return true;
}

// set desired heading and speed
void Mode::set_desired_heading_and_speed(float yaw_angle_cd, float target_speed)
{
    // handle initialisation
    _reached_destination = false;

    // record targets
    _desired_yaw_cd = yaw_angle_cd;
    _desired_speed = target_speed;
}

// get default speed for this mode (held in (CRUISE_SPEED, WP_SPEED or RTL_SPEED)
float Mode::get_speed_default(bool rtl) const
{
    if (rtl && is_positive(g2.rtl_speed)) {
        return g2.rtl_speed;
    } else if (is_positive(g2.wp_speed)) {
        return g2.wp_speed;
    } else {
        return g.speed_cruise;
    }
}

// restore desired speed to default from parameter values (CRUISE_SPEED or WP_SPEED)
void Mode::set_desired_speed_to_default(bool rtl)
{
    _desired_speed = get_speed_default(rtl);
}

void Mode::calc_throttle(float target_speed, bool nudge_allowed, bool avoidance_enabled)
{
    // add in speed nudging
    if (nudge_allowed) {
        target_speed = calc_speed_nudge(target_speed, g.speed_cruise, g.throttle_cruise * 0.01f);
    }

    // get acceleration limited target speed
    target_speed = attitude_control.get_desired_speed_accel_limited(target_speed);

    // apply object avoidance to desired speed using half vehicle's maximum acceleration/deceleration
    if (avoidance_enabled) {
        g2.avoid.adjust_speed(0.0f, 0.5f * attitude_control.get_accel_max(), ahrs.yaw, target_speed, rover.G_Dt);
    }

    // call throttle controller and convert output to -100 to +100 range
    float throttle_out;

    // call speed or stop controller
    if (is_zero(target_speed)) {
        bool stopped;
        throttle_out = 100.0f * attitude_control.get_throttle_out_stop(g2.motors.limit.throttle_lower, g2.motors.limit.throttle_upper, g.speed_cruise, g.throttle_cruise * 0.01f, stopped);
    } else {
        throttle_out = 100.0f * attitude_control.get_throttle_out_speed(target_speed, g2.motors.limit.throttle_lower, g2.motors.limit.throttle_upper, g.speed_cruise, g.throttle_cruise * 0.01f);
    }

    // send to motor
    g2.motors.set_throttle(throttle_out);
}

// performs a controlled stop with steering centered
bool Mode::stop_vehicle()
{
    // call throttle controller and convert output to -100 to +100 range
    bool stopped = false;
    float throttle_out = 100.0f * attitude_control.get_throttle_out_stop(g2.motors.limit.throttle_lower, g2.motors.limit.throttle_upper, g.speed_cruise, g.throttle_cruise * 0.01f, stopped);

    // send to motor
    g2.motors.set_throttle(throttle_out);

    // do not attempt to steer
    g2.motors.set_steering(0.0f);

    // return true once stopped
    return stopped;
}

// estimate maximum vehicle speed (in m/s)
// cruise_speed is in m/s, cruise_throttle should be in the range -1 to +1
float Mode::calc_speed_max(float cruise_speed, float cruise_throttle) const
{
    float speed_max;

    // sanity checks
    if (cruise_throttle > 1.0f || cruise_throttle < 0.05f) {
        speed_max = cruise_speed;
    } else {
        // project vehicle's maximum speed
        speed_max = (1.0f / cruise_throttle) * cruise_speed;
    }

    // constrain to 30m/s (108km/h) and return
    return constrain_float(speed_max, 0.0f, 30.0f);
}

// calculate pilot input to nudge speed up or down
//  target_speed should be in meters/sec
//  cruise_speed is vehicle's cruising speed, cruise_throttle is the throttle (from -1 to +1) that achieves the cruising speed
//  return value is a new speed (in m/s) which up to the projected maximum speed based on the cruise speed and cruise throttle
float Mode::calc_speed_nudge(float target_speed, float cruise_speed, float cruise_throttle)
{
    // return immediately if pilot is not attempting to nudge speed
    // pilot can nudge up speed if throttle (in range -100 to +100) is above 50% of center in direction of travel
    const int16_t pilot_throttle = constrain_int16(rover.channel_throttle->get_control_in(), -100, 100);
    if (((pilot_throttle <= 50) && (target_speed >= 0.0f)) ||
        ((pilot_throttle >= -50) && (target_speed <= 0.0f))) {
        return target_speed;
    }

    // sanity checks
    if (cruise_throttle > 1.0f || cruise_throttle < 0.05f) {
        return target_speed;
    }

    // project vehicle's maximum speed
    const float vehicle_speed_max = calc_speed_max(cruise_speed, cruise_throttle);

    // return unadjusted target if already over vehicle's projected maximum speed
    if (fabsf(target_speed) >= vehicle_speed_max) {
        return target_speed;
    }

    const float speed_increase_max = vehicle_speed_max - fabsf(target_speed);
    float speed_nudge = ((static_cast<float>(abs(pilot_throttle)) - 50.0f) * 0.02f) * speed_increase_max;
    if (pilot_throttle < 0) {
        speed_nudge = -speed_nudge;
    }

    return target_speed + speed_nudge;
}

// calculated a reduced speed(in m/s) based on yaw error and lateral acceleration and/or distance to a waypoint
// should be called after calc_lateral_acceleration and before calc_throttle
// relies on these internal members being updated: lateral_acceleration, _yaw_error_cd, _distance_to_destination
float Mode::calc_reduced_speed_for_turn_or_distance(float desired_speed)
{
    // this method makes use the following internal variables
    const float yaw_error_cd = _yaw_error_cd;
    const float target_lateral_accel_G = attitude_control.get_desired_lat_accel();
    const float distance_to_waypoint = _distance_to_destination;

    // calculate the yaw_error_ratio which is the error (capped at 90degrees) expressed as a ratio (from 0 ~ 1)
    float yaw_error_ratio = constrain_float(fabsf(yaw_error_cd / 9000.0f), 0.0f, 1.0f);

    // apply speed_turn_gain parameter (expressed as a percentage) to yaw_error_ratio
    yaw_error_ratio *= (100 - g.speed_turn_gain) * 0.01f;

    // calculate absolute lateral acceleration expressed as a ratio (from 0 ~ 1) of the vehicle's maximum lateral acceleration
    const float lateral_accel_ratio = constrain_float(fabsf(target_lateral_accel_G / (g.turn_max_g * GRAVITY_MSS)), 0.0f, 1.0f);

    // calculate a lateral acceleration based speed scaling
    const float lateral_accel_speed_scaling = 1.0f - lateral_accel_ratio * yaw_error_ratio;

    // calculate a pivot steering based speed scaling (default to no reduction)
    float pivot_speed_scaling = 1.0f;
    if (rover.use_pivot_steering(yaw_error_cd)) {
        pivot_speed_scaling = 0.0f;
    }

    // scaled speed
    float speed_scaled = desired_speed * MIN(lateral_accel_speed_scaling, pivot_speed_scaling);

    // limit speed based on distance to waypoint and max acceleration/deceleration
    if (is_positive(distance_to_waypoint) && is_positive(attitude_control.get_accel_max())) {
        const float speed_max = safe_sqrt(2.0f * distance_to_waypoint * attitude_control.get_accel_max() + sq(_desired_speed_final));
        speed_scaled = constrain_float(speed_scaled, -speed_max, speed_max);
    }

    // return minimum speed
    return speed_scaled;
}

// calculate the lateral acceleration target to cause the vehicle to drive along the path from origin to destination
// this function updates the _yaw_error_cd value
void Mode::calc_steering_to_waypoint(const struct Location &origin, const struct Location &destination, bool reversed)
{
    // record system time of call
    last_steer_to_wp_ms = AP_HAL::millis();

    // Calculate the required turn of the wheels
    // negative error = left turn
    // positive error = right turn
    rover.nav_controller->set_reverse(reversed);
    rover.nav_controller->update_waypoint(origin, destination, g.waypoint_radius);
    float desired_lat_accel = rover.nav_controller->lateral_acceleration();
    float desired_heading = rover.nav_controller->target_bearing_cd();
    if (reversed) {
        _yaw_error_cd = wrap_180_cd(desired_heading - ahrs.yaw_sensor + 18000);
    } else {
        _yaw_error_cd = wrap_180_cd(desired_heading - ahrs.yaw_sensor);
    }

    if (rover.use_pivot_steering(_yaw_error_cd)) {
        // for pivot turns use heading controller
        calc_steering_to_heading(desired_heading, reversed);
    } else {
        // call lateral acceleration to steering controller
        calc_steering_from_lateral_acceleration(desired_lat_accel, reversed);
    }
}

/*
    calculate steering output given lateral_acceleration
*/
void Mode::calc_steering_from_lateral_acceleration(float lat_accel, bool reversed)
{
    // add obstacle avoidance response to lateral acceleration target
    if (!reversed) {
        lat_accel += (rover.obstacle.turn_angle / 45.0f) * g.turn_max_g;
    }

    // constrain to max G force
    lat_accel = constrain_float(lat_accel, -g.turn_max_g * GRAVITY_MSS, g.turn_max_g * GRAVITY_MSS);

    // send final steering command to motor library
    const float steering_out = attitude_control.get_steering_out_lat_accel(lat_accel,
                                                                           g2.motors.have_skid_steering(),
                                                                           g2.motors.have_vectored_thrust(),
                                                                           g2.motors.limit.steer_left,
                                                                           g2.motors.limit.steer_right,
                                                                           reversed);
    g2.motors.set_steering(steering_out * 4500.0f);
}

// calculate steering output to drive towards desired heading
void Mode::calc_steering_to_heading(float desired_heading_cd, bool reversed)
{
    // calculate yaw error (in radians) and pass to steering angle controller
    const float steering_out = attitude_control.get_steering_out_heading(radians(desired_heading_cd*0.01f),
                                                                         g2.motors.have_skid_steering(),
                                                                         g2.motors.have_vectored_thrust(),
                                                                         g2.motors.limit.steer_left,
                                                                         g2.motors.limit.steer_right,
                                                                         reversed);
    g2.motors.set_steering(steering_out * 4500.0f);
}

// calculate vehicle stopping point using current location, velocity and maximum acceleration
void Mode::calc_stopping_location(Location& stopping_loc)
{
    // default stopping location
    stopping_loc = rover.current_loc;

    // get current velocity vector and speed
    const Vector2f velocity = ahrs.groundspeed_vector();
    const float speed = velocity.length();

    // avoid divide by zero
    if (!is_positive(speed)) {
        stopping_loc = rover.current_loc;
        return;
    }

    // get stopping distance in meters
    const float stopping_dist = attitude_control.get_stopping_distance(speed);

    // calculate stopping position from current location in meters
    const Vector2f stopping_offset = velocity.normalized() * stopping_dist;

    location_offset(stopping_loc, stopping_offset.x, stopping_offset.y);
}
  push: #pragma once

#include <stdint.h>
#include <GCS_MAVLink/GCS_MAVLink.h>  // for MAV_SEVERITY
#include "defines.h"

#define MODE_NEXT_HEADING_UNKNOWN   99999.0f    // used to indicate to set_desired_location method that next leg's heading is unknown

// pre-define ModeRTL so Auto can appear higher in this file
class ModeRTL;

class Mode
{
public:

    // Constructor
    Mode();

    // enter this mode, returns false if we failed to enter
    bool enter();

    // perform any cleanups required:
    void exit();

    // returns a unique number specific to this mode
    virtual uint32_t mode_number() const = 0;

    // returns short text name (up to 4 bytes)
    virtual const char *name4() const = 0;

    //
    // methods that sub classes should override to affect movement of the vehicle in this mode
    //

    // convert user input to targets, implement high level control for this mode
    virtual void update() = 0;

    //
    // attributes of the mode
    //

    // return if in non-manual mode : Auto, Guided, RTL, SmartRTL
    virtual bool is_autopilot_mode() const { return false; }

    // returns true if vehicle can be armed or disarmed from the transmitter in this mode
    virtual bool allows_arming_from_transmitter() { return !is_autopilot_mode(); }

    //
    // attributes for mavlink system status reporting
    //

    // returns true if any RC input is used
    virtual bool has_manual_input() const { return false; }

    // true if heading is controlled
    virtual bool attitude_stabilized() const { return true; }

    // true if mode requires position and/or velocity estimate
    virtual bool requires_position() const { return true; }
    virtual bool requires_velocity() const { return true; }

    //
    // navigation methods
    //

    // return distance (in meters) to destination
    virtual float get_distance_to_destination() const { return 0.0f; }

    // set desired location and speed (used in RTL, Guided, Auto)
    //   next_leg_bearing_cd should be heading to the following waypoint (used to slow the vehicle in order to make the turn)
    virtual void set_desired_location(const struct Location& destination, float next_leg_bearing_cd = MODE_NEXT_HEADING_UNKNOWN);

    // set desired location as offset from the EKF origin, return true on success
    bool set_desired_location_NED(const Vector3f& destination, float next_leg_bearing_cd = MODE_NEXT_HEADING_UNKNOWN);

    // true if vehicle has reached desired location. defaults to true because this is normally used by missions and we do not want the mission to become stuck
    virtual bool reached_destination() { return true; }

    // set desired heading and speed - supported in Auto and Guided modes
    virtual void set_desired_heading_and_speed(float yaw_angle_cd, float target_speed);

    // get speed error in m/s, returns zero for modes that do not control speed
    float speed_error() const { return _speed_error; }

    // get default speed for this mode (held in CRUISE_SPEED, WP_SPEED or RTL_SPEED)
    // rtl argument should be true if called from RTL or SmartRTL modes (handled here to avoid duplication)
    float get_speed_default(bool rtl = false) const;

    // set desired speed
    void set_desired_speed(float speed) { _desired_speed = speed; }

    // restore desired speed to default from parameter values (CRUISE_SPEED or WP_SPEED)
    // rtl argument should be true if called from RTL or SmartRTL modes (handled here to avoid duplication)
    void set_desired_speed_to_default(bool rtl = false);

protected:

    // subclasses override this to perform checks before entering the mode
    virtual bool _enter() { return true; }

    // subclasses override this to perform any required cleanup when exiting the mode
    virtual void _exit() { return; }

    // decode pilot steering held in channel_steer, channel_throttle and return in steer_out and throttle_out arguments
    // steering_out is in the range -4500 ~ +4500, throttle_out is in the range -100 ~ +100
    void get_pilot_desired_steering_and_throttle(float &steering_out, float &throttle_out);

    // calculate steering output to drive along line from origin to destination waypoint
    void calc_steering_to_waypoint(const struct Location &origin, const struct Location &destination, bool reversed = false);

    // calculate steering angle given a desired lateral acceleration
    void calc_steering_from_lateral_acceleration(float lat_accel, bool reversed = false);

    // calculate steering output to drive towards desired heading
    void calc_steering_to_heading(float desired_heading_cd, bool reversed = false);

    // calculates the amount of throttle that should be output based
    // on things like proximity to corners and current speed
    virtual void calc_throttle(float target_speed, bool nudge_allowed, bool avoidance_enabled);

    // performs a controlled stop. returns true once vehicle has stopped
    bool stop_vehicle();

    // estimate maximum vehicle speed (in m/s)
    // cruise_speed is in m/s, cruise_throttle should be in the range -1 to +1
    float calc_speed_max(float cruise_speed, float cruise_throttle) const;

    // calculate pilot input to nudge speed up or down
    //  target_speed should be in meters/sec
    //  cruise_speed is vehicle's cruising speed, cruise_throttle is the throttle (from -1 to +1) that achieves the cruising speed
    //  return value is a new speed (in m/s) which up to the projected maximum speed based on the cruise speed and cruise throttle
    float calc_speed_nudge(float target_speed, float cruise_speed, float cruise_throttle);

    // calculated a reduced speed(in m/s) based on yaw error and lateral acceleration and/or distance to a waypoint
    // should be called after calc_steering_to_waypoint and before calc_throttle
    // relies on these internal members being updated: lateral_acceleration, _yaw_error_cd, _distance_to_destination
    float calc_reduced_speed_for_turn_or_distance(float desired_speed);

    // calculate vehicle stopping location using current location, velocity and maximum acceleration
    void calc_stopping_location(Location& stopping_loc);

    // references to avoid code churn:
    class AP_AHRS &ahrs;
    class Parameters &g;
    class ParametersG2 &g2;
    class RC_Channel *&channel_steer;
    class RC_Channel *&channel_throttle;
    class AP_Mission &mission;
    class AR_AttitudeControl &attitude_control;


    // private members for waypoint navigation
    Location _origin;           // origin Location (vehicle will travel from the origin to the destination)
    Location _destination;      // destination Location when in Guided_WP
    float _distance_to_destination; // distance from vehicle to final destination in meters
    bool _reached_destination;  // true once the vehicle has reached the destination
    float _desired_yaw_cd;      // desired yaw in centi-degrees
    float _yaw_error_cd;        // error between desired yaw and actual yaw in centi-degrees
    float _desired_speed;       // desired speed in m/s
    float _desired_speed_final; // desired speed in m/s when we reach the destination
    float _speed_error;         // ground speed error in m/s
    uint32_t last_steer_to_wp_ms;   // system time of last call to calc_steering_to_waypoint
};


class ModeAcro : public Mode
{
public:

    uint32_t mode_number() const override { return ACRO; }
    const char *name4() const override { return "ACRO"; }

    // methods that affect movement of the vehicle in this mode
    void update() override;

    // attributes for mavlink system status reporting
    bool has_manual_input() const override { return true; }

    // acro mode requires a velocity estimate for non skid-steer rovers
    bool requires_position() const override { return false; }
    bool requires_velocity() const override;
};


class ModeAuto : public Mode
{
public:

    // constructor
    ModeAuto(ModeRTL& mode_rtl);

    uint32_t mode_number() const override { return AUTO; }
    const char *name4() const override { return "AUTO"; }

    // methods that affect movement of the vehicle in this mode
    void update() override;
    void calc_throttle(float target_speed, bool nudge_allowed, bool avoidance_enabled);

    // attributes of the mode
    bool is_autopilot_mode() const override { return true; }

    // return distance (in meters) to destination
    float get_distance_to_destination() const override;

    // set desired location, heading and speed
    void set_desired_location(const struct Location& destination, float next_leg_bearing_cd = MODE_NEXT_HEADING_UNKNOWN);
    bool reached_destination() override;

    // heading and speed control
    void set_desired_heading_and_speed(float yaw_angle_cd, float target_speed) override;
    bool reached_heading();

    // start RTL (within auto)
    void start_RTL();

    // execute the mission in reverse (i.e. backing up)
    void set_reversed(bool value);

protected:

    bool _enter() override;
    void _exit() override;

    enum AutoSubMode {
        Auto_WP,                // drive to a given location
        Auto_HeadingAndSpeed,   // turn to a given heading
        Auto_RTL                // perform RTL within auto mode
    } _submode;

private:

    bool check_trigger(void);

    // references
    ModeRTL& _mode_rtl;

    // this is set to true when auto has been triggered to start
    bool auto_triggered;

    bool _reached_heading;      // true when vehicle has reached desired heading in TurnToHeading sub mode
    bool _reversed;             // execute the mission by backing up
};


class ModeGuided : public Mode
{
public:

    uint32_t mode_number() const override { return GUIDED; }
    const char *name4() const override { return "GUID"; }

    // methods that affect movement of the vehicle in this mode
    void update() override;

    // attributes of the mode
    bool is_autopilot_mode() const override { return true; }

    // return distance (in meters) to destination
    float get_distance_to_destination() const override;

    // set desired location, heading and speed
    void set_desired_location(const struct Location& destination);
    void set_desired_heading_and_speed(float yaw_angle_cd, float target_speed) override;

    // set desired heading-delta, turn-rate and speed
    void set_desired_heading_delta_and_speed(float yaw_delta_cd, float target_speed);
    void set_desired_turn_rate_and_speed(float turn_rate_cds, float target_speed);

protected:

    enum GuidedMode {
        Guided_WP,
        Guided_HeadingAndSpeed,
        Guided_TurnRateAndSpeed
    };

    bool _enter() override;

    GuidedMode _guided_mode;    // stores which GUIDED mode the vehicle is in

    // attitude control
    bool have_attitude_target;  // true if we have a valid attitude target
    uint32_t _des_att_time_ms;  // system time last call to set_desired_attitude was made (used for timeout)
    float _desired_yaw_rate_cds;// target turn rate centi-degrees per second
};


class ModeHold : public Mode
{
public:

    uint32_t mode_number() const override { return HOLD; }
    const char *name4() const override { return "HOLD"; }

    // methods that affect movement of the vehicle in this mode
    void update() override;

    // attributes for mavlink system status reporting
    bool attitude_stabilized() const override { return false; }

    // hold mode does not require position or velocity estimate
    bool requires_position() const override { return false; }
    bool requires_velocity() const override { return false; }
};


class ModeManual : public Mode
{
public:

    uint32_t mode_number() const override { return MANUAL; }
    const char *name4() const override { return "MANU"; }

    // methods that affect movement of the vehicle in this mode
    void update() override;

    // attributes for mavlink system status reporting
    bool has_manual_input() const override { return true; }
    bool attitude_stabilized() const override { return false; }

    // manual mode does not require position or velocity estimate
    bool requires_position() const override { return false; }
    bool requires_velocity() const override { return false; }
};


class ModeRTL : public Mode
{
public:

    uint32_t mode_number() const override { return RTL; }
    const char *name4() const override { return "RTL"; }

    // methods that affect movement of the vehicle in this mode
    void update() override;

    // attributes of the mode
    bool is_autopilot_mode() const override { return true; }

    float get_distance_to_destination() const override { return _distance_to_destination; }
    bool reached_destination() override { return _reached_destination; }

protected:

    bool _enter() override;
};

class ModeSmartRTL : public Mode
{
public:

    uint32_t mode_number() const override { return SMART_RTL; }
    const char *name4() const override { return "SRTL"; }

    // methods that affect movement of the vehicle in this mode
    void update() override;

    // attributes of the mode
    bool is_autopilot_mode() const override { return true; }

    float get_distance_to_destination() const override { return _distance_to_destination; }
    bool reached_destination() override { return smart_rtl_state == SmartRTL_StopAtHome; }

    // save current position for use by the smart_rtl flight mode
    void save_position();

protected:

    // Safe RTL states
    enum SmartRTLState {
        SmartRTL_WaitForPathCleanup,
        SmartRTL_PathFollow,
        SmartRTL_StopAtHome,
        SmartRTL_Failure
    } smart_rtl_state;

    bool _enter() override;
    bool _load_point;
};


class ModeSteering : public Mode
{
public:

    uint32_t mode_number() const override { return STEERING; }
    const char *name4() const override { return "STER"; }

    // methods that affect movement of the vehicle in this mode
    void update() override;

    // attributes for mavlink system status reporting
    bool has_manual_input() const override { return true; }

    // steering requires velocity but not position
    bool requires_position() const override { return false; }
    bool requires_velocity() const override { return true; }
};

class ModeInitializing : public Mode
{
public:

    uint32_t mode_number() const override { return INITIALISING; }
    const char *name4() const override { return "INIT"; }

    // methods that affect movement of the vehicle in this mode
    void update() override { }

    // attributes for mavlink system status reporting
    bool has_manual_input() const override { return true; }
    bool attitude_stabilized() const override { return false; }
};
    branches: [ "main" ]
  pull_request: #include "mode.h"
#include "Rover.h"

void ModeAcro::update()
{

    // convert pilot stick input into desired steering and throttle
    float desired_steering, desired_throttle;
    get_pilot_desired_steering_and_throttle(desired_steering, desired_throttle);

    // set reverse flag backing up
    const bool reversed = is_negative(desired_throttle);
    rover.set_reverse(reversed);

    // convert pilot steering input to desired turn rate in radians/sec
    const float target_turn_rate = (desired_steering / 4500.0f) * radians(g2.acro_turn_rate);

    // run steering turn rate controller and throttle controller
    const float steering_out = attitude_control.get_steering_out_rate(
                                                                    target_turn_rate,
                                                                    g2.motors.have_skid_steering(),
                                                                    g2.motors.have_vectored_thrust(),
                                                                    g2.motors.limit.steer_left,
                                                                    g2.motors.limit.steer_right,
                                                                    reversed);

    g2.motors.set_steering(steering_out * 4500.0f);

    // get speed forward
    float speed;
    if (!attitude_control.get_forward_speed(speed)) {
        // no valid speed, just use the provided throttle
        g2.motors.set_throttle(desired_throttle);
    } else {

        // convert pilot throttle input to desired speed
        float target_speed = desired_throttle * 0.01f * calc_speed_max(g.speed_cruise, g.throttle_cruise * 0.01f);

        calc_throttle(target_speed, false, true);
    }
}

bool ModeAcro::requires_velocity() const
{
    return g2.motors.have_skid_steering()? false: true;
}
    branches: [ "main" ]
  schedule: #include "mode.h"
#include "Rover.h"

// constructor
ModeAuto::ModeAuto(ModeRTL& mode_rtl) :
    _mode_rtl(mode_rtl)
{
}

bool ModeAuto::_enter()
{
    // fail to enter auto if no mission commands
    if (mission.num_commands() == 0) {
        gcs().send_text(MAV_SEVERITY_NOTICE, "No Mission. Can't set AUTO.");
        return false;
    }

    // initialise waypoint speed
    set_desired_speed_to_default();

    // init location target
    set_desired_location(rover.current_loc);

    // other initialisation
    auto_triggered = false;

    // initialise reversed to be false
    set_reversed(false);

    // restart mission processing
    mission.start_or_resume();
    return true;
}

void ModeAuto::_exit()
{
    // stop running the mission
    if (mission.state() == AP_Mission::MISSION_RUNNING) {
        mission.stop();
    }
}

void ModeAuto::update()
{
    switch (_submode) {
        case Auto_WP:
        {
            _distance_to_destination = get_distance(rover.current_loc, _destination);
            const bool near_wp = _distance_to_destination <= rover.g.waypoint_radius;
            // check if we've reached the destination
            if (!_reached_destination && (near_wp || location_passed_point(rover.current_loc, _origin, _destination))) {
                // trigger reached
                _reached_destination = true;
            }
            // determine if we should keep navigating
            if (!_reached_destination || (rover.is_boat() && !near_wp)) {
                // continue driving towards destination
                calc_steering_to_waypoint(_reached_destination ? rover.current_loc : _origin, _destination, _reversed);
                calc_throttle(calc_reduced_speed_for_turn_or_distance(_reversed ? -_desired_speed : _desired_speed), true, false);
            } else {
                // we have reached the destination so stop
                stop_vehicle();
            }
            break;
        }

        case Auto_HeadingAndSpeed:
        {
            if (!_reached_heading) {
                // run steering and throttle controllers
                calc_steering_to_heading(_desired_yaw_cd, _desired_speed < 0);
                calc_throttle(_desired_speed, true, false);
                // check if we have reached within 5 degrees of target
                _reached_heading = (fabsf(_desired_yaw_cd - ahrs.yaw_sensor) < 500);
            } else {
                stop_vehicle();
            }
            break;
        }

        case Auto_RTL:
            _mode_rtl.update();
            break;
    }
}

// return distance (in meters) to destination
float ModeAuto::get_distance_to_destination() const
{
    if (_submode == Auto_RTL) {
        return _mode_rtl.get_distance_to_destination();
    }
    return _distance_to_destination;
}

// set desired location to drive to
void ModeAuto::set_desired_location(const struct Location& destination, float next_leg_bearing_cd)
{
    // call parent
    Mode::set_desired_location(destination, next_leg_bearing_cd);

    _submode = Auto_WP;
}

// return true if vehicle has reached or even passed destination
bool ModeAuto::reached_destination()
{
    if (_submode == Auto_WP) {
        return _reached_destination;
    }
    if (_submode == Auto_RTL) {
        return _mode_rtl.reached_destination();
    }
    // we should never reach here but just in case, return true to allow missions to continue
    return true;
}

// set desired heading in centidegrees (vehicle will turn to this heading)
void ModeAuto::set_desired_heading_and_speed(float yaw_angle_cd, float target_speed)
{
    // call parent
    Mode::set_desired_heading_and_speed(yaw_angle_cd, target_speed);

    _submode = Auto_HeadingAndSpeed;
    _reached_heading = false;
}

// return true if vehicle has reached desired heading
bool ModeAuto::reached_heading()
{
    if (_submode == Auto_HeadingAndSpeed) {
        return _reached_heading;
    }
    // we should never reach here but just in case, return true to allow missions to continue
    return true;
}

// start RTL (within auto)
void ModeAuto::start_RTL()
{
    if (_mode_rtl.enter()) {
        _submode = Auto_RTL;
    }
}

// execute the mission in reverse (i.e. backing up)
void ModeAuto::set_reversed(bool value)
{
    if (_reversed != value) {
        _reversed = value;
        rover.set_reverse(_reversed);
    }
}

/*
    check for triggering of start of auto mode
*/
bool ModeAuto::check_trigger(void)
{
    // check for user pressing the auto trigger to off
    if (auto_triggered && g.auto_trigger_pin != -1 && rover.check_digital_pin(g.auto_trigger_pin) == 1) {
        gcs().send_text(MAV_SEVERITY_WARNING, "AUTO triggered off");
        auto_triggered = false;
        return false;
    }

    // if already triggered, then return true, so you don't
    // need to hold the switch down
    if (auto_triggered) {
        return true;
    }

    // return true if auto trigger and kickstart are disabled
    if (g.auto_trigger_pin == -1 && is_zero(g.auto_kickstart)) {
        // no trigger configured - let's go!
        auto_triggered = true;
        return true;
    }

    // check if trigger pin has been pushed
    if (g.auto_trigger_pin != -1 && rover.check_digital_pin(g.auto_trigger_pin) == 0) {
        gcs().send_text(MAV_SEVERITY_WARNING, "Triggered AUTO with pin");
        auto_triggered = true;
        return true;
    }

    // check if mission is started by giving vehicle a kick with acceleration > AUTO_KICKSTART
    if (!is_zero(g.auto_kickstart)) {
        const float xaccel = rover.ins.get_accel().x;
        if (xaccel >= g.auto_kickstart) {
            gcs().send_text(MAV_SEVERITY_WARNING, "Triggered AUTO xaccel=%.1f", static_cast<double>(xaccel));
            auto_triggered = true;
            return true;
        }
    }

    return false;
}

void ModeAuto::calc_throttle(float target_speed, bool nudge_allowed, bool avoidance_enabled)
{
    // If not autostarting set the throttle to minimum
    if (!check_trigger()) {
        stop_vehicle();
        return;
    }
    Mode::calc_throttle(target_speed, nudge_allowed, avoidance_enabled);
}
    - cron: '16 4 * * 2'

env: #include "mode.h"
#include "Rover.h"

bool ModeGuided::_enter()
{
    // initialise waypoint speed
    set_desired_speed_to_default();

    // when entering guided mode we set the target as the current location.
    set_desired_location(rover.current_loc);

    // guided mode never travels in reverse
    rover.set_reverse(false);

    return true;
}

void ModeGuided::update()
{
    switch (_guided_mode) {
        case Guided_WP:
        {
            _distance_to_destination = get_distance(rover.current_loc, _destination);
            const bool near_wp = _distance_to_destination <= rover.g.waypoint_radius;
            // check if we've reached the destination
            if (!_reached_destination && (near_wp || location_passed_point(rover.current_loc, _origin, _destination))) {
                _reached_destination = true;
                rover.gcs().send_mission_item_reached_message(0);
            }
            // determine if we should keep navigating
            if (!_reached_destination || (rover.is_boat() && !near_wp)) {
                // drive towards destination
                calc_steering_to_waypoint(_reached_destination ? rover.current_loc : _origin, _destination);
                calc_throttle(calc_reduced_speed_for_turn_or_distance(_desired_speed), true, true);
            } else {
                stop_vehicle();
            }
            break;
        }

        case Guided_HeadingAndSpeed:
        {
            // stop vehicle if target not updated within 3 seconds
            if (have_attitude_target && (millis() - _des_att_time_ms) > 3000) {
                gcs().send_text(MAV_SEVERITY_WARNING, "target not received last 3secs, stopping");
                have_attitude_target = false;
            }
            if (have_attitude_target) {
                // run steering and throttle controllers
                calc_steering_to_heading(_desired_yaw_cd, _desired_speed < 0);
                calc_throttle(_desired_speed, true, true);
            } else {
                stop_vehicle();
                g2.motors.set_steering(0.0f);
            }
            break;
        }

        case Guided_TurnRateAndSpeed:
        {
            // stop vehicle if target not updated within 3 seconds
            if (have_attitude_target && (millis() - _des_att_time_ms) > 3000) {
                gcs().send_text(MAV_SEVERITY_WARNING, "target not received last 3secs, stopping");
                have_attitude_target = false;
            }
            if (have_attitude_target) {
                // run steering and throttle controllers
                float steering_out = attitude_control.get_steering_out_rate(radians(_desired_yaw_rate_cds / 100.0f),
                                                                            g2.motors.have_skid_steering(),
                                                                            g2.motors.have_vectored_thrust(),
                                                                            g2.motors.limit.steer_left,
                                                                            g2.motors.limit.steer_right,
                                                                            _desired_speed < 0);
                g2.motors.set_steering(steering_out * 4500.0f);
                calc_throttle(_desired_speed, true, true);
            } else {
                stop_vehicle();
                g2.motors.set_steering(0.0f);
            }
            break;
        }

        default:
            gcs().send_text(MAV_SEVERITY_WARNING, "Unknown GUIDED mode");
            break;
    }
}

// return distance (in meters) to destination
float ModeGuided::get_distance_to_destination() const
{
    if (_guided_mode != Guided_WP || _reached_destination) {
        return 0.0f;
    }
    return _distance_to_destination;
}

// set desired location
void ModeGuided::set_desired_location(const struct Location& destination)
{
    // call parent
    Mode::set_desired_location(destination);

    // handle guided specific initialisation and logging
    _guided_mode = ModeGuided::Guided_WP;
    rover.Log_Write_GuidedTarget(_guided_mode, Vector3f(_destination.lat, _destination.lng, 0), Vector3f(_desired_speed, 0.0f, 0.0f));
}

// set desired attitude
void ModeGuided::set_desired_heading_and_speed(float yaw_angle_cd, float target_speed)
{
    // call parent
    Mode::set_desired_heading_and_speed(yaw_angle_cd, target_speed);

    // handle guided specific initialisation and logging
    _guided_mode = ModeGuided::Guided_HeadingAndSpeed;
    _des_att_time_ms = AP_HAL::millis();
    _reached_destination = false;

    // record targets
    _desired_yaw_cd = yaw_angle_cd;
    _desired_speed = target_speed;
    have_attitude_target = true;

    // log new target
    rover.Log_Write_GuidedTarget(_guided_mode, Vector3f(_desired_yaw_cd, 0.0f, 0.0f), Vector3f(_desired_speed, 0.0f, 0.0f));
}

void ModeGuided::set_desired_heading_delta_and_speed(float yaw_delta_cd, float target_speed)
{
    // handle initialisation
    if (_guided_mode != ModeGuided::Guided_HeadingAndSpeed) {
        _guided_mode = ModeGuided::Guided_HeadingAndSpeed;
        _desired_yaw_cd = ahrs.yaw_sensor;
    }
    set_desired_heading_and_speed(wrap_180_cd(_desired_yaw_cd + yaw_delta_cd), target_speed);
}

// set desired velocity
void ModeGuided::set_desired_turn_rate_and_speed(float turn_rate_cds, float target_speed)
{
    // handle initialisation
    _guided_mode = ModeGuided::Guided_TurnRateAndSpeed;
    _des_att_time_ms = AP_HAL::millis();
    _reached_destination = false;

    // record targets
    _desired_yaw_rate_cds = turn_rate_cds;
    _desired_speed = target_speed;
    have_attitude_target = true;

    // log new target
    rover.Log_Write_GuidedTarget(_guided_mode, Vector3f(_desired_yaw_rate_cds, 0.0f, 0.0f), Vector3f(_desired_speed, 0.0f, 0.0f));
}
  # Path to the CMake build directory.
  build: '${{ #include "mode.h"
#include "Rover.h"

void ModeHold::update()
{
    // hold position - stop motors and center steering
    g2.motors.set_throttle(0.0f);
    g2.motors.set_steering(0.0f);

    // hold mode never reverses
    rover.set_reverse(false);
} }}/build'

permissions: #include "mode.h"
#include "Rover.h"

void ModeManual::update()
{
    float desired_steering, desired_throttle;
    get_pilot_desired_steering_and_throttle(desired_steering, desired_throttle);

    // copy RC scaled inputs to outputs
    g2.motors.set_throttle(desired_throttle);
    g2.motors.set_steering(desired_steering);

    // mark us as in_reverse when using a negative throttle to stop AHRS getting off
    rover.set_reverse(is_negative(g2.motors.get_throttle()));
}
  contents: read

jobs: #include "mode.h"
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
  analyze: #include "mode.h"
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
    permissions: #include "mode.h"
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
      contents: read # for actions/checkout to fetch code
      security-events: write # for github/codeql-action/upload-sarif to upload SARIF results
      actions: read # only required for a private repository by github/codeql-action/upload-sarif to get the Action run status
    name: Analyze
    runs-on: windows-latest

    steps: #include "Rover.h"

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
      - name: Checkout repository
        uses: actions/checkout@v4

      - name: Configure CMake
        run: cmake -B ${{ #include "Rover.h"

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
} }}

      # Build is not required unless generated source files are used
      # - name: Build CMake
      #   run: cmake --build ${{ env.build }}

      - name: Initialize MSVC Code Analysis
        uses: microsoft/msvc-code-analysis-action@04825f6d9e00f87422d6bf04e1a38b1f3ed60d99
        # Provide a unique ID to access the sarif output path
        id: run-analysis
        with: APMrover2 Release Notes:
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
          cmakeBuildDirectory: ${{ #include "Rover.h"

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
} }}
          # Ruleset file that will determine what checks will be run
          ruleset: NativeRecommendedRules.ruleset

      # Upload SARIF file to GitHub Code Scanning Alerts
      - name: Upload SARIF to GitHub
        uses: github/codeql-action/upload-sarif@v2
        with: /*****************************************************************************
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
          sarif_file: ${{ /*
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
}; }}

      # Upload SARIF file as an Artifact to download and view
      # - name: Upload SARIF as an Artifact
      #   uses: actions/upload-artifact@v3
      #   with: #pragma once

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
      #     name: sarif-file
      #     path: ${{ #!/usr/bin/env python
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
    ) }}
