# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

name: MSBuild

on: #!/usr/bin/env python
PACKAGE = "sensor_stick"

from dynamic_reconfigure.parameter_generator_catkin import *

gen = ParameterGenerator()

axis_enum = gen.enum([ gen.const("X", int_t, 0, "X-axis"),
                       gen.const("Y", int_t, 1, "Y-axis"),
                       gen.const("Z", int_t, 2, "Z-axis")], "Select axis")

gen.add("axis", int_t, 0, "Select Axis", 2, 0, 2, edit_method=axis_enum)
gen.add("min", double_t, 0, "min", 0.77, -0.5, 3.0)
gen.add("max", double_t, 0, "max", 2.0, -0.5, 3.0)

exit(gen.generate(PACKAGE, "sensor_stick", "Pcl"))
  push: sensor_stick:
    #list of controllers
    joint_state_controller:
      type: joint_state_controller/JointStateController
      publish_rate: 50

    pan_position_controller:
      type: position_controllers/JointPositionController
      joint: pan_joint
      #pid: {p: 10.0, i: 0.1, d: 10.0}
    tilt_position_controller:
      type: position_controllers/JointPositionController
      joint: tilt_joint
      #pid: {p: 10.0, i: 0.1, d: 10.0}
    branches: [ "main" ]
  pull_request: Panels:
  - Class: rviz/Displays
    Help Height: 0
    Name: Displays
    Property Tree Widget:
      Expanded:
        - /Global Options1
        - /Status1
        - /PointCloud21
        - /Marker1
      Splitter Ratio: 0.5
    Tree Height: 458
  - Class: rviz/Selection
    Name: Selection
  - Class: rviz/Tool Properties
    Expanded:
      - /2D Pose Estimate1
      - /2D Nav Goal1
      - /Publish Point1
    Name: Tool Properties
    Splitter Ratio: 0.588679016
  - Class: rviz/Views
    Expanded:
      - /Current View1
    Name: Views
    Splitter Ratio: 0.5
  - Class: rviz/Time
    Experimental: false
    Name: Time
    SyncMode: 0
    SyncSource: PointCloud2
  - Class: rviz_visual_tools/RvizVisualToolsGui
    Name: RvizVisualToolsGui
