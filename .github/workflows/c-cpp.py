name: C/C++ CI

on: #!/usr/bin/env python

################################################################################################
# @File MissionCheck.py
# Automated mission loading, execution and monitoring
# for Continuous Integration
#
# 
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
  push: QGC WPL 110
0        1        0        16        0        0        0        0        47.3979949951        8.54559612274        25.0        1
1        0        2        3000        4.0        0.0        0.0        0.0        47.3980331421        8.54578971863        25.0        1
2        0        3        16        0.0        0.0        -0.0        0.0        47.399269104        8.54557228088        25.0        1
3        0        3        16        0.0        0.0        -0.0        0.0        47.3992652893        8.54230213165        25.0        1
4        0        3        16        0.0        0.0        -0.0        0.0        47.3974761963        8.54239082336        25.0        1
5        0        3        16        0.0        0.0        -0.0        0.0        47.3976669312        8.54509449005        25.0        1
6        0        2        3000        3.0        0.0        0.0        0.0        47.3977851868        8.54526233673        25.0        1
7        0        3        21        25.0        0.0        -0.0        0.0        47.3979797363        8.54460906982        25.0        1
    branches: [ "main" ]
  pull_request: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160511181141 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $table = $schema->createTable('user');
        $table->addColumn('id', 'integer', [
            'autoincrement' => true,
        ]);
        $table->addColumn('name', 'string', [
            'notnull' => false,
        ]);
        $table->addColumn('salt', 'string', [
            'length' => 40,
            'notnull' => false,
            'fixed' => true,
        ]);
        $table->addColumn('hash', 'string', [
            'length' => 40,
            'notnull' => false,
            'fixed' => true,
        ]);
        $table->addColumn('email', 'string', [
            'notnull' => false,
        ]);
        $table->addColumn('login', 'string', [
            'notnull' => false,
        ]);
        $table->addColumn('accessToken', 'string');

        $table->setPrimaryKey(['id']);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $schema->dropTable('user');
    }
}
  branches: [ "main" ]

