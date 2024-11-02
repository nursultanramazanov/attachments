name: Rust

on: 
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
  push: #pragma once

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
    branches: [ "main" ]
  pull_request: 
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
    branches: [ "main" ]

env: #pragma once

#include "../Command.h"

// Sends a quit signal to the session.
// No params.
class QuitCommand : public Command
{
public:
    QuitCommand(Chess &chess);

    Result apply(const std::vector<std::string> &params) override;
}; 
  CARGO_TERM_COLOR: 
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

jobs: #pragma once

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
  build: 
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

    runs-on: #pragma once

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
}; 

    steps: #pragma once

struct Result
{
    bool error;
    bool quit;
}; 
    - uses: #!/usr/bin/env python

################################################################################################
# @File MissionCheck.py
# Automated mission loading, execution and monitoring
# for Continuous Integration
#
# @author   <>
#
# Code partly based on DroneKit (c) Copyright 2015-2016, 3D Robotics.
################################################################################################


################################################################################################
# Settings
################################################################################################

connection_string       = '127.0.0.1:14540'

import_mission_filename = 'VTOLmission.txt'
max_execution_time      = 250
alt_acceptance_radius   = 10

################################################################################################
# Init
################################################################################################

# Import DroneKit-Python
from dronekit import connect, Command
from pymavlink import mavutil
import time, sys, argparse

parser = argparse.ArgumentParser()
parser.add_argument("-c", "--connect", help="connection string")
parser.add_argument("-f", "--filename", help="mission filename")
parser.add_argument("-t", "--timeout", help="execution timeout", type=float)
parser.add_argument("-a", "--altrad", help="altitude acceptance radius", type=float)
args = parser.parse_args()

if args.connect:
    connection_string = args.connect
if args.filename:
    import_mission_filename = args.filename
if args.timeout:
    max_execution_time = args.timeout
if args.altrad:
    alt_acceptance_radius = args.altrad



mission_failed = False
MAV_MODE_AUTO = 4

# start time counter
start_time = time.time()
elapsed_time = time.time() - start_time



# Connect to the Vehicle
print "Connecting"
vehicle = connect(connection_string, wait_ready=True)

while not vehicle.system_status.state == "STANDBY" or vehicle.gps_0.fix_type < 3:
    if time.time() - start_time > 20:
        print "FAILED: SITL did not reach standby with GPS fix within 20 seconds"
        sys.exit(98)
    print "Waiting for vehicle to initialise... %s " % vehicle.system_status.state
    time.sleep(1)

# Display basic vehicle state
print " Type: %s" % vehicle._vehicle_type
print " Armed?: %s" % vehicle.armed
print " System status: %s" % vehicle.system_status.state
print " GPS: %s" % vehicle.gps_0
print " Alt: %s" % vehicle.location.global_relative_frame.alt


################################################################################################
# Functions
################################################################################################


def readmission(aFileName):
    """
    Load a mission from a file into a list. The mission definition is in the Waypoint file
    format (http://qgroundcontrol.org/mavlink/waypoint_protocol#waypoint_file_format).
    This function is used by upload_mission().
    """
    cmds = vehicle.commands
    missionlist=[]
    with open(aFileName) as f:
        for i, line in enumerate(f):
            if i==0:
                if not line.startswith('QGC WPL 110'):
                    raise Exception('File is not supported WP version')
            else:
                linearray=line.split('\t')
                ln_index=int(linearray[0])
                ln_currentwp=int(linearray[1])
                ln_frame=int(linearray[2])
                ln_command=int(linearray[3])
                ln_param1=float(linearray[4])
                ln_param2=float(linearray[5])
                ln_param3=float(linearray[6])
                ln_param4=float(linearray[7])
                ln_param5=float(linearray[8])
                ln_param6=float(linearray[9])
                ln_param7=float(linearray[10])
                ln_autocontinue=int(linearray[11].strip())
                cmd = Command( 0, 0, 0, ln_frame, ln_command, ln_currentwp, ln_autocontinue, ln_param1, ln_param2, ln_param3, ln_param4, ln_param5, ln_param6, ln_param7)
                missionlist.append(cmd)
    return missionlist