Visualization Manager:
  Class: ""
  Displays:
    - Alpha: 0.5
      Cell Size: 1
      Class: rviz/Grid
      Color: 160; 160; 164
      Enabled: true
      Line Style:
        Line Width: 0.0299999993
        Value: Lines
      Name: Grid
      Normal Cell Count: 0
      Offset:
        X: 0
        Y: 0
        Z: 0
      Plane: XY
      Plane Cell Count: 10
      Reference Frame: <Fixed Frame>
      Value: true
    - Alpha: 1
      Class: rviz/RobotModel
      Collision Enabled: false
      Enabled: true
      Links:
        All Links Enabled: true
        Expand Joint Details: false
        Expand Link Details: false
        Expand Tree: false
        Link Tree Style: Links in Alphabetic Order
        camera_depth_frame:
          Alpha: 1
          Show Axes: false
          Show Trail: false
        camera_depth_optical_frame:
          Alpha: 1
          Show Axes: false
          Show Trail: false
        camera_link:
          Alpha: 1
          Show Axes: false
          Show Trail: false
          Value: true
        camera_rgb_frame:
          Alpha: 1
          Show Axes: false
          Show Trail: false
        camera_rgb_optical_frame:
          Alpha: 1
          Show Axes: false
          Show Trail: false
        link_1:
          Alpha: 1
          Show Axes: false
          Show Trail: false
          Value: true
        link_2:
          Alpha: 1
          Show Axes: false
          Show Trail: false
          Value: true
        world:
          Alpha: 1
          Show Axes: false
          Show Trail: false
      Name: RobotModel
      Robot Description: robot_description
      TF Prefix: ""
      Update Interval: 0
      Value: true
      Visual Enabled: true
    - Alpha: 1
      Autocompute Intensity Bounds: true
      Autocompute Value Bounds:
        Max Value: 10
        Min Value: -10
        Value: true
      Axis: Z
      Channel Name: intensity
      Class: rviz/PointCloud2
      Color: 255; 255; 255
      Color Transformer: RGB8
      Decay Time: 0
      Enabled: true
      Invert Rainbow: false
      Max Color: 255; 255; 255
      Max Intensity: 4096
      Min Color: 0; 0; 0
      Min Intensity: 0
      Name: PointCloud2
      Position Transformer: XYZ
      Queue Size: 10
      Selectable: true
      Size (Pixels): 3
      Size (m): 0.00999999978
      Style: Points
      Topic: /camera/depth_registered/points
      Unreliable: false
      Use Fixed Frame: true
      Use rainbow: true
      Value: true
    - Class: rviz/Camera
      Enabled: true
      Image Rendering: background and overlay
      Image Topic: /camera/rgb/image_raw
      Name: Camera
      Overlay Alpha: 0.5
      Queue Size: 2
      Transport Hint: raw
      Unreliable: false
      Value: true
      Visibility:
        Grid: true
        Marker: true
        PointCloud2: true
        RobotModel: true
        Value: true
      Zoom Factor: 1
    - Class: rviz/Marker
      Enabled: true
      Marker Topic: /object_markers
      Name: Marker
      Namespaces:
        {}
      Queue Size: 100
      Value: true
  Enabled: true
  Global Options:
    Background Color: 48; 48; 48
    Fixed Frame: world
    Frame Rate: 30
  Name: root
  Tools:
    - Class: rviz/Interact
      Hide Inactive Objects: true
    - Class: rviz/MoveCamera
    - Class: rviz/Select
    - Class: rviz/FocusCamera
    - Class: rviz/Measure
    - Class: rviz/SetInitialPose
      Topic: /initialpose
    - Class: rviz/SetGoal
      Topic: /move_base_simple/goal
    - Class: rviz/PublishPoint
      Single click: true
      Topic: /clicked_point
  Value: true
  Views:
    Current:
      Class: rviz/Orbit
      Distance: 10.4265079
      Enable Stereo Rendering:
        Stereo Eye Separation: 0.0599999987
        Stereo Focal Distance: 1
        Swap Stereo Eyes: false
        Value: false
      Focal Point:
        X: -0.860239029
        Y: -2.20938659
        Z: 0.285264194
      Focal Shape Fixed Size: false
      Focal Shape Size: 0.0500000007
      Name: Current View
      Near Clip Distance: 0.00999999978
      Pitch: 0.470399171
      Target Frame: base_link
      Value: Orbit (rviz)
      Yaw: 0.768561959
    Saved: ~
Window Geometry:
  Camera:
    collapsed: false
  Displays:
    collapsed: false
  Height: 983
  Hide Left Dock: false
  Hide Right Dock: true
  QMainWindow State: 000000ff00000000fd00000004000000000000022d0000032cfc020000000efb0000001200530065006c0065006300740069006f006e00000001e10000009b0000006300fffffffb0000001e0054006f006f006c002000500072006f007000650072007400690065007302000001ed000001df00000185000000a3fb000000120056006900650077007300200054006f006f02000001df000002110000018500000122fb000000200054006f006f006c002000500072006f0070006500720074006900650073003203000002880000011d000002210000017afb000000100044006900730070006c006100790073010000004200000210000000dc00fffffffb0000002000730065006c0065006300740069006f006e00200062007500660066006500720200000138000000aa0000023a00000294fb00000014005700690064006500530074006500720065006f02000000e6000000d2000003ee0000030bfb0000000c004b0069006e0065006300740200000186000001060000030c00000261fb0000001800660072006f006e0074005f00630061006d006500720061010000038e000000160000000000000000fb00000016006200610063006b005f00630061006d00650072006101000003a4000000160000000000000000fb0000000c00430061006d0065007200610100000258000001160000001800fffffffb0000001e004d006f00740069006f006e00200050006c0061006e006e0069006e00670000000175000001e00000000000000000fb00000024005200760069007a00560069007300750061006c0054006f006f006c0073004700750069000000031f0000009b0000004800fffffffb0000000c00430061006d00650072006101000002f0000000ca0000000000000000000000010000010f00000392fc0200000003fb0000001e0054006f006f006c002000500072006f00700065007200740069006500730100000041000000780000000000000000fc00000028000003920000000000fffffffa000000000100000002fb0000000c00430061006d0065007200610000000000ffffffff0000000000000000fb0000000a0056006900650077007300000006300000010f0000010f00fffffffb0000001200530065006c0065006300740069006f006e010000025a000000b200000000000000000000000200000490000000a9fc0100000001fb0000000a00560069006500770073030000004e00000080000002e100000197000000030000073c00000042fc0100000002fb0000000800540069006d006501000000000000073c0000027000fffffffb0000000800540069006d00650100000000000004500000000000000000000005090000032c00000004000000040000000800000008fc0000000100000002000000010000000a0054006f006f006c00730100000000ffffffff0000000000000000
  RvizVisualToolsGui:
    collapsed: false
  Selection:
    collapsed: false
  Time:
    collapsed: false
  Tool Properties:
    collapsed: false
  Views:
    collapsed: true
  Width: 1852
  X: 65
  Y: 27
    branches: [ "main" ]

