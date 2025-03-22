# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

name: Synopsys Intelligent Security Scan

on: version: 2

aliases:
  docker-cp: &docker-cp
    run:
      name: Create Docker Volume
      command: |
        docker create -v /<component name="InspectionProjectProfileManager">
  <settings>
    <option name="USE_PROJECT_PROFILE" value="false" />
    <version value="1.0" />
  </settings>
</component> --name #!usr/bin/env python
__author__ = 
__email__ = "xzhang@westwoodrobotics.io"
__copyright__ = "Copyright 2020 Westwood Robotics"
__date__ = "Aug 20, 2020"
__version__ = "0.1.0"
__status__ = "Production"

# -----------------------------
# Move BEAR from the current angle to a specified angle
# For more details, please check the SDK manual (https://www.westwoodrobotics.io/support/)

from pybear import Manager
import time

error = False
bear = Manager.BEAR(port="/dev/ttyUSB0", baudrate=8000000)  # need to identify the port name on your PC

m_id = 1  # BEAR ID (default is 1)

p_gain = 5.0  # Set P gain as spring stiffness
d_gain = 0.2  # Set D gain as damper strength
i_gain = 0.0  # I gain is usually not needed
iq_max = 1.5  # Max iq

BEAR_connected = bear.ping(m_id)[0]
if not BEAR_connected:
    # BEAR is offline
    print("BEAR is offline. Check power and communication.")
    error = True
    exit()

if not error:
    # BEAR is online
    # Set PID, mode, and limit
    print("Welcome aboard, Captain!")
    # PID id/iq
    bear.set_p_gain_iq((m_id, 0.02))
    bear.set_i_gain_iq((m_id, 0.02))
    bear.set_d_gain_iq((m_id, 0))
    bear.set_p_gain_id((m_id, 0.02))
    bear.set_i_gain_id((m_id, 0.02))
    bear.set_d_gain_id((m_id, 0))

    # PID position mode
    bear.set_p_gain_position((m_id, p_gain))
    bear.set_i_gain_position((m_id, i_gain))
    bear.set_d_gain_position((m_id, d_gain))

    # Put into position mode
    bear.set_mode((m_id, 2))

    # Set iq limit
    bear.set_limit_iq_max((m_id, iq_max))

    # Start demo
    input('Press Enter to start demo. ')

    # Get home position
    home = bear.get_present_position(m_id)[0][0][0]
    print(home)

    # Set goal position before enabling BEAR
    bear.set_goal_position((m_id, home))

    # Enable BEAR
    bear.set_torque_enable((m_id, 1))

    # Demo spring-damping system
    print('You can play with BEAR now! It is simulating a spring-damping system.')
    time.sleep(2)

    # Get command position
    angle = float(input('Input the angle in radians you want to move BEAR to (e.g., -0.2): '))

    # Let's move to the target angle smoothly
    num = 100                  # split it into 100 pieces
    delta_angle = angle / num  # angle for each time
    for i in range(num):
        goal_pos = home + delta_angle * (i + 1)
        bear.set_goal_position((m_id, goal_pos))
        time.sleep(0.01)

    print('BEAR arrived target angle!')
    time.sleep(2)

    # Turn off BEAR
    input('Press Enter to turn off BEAR.')

    # Disable BEAR
    bear.set_torque_enable((m_id, 0))
    print("Thanks for using BEAR!") --privileged tizenrt/tizenrt:#!usr/bin/env python
__author__ = 
__email__ = "xzhang@westwoodrobotics.io"
__copyright__ = "Copyright 2020 Westwood Robotics"
__date__ = "Aug 20, 2020"
__version__ = "0.1.0"
__status__ = "Production"

# -----------------------------
# BEAR with a bar, spring on one side, damper to the other side, with the 90 degree in the center being free.

from pybear import Manager
import sys
import select
import os

error = False
bear = Manager.BEAR(port="COM9", baudrate=8000000)

m_id = 1  # Set motor ID

p_gain = 5  # Set P gain as the K of spring
d_gain = 2  # Set D gain as damper strength
iq_max = 1.5  # Max iq

if not(bear.ping(m_id)[0]):
    # BEAR is offline
    print("BEAR is offline. Check power and communication.")
    error = True

if not error:
    # BEAR is online
    # Set PID, mode and limit
    print("Welcome aboard, Captain!")
    # PID idiq
    bear.set_p_gain_iq((m_id, 0.02))
    bear.set_i_gain_iq((m_id, 0.02))
    bear.set_d_gain_iq((m_id, 0))
    bear.set_p_gain_id((m_id, 0.02))
    bear.set_i_gain_id((m_id, 0.02))
    bear.set_d_gain_id((m_id, 0))

    # Clear PID direct force
    bear.set_p_gain_force((m_id, 0))
    bear.set_i_gain_force((m_id, 0))
    bear.set_d_gain_force((m_id, 0))

    # Put into torque mode
    bear.set_mode((m_id, 3))

    # Set iq limit
    bear.set_limit_iq_max((m_id, iq_max))

    # Put bar in middle
    # usr = input("Please move the bar to upright position, then press enter to continue.")
    # Get home position
    home = bear.get_present_position(m_id)[0][0][0]
    print("Demo started.")
    run = True
    while run:
        os.system('cls' if os.name == 'nt' else 'clear')
        pos = bear.get_present_position(m_id)[0][0][0]
        if abs(pos - home) < 0.78539816339:  # If distance between present_pos and home is lower than 45 deg
            # Disable and do nothing
            print("BEAR in free range.")
            bear.set_torque_enable((m_id, 0))
        elif (pos - home) > 0.78539816339: # If BEAR on the right side of free range
            # Enable and get into damping only
            print("BEAR in damping.")
            bear.set_p_gain_force((m_id, 0))
            bear.set_i_gain_force((m_id, 0))
            bear.set_d_gain_force((m_id, d_gain))
            bear.set_torque_enable((m_id, 1))

        elif (pos - home) < -0.78539816339:  # If BEAR on the left side of free range
            # Enable and get into spring only
            print("BEAR is now a spring.")
            bear.set_p_gain_force((m_id, p_gain))
            bear.set_i_gain_force((m_id, 0))
            bear.set_d_gain_force((m_id, 0))
            bear.set_torque_enable((m_id, 1))
            bear.set_goal_position((m_id, (home-0.78539816339)))
            # Check if iq_max reached
            iq = bear.get_present_iq(m_id)[0][0]
            if iq > iq_max:
                print("iq max reached.")
        print("Press any key to stop.")
        if sys.stdin in select.select([sys.stdin], [], [], 0)[0]:
            line = input()
            run = False
            print("Demo terminated by user.")
            break
    print("Thanks for using BEAR!") /bin/bash
        docker cp ./. message("Adding Custom Plugin")

#-- Version control
#   Major and minor versions are defined here (manually)

CUSTOM_QGC_VER_MAJOR = 0
CUSTOM_QGC_VER_MINOR = 0
CUSTOM_QGC_VER_FIRST_BUILD = 0

# Build number is automatic
# Uses the current branch. This way it works on any branch including build-server's PR branches
CUSTOM_QGC_VER_BUILD = $$system(git --git-dir ../.git rev-list $$GIT_BRANCH --first-parent --count)
win32 {
    CUSTOM_QGC_VER_BUILD = $$system("set /a $$CUSTOM_QGC_VER_BUILD - $$CUSTOM_QGC_VER_FIRST_BUILD")
} else {
    CUSTOM_QGC_VER_BUILD = $$system("echo $(($$CUSTOM_QGC_VER_BUILD - $$CUSTOM_QGC_VER_FIRST_BUILD))")
}
CUSTOM_QGC_VERSION = $${CUSTOM_QGC_VER_MAJOR}.$${CUSTOM_QGC_VER_MINOR}.$${CUSTOM_QGC_VER_BUILD}

DEFINES -= GIT_VERSION=\"\\\"$$GIT_VERSION\\\"\"
DEFINES += GIT_VERSION=\"\\\"$$CUSTOM_QGC_VERSION\\\"\"

message(Custom QGC Version: $${CUSTOM_QGC_VERSION})

# Build a single flight stack by disabling APM support
MAVLINK_CONF = common
CONFIG  += QGC_DISABLE_APM_MAVLINK
CONFIG  += QGC_DISABLE_APM_PLUGIN QGC_DISABLE_APM_PLUGIN_FACTORY

# We implement our own PX4 plugin factory
CONFIG  += QGC_DISABLE_PX4_PLUGIN_FACTORY

# Branding

DEFINES += CUSTOMHEADER=\"\\\"CustomPlugin.h\\\"\"
DEFINES += CUSTOMCLASS=CustomPlugin

TARGET   = CustomQGroundControl
DEFINES += QGC_APPLICATION_NAME='"\\\"Custom QGroundControl\\\""'