def upload_mission(aFileName):
    """
    Upload a mission from a file.
    """
    #Read mission from file
    missionlist = readmission(aFileName)

    #Clear existing mission from vehicle
    cmds = vehicle.commands
    cmds.clear()
    #Add new mission to vehicle
    for command in missionlist:
        cmds.add(command)
    print ' Uploaded mission with %s items' % len(missionlist)
    vehicle.commands.upload()
    return missionlist


def download_mission():
    """
    Downloads the current mission and returns it in a list.
    It is used in save_mission() to get the file information to save.
    """
    print " Download mission from vehicle"
    missionlist=[]
    cmds = vehicle.commands
    cmds.download()
    cmds.wait_ready()
    for cmd in cmds:
        missionlist.append(cmd)
    return missionlist

def save_mission(aFileName):
    """
    Save a mission in the Waypoint file format
    (http://qgroundcontrol.org/mavlink/waypoint_protocol#waypoint_file_format).
    """
    print "\nSave mission from Vehicle to file: %s" % export_mission_filename
    #Download mission from vehicle
    missionlist = download_mission()
    #Add file-format information
    output='QGC WPL 110\n'
    #Add home location as 0th waypoint
    home = vehicle.home_location
    output+="%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n" % (0,1,0,16,0,0,0,0,home.lat,home.lon,home.alt,1)
    #Add commands
    for cmd in missionlist:
        commandline="%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n" % (cmd.seq,cmd.current,cmd.frame,cmd.command,cmd.param1,cmd.param2,cmd.param3,cmd.param4,cmd.x,cmd.y,cmd.z,cmd.autocontinue)
        output+=commandline
    with open(aFileName, 'w') as file_:
        print " Write mission to file"
        file_.write(output)


def printfile(aFileName):
    """
    Print a mission file to demonstrate "round trip"
    """
    print "\nMission file: %s" % aFileName
    with open(aFileName) as f:
        for line in f:
            print ' %s' % line.strip()


################################################################################################
# Listeners
################################################################################################

current_sequence = -1
current_sequence_changed = False
current_landed_state = -1
home_position_set = False

#Create a message listener for mission sequence number
@vehicle.on_message('MISSION_CURRENT')
def listener(self, name, mission_current):
    global current_sequence, current_sequence_changed
    if (current_sequence <> mission_current.seq):
        current_sequence = mission_current.seq;
        current_sequence_changed = True
        print 'current mission sequence: %s' % mission_current.seq

#Create a message listener for mission sequence number
@vehicle.on_message('EXTENDED_SYS_STATE')
def listener(self, name, extended_sys_state):
    global current_landed_state
    if (current_landed_state <> extended_sys_state.landed_state):
        current_landed_state = extended_sys_state.landed_state;

#Create a message listener for home position fix
@vehicle.on_message('HOME_POSITION')
def listener(self, name, home_position):
    global home_position_set
    home_position_set = True



################################################################################################
# Start mission test
################################################################################################


while not home_position_set:
    if time.time() - start_time > 30:
        print "FAILED: getting home position 30 seconds"
        sys.exit(98)
    print "Waiting for home position..."
    time.sleep(1)


#Upload mission from file
missionlist = upload_mission(import_mission_filename)
time.sleep(2)

# set mission mode the hard way
vehicle._master.mav.command_long_send(vehicle._master.target_system, vehicle._master.target_component,
                                           mavutil.mavlink.MAV_CMD_DO_SET_MODE, 0,
                                           MAV_MODE_AUTO,
                                           0, 0, 0, 0, 0, 0)
time.sleep(1)