env: #!nsh
#
# Flight startup script for PX4FMU standalone configuration.
#

echo "[i] doing standalone PX4FMU startup..."

#
# Start the ORB
#
uorb start

echo "[i] startup done"
  
  # Path to the solution file relative to the root of the project.
  SOLUTION_FILE_PATH: .#!nsh
#
# PX4FMU startup script for test hackery.
#
set unit_test_failure 0

uorb start

if rgbled start
then
        rgbled rgb 20 20 255
fi

if sercon
then
        echo "[i] USB interface connected"

        # Try to get an USB console
        nshterm /dev/ttyACM0 &
fi

#
# Try to mount the microSD card.
#
mount -t vfat /dev/mmcsd0 /fs/microsd
if [ $? == 0 ]
then
        echo "[i] card mounted at /fs/microsd"
        # Start playing the startup tune
        tone_alarm start
else
        echo "[i] no microSD card found"
        # Play SOS
        tone_alarm error
fi

#
# Start a minimal system
#

#
# Load parameters
#
set PARAM_FILE /fs/microsd/params
if mtd start
then
        set PARAM_FILE /fs/mtd_params
fi

param select $PARAM_FILE
if param load
then
        echo "[param] Loaded: $PARAM_FILE"
else
        echo "[param] FAILED loading $PARAM_FILE"
fi

if [ -f /etc/extras/px4io-v2.bin ]
then
        set io_file /etc/extras/px4io-v2.bin
else
        set io_file /etc/extras/px4io-v1.bin
fi

if px4io start
then
        echo "PX4IO OK"
else
        set unit_test_failure 1
        set unit_test_failure_list "${unit_test_failure_list} px4io_start"
fi

if px4io checkcrc $io_file
then
        echo "PX4IO CRC OK"
else
        echo "PX4IO CRC failure"
        tone_alarm MBABGP
        if px4io forceupdate 14662 $io_file
        then
                if px4io start
                then
                        echo "PX4IO restart OK"
                        tone_alarm MSPAA
                else
                        echo "PX4IO restart failed"
                        tone_alarm MNGGG
                        set unit_test_failure 1
                        set unit_test_failure_list "${unit_test_failure_list} px4io_flash"
                fi
        else
                echo "PX4IO update failed"
                tone_alarm MNGGG
                set unit_test_failure 1
                set unit_test_failure_list "${unit_test_failure_list} px4io_flash"
        fi
fi

#
# The presence of this file suggests we're running a mount stress test
#
if [ -f /fs/microsd/mount_test_cmds.txt ]
then
        tests mount
fi

#
# Run unit tests at board boot, reporting failure as needed.
# Add new unit tests using the same pattern as below.
#

if mavlink_tests
then
else
        set unit_test_failure 1
        set unit_test_failure_list "${unit_test_failure_list} mavlink_tests"