DEFINES += QGC_ORG_NAME=\"\\\"qgroundcontrol.org\\\"\"
DEFINES += QGC_ORG_DOMAIN=\"\\\"org.qgroundcontrol\\\"\"

QGC_APP_NAME        = "Custom QGroundControl"
QGC_BINARY_NAME     = "CustomQGroundControl"
QGC_ORG_NAME        = "Custom"
QGC_ORG_DOMAIN      = "org.custom"
QGC_APP_DESCRIPTION = "Custom QGroundControl"
QGC_APP_COPYRIGHT   = "Copyright (C) 2020 QGroundControl Development Team. All rights reserved."

# Our own, custom resources
RESOURCES += \
    $$PWD/custom.qrc

QML_IMPORT_PATH += \
   $$PWD/res

# Our own, custom sources
SOURCES += \
    $$PWD/src/CustomPlugin.cc \

HEADERS += \
    $$PWD/src/CustomPlugin.h \

INCLUDEPATH += \
    $$PWD/src \

#-------------------------------------------------------------------------------------
# Custom Firmware/AutoPilot Plugin

INCLUDEPATH += \
    $$PWD/src/FirmwarePlugin \
    $$PWD/src/AutoPilotPlugin

HEADERS+= \
    $$PWD/src/AutoPilotPlugin/CustomAutoPilotPlugin.h \
    $$PWD/src/FirmwarePlugin/CustomFirmwarePlugin.h \
    $$PWD/src/FirmwarePlugin/CustomFirmwarePluginFactory.h \

SOURCES += \
    $$PWD/src/AutoPilotPlugin/CustomAutoPilotPlugin.cc \
    $$PWD/src/FirmwarePlugin/CustomFirmwarePlugin.cc \
    $$PWD/src/FirmwarePlugin/CustomFirmwarePluginFactory.cc \
:/<RCC>
    <qresource prefix="/custom">
        <file alias="CustomFlyViewOverlay.qml">res/CustomFlyViewOverlay.qml</file>
    </qresource>
    <qresource prefix="custom/img">
        <file alias="altitude.svg">res/Images/altitude.svg</file>
        <file alias="attitude_crosshair.svg">res/Images/attitude_crosshair.svg</file>
        <file alias="attitude_dial.svg">res/Images/attitude_dial.svg</file>
        <file alias="attitude_pointer.svg">res/Images/attitude_pointer.svg</file>
        <file alias="chronometer.svg">res/Images/chronometer.svg</file>
        <file alias="compass_needle.svg">res/Images/compass_needle.svg</file>
        <file alias="compass_pointer.svg">res/Images/compass_pointer.svg</file>
        <file alias="distance.svg">res/Images/distance.svg</file>
        <file alias="horizontal_speed.svg">res/Images/horizontal_speed.svg</file>
        <file alias="microSD.svg">res/Images/microSD.svg</file>
        <file alias="odometer.svg">res/Images/odometer.svg</file>
        <file alias="vertical_speed.svg">res/Images/vertical_speed.svg</file>
        <file alias="CustomAppIcon.png">res/Images/CustomAppIcon.png</file>
    </qresource>
    <qresource prefix="/qmlimages">
	<file alias="PaperPlane.svg">res/Images/CustomVehicleIcon.svg</file>
    </qresource>
    <qresource prefix="Custom/Widgets">
        <file alias="Custom/Widgets/CustomArtificialHorizon.qml">res/Custom/Widgets/CustomArtificialHorizon.qml</file>
        <file alias="Custom/Widgets/CustomAttitudeWidget.qml">res/Custom/Widgets/CustomAttitudeWidget.qml</file>
        <file alias="Custom/Widgets/CustomIconButton.qml">res/Custom/Widgets/CustomIconButton.qml</file>
        <file alias="Custom/Widgets/CustomOnOffSwitch.qml">res/Custom/Widgets/CustomOnOffSwitch.qml</file>
        <file alias="Custom/Widgets/CustomQuickButton.qml">res/Custom/Widgets/CustomQuickButton.qml</file>
        <file alias="Custom/Widgets/CustomSignalStrength.qml">res/Custom/Widgets/CustomSignalStrength.qml</file>
        <file alias="Custom/Widgets/CustomToolBarButton.qml">res/Custom/Widgets/CustomToolBarButton.qml</file>
        <file alias="Custom/Widgets/CustomVehicleButton.qml">res/Custom/Widgets/CustomVehicleButton.qml</file>
        <file alias="Custom/Widgets/qmldir">res/Custom/Widgets/qmldir</file>
    </qresource>
    <qresource prefix="/qml">
        <file alias="QGroundControl/FlightDisplay/FlyViewCustomLayer.qml">res/CustomFlyViewOverlay.qml</file>
    </qresource>
</RCC>                                                                                               
        docker run -d --rm -it --name         <file alias="PaperPlane.svg">src/ui/toolbar/Images/PaperPlane.svg</file>
        <file alias="QGroundControl/FlightDisplay/FlyViewCustomLayer.qml">src/FlightDisplay/FlyViewCustomLayer.qml</file>
 --volumes-from=<RCC>
	<qresource prefix="/fonts">
		<file alias="opensans">../resources/fonts/OpenSans-Regular.ttf</file>
		<file alias="opensans-demibold">../resources/fonts/OpenSans-Semibold.ttf</file>
		<file alias="NanumGothic-Regular">../resources/fonts/NanumGothic-Regular.ttf</file>
		<file alias="NanumGothic-Bold">../resources/fonts/NanumGothic-Bold.ttf</file>
	</qresource>
	<qresource prefix="/res">
		<file alias="action.svg">../resources/action.svg</file>
		<file alias="AntennaRC">../resources/Antenna_RC.svg</file>
		<file alias="AntennaT">../resources/Antenna_T.svg</file>
		<file alias="ArrowDown.svg">../resources/ArrowDown.svg</file>
		<file alias="ArrowRight.svg">../resources/ArrowRight.svg</file>
		<file alias="buttonLeft.svg">../resources/buttonLeft.svg</file>
		<file alias="buttonRight.svg">../resources/buttonRight.svg</file>
		<file alias="cancel.svg">../resources/cancel.svg</file>
		<file alias="clockwise-arrow.svg">../resources/clockwise-arrow.svg</file>
		<file alias="counter-clockwise-arrow.svg">../resources/counter-clockwise-arrow.svg</file>
		<file alias="chevron-down.svg">../resources/chevron-down.svg</file>
		<file alias="chevron-up.svg">../resources/chevron-up.svg</file>
		<file alias="DropArrow.svg">../resources/DropArrow.svg</file>
		<file alias="gear-black.svg">../resources/gear-black.svg</file>
		<file alias="gear-white.svg">../resources/gear-white.svg</file>
		<file alias="helicoptericon.svg">../resources/helicoptericon.svg</file>
		<file alias="JoystickBezel.png">../resources/JoystickBezel.png</file>
		<file alias="JoystickBezelLight.png">../resources/JoystickBezelLight.png</file>
		<file alias="land.svg">../resources/land.svg</file>
		<file alias="LockClosed.svg">../resources/LockClosed.svg</file>
		<file alias="LockOpen.svg">../resources/LockOpen.svg</file>
		<file alias="notile.png">../resources/notile.png</file>
		<file alias="Pause.svg">../resources/Pause.svg</file>
		<file alias="pause-mission.svg">../resources/pause-mission.svg</file>
		<file alias="Play">../resources/Play.svg</file>
		<file alias="PowerButton">../resources/PowerButton.svg</file>
		<file alias="QGCLogoBlack">../resources/QGCLogoBlack.svg</file>
		<file alias="QGCLogoFull">../resources/QGCLogoFull.svg</file>
		<file alias="QGCLogoWhite">../resources/QGCLogoWhite.svg</file>
		<file alias="QGCLogoArrow">../resources/QGCLogoArrow.svg</file>
		<file alias="QGroundControlConnect">../resources/QGroundControlConnect.svg</file>
		<file alias="rtl.svg">../resources/rtl.svg</file>
		<file alias="SplashScreen">../resources/SplashScreen.png</file>
		<file alias="Stop">../resources/Stop.svg</file>
		<file alias="takeoff.svg">../resources/takeoff.svg</file>
		<file alias="TrashDelete.svg">../resources/TrashDelete.svg</file>
		<file alias="waves.svg">../resources/waves.svg</file>
		<file alias="wind-guru.svg">../resources/wind-guru.svg</file>
		<file alias="wind-rose.svg">../resources/wind-rose.svg</file>
		<file alias="wind-roseBlack.svg">../resources/wind-roseBlack.svg</file>
		<file alias="wind-rose-arrow.svg">../resources/wind-rose-arrow.svg</file>
		<file alias="XDelete.svg">../resources/XDelete.svg</file>
		<file alias="XDeleteBlack.svg">../resources/XDeleteBlack.svg</file>
		<file alias="waypoint.svg">../resources/waypoint.svg</file>
		<file>../resources/icons/qgroundcontrol.ico</file>
	</qresource>
	<qresource prefix="/res/firmware">
		<file alias="3drradio.png">../resources/firmware/3drradio.png</file>
		<file alias="apm.png">../resources/firmware/apm.png</file>
		<file alias="px4.png">../resources/firmware/px4.png</file>
	</qresource>
	<qresource prefix="/res/calibration">
		<file alias="accel_back.png">../resources/calibration/accel_back.png</file>
		<file alias="accel_down.png">../resources/calibration/accel_down.png</file>
		<file alias="accel_front.png">../resources/calibration/accel_front.png</file>
		<file alias="accel_left.png">../resources/calibration/accel_left.png</file>
		<file alias="accel_right.png">../resources/calibration/accel_right.png</file>
		<file alias="accel_up.png">../resources/calibration/accel_up.png</file>
	</qresource>
	<qresource prefix="/qml/calibration/mode1">
		<file alias="radioCenter.png">../resources/calibration/mode1/radioCenter.png</file>
		<file alias="radioHome.png">../resources/calibration/mode1/radioHome.png</file>
		<file alias="radioPitchDown.png">../resources/calibration/mode1/radioPitchDown.png</file>
		<file alias="radioPitchUp.png">../resources/calibration/mode1/radioPitchUp.png</file>
		<file alias="radioRollLeft.png">../resources/calibration/mode1/radioRollLeft.png</file>
		<file alias="radioRollRight.png">../resources/calibration/mode1/radioRollRight.png</file>
		<file alias="radioSwitchMinMax.png">../resources/calibration/mode1/radioSwitchMinMax.png</file>
		<file alias="radioThrottleDown.png">../resources/calibration/mode1/radioThrottleDown.png</file>
		<file alias="radioThrottleUp.png">../resources/calibration/mode1/radioThrottleUp.png</file>
		<file alias="radioYawLeft.png">../resources/calibration/mode1/radioYawLeft.png</file>
		<file alias="radioYawRight.png">../resources/calibration/mode1/radioYawRight.png</file>
	</qresource>
	<qresource prefix="/qml/calibration/mode2">
		<file alias="radioCenter.png">../resources/calibration/mode2/radioCenter.png</file>
		<file alias="radioHome.png">../resources/calibration/mode2/radioHome.png</file>
		<file alias="radioPitchDown.png">../resources/calibration/mode2/radioPitchDown.png</file>
		<file alias="radioPitchUp.png">../resources/calibration/mode2/radioPitchUp.png</file>
		<file alias="radioRollLeft.png">../resources/calibration/mode2/radioRollLeft.png</file>
		<file alias="radioRollRight.png">../resources/calibration/mode2/radioRollRight.png</file>
		<file alias="radioSwitchMinMax.png">../resources/calibration/mode2/radioSwitchMinMax.png</file>
		<file alias="radioThrottleDown.png">../resources/calibration/mode2/radioThrottleDown.png</file>
		<file alias="radioThrottleUp.png">../resources/calibration/mode2/radioThrottleUp.png</file>
		<file alias="radioYawLeft.png">../resources/calibration/mode2/radioYawLeft.png</file>
		<file alias="radioYawRight.png">../resources/calibration/mode2/radioYawRight.png</file>
	</qresource>
	<qresource prefix="/db/mapping/joystick">
		<file alias="gamecontrollerdb.txt">../resources/SDL_GameControllerDB/gamecontrollerdb.txt</file>
	</qresource>
	<qresource prefix="/res/audio">
		<file alias="Alert">../resources/audio/alert.wav</file>
	</qresource>
	<qresource prefix="/opengl">
		<file>../resources/opengl/buglist.json</file>
	</qresource>