# Arm vehicle
vehicle.armed = True

while not vehicle.system_status.state == "ACTIVE":
    if time.time() - start_time > 30:
        print "FAILED: vehicle did not arm within 30 seconds"
        sys.exit(98)
    print "Waiting for vehicle to arm..."
    time.sleep(1)



# Wait for completion of mission items
while (current_sequence < len(missionlist)-1 and elapsed_time < max_execution_time):
    time.sleep(.2)
    if current_sequence > 0 and current_sequence_changed:

        if missionlist[current_sequence-1].z - alt_acceptance_radius > vehicle.location.global_relative_frame.alt or missionlist[current_sequence-1].z + alt_acceptance_radius < vehicle.location.global_relative_frame.alt:
            print "waypoint %s out of bounds altitude %s gps altitude: %s" % (current_sequence, missionlist[current_sequence-1].z, vehicle.location.global_relative_frame.alt)
            mission_failed = True
        current_sequence_changed = False
    elapsed_time = time.time() - start_time

if elapsed_time < max_execution_time:
    print "Mission items have been executed"

# wait for the vehicle to have landed
while (current_landed_state != 1 and elapsed_time < max_execution_time):
    time.sleep(1)
    elapsed_time = time.time() - start_time

if elapsed_time < max_execution_time:
    print "Vehicle has landed"

# Disarm vehicle
vehicle.armed = False

# count elapsed time
elapsed_time = time.time() - start_time

# Close vehicle object before exiting script
vehicle.close()
time.sleep(2)

# Validate time constraint
if elapsed_time <= max_execution_time and not mission_failed:
    print "Mission succesful time elapsed %s" % elapsed_time
    sys.exit(0)

if elapsed_time > max_execution_time:
    print "Mission FAILED to execute within %s seconds" % max_execution_time
    sys.exit(99)

if mission_failed:
    print "Mission FAILED out of bounds"
    sys.exit(100)

print "Mission FAILED something strange happened"
sys.exit(101)  
    - name: QGC WPL 110
0        1        0        16        0        0        0        0        47.3979949951        8.54559612274        25.0        1
1        0        2        3000        4.0        0.0        0.0        0.0        47.3980331421        8.54578971863        25.0        1
2        0        3        16        0.0        0.0        -0.0        0.0        47.399269104        8.54557228088        25.0        1
3        0        3        16        0.0        0.0        -0.0        0.0        47.3992652893        8.54230213165        25.0        1
4        0        3        16        0.0        0.0        -0.0        0.0        47.3974761963        8.54239082336        25.0        1
5        0        3        16        0.0        0.0        -0.0        0.0        47.3976669312        8.54509449005        25.0        1
6        0        2        3000        3.0        0.0        0.0        0.0        47.3977851868        8.54526233673        25.0        1
7        0        3        21        25.0        0.0        -0.0        0.0        47.3979797363        8.54460906982        25.0        1                 
      run: % Copyright (c) 2009, nursultanramazanov 
% All rights reserved.
%
% Redistribution and use in source and binary forms, with or without
% modification, are permitted provided that the following conditions are
% met:
%
%     * Redistributions of source code must retain the above copyright
%       notice, this list of conditions and the following disclaimer.
%     * Redistributions in binary form must reproduce the above copyright
%       notice, this list of conditions and the following disclaimer in
%       the documentation and/or other materials provided with the distribution
%
% THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
% AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
% IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
% ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
% LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
% CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
% SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
% INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
% CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
% ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
% POSSIBILITY OF SUCH DAMAGE.
%