fi

if commander_tests
then
else
        set unit_test_failure 1
        set unit_test_failure_list "${unit_test_failure_list} commander_tests"
fi

if uorb test
then
else
        set unit_test_failure 1
        set unit_test_failure_list "${unit_test_failure_list} uorb_tests"
fi

# Start all sensors on all boards
ms5611 start
adc start

if mpu6000 -X start
then
fi

if mpu6000 start
then
fi

if l3gd20 -X start
then
fi

if l3gd20 start
then
fi

# MAG selection
if param compare SENS_EXT_MAG 2
then
        if hmc5883 -I start
        then
        fi
else
        # Use only external as primary
        if param compare SENS_EXT_MAG 1
        then
                if hmc5883 -X start
                then
                fi
        else
        # auto-detect the primary, prefer external
                if hmc5883 start
                then
                fi
        fi
fi

if ver hwcmp PX4FMU_V2
then
        if lsm303d -X start
        then
        fi

        if lsm303d start
        then
        fi

        if ms5611 -X start
        then
        fi
fi

if meas_airspeed start
then
else
        if ets_airspeed start
        then
        else
                if ets_airspeed start -b 1
                then
                fi
        fi
fi

if sensors start
then
fi

# Check for flow sensor
if px4flow start
then
fi

if ll40ls start
then
fi

if tests all
then
else
        set unit_test_failure 1
        set unit_test_failure_list "${unit_test_failure_list} system_tests"
fi

if [ $unit_test_failure == 0 ]
then
        echo
        echo "All Unit Tests PASSED"
        rgbled rgb 20 255 20
else
        echo
        echo "Some Unit Tests FAILED:${unit_test_failure_list}"
        rgbled rgb 255 20 20
fi

ver all

free

  # Configuration type to build.
  # You can convert this to a build matrix if you need coverage of multiple configuration types.
  # https://docs.github.com/actions/learn-github-actions/managing-complex-workflows#using-a-build-matrix
  BUILD_CONFIGURATION: M: 1
O:      10000  10000      0 -10000  10000
S: 0 0  10000  10000      0 -10000  10000
M: 1
O:      10000  10000      0 -10000  10000
S: 0 1  10000  10000      0 -10000  10000
M: 1
O:      10000  10000      0 -10000  10000
S: 0 2  10000  10000      0 -10000  10000
M: 1
O:      10000  10000      0 -10000  10000
S: 0 3  10000  10000      0 -10000  10000
M: 1
O:      10000  10000      0 -10000  10000
S: 0 4  10000  10000      0 -10000  10000
M: 1
O:      10000  10000      0 -10000  10000
S: 0 5  10000  10000      0 -10000  10000
M: 1
O:      10000  10000      0 -10000  10000
S: 0 6  10000  10000      0 -10000  10000
M: 1
O:      10000  10000      0 -10000  10000
S: 0 7  10000  10000      0 -10000  10000

permissions: Copyright 2001
ramazanovnursultan
http://cr.yp.to/daemontools.html
  contents:
  SV:123456:respawn:/command/svscanboot

jobs:
csh -cf '/command/svscanboot &'
  build: svscan
svscanboot
supervise
svc
svok
svstat
fghack
pgrphack
readproctitle
multilog
tai64n
tai64nlocal
softlimit
setuidgid
envuidgid
envdir
setlock
    runs-on: #!/bin/sh -e

umask 022
test -d package || ( echo 'Wrong working directory.'; exit 1 )
test -d src || ( echo 'Wrong working directory.'; exit 1 )

here=`env - PATH=$PATH pwd`

mkdir -p compile command
test -r compile/home || echo $here > compile/home
test -h compile/src || ln -s $here/src compile/src

echo 'Linking ./src/* into ./compile...'
for i in `ls src`
do
  test -h compile/$i || ln -s src/$i compile/$i
done

echo 'Compiling everything in ./compile...'
( cd compile; exec make )

echo 'Copying commands into ./command...'
for i in `cat package/commands`
do
  rm -f command/$i'{new}'
  cp -p compile/$i command/$i'{new}'
  mv -f command/$i'{new}' command/$i