</RCC> -w /<RCC>
	<qresource prefix="/unittest">
		<file alias="FactSystemTest.qml">../src/FactSystem/FactSystemTest.qml</file>
	</qresource>
	<qresource prefix="/toolbar">
		<file alias="ArmedIndicator.qml">../src/ui/toolbar/ArmedIndicator.qml</file>
		<file alias="BatteryIndicator.qml">../src/ui/toolbar/BatteryIndicator.qml</file>
		<file alias="GPSIndicator.qml">../src/ui/toolbar/GPSIndicator.qml</file>
		<file alias="GPSRTKIndicator.qml">../src/ui/toolbar/GPSRTKIndicator.qml</file>
		<file alias="JoystickIndicator.qml">../src/ui/toolbar/JoystickIndicator.qml</file>
		<file alias="LinkIndicator.qml">../src/ui/toolbar/LinkIndicator.qml</file>
		<file alias="MainToolBar.qml">../src/ui/toolbar/MainToolBar.qml</file>
		<file alias="MainToolBarIndicators.qml">../src/ui/toolbar/MainToolBarIndicators.qml</file>
		<file alias="MessageIndicator.qml">../src/ui/toolbar/MessageIndicator.qml</file>
		<file alias="ModeIndicator.qml">../src/ui/toolbar/ModeIndicator.qml</file>
		<file alias="MultiVehicleSelector.qml">../src/ui/toolbar/MultiVehicleSelector.qml</file>
		<file alias="RCRSSIIndicator.qml">../src/ui/toolbar/RCRSSIIndicator.qml</file>
		<file alias="ROIIndicator.qml">../src/ui/toolbar/ROIIndicator.qml</file>
		<file alias="TelemetryRSSIIndicator.qml">../src/ui/toolbar/TelemetryRSSIIndicator.qml</file>
		<file alias="VTOLModeIndicator.qml">../src/ui/toolbar/VTOLModeIndicator.qml</file>
	</qresource>
	<qresource prefix="/checklists">
		<file alias="DefaultChecklist.qml">../src/FlightDisplay/DefaultChecklist.qml</file>
		<file alias="MultiRotorChecklist.qml">../src/FlightDisplay/MultiRotorChecklist.qml</file>
		<file alias="FixedWingChecklist.qml">../src/FlightDisplay/FixedWingChecklist.qml</file>
		<file alias="VTOLChecklist.qml">../src/FlightDisplay/VTOLChecklist.qml</file>
		<file alias="RoverChecklist.qml">../src/FlightDisplay/RoverChecklist.qml</file>
		<file alias="SubChecklist.qml">../src/FlightDisplay/SubChecklist.qml</file>
	</qresource>
	<qresource prefix="/qml">
		<file alias="QGroundControl/Controls/HeightIndicator.qml">../src/QmlControls/HeightIndicator.qml</file>
		<file alias="QGroundControl/Controls/QGCDynamicObjectManager.qml">../src/QmlControls/QGCDynamicObjectManager.qml</file>
		<file alias="QGroundControl/Controls/QGCOptionsComboBox.qml">../src/QmlControls/QGCOptionsComboBox.qml</file>
		<file alias="QGroundControl/Controls/TransectStyleMapVisuals.qml">../src/PlanView/TransectStyleMapVisuals.qml</file>
		<file alias="QGroundControl/FlightMap/MapLineArrow.qml">../src/MissionManager/MapLineArrow.qml</file>
		<file alias="QGroundControl/FlightMap/SplitIndicator.qml">../src/FlightMap/MapItems/SplitIndicator.qml</file>
		<file alias="AnalyzeView.qml">../src/AnalyzeView/AnalyzeView.qml</file>
		<file alias="AppSettings.qml">../src/ui/AppSettings.qml</file>
		<file alias="BluetoothSettings.qml">../src/ui/preferences/BluetoothSettings.qml</file>
		<file alias="CameraPageWidget.qml">../src/FlightMap/Widgets/CameraPageWidget.qml</file>
		<file alias="CorridorScanEditor.qml">../src/PlanView/CorridorScanEditor.qml</file>
		<file alias="CustomCommandWidget.qml">../src/ViewWidgets/CustomCommandWidget.qml</file>
		<file alias="DebugWindow.qml">../src/ui/preferences/DebugWindow.qml</file>
		<file alias="ESP8266Component.qml">../src/AutoPilotPlugins/Common/ESP8266Component.qml</file>
		<file alias="ESP8266ComponentSummary.qml">../src/AutoPilotPlugins/Common/ESP8266ComponentSummary.qml</file>
		<file alias="ExitWithErrorWindow.qml">../src/ui/ExitWithErrorWindow.qml</file>
		<file alias="FirmwareUpgrade.qml">../src/VehicleSetup/FirmwareUpgrade.qml</file>
		<file alias="FlightDisplayViewDummy.qml">../src/FlightDisplay/FlightDisplayViewDummy.qml</file>
		<file alias="FlightDisplayViewUVC.qml">../src/FlightDisplay/FlightDisplayViewUVC.qml</file>
		<file alias="FWLandingPatternEditor.qml">../src/PlanView/FWLandingPatternEditor.qml</file>
		<file alias="GeneralSettings.qml">../src/ui/preferences/GeneralSettings.qml</file>
		<file alias="GeoTagPage.qml">../src/AnalyzeView/GeoTagPage.qml</file>
		<file alias="HealthPageWidget.qml">../src/FlightMap/Widgets/HealthPageWidget.qml</file>
		<file alias="HelpSettings.qml">../src/ui/preferences/HelpSettings.qml</file>
		<file alias="JoystickConfig.qml">../src/VehicleSetup/JoystickConfig.qml</file>
		<file alias="JoystickConfigAdvanced.qml">../src/VehicleSetup/JoystickConfigAdvanced.qml</file>
		<file alias="JoystickConfigButtons.qml">../src/VehicleSetup/JoystickConfigButtons.qml</file>
		<file alias="JoystickConfigCalibration.qml">../src/VehicleSetup/JoystickConfigCalibration.qml</file>
		<file alias="JoystickConfigGeneral.qml">../src/VehicleSetup/JoystickConfigGeneral.qml</file>
		<file alias="LinkSettings.qml">../src/ui/preferences/LinkSettings.qml</file>
		<file alias="LogDownloadPage.qml">../src/AnalyzeView/LogDownloadPage.qml</file>
		<file alias="LogReplaySettings.qml">../src/ui/preferences/LogReplaySettings.qml</file>
		<file alias="MainRootWindow.qml">../src/ui/MainRootWindow.qml</file>
		<file alias="MavlinkConsolePage.qml">../src/AnalyzeView/MavlinkConsolePage.qml</file>
		<file alias="MAVLinkInspectorPage.qml">../src/AnalyzeView/MAVLinkInspectorPage.qml</file>
		<file alias="MavlinkSettings.qml">../src/ui/preferences/MavlinkSettings.qml</file>
		<file alias="MicrohardSettings.qml">../src/Microhard/MicrohardSettings.qml</file>
		<file alias="MissionSettingsEditor.qml">../src/PlanView/MissionSettingsEditor.qml</file>
		<file alias="MockLink.qml">../src/ui/preferences/MockLink.qml</file>
		<file alias="MockLinkSettings.qml">../src/ui/preferences/MockLinkSettings.qml</file>
		<file alias="MotorComponent.qml">../src/AutoPilotPlugins/Common/MotorComponent.qml</file>
		<file alias="OfflineMap.qml">../src/QtLocationPlugin/QMLControl/OfflineMap.qml</file>
		<file alias="PlanToolBar.qml">../src/PlanView/PlanToolBar.qml</file>
		<file alias="PlanToolBarIndicators.qml">../src/PlanView/PlanToolBarIndicators.qml</file>
		<file alias="PlanView.qml">../src/PlanView/PlanView.qml</file>
		<file alias="PreFlightCheckList.qml">../src/FlightDisplay/PreFlightCheckList.qml</file>
		<file alias="PX4FlowSensor.qml">../src/VehicleSetup/PX4FlowSensor.qml</file>
		<file alias="QGCInstrumentWidget.qml">../src/FlightMap/Widgets/QGCInstrumentWidget.qml</file>
		<file alias="QGCInstrumentWidgetAlternate.qml">../src/FlightMap/Widgets/QGCInstrumentWidgetAlternate.qml</file>
		<file alias="QGCViewDialogContainer.qml">../src/QmlControls/QGCViewDialogContainer.qml</file>
		<file alias="QGroundControl/Controls/AnalyzePage.qml">../src/AnalyzeView/AnalyzePage.qml</file>
		<file alias="QGroundControl/Controls/AppMessages.qml">../src/QmlControls/AppMessages.qml</file>
		<file alias="QGroundControl/Controls/AxisMonitor.qml">../src/QmlControls/AxisMonitor.qml</file>
		<file alias="QGroundControl/Controls/CameraCalcCamera.qml">../src/PlanView/CameraCalcCamera.qml</file>
		<file alias="QGroundControl/Controls/CameraCalcGrid.qml">../src/PlanView/CameraCalcGrid.qml</file>
		<file alias="QGroundControl/Controls/CameraSection.qml">../src/PlanView/CameraSection.qml</file>
		<file alias="QGroundControl/Controls/ClickableColor.qml">../src/QmlControls/ClickableColor.qml</file>
		<file alias="QGroundControl/Controls/CorridorScanMapVisual.qml">../src/PlanView/CorridorScanMapVisual.qml</file>
		<file alias="QGroundControl/Controls/DeadMouseArea.qml">../src/QmlControls/DeadMouseArea.qml</file>
		<file alias="QGroundControl/Controls/DropButton.qml">../src/QmlControls/DropButton.qml</file>
		<file alias="QGroundControl/Controls/DropPanel.qml">../src/QmlControls/DropPanel.qml</file>
		<file alias="QGroundControl/Controls/EditPositionDialog.qml">../src/QmlControls/EditPositionDialog.qml</file>
		<file alias="QGroundControl/Controls/ExclusiveGroupItem.qml">../src/QmlControls/ExclusiveGroupItem.qml</file>
		<file alias="QGroundControl/Controls/FactSliderPanel.qml">../src/QmlControls/FactSliderPanel.qml</file>
		<file alias="QGroundControl/Controls/FirstRunPrompt.qml">../src/FirstRunPromptDialogs/FirstRunPrompt.qml</file>
		<file alias="QGroundControl/Controls/FileButton.qml">../src/QmlControls/FileButton.qml</file>
		<file alias="QGroundControl/Controls/FlightModeDropdown.qml">../src/QmlControls/FlightModeDropdown.qml</file>
		<file alias="QGroundControl/Controls/FlightModeMenu.qml">../src/QmlControls/FlightModeMenu.qml</file>
		<file alias="QGroundControl/Controls/FWLandingPatternMapVisual.qml">../src/PlanView/FWLandingPatternMapVisual.qml</file>
		<file alias="QGroundControl/Controls/GeoFenceEditor.qml">../src/PlanView/GeoFenceEditor.qml</file>
		<file alias="QGroundControl/Controls/GeoFenceMapVisuals.qml">../src/PlanView/GeoFenceMapVisuals.qml</file>
		<file alias="QGroundControl/Controls/HorizontalFactValueGrid.qml">../src/QmlControls/HorizontalFactValueGrid.qml</file>
		<file alias="QGroundControl/Controls/IndicatorButton.qml">../src/QmlControls/IndicatorButton.qml</file>
		<file alias="QGroundControl/Controls/InstrumentValueLabel.qml">../src/QmlControls/InstrumentValueLabel.qml</file>
		<file alias="QGroundControl/Controls/InstrumentValueValue.qml">../src/QmlControls/InstrumentValueValue.qml</file>
		<file alias="QGroundControl/Controls/InstrumentValueEditDialog.qml">../src/QmlControls/InstrumentValueEditDialog.qml</file>
		<file alias="QGroundControl/Controls/JoystickThumbPad.qml">../src/QmlControls/JoystickThumbPad.qml</file>
		<file alias="QGroundControl/Controls/KMLOrSHPFileDialog.qml">../src/QmlControls/KMLOrSHPFileDialog.qml</file>
		<file alias="QGroundControl/Controls/LogReplayStatusBar.qml">../src/QmlControls/LogReplayStatusBar.qml</file>
		<file alias="QGroundControl/Controls/MainWindowSavedState.qml">../src/QmlControls/MainWindowSavedState.qml</file>
		<file alias="QGroundControl/Controls/MAVLinkChart.qml">../src/QmlControls/MAVLinkChart.qml</file>
		<file alias="QGroundControl/Controls/MAVLinkMessageButton.qml">../src/QmlControls/MAVLinkMessageButton.qml</file>
		<file alias="QGroundControl/Controls/MissionCommandDialog.qml">../src/QmlControls/MissionCommandDialog.qml</file>
		<file alias="QGroundControl/Controls/MissionItemEditor.qml">../src/PlanView/MissionItemEditor.qml</file>
		<file alias="QGroundControl/Controls/MissionItemIndexLabel.qml">../src/QmlControls/MissionItemIndexLabel.qml</file>
		<file alias="QGroundControl/Controls/MissionItemMapVisual.qml">../src/PlanView/MissionItemMapVisual.qml</file>
		<file alias="QGroundControl/Controls/MissionItemStatus.qml">../src/PlanView/MissionItemStatus.qml</file>
		<file alias="QGroundControl/Controls/ModeSwitchDisplay.qml">../src/QmlControls/ModeSwitchDisplay.qml</file>
		<file alias="QGroundControl/Controls/MultiRotorMotorDisplay.qml">../src/QmlControls/MultiRotorMotorDisplay.qml</file>
		<file alias="QGroundControl/Controls/OfflineMapButton.qml">../src/QmlControls/OfflineMapButton.qml</file>
		<file alias="QGroundControl/Controls/PageView.qml">../src/QmlControls/PageView.qml</file>
		<file alias="QGroundControl/Controls/ParameterEditor.qml">../src/QmlControls/ParameterEditor.qml</file>
		<file alias="QGroundControl/Controls/ParameterEditorDialog.qml">../src/QmlControls/ParameterEditorDialog.qml</file>
		<file alias="QGroundControl/Controls/PIDTuning.qml">../src/QmlControls/PIDTuning.qml</file>
		<file alias="QGroundControl/Controls/PlanEditToolbar.qml">../src/PlanView/PlanEditToolbar.qml</file>
		<file alias="QGroundControl/Controls/PreFlightCheckButton.qml">../src/QmlControls/PreFlightCheckButton.qml</file>
		<file alias="QGroundControl/Controls/PreFlightCheckGroup.qml">../src/QmlControls/PreFlightCheckGroup.qml</file>
		<file alias="QGroundControl/Controls/PreFlightCheckModel.qml">../src/QmlControls/PreFlightCheckModel.qml</file>
		<file alias="QGroundControl/Controls/QGCButton.qml">../src/QmlControls/QGCButton.qml</file>
		<file alias="QGroundControl/Controls/QGCCheckBox.qml">../src/QmlControls/QGCCheckBox.qml</file>
		<file alias="QGroundControl/Controls/QGCColoredImage.qml">../src/QmlControls/QGCColoredImage.qml</file>
		<file alias="QGroundControl/Controls/QGCControlDebug.qml">../src/QmlControls/QGCControlDebug.qml</file>
		<file alias="QGroundControl/Controls/QGCComboBox.qml">../src/QmlControls/QGCComboBox.qml</file>
		<file alias="QGroundControl/Controls/QGCFileDialog.qml">../src/QmlControls/QGCFileDialog.qml</file>
		<file alias="QGroundControl/Controls/QGCFlickable.qml">../src/QmlControls/QGCFlickable.qml</file>
		<file alias="QGroundControl/Controls/QGCFlickableHorizontalIndicator.qml">../src/QmlControls/QGCFlickableHorizontalIndicator.qml</file>
		<file alias="QGroundControl/Controls/QGCFlickableVerticalIndicator.qml">../src/QmlControls/QGCFlickableVerticalIndicator.qml</file>
		<file alias="QGroundControl/Controls/QGCGroupBox.qml">../src/QmlControls/QGCGroupBox.qml</file>
		<file alias="QGroundControl/Controls/QGCHoverButton.qml">../src/QmlControls/QGCHoverButton.qml</file>
		<file alias="QGroundControl/Controls/QGCLabel.qml">../src/QmlControls/QGCLabel.qml</file>
		<file alias="QGroundControl/Controls/QGCListView.qml">../src/QmlControls/QGCListView.qml</file>
		<file alias="QGroundControl/Controls/QGCMapCircleVisuals.qml">../src/MissionManager/QGCMapCircleVisuals.qml</file>
		<file alias="QGroundControl/Controls/QGCMapLabel.qml">../src/QmlControls/QGCMapLabel.qml</file>
		<file alias="QGroundControl/Controls/QGCMapPolygonVisuals.qml">../src/MissionManager/QGCMapPolygonVisuals.qml</file>
		<file alias="QGroundControl/Controls/QGCMapPolylineVisuals.qml">../src/MissionManager/QGCMapPolylineVisuals.qml</file>
		<file alias="QGroundControl/Controls/QGCMenu.qml">../src/QmlControls/QGCMenu.qml</file>
		<file alias="QGroundControl/Controls/QGCMenuItem.qml">../src/QmlControls/QGCMenuItem.qml</file>
		<file alias="QGroundControl/Controls/QGCMenuSeparator.qml">../src/QmlControls/QGCMenuSeparator.qml</file>
		<file alias="QGroundControl/Controls/QGCMouseArea.qml">../src/QmlControls/QGCMouseArea.qml</file>
		<file alias="QGroundControl/Controls/QGCMovableItem.qml">../src/QmlControls/QGCMovableItem.qml</file>
		<file alias="QGroundControl/Controls/QGCPopupDialog.qml">../src/QmlControls/QGCPopupDialog.qml</file>
		<file alias="QGroundControl/Controls/QGCPopupDialogContainer.qml">../src/QmlControls/QGCPopupDialogContainer.qml</file>
		<file alias="QGroundControl/Controls/QGCPipOverlay.qml">../src/QmlControls/QGCPipOverlay.qml</file>
		<file alias="QGroundControl/Controls/QGCPipState.qml">../src/QmlControls/QGCPipState.qml</file>
		<file alias="QGroundControl/Controls/QGCRadioButton.qml">../src/QmlControls/QGCRadioButton.qml</file>
		<file alias="QGroundControl/Controls/QGCSlider.qml">../src/QmlControls/QGCSlider.qml</file>
		<file alias="QGroundControl/Controls/QGCSwitch.qml">../src/QmlControls/QGCSwitch.qml</file>
		<file alias="QGroundControl/Controls/QGCTabBar.qml">../src/QmlControls/QGCTabBar.qml</file>
		<file alias="QGroundControl/Controls/QGCTabButton.qml">../src/QmlControls/QGCTabButton.qml</file>
		<file alias="QGroundControl/Controls/QGCTextField.qml">../src/QmlControls/QGCTextField.qml</file>
		<file alias="QGroundControl/Controls/QGCToolBarButton.qml">../src/QmlControls/QGCToolBarButton.qml</file>
		<file alias="QGroundControl/Controls/QGCToolInsets.qml">../src/QmlControls/QGCToolInsets.qml</file>
		<file alias="QGroundControl/Controls/QGCViewDialog.qml">../src/QmlControls/QGCViewDialog.qml</file>
		<file alias="QGroundControl/Controls/QGCViewMessage.qml">../src/QmlControls/QGCViewMessage.qml</file>
		<file alias="QGroundControl/Controls/qmldir">../src/QmlControls/QGroundControl/Controls/qmldir</file>
		<file alias="QGroundControl/Controls/RallyPointEditorHeader.qml">../src/PlanView/RallyPointEditorHeader.qml</file>
		<file alias="QGroundControl/Controls/RallyPointItemEditor.qml">../src/PlanView/RallyPointItemEditor.qml</file>
		<file alias="QGroundControl/Controls/RallyPointMapVisuals.qml">../src/PlanView/RallyPointMapVisuals.qml</file>
		<file alias="QGroundControl/Controls/RCChannelMonitor.qml">../src/QmlControls/RCChannelMonitor.qml</file>
		<file alias="QGroundControl/Controls/RCToParamDialog.qml">../src/QmlControls/RCToParamDialog.qml</file>
		<file alias="QGroundControl/Controls/RoundButton.qml">../src/QmlControls/RoundButton.qml</file>
		<file alias="QGroundControl/Controls/SectionHeader.qml">../src/QmlControls/SectionHeader.qml</file>
		<file alias="QGroundControl/Controls/SetupPage.qml">../src/AutoPilotPlugins/Common/SetupPage.qml</file>
		<file alias="QGroundControl/Controls/SignalStrength.qml">../src/ui/toolbar/SignalStrength.qml</file>
		<file alias="QGroundControl/Controls/SimpleItemMapVisual.qml">../src/PlanView/SimpleItemMapVisual.qml</file>
		<file alias="QGroundControl/Controls/SliderSwitch.qml">../src/QmlControls/SliderSwitch.qml</file>
		<file alias="QGroundControl/Controls/StructureScanMapVisual.qml">../src/PlanView/StructureScanMapVisual.qml</file>
		<file alias="QGroundControl/Controls/SubMenuButton.qml">../src/QmlControls/SubMenuButton.qml</file>
		<file alias="QGroundControl/Controls/SurveyMapVisual.qml">../src/PlanView/SurveyMapVisual.qml</file>
		<file alias="QGroundControl/Controls/TerrainStatus.qml">../src/PlanView/TerrainStatus.qml</file>
		<file alias="QGroundControl/Controls/TakeoffItemMapVisual.qml">../src/PlanView/TakeoffItemMapVisual.qml</file>
		<file alias="QGroundControl/Controls/ToolBarBase.qml">../src/ui/toolbar/ToolBarBase.qml</file>
		<file alias="QGroundControl/Controls/ToolStrip.qml">../src/QmlControls/ToolStrip.qml</file>
		<file alias="QGroundControl/Controls/TransectStyleComplexItemStats.qml">../src/PlanView/TransectStyleComplexItemStats.qml</file>
		<file alias="QGroundControl/Controls/VehicleRotationCal.qml">../src/QmlControls/VehicleRotationCal.qml</file>
		<file alias="QGroundControl/Controls/VehicleSummaryRow.qml">../src/QmlControls/VehicleSummaryRow.qml</file>
		<file alias="QGroundControl/Controls/VerticalFactValueGrid.qml">../src/QmlControls/VerticalFactValueGrid.qml</file>
		<file alias="QGroundControl/Controls/ViewWidget.qml">../src/ViewWidgets/ViewWidget.qml</file>
		<file alias="QGroundControl/Controls/VTOLLandingPatternMapVisual.qml">../src/PlanView/VTOLLandingPatternMapVisual.qml</file>
		<file alias="QGroundControl/FactControls/AltitudeFactTextField.qml">../src/FactSystem/FactControls/AltitudeFactTextField.qml</file>
		<file alias="QGroundControl/FactControls/FactBitmask.qml">../src/FactSystem/FactControls/FactBitmask.qml</file>
		<file alias="QGroundControl/FactControls/FactCheckBox.qml">../src/FactSystem/FactControls/FactCheckBox.qml</file>
		<file alias="QGroundControl/FactControls/FactComboBox.qml">../src/FactSystem/FactControls/FactComboBox.qml</file>
		<file alias="QGroundControl/FactControls/FactLabel.qml">../src/FactSystem/FactControls/FactLabel.qml</file>
		<file alias="QGroundControl/FactControls/FactTextField.qml">../src/FactSystem/FactControls/FactTextField.qml</file>
		<file alias="QGroundControl/FactControls/FactTextFieldGrid.qml">../src/FactSystem/FactControls/FactTextFieldGrid.qml</file>
		<file alias="QGroundControl/FactControls/FactTextFieldRow.qml">../src/FactSystem/FactControls/FactTextFieldRow.qml</file>
		<file alias="QGroundControl/FactControls/FactTextFieldSlider.qml">../src/FactSystem/FactControls/FactTextFieldSlider.qml</file>
		<file alias="QGroundControl/FactControls/FactValueSlider.qml">../src/FactSystem/FactControls/FactValueSlider.qml</file>
		<file alias="QGroundControl/FactControls/qmldir">../src/QmlControls/QGroundControl/FactControls/qmldir</file>
		<file alias="QGroundControl/FlightDisplay/FlightDisplayViewVideo.qml">../src/FlightDisplay/FlightDisplayViewVideo.qml</file>
		<file alias="QGroundControl/FlightDisplay/FlightDisplayViewWidgets.qml">../src/FlightDisplay/FlightDisplayViewWidgets.qml</file>
		<file alias="QGroundControl/FlightDisplay/FlyViewAirspaceIndicator.qml">../src/FlightDisplay/FlyViewAirspaceIndicator.qml</file>
		<file alias="QGroundControl/FlightDisplay/FlyView.qml">../src/FlightDisplay/FlyView.qml</file>
		<file alias="QGroundControl/FlightDisplay/FlyViewCustomLayer.qml">../src/FlightDisplay/FlyViewCustomLayer.qml</file>
		<file alias="QGroundControl/FlightDisplay/FlyViewInstrumentPanel.qml">../src/FlightDisplay/FlyViewInstrumentPanel.qml</file>
		<file alias="QGroundControl/FlightDisplay/FlyViewMap.qml">../src/FlightDisplay/FlyViewMap.qml</file>
		<file alias="QGroundControl/FlightDisplay/FlyViewMissionCompleteDialog.qml">../src/FlightDisplay/FlyViewMissionCompleteDialog.qml</file>
		<file alias="QGroundControl/FlightDisplay/FlyViewPreFlightChecklistPopup.qml">../src/FlightDisplay/FlyViewPreFlightChecklistPopup.qml</file>
		<file alias="QGroundControl/FlightDisplay/FlyViewToolStrip.qml">../src/FlightDisplay/FlyViewToolStrip.qml</file>
		<file alias="QGroundControl/FlightDisplay/FlyViewToolStripActionList.qml">../src/FlightDisplay/FlyViewToolStripActionList.qml</file>
		<file alias="QGroundControl/FlightDisplay/FlyViewVideo.qml">../src/FlightDisplay/FlyViewVideo.qml</file>
		<file alias="QGroundControl/FlightDisplay/FlyViewWidgetLayer.qml">../src/FlightDisplay/FlyViewWidgetLayer.qml</file>
		<file alias="QGroundControl/FlightDisplay/GuidedActionActionList.qml">../src/FlightDisplay/GuidedActionActionList.qml</file>
		<file alias="QGroundControl/FlightDisplay/GuidedActionConfirm.qml">../src/FlightDisplay/GuidedActionConfirm.qml</file>
		<file alias="QGroundControl/FlightDisplay/GuidedActionsController.qml">../src/FlightDisplay/GuidedActionsController.qml</file>
		<file alias="QGroundControl/FlightDisplay/GuidedActionLand.qml">../src/FlightDisplay/GuidedActionLand.qml</file>
		<file alias="QGroundControl/FlightDisplay/GuidedActionList.qml">../src/FlightDisplay/GuidedActionList.qml</file>
		<file alias="QGroundControl/FlightDisplay/GuidedActionTakeoff.qml">../src/FlightDisplay/GuidedActionTakeoff.qml</file>
		<file alias="QGroundControl/FlightDisplay/GuidedActionPause.qml">../src/FlightDisplay/GuidedActionPause.qml</file>
		<file alias="QGroundControl/FlightDisplay/GuidedActionRTL.qml">../src/FlightDisplay/GuidedActionRTL.qml</file>
		<file alias="QGroundControl/FlightDisplay/GuidedAltitudeSlider.qml">../src/FlightDisplay/GuidedAltitudeSlider.qml</file>
		<file alias="QGroundControl/FlightDisplay/GuidedToolStripAction.qml">../src/FlightDisplay/GuidedToolStripAction.qml</file>
		<file alias="QGroundControl/FlightDisplay/MultiVehicleList.qml">../src/FlightDisplay/MultiVehicleList.qml</file>
		<file alias="QGroundControl/FlightDisplay/MultiVehiclePanel.qml">../src/FlightDisplay/MultiVehiclePanel.qml</file>
		<file alias="QGroundControl/FlightDisplay/PreFlightBatteryCheck.qml">../src/FlightDisplay/PreFlightBatteryCheck.qml</file>
		<file alias="QGroundControl/FlightDisplay/PreFlightGPSCheck.qml">../src/FlightDisplay/PreFlightGPSCheck.qml</file>
		<file alias="QGroundControl/FlightDisplay/PreFlightRCCheck.qml">../src/FlightDisplay/PreFlightRCCheck.qml</file>
		<file alias="QGroundControl/FlightDisplay/PreFlightSensorsHealthCheck.qml">../src/FlightDisplay/PreFlightSensorsHealthCheck.qml</file>
		<file alias="QGroundControl/FlightDisplay/PreFlightSoundCheck.qml">../src/FlightDisplay/PreFlightSoundCheck.qml</file>
		<file alias="QGroundControl/FlightDisplay/PreFlightCheckListShowAction.qml">../src/FlightDisplay/PreFlightCheckListShowAction.qml</file>
		<file alias="QGroundControl/FlightDisplay/TerrainProgress.qml">../src/FlightDisplay/TerrainProgress.qml</file>
		<file alias="QGroundControl/FlightDisplay/VehicleWarnings.qml">../src/FlightDisplay/VehicleWarnings.qml</file>
		<file alias="QGroundControl/FlightDisplay/qmldir">../src/QmlControls/QGroundControl/FlightDisplay/qmldir</file>
		<file alias="QGroundControl/FlightMap/CameraTriggerIndicator.qml">../src/FlightMap/MapItems/CameraTriggerIndicator.qml</file>
		<file alias="QGroundControl/FlightMap/CenterMapDropButton.qml">../src/FlightMap/Widgets/CenterMapDropButton.qml</file>
		<file alias="QGroundControl/FlightMap/CenterMapDropPanel.qml">../src/FlightMap/Widgets/CenterMapDropPanel.qml</file>
		<file alias="QGroundControl/FlightMap/CompassRing.qml">../src/FlightMap/Widgets/CompassRing.qml</file>
		<file alias="QGroundControl/FlightMap/CustomMapItems.qml">../src/FlightMap/MapItems/CustomMapItems.qml</file>
		<file alias="QGroundControl/FlightMap/FlightMap.qml">../src/FlightMap/FlightMap.qml</file>
		<file alias="QGroundControl/FlightMap/InstrumentSwipeView.qml">../src/FlightMap/Widgets/InstrumentSwipeView.qml</file>
		<file alias="QGroundControl/FlightMap/MapFitFunctions.qml">../src/FlightMap/Widgets/MapFitFunctions.qml</file>
		<file alias="QGroundControl/FlightMap/MapScale.qml">../src/FlightMap/MapScale.qml</file>
		<file alias="QGroundControl/FlightMap/MissionItemIndicator.qml">../src/FlightMap/MapItems/MissionItemIndicator.qml</file>
		<file alias="QGroundControl/FlightMap/MissionItemIndicatorDrag.qml">../src/FlightMap/MapItems/MissionItemIndicatorDrag.qml</file>
		<file alias="QGroundControl/FlightMap/MissionItemView.qml">../src/FlightMap/MapItems/MissionItemView.qml</file>
		<file alias="QGroundControl/FlightMap/MissionLineView.qml">../src/FlightMap/MapItems/MissionLineView.qml</file>
		<file alias="QGroundControl/FlightMap/PlanMapItems.qml">../src/FlightMap/MapItems/PlanMapItems.qml</file>
		<file alias="QGroundControl/FlightMap/PolygonEditor.qml">../src/FlightMap/MapItems/PolygonEditor.qml</file>
		<file alias="QGroundControl/FlightMap/QGCArtificialHorizon.qml">../src/FlightMap/Widgets/QGCArtificialHorizon.qml</file>
		<file alias="QGroundControl/FlightMap/QGCAttitudeHUD.qml">../src/FlightMap/Widgets/QGCAttitudeHUD.qml</file>
		<file alias="QGroundControl/FlightMap/QGCAttitudeWidget.qml">../src/FlightMap/Widgets/QGCAttitudeWidget.qml</file>
		<file alias="QGroundControl/FlightMap/QGCCompassWidget.qml">../src/FlightMap/Widgets/QGCCompassWidget.qml</file>
		<file alias="QGroundControl/FlightMap/QGCPitchIndicator.qml">../src/FlightMap/Widgets/QGCPitchIndicator.qml</file>
		<file alias="QGroundControl/FlightMap/QGCVideoBackground.qml">../src/FlightMap/QGCVideoBackground.qml</file>
		<file alias="QGroundControl/FlightMap/qmldir">../src/QmlControls/QGroundControl/FlightMap/qmldir</file>
		<file alias="QGroundControl/FlightMap/VehicleMapItem.qml">../src/FlightMap/MapItems/VehicleMapItem.qml</file>
		<file alias="QGroundControl/ScreenTools/qmldir">../src/QmlControls/QGroundControl/ScreenTools/qmldir</file>
		<file alias="QGroundControl/ScreenTools/ScreenTools.qml">../src/QmlControls/ScreenTools.qml</file>
		<file alias="QmlTest.qml">../src/QmlControls/QmlTest.qml</file>
		<file alias="RadioComponent.qml">../src/AutoPilotPlugins/Common/RadioComponent.qml</file>
		<file alias="SerialSettings.qml">../src/ui/preferences/SerialSettings.qml</file>
		<file alias="SetupParameterEditor.qml">../src/VehicleSetup/SetupParameterEditor.qml</file>
		<file alias="SetupView.qml">../src/VehicleSetup/SetupView.qml</file>
		<file alias="SimpleItemEditor.qml">../src/PlanView/SimpleItemEditor.qml</file>
		<file alias="StructureScanEditor.qml">../src/PlanView/StructureScanEditor.qml</file>
		<file alias="SurveyItemEditor.qml">../src/PlanView/SurveyItemEditor.qml</file>
		<file alias="SyslinkComponent.qml">../src/AutoPilotPlugins/Common/SyslinkComponent.qml</file>
		<file alias="TaisyncSettings.qml">../src/Taisync/TaisyncSettings.qml</file>
		<file alias="TcpSettings.qml">../src/ui/preferences/TcpSettings.qml</file>
		<file alias="test.qml">../src/test.qml</file>
		<file alias="UdpSettings.qml">../src/ui/preferences/UdpSettings.qml</file>
		<file alias="ValuePageWidget.qml">../src/FlightMap/Widgets/ValuePageWidget.qml</file>
		<file alias="VehicleSummary.qml">../src/VehicleSetup/VehicleSummary.qml</file>
		<file alias="VibrationPageWidget.qml">../src/FlightMap/Widgets/VibrationPageWidget.qml</file>
		<file alias="VideoPageWidget.qml">../src/FlightMap/Widgets/VideoPageWidget.qml</file>
		<file alias="VirtualJoystick.qml">../src/FlightDisplay/VirtualJoystick.qml</file>
		<file alias="VTOLLandingPatternEditor.qml">../src/PlanView/VTOLLandingPatternEditor.qml</file>
	</qresource>
	<qresource prefix="/FirstRunPromptDialogs">
		<file alias="UnitsFirstRunPrompt.qml">../src/FirstRunPromptDialogs/UnitsFirstRunPrompt.qml</file>
		<file alias="OfflineVehicleFirstRunPrompt.qml">../src/FirstRunPromptDialogs/OfflineVehicleFirstRunPrompt.qml</file>
	</qresource>
	<qresource prefix="/json">
		<file alias="ADSBVehicleManager.SettingsGroup.json">../src/Settings/ADSBVehicleManager.SettingsGroup.json</file>
		<file alias="APMMavlinkStreamRate.SettingsGroup.json">../src/Settings/APMMavlinkStreamRate.SettingsGroup.json</file>
		<file alias="App.SettingsGroup.json">../src/Settings/App.SettingsGroup.json</file>
		<file alias="AutoConnect.SettingsGroup.json">../src/Settings/AutoConnect.SettingsGroup.json</file>
		<file alias="BrandImage.SettingsGroup.json">../src/Settings/BrandImage.SettingsGroup.json</file>
		<file alias="BreachReturn.FactMetaData.json">../src/MissionManager/BreachReturn.FactMetaData.json</file>
		<file alias="CameraCalc.FactMetaData.json">../src/MissionManager/CameraCalc.FactMetaData.json</file>
		<file alias="CameraSection.FactMetaData.json">../src/MissionManager/CameraSection.FactMetaData.json</file>
		<file alias="CameraSpec.FactMetaData.json">../src/MissionManager/CameraSpec.FactMetaData.json</file>
		<file alias="CorridorScan.SettingsGroup.json">../src/MissionManager/CorridorScan.SettingsGroup.json</file>
		<file alias="EditPositionDialog.FactMetaData.json">../src/QmlControls/EditPositionDialog.FactMetaData.json</file>
		<file alias="FirmwareUpgrade.SettingsGroup.json">../src/Settings/FirmwareUpgrade.SettingsGroup.json</file>
		<file alias="FlightMap.SettingsGroup.json">../src/Settings/FlightMap.SettingsGroup.json</file>
		<file alias="FlyView.SettingsGroup.json">../src/Settings/FlyView.SettingsGroup.json</file>
		<file alias="FWLandingPattern.FactMetaData.json">../src/MissionManager/FWLandingPattern.FactMetaData.json</file>
		<file alias="MavCmdInfoCommon.json">../src/MissionManager/MavCmdInfoCommon.json</file>
		<file alias="MavCmdInfoFixedWing.json">../src/MissionManager/MavCmdInfoFixedWing.json</file>
		<file alias="MavCmdInfoMultiRotor.json">../src/MissionManager/MavCmdInfoMultiRotor.json</file>
		<file alias="MavCmdInfoRover.json">../src/MissionManager/MavCmdInfoRover.json</file>
		<file alias="MavCmdInfoSub.json">../src/MissionManager/MavCmdInfoSub.json</file>
		<file alias="MavCmdInfoVTOL.json">../src/MissionManager/MavCmdInfoVTOL.json</file>
		<file alias="MissionSettings.FactMetaData.json">../src/MissionManager/MissionSettings.FactMetaData.json</file>
		<file alias="OfflineMaps.SettingsGroup.json">../src/Settings/OfflineMaps.SettingsGroup.json</file>
		<file alias="PlanView.SettingsGroup.json">../src/Settings/PlanView.SettingsGroup.json</file>
		<file alias="QGCMapCircle.Facts.json">../src/MissionManager/QGCMapCircle.Facts.json</file>
		<file alias="RallyPoint.FactMetaData.json">../src/MissionManager/RallyPoint.FactMetaData.json</file>
		<file alias="RCToParamDialog.FactMetaData.json">../src/QmlControls/RCToParamDialog.FactMetaData.json</file>
		<file alias="RTK.SettingsGroup.json">../src/Settings/RTK.SettingsGroup.json</file>
		<file alias="SpeedSection.FactMetaData.json">../src/MissionManager/SpeedSection.FactMetaData.json</file>
		<file alias="StructureScan.SettingsGroup.json">../src/MissionManager/StructureScan.SettingsGroup.json</file>
		<file alias="Survey.SettingsGroup.json">../src/MissionManager/Survey.SettingsGroup.json</file>
		<file alias="TransectStyle.SettingsGroup.json">../src/MissionManager/TransectStyle.SettingsGroup.json</file>
		<file alias="Units.SettingsGroup.json">../src/Settings/Units.SettingsGroup.json</file>
		<file alias="USBBoardInfo.json">../src/comm/USBBoardInfo.json</file>
		<file alias="Vehicle/BatteryFact.json">../src/Vehicle/BatteryFact.json</file>
		<file alias="Vehicle/ClockFact.json">../src/Vehicle/ClockFact.json</file>
		<file alias="Vehicle/DistanceSensorFact.json">../src/Vehicle/DistanceSensorFact.json</file>
		<file alias="Vehicle/EstimatorStatusFactGroup.json">../src/Vehicle/EstimatorStatusFactGroup.json</file>
		<file alias="Vehicle/GPSFact.json">../src/Vehicle/GPSFact.json</file>
		<file alias="Vehicle/GPSRTKFact.json">../src/Vehicle/GPSRTKFact.json</file>
		<file alias="Vehicle/SetpointFact.json">../src/Vehicle/SetpointFact.json</file>
		<file alias="Vehicle/SubmarineFact.json">../src/Vehicle/SubmarineFact.json</file>
		<file alias="Vehicle/TemperatureFact.json">../src/Vehicle/TemperatureFact.json</file>
		<file alias="Vehicle/TerrainFactGroup.json">../src/Vehicle/TerrainFactGroup.json</file>
		<file alias="Vehicle/VehicleFact.json">../src/Vehicle/VehicleFact.json</file>
		<file alias="Vehicle/VibrationFact.json">../src/Vehicle/VibrationFact.json</file>
		<file alias="Vehicle/WindFact.json">../src/Vehicle/WindFact.json</file>
		<file alias="Video.SettingsGroup.json">../src/Settings/Video.SettingsGroup.json</file>
		<file alias="VTOLLandingPattern.FactMetaData.json">../src/MissionManager/VTOLLandingPattern.FactMetaData.json</file>
	</qresource>
	<qresource prefix="/MockLink">
		<file alias="APMArduSubMockLink.params">../src/comm/APMArduSubMockLink.params</file>
		<file alias="PX4MockLink.params">../src/comm/PX4MockLink.params</file>
	</qresource>
