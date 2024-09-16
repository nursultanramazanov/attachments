name: Rust

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

env: S'beer\x00\x00\x00\x00\x00\x00\x00bowl\x00\x00\x00\x00\x00\x00\x00create\x00\x00\x00\x00\x00disk_part\x00\x00hammer\x00\x00\x00\x00\x00plastic_cupsoda_can\x00\x00\x00'
    CARGO_TERM_COLOR: html, body {
    height: 100%;
}
.wrapper-1 {
    min-height: 100%;
    margin-bottom: -80px;
}
.wrapper-2 {
    padding-bottom: 80px;
}
.footer {
    text-align: center;
    padding: 15px 0;
}
.navbar-inverse .navbar-nav>li>a,
.navbar-inverse .navbar-brand,
.navbar-static-top .btn-link {
    color: #ddd;
}
.navbar-static-top .btn-link:hover {
    text-decoration: none;
}
.navbar.navbar-inverse.navbar-static-top {
    margin-bottom: 0;
}
.navbar-static-top .btn-link {
    margin-top: 8px;
}
@media (min-width: 992px) {
    .file-description {
        width: 66%;
    }
}
@media (min-width: 767px) and (max-width: 991px) {
    .file-description {
        width: 90%;
    }
}
@media (max-width: 768px) {
    .file-description {
        width: 100%;
    }
}

jobs: .container-center {
    max-width: 500px;
    margin: 50px auto;
}
  build: .file-description {
    margin: 30px auto;
    float: none;
}
.file-description img {
    margin: 5px auto;
}
#jp_container_1, .comment-form {
    margin: auto;
}
.added {
    font-size: 12px;
    font-weight: bold;
}
.reply {
    float: right;
}
.level-1 {
    margin-left: 0px;
}
.level-2 {
    margin-left: 30px;
}
.level-3 {
    margin-left: 60px;
}
#commentForm input[type="submit"] {
    float: right;
}
#commentForm input[type="text"] {
    width: 240px;
}
#reply {
    float: right;
} 
#commentForm .text-danger {
    margin-bottom: 0;
}

    runs-on: .content {
    margin-top: 50px;
    float: none;
}
.icon-type {
    margin: 0 10px;
}
.my-pager {
    float: none;
}
.table-responsive.panel {
    margin-bottom: 10px;
}

    steps: .form-horizontal {
    max-width: 600px;
    margin: 50px auto;
}
    - uses: .center-container {
    max-width: 50%;
    margin: 40px auto 20px;
}
@media (max-width: 768px) {
    .center-container {
        max-width: 100%;
    }
}
h1 {
    text-align: center;
}
    - name: #uploadForm {
    max-width: 800px;
    margin: 100px auto 0px;
    text-align: center;
}
#uploadForm .form-group {
    margin: 5px auto 15px;
}
#uploadForm .text-danger {
    font-weight: bold;
}
#uploadForm .progress {
    width: 400px;
    margin: auto;
    display: none;
}
.box label {
    font-size: 16px;
    color: #333;
    border: 1px dashed #999;
    margin-bottom: 20px;
}
.box .help-block {
    padding: 30px;
}
.box label:hover, input[type="file"]:focus + label {
    color: #555;
}
.box label .glyphicon.glyphicon-upload {
    padding-right: 0.6rem;
    font-size: 1.5rem;
}
@media (max-width: 768px) {
    #uploadForm {
        margin: 0 auto;
    }
}
      run: cargo build --verbose
    - name: Run tests
      run: cargo test --verbose