function [ center, radii, evecs, v ] = ellipsoid_fit( X, flag, equals )
%
% Fit an ellispoid/sphere to a set of xyz data points:
%
%   [center, radii, evecs, pars ] = ellipsoid_fit( X )
%   [center, radii, evecs, pars ] = ellipsoid_fit( [x y z] );
%   [center, radii, evecs, pars ] = ellipsoid_fit( X, 1 );
%   [center, radii, evecs, pars ] = ellipsoid_fit( X, 2, 'xz' );
%   [center, radii, evecs, pars ] = ellipsoid_fit( X, 3 );
%
% Parameters:
% * X, [x y z]   - Cartesian data, n x 3 matrix or three n x 1 vectors
% * flag         - 0 fits an arbitrary ellipsoid (default),
%                - 1 fits an ellipsoid with its axes along [x y z] axes
%                - 2 followed by, say, 'xy' fits as 1 but also x_rad = y_rad
%                - 3 fits a sphere
%
% Output:
% * center    -  ellispoid center coordinates [xc; yc; zc]
% * ax        -  ellipsoid radii [a; b; c]
% * evecs     -  ellipsoid radii directions as columns of the 3x3 matrix
% * v         -  the 9 parameters describing the ellipsoid algebraically: 
%                Ax^2 + By^2 + Cz^2 + 2Dxy + 2Exz + 2Fyz + 2Gx + 2Hy + 2Iz = 1
%
% Author:
% Yury Petrov, Northeastern University, Boston, MA
%

error( nargchk( 1, 3, nargin ) );  % check input arguments
if nargin == 1
    flag = 0;  % default to a free ellipsoid
end
if flag == 2 && nargin == 2
    equals = 'xy';
end

if size( X, 2 ) ~= 3
    error( 'Input data must have three columns!' );
else
    x = X( :, 1 );
    y = X( :, 2 );
    z = X( :, 3 );
end

% need nine or more data points
if length( x ) < 9 && flag == 0 
   error( 'Must have at least 9 points to fit a unique ellipsoid' );
end
if length( x ) < 6 && flag == 1
   error( 'Must have at least 6 points to fit a unique oriented ellipsoid' );
end
if length( x ) < 5 && flag == 2
   error( 'Must have at least 5 points to fit a unique oriented ellipsoid with two axes equal' );
end
if length( x ) < 3 && flag == 3
   error( 'Must have at least 4 points to fit a unique sphere' );
end

if flag == 0
    % fit ellipsoid in the form Ax^2 + By^2 + Cz^2 + 2Dxy + 2Exz + 2Fyz + 2Gx + 2Hy + 2Iz = 1
    D = [ x .* x, ...
          y .* y, ...
          z .* z, ...
      2 * x .* y, ...
      2 * x .* z, ...
      2 * y .* z, ...
      2 * x, ...
      2 * y, ... 
      2 * z ];  % ndatapoints x 9 ellipsoid parameters
elseif flag == 1
    % fit ellipsoid in the form Ax^2 + By^2 + Cz^2 + 2Gx + 2Hy + 2Iz = 1
    D = [ x .* x, ...
          y .* y, ...
          z .* z, ...
      2 * x, ...
      2 * y, ... 
      2 * z ];  % ndatapoints x 6 ellipsoid parameters
elseif flag == 2
    % fit ellipsoid in the form Ax^2 + By^2 + Cz^2 + 2Gx + 2Hy + 2Iz = 1,
    % where A = B or B = C or A = C
    if strcmp( equals, 'yz' ) || strcmp( equals, 'zy' )
        D = [ y .* y + z .* z, ...
            x .* x, ...
            2 * x, ...
            2 * y, ...
            2 * z ];
    elseif strcmp( equals, 'xz' ) || strcmp( equals, 'zx' )
        D = [ x .* x + z .* z, ...
            y .* y, ...
            2 * x, ...
            2 * y, ...
            2 * z ];
    else
        D = [ x .* x + y .* y, ...
            z .* z, ...
            2 * x, ...
            2 * y, ...
            2 * z ];
    end
else
    % fit sphere in the form A(x^2 + y^2 + z^2) + 2Gx + 2Hy + 2Iz = 1
    D = [ x .* x + y .* y + z .* z, ...
      2 * x, ...
      2 * y, ... 
      2 * z ];  % ndatapoints x 4 sphere parameters