</RCC>/os --privileged tizenrt/tizenrt:#!/usr/bin/env python
import os

qgc_rc  = "qgroundcontrol.qrc"
res_rc  = "qgcresources.qrc"
qgc_exc = "qgroundcontrol.exclusion"
res_exc = "qgcresources.exclusion"

def read_file(filename):
    with open(filename) as src:
        return [line.rstrip().lstrip() for line in src.readlines()]

def process(src, exclusion, dst):
    file1 = read_file(src)
    file2 = read_file(exclusion)
    file3 = open(dst, 'w')
    for line in file1:
        if line not in file2:
            if line.startswith('<qresource') or line.startswith('</qresource'):
                file3.write('\t')
            if line.startswith('<file') or line.startswith('</file'):
                file3.write('\t\t')
            newLine = str(line)
            if line.startswith('<file'):
                newLine = newLine.replace(">", ">../", 1)
            file3.write(newLine + '\n')
        else:
            print 'Excluded:', line
    file3.close()

def main():
    if(os.path.isfile(qgc_exc)):
        process(os.path.join("../",qgc_rc), qgc_exc, qgc_rc)
    if(os.path.isfile(res_exc)):
        process(os.path.join("../",res_rc), res_exc, res_rc)

if __name__ == '__main__':
    main() /bin/bash

  arm-version: &arm-version
      run: 
      name: ARM GCC Version
      command: |
        docker exec ${BUILDER} arm-none-eabi-gcc --version

  build-job: &build-job
    run:
      name: TizenRT Build Test
      command: |
        docker exec -it ${BUILDER} bash -c "cd tools; ./configure.sh ${CIRCLE_JOB}"
        docker exec -it ${BUILDER} bash -c "make"