jobs: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160511202416 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $userTable = $schema->getTable('user');

        $testTable = $schema->createTable('test');
        $testTable->addColumn('id', 'integer', [
            'autoincrement' => true,
        ]);
        $testTable->addColumn('user_id', 'integer');
        $testTable->addForeignKeyConstraint($userTable, ['user_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
        $testTable->addColumn('title', 'string');
        $testTable->addColumn('description', 'text', [
            'notnull' => false,
        ]);
        $testTable->addColumn('time_limit', 'integer', [
            'default' => 0,
        ]);
        $testTable->addColumn('added', 'datetime');

        $testTable->setPrimaryKey(['id']);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $schema->dropTable('test');
    }
}
  build: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160511212105 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $test = $schema->getTable('test');

        $tag = $schema->createTable('tag');
        $tag->addColumn('id', 'integer', [
            'autoincrement' => true,
        ]);
        $tag->addColumn('name', 'string');
        $tag->addUniqueIndex(['name']);
        $tag->setPrimaryKey(['id']);

        $test_tag = $schema->createTable('test_tag');
        $test_tag->addColumn('test_id', 'integer');
        $test_tag->addColumn('tag_id', 'integer');
        $test_tag->addForeignKeyConstraint($tag, ['tag_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
        $test_tag->addForeignKeyConstraint($test, ['test_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
        $test_tag->setPrimaryKey(['test_id', 'tag_id']);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $schema->dropTable('test_tag');
        $schema->dropTable('tag');
    }
}

    runs-on: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160512025516 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $testTable = $schema->getTable('test');

        $table = $schema->createTable('question');
        $table->addColumn('id', 'integer', [
            'autoincrement' => true,
        ]);
        $table->addColumn('description', 'text');
        $table->addColumn('points', 'integer');
        $table->addColumn('sequence_number', 'integer');
        $table->addColumn('test_id', 'integer');
        $table->addForeignKeyConstraint($testTable, ['test_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
        $table->addColumn('answer_text', 'string', [
            'notnull' => false,
        ]);
        $table->addColumn('answer_decimal', 'string', [
            'notnull' => false,
        ]);
        $table->addColumn('precision', 'decimal', [
            'notnull' => false,
        ]);
        $table->addColumn('type', 'string');

        $table->setPrimaryKey(['id']);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $schema->dropTable('question');
    }
}

    steps: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160512133552 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $question = $schema->getTable('question');

        $variant = $schema->createTable('variant');
        $variant->addColumn('id', 'integer', [
            'autoincrement' => true,
        ]);
        $variant->addColumn('value', 'string');
        $variant->addColumn('is_right', 'string');
        $variant->addColumn('question_id', 'integer');
        $variant->addForeignKeyConstraint($question, ['question_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);

        $variant->setPrimaryKey(['id']);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $schema->dropTable('variant');
    }
}
    - uses: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160512135536 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $user = $schema->getTable('user');

        $attempt = $schema->createTable('attempt');
        $attempt->addColumn('id', 'integer', [
            'autoincrement' => true,
        ]);
        $attempt->addColumn('started', 'datetime');
        $attempt->addColumn('user_id', 'integer');
        $attempt->addForeignKeyConstraint($user, ['user_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);

        $attempt->setPrimaryKey(['id']);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $schema->dropTable('attempt');
    }
}
    - name: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160512145224 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $answer = $schema->createTable('answer');
        $answer->addColumn('id', 'integer', [
            'autoincrement' => true,
        ]);
        $answer->addColumn('question_id', 'integer');
        $answer->addColumn('attempt_id', 'integer');
        $answer->addColumn('variant_id', 'integer', [
            'notnull' => false
        ]);
        $answer->addForeignKeyConstraint('question', ['question_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
        $answer->addForeignKeyConstraint('attempt', ['attempt_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
        $answer->addForeignKeyConstraint('variant', ['variant_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
        $answer->addColumn('decimal_answer', 'decimal', [
            'notnull' => false,
        ]);
        $answer->addColumn('text_answer', 'string', [
            'notnull' => false,
        ]);
        $answer->addColumn('type', 'string');

        $answer->setPrimaryKey(['id']);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $schema->dropTable('answer');
    }
}
      run: ./<?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160513005635 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $attempt = $schema->getTable('attempt');
        $attempt->addColumn('test_id', 'integer');
        $attempt->addForeignKeyConstraint('test', ['test_id'], ['id'], [
            'onUpdate' => 'CASCADE',
            'onDelete' => 'CASCADE',
        ]);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $attempt = $schema->getTable('attempt');
        $attempt->dropColumn('test_id');
    }
}
    - name: <?php

namespace TestHub\Migrations;

use Doctrine\DBAL\Migrations\AbstractMigration;
use Doctrine\DBAL\Schema\Schema;

/**
 * Migration
 */
class Version20160518161400 extends AbstractMigration
{
    /**
     * @param Schema $schema
     */
    public function up(Schema $schema)
    {
        $attempt = $schema->getTable('attempt');
        $attempt->addColumn('status', 'string', [
            'default' => 'active',
        ]);
    }

    /**
     * @param Schema $schema
     */
    public function down(Schema $schema)
    {
        $attempt = $schema->getTable('attempt');
        $attempt->dropColumn('status');
    }
}
      run: cmake_minimum_required(VERSION 3.10)

project(VideoReceiverApp LANGUAGES C CXX)

set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug;Release;RelWithDebInfo;MinSizeRel;Coverage")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(FeatureSummary)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-Wall -Wextra -Wno-address-of-packed-member)
endif()

# CMake build type
# Debug Release RelWithDebInfo MinSizeRel Coverage
if (NOT CMAKE_BUILD_TYPE)
    # default to release with debug symbols
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING "Build type" FORCE)
endif()

set(QGC_ROOT ${CMAKE_SOURCE_DIR}/..)

# Add folder where are supportive functions
list(APPEND CMAKE_MODULE_PATH ${QGC_ROOT}/cmake)

# Configure Qt5 to get necessary variables
include(Qt5QGCConfiguration)
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Qt version: ${QT_VERSION}")
message(STATUS "Qt spec: ${QT_MKSPEC}")

set(COMPANY "Auterion")
set(COPYRIGHT "Copyright (c) 2020 VideoReceiverApp. All rights reserved.")
set(IDENTIFIER "labs.auterion.VideoReceiverApp")

include(Git)
message(STATUS "VideoReceiverApp version: ${GIT_VERSION}")

#=============================================================================
# ccache
#
option(CCACHE "Use ccache if available" ON)
find_program(CCACHE_PROGRAM ccache)
if (CCACHE AND CCACHE_PROGRAM AND NOT DEFINED ENV{CCACHE_DISABLE})
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()

#=============================================================================
# Compile QML
#
option(COMPILE_QML "Pre-compile QML files using the Qt Quick compiler." FALSE)
add_feature_info(COMPILE_QML COMPILE_QML "Pre-compile QML files using the Qt Quick compiler.")
if(COMPILE_QML)
    find_package(Qt5QuickCompiler)

    set_package_properties(Qt5QuickCompiler PROPERTIES
        DESCRIPTION "Pre-compile QML files using the Qt Quick compiler."
        TYPE OPTIONAL
        )
endif()

#=============================================================================
# Debug QML
#
option(DEBUG_QML "Build VideoReceiverApp with QML debugging/profiling support." FALSE)
add_feature_info(DEBUG_QML DEBUG_QML "Build VideoReceiverApp with QML debugging/profiling support.")
if(DEBUG_QML)
    message(STATUS "To enable the QML debugger/profiler, run with: '-qmljsdebugger=port:1234'")
    add_definitions(-DQMLJSDEBUGGER)
    add_definitions(-DQT_DECLARATIVE_DEBUG)
    add_definitions(-DQT_QML_DEBUG)
endif()

#=============================================================================
# GStreamer
#
find_package(PkgConfig)
pkg_check_modules(GST
    gstreamer-1.0>=1.14
    gstreamer-video-1.0>=1.14
    gstreamer-gl-1.0>=1.14
    egl
    )

if (GST_FOUND)
    include_directories(
        ${GST_INCLUDE_DIRS}
        )
endif()

#=============================================================================
# Qt5
#
find_package(Qt5 ${QT_VERSION}
    COMPONENTS
    Bluetooth
    Charts
    Concurrent
    Core
    Location
    Multimedia
    Network
    Positioning
    Quick
    QuickWidgets
    OpenGL
    Sql
    Svg
    Test
    TextToSpeech
    Widgets
    Xml
    REQUIRED
    HINTS
    ${QT_LIBRARY_HINTS}
    )

# Sets the default flags for compilation and linking.
include(CompileOptions)

include_directories(
    ${QGC_ROOT}/src
    ${CMAKE_CURRENT_BINARY_DIR}
    ${Qt5Location_PRIVATE_INCLUDE_DIRS}
    VideoReceiver
    )

add_subdirectory(${QGC_ROOT}/libs/qmlglsink qmlglsink.build)
add_subdirectory(${QGC_ROOT}/src/VideoReceiver VideoReceiver.build)

set(VIDEORECIVERAPP_SOURCES main.cpp ${QGC_ROOT}/src/QGCLoggingCategory.cc)
set(VIDEORECIVERAPP_RESOURCES qml.qrc)

if(ANDROID)
    add_library(VideoReceiverApp SHARED ${VIDEORECIVERAPP_SOURCES} ${VIDEORECIVERAPP_RESOURCES})
else()
    add_executable(VideoReceiverApp ${VIDEORECIVERAPP_SOURCES} ${VIDEORECIVERAPP_RESOURCES})
endif()

target_link_libraries(VideoReceiverApp
    PRIVATE
        VideoReceiver
        Qt5::Core
        Qt5::Multimedia
        Qt5::OpenGL
        Qt5::Quick
        Qt5::QuickWidgets
)
    - name: <?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
        <key>CFBundleDisplayName</key>
        <string>QQmlGlSinkTest</string>
        <key>CFBundleExecutable</key>
        <string>$(EXECUTABLE_NAME)</string>
        <key>NSHumanReadableCopyright</key>
        <string>Open Source Flight Systems GmbH - Internal Build</string>
        <key>CFBundleIconFile</key>
        <string></string>
        <key>CFBundleIdentifier</key>
        <string>labs.auterion.VideoReceiverApp</string>
        <key>CFBundleName</key>
        <string>$(PRODUCT_NAME)</string>
        <key>CFBundlePackageType</key>
        <string>APPL</string>
        <key>CFBundleShortVersionString</key>
        <string>0.0.0</string>
        <key>CFBundleSignature</key>
        <string>????</string>
        <key>CFBundleVersion</key>
        <string>1</string>
        <key>LSRequiresIPhoneOS</key>
        <true/>
        <key>UIFileSharingEnabled</key>
        <true/>
</dict>
</plist>
      run: #include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QQuickWindow>
#include <QQuickItem>
#include <QRunnable>
#include <QCommandLineParser>
#include <QTimer>

#include <gst/gst.h>

#include "QGCLoggingCategory.h"

QGC_LOGGING_CATEGORY(AppLog, "VideoReceiverApp")

#if defined(__android__)
#include <QtAndroidExtras>

#include <jni.h>

#include <android/log.h>

static jobject _class_loader = nullptr;
static jobject _context = nullptr;

extern "C" {
    void gst_amc_jni_set_java_vm(JavaVM *java_vm);

    jobject gst_android_get_application_class_loader(void) {
        return _class_loader;
    }
}

static void
gst_android_init(JNIEnv* env, jobject context)
{
    jobject class_loader = nullptr;

    jclass context_cls = env->GetObjectClass(context);

    if (!context_cls) {
        return;
    }

    jmethodID get_class_loader_id = env->GetMethodID(context_cls, "getClassLoader", "()Ljava/lang/ClassLoader;");

    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        return;
    }

    class_loader = env->CallObjectMethod(context, get_class_loader_id);

    if (env->ExceptionCheck()) {
        env->ExceptionDescribe();
        env->ExceptionClear();
        return;
    }

    _context = env->NewGlobalRef(context);
    _class_loader = env->NewGlobalRef(class_loader);
}

static const char kJniClassName[] {"labs/mavlink/VideoReceiverApp/QGLSinkActivity"};

static void setNativeMethods(void)
{
    JNINativeMethod javaMethods[] {
        {"nativeInit", "()V", reinterpret_cast<void *>(gst_android_init)}
    };

    QAndroidJniEnvironment jniEnv;

    if (jniEnv->ExceptionCheck()) {
        jniEnv->ExceptionDescribe();
        jniEnv->ExceptionClear();
    }

    jclass objectClass = jniEnv->FindClass(kJniClassName);

    if (!objectClass) {
        qWarning() << "Couldn't find class:" << kJniClassName;
        return;
    }

    jint val = jniEnv->RegisterNatives(objectClass, javaMethods, sizeof(javaMethods) / sizeof(javaMethods[0]));

    if (val < 0) {
        qWarning() << "Error registering methods: " << val;
    } else {
        qDebug() << "Main Native Functions Registered";
    }

    if (jniEnv->ExceptionCheck()) {
        jniEnv->ExceptionDescribe();
        jniEnv->ExceptionClear();
    }
}

jint JNI_OnLoad(JavaVM* vm, void* reserved)
{
    Q_UNUSED(reserved);

    JNIEnv* env;

    if (vm->GetEnv(reinterpret_cast<void**>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return -1;
    }

    setNativeMethods();

    gst_amc_jni_set_java_vm(vm);

    return JNI_VERSION_1_6;
}
#endif

#include <GStreamer.h>
#include <VideoReceiver.h>

class VideoReceiverApp : public QRunnable
{
public:
    VideoReceiverApp(QCoreApplication& app, bool qmlAllowed)
        : _app(app)
        , _qmlAllowed(qmlAllowed)
    {}

    void run();

    int exec();

    void startStreaming();
    void startDecoding();
    void startRecording();

protected:
    void _dispatch(std::function<void()> code);

private:
    QCoreApplication& _app;
    bool _qmlAllowed;
    VideoReceiver* _receiver = nullptr;
    QQuickWindow* _window = nullptr;
    QQuickItem* _widget = nullptr;
    void* _videoSink = nullptr;
    QString _url;
    unsigned _timeout = 5;
    unsigned _connect = 1;
    bool _decode = true;
    unsigned _stopDecodingAfter = 0;
    bool _record = false;
    QString _videoFile;
    unsigned int _fileFormat = VideoReceiver::FILE_FORMAT_MIN;
    unsigned _stopRecordingAfter = 15;
    bool _useFakeSink = false;
    bool _streaming = false;
    bool _decoding = false;
    bool _recording = false;
};

void
VideoReceiverApp::run()
{
    if((_videoSink = GStreamer::createVideoSink(nullptr, _widget)) == nullptr) {
        qCDebug(AppLog) << "createVideoSink failed";
        return;
    }

    _receiver->startDecoding(_videoSink);
}

int
VideoReceiverApp::exec()
{
    QCommandLineParser parser;

    parser.addHelpOption();

    parser.addPositionalArgument("url",
        QCoreApplication::translate("main", "Source URL."));

    QCommandLineOption timeoutOption(QStringList() << "t" << "timeout",
        QCoreApplication::translate("main", "Source timeout."),
        QCoreApplication::translate("main", "seconds"));

    parser.addOption(timeoutOption);

    QCommandLineOption connectOption(QStringList() << "c" << "connect",
        QCoreApplication::translate("main", "Number of connection attempts."),
        QCoreApplication::translate("main", "attempts"));

    parser.addOption(connectOption);

    QCommandLineOption decodeOption(QStringList() << "d" << "decode",
        QCoreApplication::translate("main", "Decode and render video."));

    parser.addOption(decodeOption);

    QCommandLineOption noDecodeOption("no-decode",
        QCoreApplication::translate("main", "Don't decode and render video."));

    parser.addOption(noDecodeOption);

    QCommandLineOption stopDecodingOption("stop-decoding",
        QCoreApplication::translate("main", "Stop decoding after time."),
        QCoreApplication::translate("main", "seconds"));

    parser.addOption(stopDecodingOption);

    QCommandLineOption recordOption(QStringList() << "r" << "record",
        QCoreApplication::translate("main", "Record video."),
        QGuiApplication::translate("main", "file"));

    parser.addOption(recordOption);

    QCommandLineOption formatOption(QStringList() << "f" << "format",
        QCoreApplication::translate("main", "File format."),
        QCoreApplication::translate("main", "format"));

    parser.addOption(formatOption);

    QCommandLineOption stopRecordingOption("stop-recording",
        QCoreApplication::translate("main", "Stop recording after time."),
        QCoreApplication::translate("main", "seconds"));

    parser.addOption(stopRecordingOption);

    QCommandLineOption videoSinkOption("video-sink",
        QCoreApplication::translate("main", "Use video sink: 0 - autovideosink, 1 - fakesink"),
        QCoreApplication::translate("main", "sink"));

    if (!_qmlAllowed) {
        parser.addOption(videoSinkOption);
    }

    parser.process(_app);

    const QStringList args = parser.positionalArguments();

    if (args.size() != 1) {
        parser.showHelp(0);
    }

    _url = args.at(0);

    if (parser.isSet(timeoutOption)) {
        _timeout = parser.value(timeoutOption).toUInt();
    }

    if (parser.isSet(connectOption)) {
        _connect = parser.value(connectOption).toUInt();
    }

    if (parser.isSet(decodeOption) && parser.isSet(noDecodeOption)) {
        parser.showHelp(0);
    }

    if (parser.isSet(decodeOption)) {
        _decode = true;
    }

    if (parser.isSet(noDecodeOption)) {
        _decode = false;
    }

    if (_decode && parser.isSet(stopDecodingOption)) {
        _stopDecodingAfter = parser.value(stopDecodingOption).toUInt();
    }

    if (parser.isSet(recordOption)) {
        _record = true;
        _videoFile = parser.value(recordOption);
    }

    if (parser.isSet(formatOption)) {
        _fileFormat += parser.value(formatOption).toUInt();
    }

    if (_record && parser.isSet(stopRecordingOption)) {
        _stopRecordingAfter = parser.value(stopRecordingOption).toUInt();
    }

    if (parser.isSet(videoSinkOption)) {
        _useFakeSink = parser.value(videoSinkOption).toUInt() > 0;
    }

    _receiver = GStreamer::createVideoReceiver(nullptr);

    QQmlApplicationEngine engine;

    if (_decode && _qmlAllowed) {
        engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

        _window = static_cast<QQuickWindow*>(engine.rootObjects().first());
        Q_ASSERT(_window != nullptr);

        _widget = _window->findChild<QQuickItem*>("videoItem");
        Q_ASSERT(_widget != nullptr);
    }

    startStreaming();

    QObject::connect(_receiver, &VideoReceiver::timeout, [](){
        qCDebug(AppLog) << "Streaming timeout";
     });

    QObject::connect(_receiver, &VideoReceiver::streamingChanged, [this](bool active){
        _streaming = active;
        if (_streaming) {
            qCDebug(AppLog) << "Streaming started";
        } else {
            qCDebug(AppLog) << "Streaming stopped";
        }
     });

    QObject::connect(_receiver, &VideoReceiver::decodingChanged, [this](bool active){
        _decoding = active;
        if (_decoding) {
            qCDebug(AppLog) << "Decoding started";
        } else {
            qCDebug(AppLog) << "Decoding stopped";
            if (_streaming) {
                if (!_recording) {
                    _dispatch([this](){
                        _receiver->stop();
                    });
                }
            }
        }
     });

    QObject::connect(_receiver, &VideoReceiver::recordingChanged, [this](bool active){
        _recording = active;
        if (_recording) {
            qCDebug(AppLog) << "Recording started";
        } else {
            qCDebug(AppLog) << "Recording stopped";
            if (_streaming) {
                if (!_decoding) {
                    _dispatch([this](){
                        _receiver->stop();
                    });
                }
            }
        }
     });

    QObject::connect(_receiver, &VideoReceiver::onStartComplete, [this](VideoReceiver::STATUS status){
        if (status != VideoReceiver::STATUS_OK) {
            qCDebug(AppLog) << "Video receiver start failed";
            _dispatch([this](){
                if (--_connect > 0) {
                    qCDebug(AppLog) << "Restarting ...";
                    _dispatch([this](){
                        startStreaming();
                    });
                } else {
                    qCDebug(AppLog) << "Closing...";
                    delete _receiver;
                    _app.exit();
                }
            });
        } else {
            qCDebug(AppLog) << "Video receiver started";
        }
     });

    QObject::connect(_receiver, &VideoReceiver::onStopComplete, [this](VideoReceiver::STATUS ){
        qCDebug(AppLog) << "Video receiver stopped";

        _dispatch([this](){
            if (--_connect > 0) {
                qCDebug(AppLog) << "Restarting ...";
                _dispatch([this](){
                    startStreaming();
                });
            } else {
                qCDebug(AppLog) << "Closing...";
                delete _receiver;
                _app.exit();
            }
        });
     });


    return _app.exec();
}

void
VideoReceiverApp::startStreaming()
{
    _receiver->start(_url, _timeout);

    if (_decode) {
        startDecoding();
    }

    if (_record) {
        startRecording();
    }
}

void
VideoReceiverApp::startDecoding()
{
    if (_qmlAllowed) {
        _window->scheduleRenderJob(this, QQuickWindow::BeforeSynchronizingStage);
    } else {
        if (_videoSink == nullptr) {
            if ((_videoSink = gst_element_factory_make(_useFakeSink ? "fakesink" : "autovideosink", nullptr)) == nullptr) {
                qCDebug(AppLog) << "Failed to create video sink";
                return;
            }
        }

        _receiver->startDecoding(_videoSink);
    }

    if (_stopDecodingAfter > 0) {
        unsigned connect = _connect;
        QTimer::singleShot(_stopDecodingAfter * 1000, Qt::PreciseTimer, [this, connect](){
            if (connect != _connect) {
                return;
            }
            _receiver->stopDecoding();
        });
    }
}

void
VideoReceiverApp::startRecording()
{
    _receiver->startRecording(_videoFile, static_cast<VideoReceiver::FILE_FORMAT>(_fileFormat));

    if (_stopRecordingAfter > 0) {
        unsigned connect = _connect;
        QTimer::singleShot(_stopRecordingAfter * 1000, [this, connect](){
            if (connect != _connect) {
                return;
            }
            _receiver->stopRecording();
        });
    }
}

void
VideoReceiverApp::_dispatch(std::function<void()> code)
{
    QTimer* timer = new QTimer();
    timer->moveToThread(qApp->thread());
    timer->setSingleShot(true);
    QObject::connect(timer, &QTimer::timeout, [=](){
        code();
        timer->deleteLater();
    });
    QMetaObject::invokeMethod(timer, "start", Qt::QueuedConnection, Q_ARG(int, 0));
}


static bool isQtApp(const char* app)
{
    const char* s;

#if defined(Q_OS_WIN)
    if ((s = strrchr(app, '\\')) != nullptr) {
#else
    if ((s = strrchr(app, '/')) != nullptr) {
#endif
        s += 1;
    } else {
        s = app;
    }

    return s[0] == 'Q' || s[0] == 'q';
}

int main(int argc, char *argv[])
{
    if (argc < 1) {
        return 0;
    }

    GStreamer::initialize(argc, argv, 3);

    if (isQtApp(argv[0])) {
        QGuiApplication app(argc, argv);
        VideoReceiverApp videoApp(app, true);
        return videoApp.exec();
    } else {
        QCoreApplication app(argc, argv);
        VideoReceiverApp videoApp(app, false);
        return videoApp.exec();
    }
}
    - name: import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Layouts 1.3
import org.freedesktop.gstreamer.GLVideoItem 1.0

Window {
    visible: true
    width: 640
    height: 480
    title: qsTr("VideoReceiverApp")

    RowLayout {
        anchors.fill: parent
        spacing: 0

        GstGLVideoItem {
            id: video
            objectName: "videoItem"
            Layout.fillWidth: true
            Layout.fillHeight: true
        }
    }
}
      run: <RCC>
    <qresource prefix="/">
        <file>main.qml</file>
    </qresource>
</RCC>