end

% solve the normal system of equations
v = ( D' * D ) \ ( D' * ones( size( x, 1 ), 1 ) );

% find the ellipsoid parameters
if flag == 0
    % form the algebraic form of the ellipsoid
    A = [ v(1) v(4) v(5) v(7); ...
          v(4) v(2) v(6) v(8); ...
          v(5) v(6) v(3) v(9); ...
          v(7) v(8) v(9) -1 ];
    % find the center of the ellipsoid
    center = -A( 1:3, 1:3 ) \ [ v(7); v(8); v(9) ];
    % form the corresponding translation matrix
    T = eye( 4 );
    T( 4, 1:3 ) = center';
    % translate to the center
    R = T * A * T';
    % solve the eigenproblem
    [ evecs evals ] = eig( R( 1:3, 1:3 ) / -R( 4, 4 ) );
    radii = sqrt( 1 ./ diag( evals ) );
else
    if flag == 1
        v = [ v(1) v(2) v(3) 0 0 0 v(4) v(5) v(6) ];
    elseif flag == 2
        if strcmp( equals, 'xz' ) || strcmp( equals, 'zx' )
            v = [ v(1) v(2) v(1) 0 0 0 v(3) v(4) v(5) ];
        elseif strcmp( equals, 'yz' ) || strcmp( equals, 'zy' )
            v = [ v(2) v(1) v(1) 0 0 0 v(3) v(4) v(5) ];
        else % xy
            v = [ v(1) v(1) v(2) 0 0 0 v(3) v(4) v(5) ];
        end
    else
        v = [ v(1) v(1) v(1) 0 0 0 v(2) v(3) v(4) ];
    end
    center = ( -v( 7:9 ) ./ v( 1:3 ) )';
    gam = 1 + ( v(7)^2 / v(1) + v(8)^2 / v(2) + v(9)^2 / v(3) );
    radii = ( sqrt( gam ./ v( 1:3 ) ) )';
    evecs = eye( 3 );
end

     
    - name: clear all;
close all;

% Measurement data
% 1045 propeller
% Robbe Roxxy Motor (1100 kV, data collected in 2010)
data = [ 45, 7.4;...
         38, 5.6;...
         33, 4.3;...
         26, 3.0;...
         18, 2.0;...
         10, 1.0 ];

% Normalize the data, as we're operating later
% anyways in normalized units
data(:,1) = data(:,1) ./ max(data(:,1));
data(:,2) = data(:,2) ./ max(data(:,2));

% Fit a 2nd degree polygon to the data and
% print the x2, x1, x0 coefficients
p = polyfit(data(:,2), data(:,1),2)

% Override the first coffefficient for testing
% purposes
pf = 0.62;

% Generate plotting data
px1 = linspace(0, max(data(:,2)));
py1 = polyval(p, px1);

pyt = zeros(size(data, 1), 1);
corr = zeros(size(data, 1), 1);

% Actual code test
% the two lines below are the ones needed to be ported to C:
%   pf: Power factor parameter.
%   px1(i): The current normalized motor command (-1..1)
%   corr(i): The required correction. The motor speed is:
%            px1(i) 
for i=1:size(px1, 2)

    % The actual output throttle
    pyt(i) = -pf * (px1(i) * px1(i)) + (1 + pf) * px1(i);

    % Solve for input throttle
    % y = -p * x^2 + (1+p) * x;
    % 
end

plot(data(:,2), data(:,1), '*r');
hold on;
plot(px1, py1, '*b');
hold on;
plot([0 px1(end)], [0 py1(end)], '-k');
hold on;
plot(px1, pyt, '-b');
hold on;
plot(px1, corr, '-m');       
      run: %