jobs:
  checkout_code:
    machine: 
      image: default
    working_directory: ~/TizenRT
    steps:
      - checkout
      - persist_to_workspace:
          root: ~/TizenRT
          paths:
            - ./
            
  artik055s/audio:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  artik053/tc:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  qemu/build_test:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  esp_wrover_kit/hello_with_tash:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  imxrt1020-evk/loadable_elf_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8721csm/hello:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8721csm/loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8720e/hello:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8720e/loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e/flat_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e/loadable_apps:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

  rtl8730e/flat_dev_ddr:
    machine:
      image: default
    working_directory: ~/TizenRT
    steps:
      - attach_workspace:
          at: ~/TizenRT
      - *docker-cp
      - *build-job

workflows:
  version: 2
  build-tizenrt:
    jobs:
      - checkout_code
      - artik055s/audio:
          requires:
            - checkout_code
      - artik053/tc:
          requires:
            - checkout_code
      - qemu/build_test:
          requires:
            - checkout_code
      - esp_wrover_kit/hello_with_tash:
          requires:
            - checkout_code
      - imxrt1020-evk/loadable_elf_apps:
          requires:
            - checkout_code
      - rtl8721csm/hello:
          requires:
            - checkout_code
      - rtl8721csm/loadable_apps:
          requires:
            - checkout_code
      - rtl8720e/hello:
          requires:
            - checkout_code
      - rtl8720e/loadable_apps:
          requires:
            - checkout_code
      - rtl8730e/flat_apps:
          requires:
            - checkout_code
      - rtl8730e/loadable_apps:
          requires:
            - checkout_code
      - rtl8730e/flat_dev_ddr:
          requires:
            - checkout_code 
  push:
    branches: [ "main" ]
  pull_request:
    # The branches below must be a subset of the branches above
    branches: [ "main" ]
  schedule:
    - cron: '45 19 * * 1'