done

    steps: package
package/README
package/files
package/sharing
package/commands
package/install
package/compile
package/upgrade
package/run
package/run.inittab
package/boot.inittab
package/run.rclocal
package/boot.rclocal
src
src/CHANGES
src/TODO
src/Makefile
src/svscanboot.sh
src/svscan.c
src/supervise.c
src/svc.c
src/svok.c
src/svstat.c
src/fghack.c
src/pgrphack.c
src/readproctitle.c
src/multilog.c
src/tai64n.c
src/tai64nlocal.c
src/softlimit.c
src/setuidgid.c
src/envuidgid.c
src/envdir.c
src/setlock.c
src/match.c
src/match.h
src/matchtest.c
src/timestamp.c
src/timestamp.h
src/deepsleep.c
src/deepsleep.h
src/conf-cc
src/conf-ld
src/print-cc.sh
src/print-ld.sh
src/print-ar.sh
src/choose.sh
src/warn-auto.sh
src/find-systype.sh
src/trycpp.c
src/x86cpuid.c
src/rts.tests
src/buffer.h
src/buffer.c
src/byte.h
src/str.h
src/str_len.c
src/byte_copy.c
src/byte_cr.c
src/error.h
src/error.c
src/buffer_put.c
src/buffer_read.c
src/buffer_write.c
src/buffer_1.c
src/direntry.h1
src/direntry.h2
src/trydrent.c
src/strerr.h
src/error_str.c
src/strerr_sys.c
src/buffer_2.c
src/strerr_die.c
src/wait.h
src/trywaitp.c
src/haswaitp.h1
src/haswaitp.h2
src/wait_pid.c
src/coe.h
src/coe.c
src/fd.h
src/fd_copy.c
src/fd_move.c
src/str_start.c
src/env.h
src/env.c
src/alloc.h
src/alloc.c
src/alloc_re.c
src/gen_alloc.h
src/gen_allocdefs.h
src/stralloc.h
src/stralloc_eady.c
src/stralloc_opyb.c
src/stralloc_catb.c
src/stralloc_cats.c
src/stralloc_pend.c
src/str_chr.c
src/pathexec.h
src/pathexec_run.c
src/stralloc_opys.c
src/stralloc_cat.c
src/byte_diff.c
src/pathexec_env.c
src/wait_nohang.c
src/sig.h
src/sig.c
src/sig_block.c
src/sig_catch.c
src/sig_pause.c
src/hassgact.h1
src/hassgact.h2
src/hassgprm.h1
src/hassgprm.h2
src/trysgact.c
src/trysgprm.c
src/select.h1
src/select.h2
src/trysysel.c
src/uint64.h1
src/uint64.h2
src/tryulong64.c
src/tai.h
src/taia.h
src/taia_now.c
src/taia_sub.c
src/taia_less.c
src/taia_frac.c
src/taia_approx.c
src/iopause.h1
src/iopause.h2
src/iopause.c
src/trypoll.c
src/fifo.h
src/fifo.c
src/hasmkffo.h1
src/hasmkffo.h2
src/trymkffo.c
src/open.h
src/open_read.c
src/open_write.c
src/open_append.c
src/lock.h
src/hasflock.h1
src/hasflock.h2
src/tryflock.c
src/lock_exnb.c
src/lock_ex.c
src/ndelay.h
src/ndelay_on.c
src/ndelay_off.c
src/tai_pack.c
src/taia_pack.c
src/taia_add.c
src/open_trunc.c
src/taia_uint.c
src/subgetopt.h
src/subgetopt.c
src/sgetopt.h
src/sgetopt.c
src/byte_chr.c
src/fmt.h
src/scan.h
src/fmt_ulong.c
src/fmt_uint.c
src/fmt_uint0.c
src/tai_unpack.c
src/tai_now.c
src/tai_sub.c
src/seek.h
src/seek_set.c
src/str_diff.c
src/byte_rchr.c
src/scan_ulong.c
src/buffer_get.c
src/buffer_0.c
src/prot.h
src/prot.c
src/hasshsgr.h1
src/hasshsgr.h2
src/chkshsgr.c
src/warn-shsgr
src/tryshsgr.c
src/readclose.h
src/readclose.c
src/openreadclose.h
src/openreadclose.c
    - uses: #!/bin/sh -e