% Tool for plotting mag data
%
% Reference values:
% telem> [cal] mag #0 off: x:0.15    y:0.07     z:0.14 Ga
%                  MATLAB: x:0.1581  y: 0.0701  z: 0.1439 Ga
% telem> [cal] mag #0 scale: x:1.10 y:0.97 z:1.02
%                  MATLAB: 0.5499, 0.5190, 0.4907
%
% telem> [cal] mag #1 off: x:-0.18    y:0.11    z:-0.09 Ga
%                  MATLAB: x:-0.1827  y:0.1147  z:-0.0848 Ga
% telem> [cal] mag #1 scale: x:1.00 y:1.00 z:1.00
%                  MATLAB: 0.5122, 0.5065, 0.4915
%
%
% User-guided values:
%
% telem> [cal] mag #0 off: x:0.12 y:0.09 z:0.14 Ga
% telem> [cal] mag #0 scale: x:0.88 y:0.99 z:0.95
% telem> [cal] mag #1 off: x:-0.18 y:0.11 z:-0.09 Ga
% telem> [cal] mag #1 scale: x:1.00 y:1.00 z:1.00

close all;
clear all;

plot_scale = 0.8;

xmax = plot_scale;
xmin = -xmax;
ymax = plot_scale;
ymin = -ymax;
zmax = plot_scale;
zmin = -zmax;

mag0_raw = load('../../mag0_raw3.csv');
mag1_raw = load('../../mag1_raw3.csv');

mag0_cal = load('../../mag0_cal3.csv');
mag1_cal = load('../../mag1_cal3.csv');

fm0r = figure();

mag0_x_scale = 0.88;
mag0_y_scale = 0.99;
mag0_z_scale = 0.95;

plot3(mag0_raw(:,1), mag0_raw(:,2), mag0_raw(:,3), '*r');
[mag0_raw_center, mag0_raw_radii, evecs, pars ] = ellipsoid_fit( [mag0_raw(:,1) mag0_raw(:,2) mag0_raw(:,3)] );
mag0_raw_center
mag0_raw_radii
axis([xmin xmax ymin ymax zmin zmax])
viscircles([mag0_raw_center(1), mag0_raw_center(2)], [mag0_raw_radii(1)]);

fm1r = figure();
plot3(mag1_raw(:,1), mag1_raw(:,2), mag1_raw(:,3), '*r');
[center, radii, evecs, pars ] = ellipsoid_fit( [mag1_raw(:,1) mag1_raw(:,2) mag1_raw(:,3)] );
center
radii
axis([xmin xmax ymin ymax zmin zmax])

fm0c = figure();
plot3(mag0_cal(:,1) .* mag0_x_scale, mag0_cal(:,2) .* mag0_y_scale, mag0_cal(:,3) .* mag0_z_scale, '*b');
[mag0_cal_center, mag0_cal_radii, evecs, pars ] = ellipsoid_fit( [mag1_raw(:,1) .* mag0_x_scale mag1_raw(:,2) .* mag0_y_scale mag1_raw(:,3) .* mag0_z_scale] );
mag0_cal_center
mag0_cal_radii
axis([xmin xmax ymin ymax zmin zmax])
viscircles([0, 0], [mag0_cal_radii(3)]);

fm1c = figure();
plot3(mag1_cal(:,1), mag1_cal(:,2), mag1_cal(:,3), '*b');
axis([xmin xmax ymin ymax zmin zmax])
[center, radii, evecs, pars ] = ellipsoid_fit( [mag1_raw(:,1) mag1_raw(:,2) mag1_raw(:,3)] );
viscircles([0, 0], [radii(3)]);

mag0_x_scale_matlab = 1 / (mag0_cal_radii(1) / mag0_raw_radii(1))
mag0_y_scale_matlab = 1 / (mag0_cal_radii(2) / mag0_raw_radii(2))
mag0_z_scale_matlab = 1 / (mag0_cal_radii(3) / mag0_raw_radii(3))       