jobs:
  analyze:
    name: Analyze
    runs-on: ubuntu-latest
    permissions:
      actions: read
      contents: read
      security-events: write

    steps:
    - name: Checkout repository
      uses: actions/checkout@v4

    - name: Synopsys Intelligent Security Scan
      id: prescription
      uses: synopsys-sig/intelligent-security-scan@48eedfcd42bc342a294dc495ac452797b2d9ff08
      with:
        ioServerUrl: ${{secrets.IO_SERVER_URL}}
        ioServerToken: ${{secrets.IO_SERVER_TOKEN}}
        workflowServerUrl: ${{secrets.WORKFLOW_SERVER_URL}}
        additionalWorkflowArgs: --polaris.url=${{secrets.POLARIS_SERVER_URL}} --polaris.token=${{secrets.POLARIS_ACCESS_TOKEN}}
        stage: "IO"

    # Please note that the ID in previous step was set to prescription
    # in order for this logic to work also make sure that POLARIS_ACCESS_TOKEN
    # is defined in settings
    - name: Static Analysis with Polaris
      if: ${{steps.prescription.outputs.sastScan == 'true' }}
      run: |
          export POLARIS_SERVER_URL=${{ secrets.POLARIS_SERVER_URL}}
          export POLARIS_ACCESS_TOKEN=${{ secrets.POLARIS_ACCESS_TOKEN}}
          wget -q ${{ secrets.POLARIS_SERVER_URL}}/api/tools/polaris_cli-linux64.zip
          unzip -j polaris_cli-linux64.zip -d /tmp
          /tmp/polaris analyze -w

    # Please note that the ID in previous step was set to prescription
    # in order for this logic to work
    - name: Software Composition Analysis with Black Duck
      if: ${{steps.prescription.outputs.scaScan == 'true' }}
      uses: blackducksoftware/github-action@9ea442b34409737f64743781e9adc71fd8e17d38
      with:
         args: '--blackduck.url="${{ secrets.BLACKDUCK_URL}}" --blackduck.api.token="${{ secrets.BLACKDUCK_TOKEN}}" --detect.tools="SIGNATURE_SCAN,DETECTOR"'

    - name: Synopsys Intelligent Security Scan
      if: ${{ steps.prescription.outputs.sastScan == 'true' || steps.prescription.outputs.scaScan == 'true' }}
      uses: synopsys-sig/intelligent-security-scan@48eedfcd42bc342a294dc495ac452797b2d9ff08
      with:
        ioServerUrl: ${{secrets.IO_SERVER_URL}}
        ioServerToken: ${{secrets.IO_SERVER_TOKEN}}
        workflowServerUrl: ${{secrets.WORKFLOW_SERVER_URL}}
        additionalWorkflowArgs: --IS_SAST_ENABLED=${{steps.prescription.outputs.sastScan}} --IS_SCA_ENABLED=${{steps.prescription.outputs.scaScan}}
                --polaris.project.name={{PROJECT_NAME}} --polaris.url=${{secrets.POLARIS_SERVER_URL}} --polaris.token=${{secrets.POLARIS_ACCESS_TOKEN}}
                --blackduck.project.name={{PROJECT_NAME}}:{{PROJECT_VERSION}} --blackduck.url=${{secrets.BLACKDUCK_URL}} --blackduck.api.token=${{secrets.BLACKDUCK_TOKEN}}
        stage: "WORKFLOW"

    - name: Upload SARIF file
      if: ${{steps.prescription.outputs.sastScan == 'true' }}
      uses: github/codeql-action/upload-sarif@v3
      with:
        # Path to SARIF file relative to the root of the repository
        sarif_file: workflowengine-results.sarif.json
                                                      