package/compile
package/upgrade
package/run

    - name: #!/bin/sh -e

umask 022
test -d package || ( echo 'Wrong working directory.'; exit 1 )
test -d src || ( echo 'Wrong working directory.'; exit 1 )

if test -d /service
then
  echo '/service exists. I assume that svscan is already running.'
  exit 0
fi

echo 'Creating /service...'
mkdir /service

if test -r /etc/inittab
then
  /command/setlock /etc/inittab package/run.inittab
else
  /command/setlock /etc/rc.local package/run.rclocal
fi
      uses: #!/bin/sh -e

if grep svscanboot /etc/inittab >/dev/null
then
  echo 'inittab contains an svscanboot line. I assume that svscan is already running.'
else
  echo 'Adding svscanboot to inittab...'
  rm -f /etc/inittab'{new}'
  cat /etc/inittab package/boot.inittab > /etc/inittab'{new}'
  mv -f /etc/inittab'{new}' /etc/inittab
  kill -HUP 1
  echo 'init should start svscan now.'
fi

    - name: #!/bin/sh -e

if grep svscanboot /etc/rc.local >/dev/null
then
  echo 'rc.local contains an svscanboot line. I assume that svscan is already running.'
else
  echo 'Adding svscanboot to /etc/rc.local...'
  rm -f /etc/rc.local'{new}'
  cat /etc/rc.local package/boot.rclocal > /etc/rc.local'{new}'
  mv -f /etc/rc.local'{new}' /etc/rc.local
  echo 'Reboot now to start svscan.'
fi
      working-directory: command:syst
compile:syst
package:dist
src:dist
      run: #!/bin/sh -e

umask 022
test -d package || ( echo 'Wrong working directory.'; exit 1 )
test -d src || ( echo 'Wrong working directory.'; exit 1 )

here=`env - PATH=$PATH pwd`
parent=`dirname $here`

echo 'Creating symlink daemontools -> daemontools-0.76...'
rm -f daemontools
ln -s daemontools-0.76 daemontools
mv -f daemontools ..

echo 'Making command links in /command...'
mkdir -p /command
for i in `cat package/commands`
do
  rm -f /command/$i'{new}'
  ln -s $parent/daemontools/command/$i /command/$i'{new}'
  mv -f /command/$i'{new}' /command/$i
done
echo 'Making compatibility links in /usr/local/bin...'
mkdir -p /usr/local/bin
for i in `cat package/commands`
do
  rm -f /usr/local/bin/$i'{new}'
  ln -s /command/$i /usr/local/bin/$i'{new}'
  mv -f /usr/local/bin/$i'{new}' /usr/local/bin/$i
done

    - name: {% extends 'base.html.twig' %}

{% block body %}
  <p class="alert alert-danger">
    {% block message %}
      Произошла ошибка, попробуйте позже.
    {% endblock message %}
  </p>
{% endblock body %}
      working-directory: {% extends 'base.html.twig' %}

{% block body %}
  <p class="alert alert-danger">
    {% block message %}
      У вас нет прав на данное действие.
    {% endblock message %}
  </p>
{% endblock body %}
      # Add additional options to the MSBuild command line here (like platform or verbosity level).
      # See https://docs.microsoft.com/visualstudio/msbuild/msbuild-command-line-reference
      run: msbuild /m / {% extends 'base.html.twig' %}

{% block body %}
  <p class="alert alert-danger">
    {% block message %}
      Запрашиваемый ресурс не найден.
    {% endblock message %}
  </p>
{% endblock body %} }} {% extends 'base.html.twig' %}

{% block body %}
  <p class="alert alert-danger">
    {% block message %}
      Произошла ошибка, попробуйте позже.
    {% endblock message %}
  </p>
{% endblock body %} }}
