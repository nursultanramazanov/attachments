name: Rust

on: CC := gcc
ifeq ($(USE_GPU),1)
CUCC := nvcc
endif

# Select backend files based on selected backend
# Supported values: naive, onednn
BACKEND ?= onednn

# Set to 1 to use accelerated matrix products when using naive backend
USE_AVX ?= 0

# The root directory of the oneDNN library, only needed when using
# onednn backend
ONEDNN_ROOT_DIR ?= lib/onednn
ONEDNN_INCLUDE_DIR := $(ONEDNN_ROOT_DIR)/include
ONEDNN_SHARED_DIR := $(ONEDNN_ROOT_DIR)/lib/

# Can be set in case the directory where libcudart.so is located is not
# in the default directories
CUDA_LIB_DIR ?=

# Select log level
# Supported values: 1(error), 2(warn), 3(info), 4(trace)
LOG_LEVEL ?= 3

# Set to 1 to enable gdb support
DEBUG ?= 0


ifeq ($(DEBUG),1)
ifeq ($(USE_AVX),1)
$(error Can not have DEBUG=1 and USE_AVX=1 at the same time)
endif
endif


CFLAGS :=
CUFLAGS :=
ifdef LOG_LEVEL
CFLAGS += -DLOG_LEVEL=$(LOG_LEVEL)
CUFLAGS += -DLOG_LEVEL=$(LOG_LEVEL)
endif
ifeq ($(USE_AVX),1)
CFLAGS += -march=haswell -DUSE_AVX
endif
ifeq ($(USE_GPU),1)
CFLAGS += -DUSE_GPU
CUFLAGS += -DUSE_GPU
endif
ifeq ($(DEBUG),1)
CFLAGS += -g -DDEBUG
CUFLAGS += -g -DDEBUG
else
CFLAGS += -O3 -Ofast
CUFLAGS += -O3
endif


# math library
LDFLAGS := -lm


SOURCEDIR := src

# INCLUDE and SOURCE file located in the src directory
INCLUDE := -I$(SOURCEDIR)/lib -I$(SOURCEDIR)/common
SRC := $(shell find $(SOURCEDIR)/common -name '*.c')
SRC += $(SOURCEDIR)/lib/log.c $(SOURCEDIR)/lib/config_info.c $(SOURCEDIR)/lib/random.c
# Also add the target source file
SRC += $(TARGET).c


# Select backend files based on selected backend
ifeq ($(BACKEND),naive)
INCLUDE += -I$(SOURCEDIR)/naive -I$(SOURCEDIR)/include
SRC += $(shell find $(SOURCEDIR)/naive -name '*.c')
ifeq ($(USE_GPU),1)
SRC += $(shell find $(SOURCEDIR)/naive -name '*.cu')
ifneq ($(CUDA_LIB_DIR),)
LDFLAGS += -L$(CUDA_LIB_DIR)
endif
LDFLAGS += -lcudart
endif
CFLAGS += -DBACKEND_NAIVE
CUFLAGS += -DBACKEND_NAIVE
else ifeq ($(BACKEND),onednn)
INCLUDE += -I$(SOURCEDIR)/onednn -I$(ONEDNN_INCLUDE_DIR)
SRC += $(shell find $(SOURCEDIR)/onednn -name '*.c')
LDFLAGS += -L$(ONEDNN_SHARED_DIR) -ldnnl 
CFLAGS += -DBACKEND_ONEDNN
else
$(error Only naive and onednn implementation available.)
endif


# Object files are placed in same directory as src files, just with different file extension
OBJ := $(SRC:.c=.o)
ifeq ($(USE_GPU),1)
OBJ := $(OBJ:.cu=.o)
endif               
  push: include config/defines.mk


# Link all object files into a source file
$(TARGET): $(OBJ)
        $(CC) $^ -o $@ $(LDFLAGS)


# Rule to compile a single translation unit
%.o: %.c
        $(CC) $(INCLUDE) $(CFLAGS) -c $< -o $@

# Rule to compile a single cuda translation unit
ifeq ($(USE_GPU),1)
%.o: %.cu
        $(CUCC) $(INCLUDE) $(CUFLAGS) -c $< -o $@
endif


clean:
        @$(RM) -rv $(TARGET) $(OBJ)


rebuild:
        make clean && make


run: $(TARGET)
# since oneDNN is built as a shared library, need to add its location
# to LD_LIBRARY_PATH so that the target executable can find it
ifeq ($(BACKEND),onednn)
run: export LD_LIBRARY_PATH=$$LD_LIBRARY_PATH:$(ONEDNN_SHARED_DIR)
endif
run:
        @$(TARGET)


.PHONY: clean rebuild run            
    branches: [ "main" ]
  pull_request: import QtQuick 2.4
import QtGraphicalEffects 1.0

Item {
    id: item

    property alias source: mask.source

    Rectangle {
        id: circleMask
        anchors.fill: parent

        smooth: true
        visible: false

        radius: Math.max(width/2, height/2)
    }

    OpacityMask {
        id: mask

        anchors.fill: parent
        maskSource: circleMask
    }
}         
    branches: [ "main" ]

env: import QtQuick.Dialogs 1.0
import QtQuick 2.0

Dialog {
    title: qsTr("Nearby devices")

    width: 300
    height: 300

    contentItem: ListView {
        id: nearbyDevicesView

        model: ListModel {
            ListElement {
                name: "IPHONE 5c"
                address: "45:34:23:F5"
                classId: "2342342"
            }

            ListElement {
                name: "NOKIA 3320"
                address: "45:B4:A3:F5"
                classId: "234234232"
            }

            ListElement {
                name: "SAMSUNG J7"
                address: "45:B4:A3:F5"
                classId: "234234232"
            }
        }

        delegate: DeviceDelegate {
            width: nearbyDevicesView.width
        }
    }
}              
  CARGO_TERM_COLOR: import QtQuick 2.0
import QtQuick.Controls 2.0
import "."

Item {
    height: layout.implicitHeight + 20

    SystemPalette { id: myPalette; colorGroup: SystemPalette.Active }

    Theme {
        id: theme
    }

    Rectangle {
        anchors.fill: parent
        color: hoverArea.containsMouse

        Behavior on color { ColorAnimation { duration: 250 } }
    }

    Image {
        id: devicePicture

        width: 48
        height: 48

        source: "images/device_%1.svg".arg(type)
        smooth: true
        antialiasing: true

        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
    }

    Column {
        id: layout
        anchors.left: devicePicture.right
        anchors.right: actions.left
        anchors.top: devicePicture.top

        Label {
            text: name
            font.bold: true
            font.pixelSize: theme.titlePixelSize
            color: myPalette.windowText
        }

        Label {
            text: qsTr("   <b>MAC</b>: %1").arg(address)
            font.bold: true
            font.pixelSize: theme.subtitlePixelSize
            color: myPalette.windowText
        }

        Label {
            text: qsTr("   <b>ID</b>: %1").arg(classId)
            font.bold: true
            font.pixelSize: theme.subtitlePixelSize
            color: myPalette.windowText
        }
    }

    Item {
        id: actions

        width: 40

        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter

        opacity: hoverArea.containsMouse ? 1 : 0

        Behavior on opacity { SmoothedAnimation { duration: 250 } }
    }

    MouseArea {
        id: hoverArea
        hoverEnabled: true
        propagateComposedEvents: false
    }
}         

jobs: import QtQuick 2.4
import QtQuick.Controls 1.2

Item {
    property string pageTitle: qsTr("Near by devices")

    Theme {
        id: theme
    }

    Item {
        id: controls

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 10

        height: 40

        RadiusButton {
            source: "images/up.svg"

            anchors.left: parent.left
            anchors.top: parent.top
            anchors.topMargin: 30

            transform: Rotation { angle: -90 }

            onClicked: {
                console.log('clicked back')
                stackView.back()
            }
        }

        RadiusButton {
            source: "images/search.svg"

            anchors.right: parent.right
            anchors.top: parent.top

            visible: !BTManager.discovering

            onClicked: {
                console.log('clicked search')
                BTManager.search()
            }
        }
    }

    ListView {
        id: devicesListView

        clip: true

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.top: controls.bottom
        anchors.margins: 10

        model: BTManager.nearby_devices

        delegate: Item {
            width: devicesListView.width; height: 55;

            Item {
                anchors.fill: parent
                anchors.topMargin: 10
    //            Rectangle {
    //                anchors.fill: parent
    //                color: Qt.rgba(1.000, 0.514, 0.133, 0.4)
    //            }

                Image {
                    id: device_icon
                    source: "images/device_%1.svg".arg(type)
                    width: 40
                    height: 40

                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                }

                Text {
                    id: title_text
                    text: name
                    font.pixelSize: theme.titlePixelSize
                    font.bold: true
                    anchors.left: device_icon.right
                    anchors.leftMargin: 20
                    anchors.right: inline_controls.left
                }

                Text {
                    text: qsTr("Address: %1").arg(host)
                    font.pixelSize: theme.subtitlePixelSize
                    anchors.left: device_icon.right
                    anchors.right: inline_controls.left
                    anchors.top: title_text.bottom
                    anchors.leftMargin: 20
                    anchors.topMargin: 5
                }

                Item {
                    id: inline_controls
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.topMargin: 7

                    width: 80
                    height: 45

                    Row {
                        anchors.fill: parent
                        spacing: 10

                        RadiusButton {
                            source: "images/select.svg"
                            width: 32
                            height: 32
                            visible: BTManager.my_device.host != host
                            onClicked: {
                                console.log('clicked select', modelData.host)
                                BTManager.my_device = modelData
                            }
                        }

                        RadiusButton {
                            source: "images/services.svg"

                            width: 32
                            height: 32

                            visible: !BTManager.discovering

                            onClicked: {
                                console.log('clicked search')
                            }
                        }
                    }
                }
            }
        }
    }
}                                   
  build: import QtQuick 2.4
import QtQuick.Controls 1.3

Item {

    Theme {
        id: theme
    }

    Rectangle {
        anchors.fill: parent
        color: theme.footerColor
    }

    Item {
        id: near_by_updating

        anchors.fill: parent
        visible: BTManager.discovering && !tool_tip.visible

        BusyIndicator {
            id: busy_indicator
            width: 18
            height: 18

            anchors.left: parent.left
            anchors.margins: 10
            anchors.verticalCenter: parent.verticalCenter
        }

        Text {
            text: qsTr("Updating near by devices...")
            color: theme.footerTextColor

            anchors.left: busy_indicator.right
            anchors.margins: 10
            anchors.verticalCenter: parent.verticalCenter
        }
    }

    Item {
        id: tool_tip

        anchors.fill: parent
        visible: BTNotifier.tooltip && BTNotifier.tooltip.length

        Text {
            text: BTNotifier.tooltip
            color: theme.footerTextColor

            anchors.left: parent.left
            anchors.margins: 10
            anchors.verticalCenter: parent.verticalCenter
        }
    }
}                    
                    

    runs-on: import QtQuick 2.4
//import Material 0.2
//import Material.Extras 0.1

MouseArea {
    id: view

    clip: true
    hoverEnabled: true
    z: 2

    property int startRadius: circular ? width/10 : width/6
    property int endRadius

    property Item lastCircle
    property color color: Qt.rgba(0,0,0,0.1)

    property bool circular: false
    property bool centered: false

    property int focusWidth: width - 32
    property bool focused
    property color focusColor: "transparent"

    property bool showFocus: true

    onPressed: {
        createTapCircle(mouse.x, mouse.y)
    }

    onCanceled: {
        lastCircle.removeCircle();
    }

    onReleased: {
        lastCircle.removeCircle();
    }

    function createTapCircle(x, y) {
        endRadius = centered ? width/2 : radius(x, y)
        showFocus = false

        lastCircle = tapCircle.createObject(view, {
                                                       "circleX": centered ? width/2 : x,
                                                       "circleY": centered ? height/2 : y
                                                   });
    }

    function radius(x, y) {
        var dist1 = Math.max(dist(x, y, 0, 0), dist(x, y, width, height))
        var dist2 = Math.max(dist(x, y, width, 0), dist(x, y, 0, height))

        return Math.max(dist1, dist2)
    }

    function dist(x1, y1, x2, y2) {
        var xs = 0;
        var ys = 0;

        xs = x2 - x1;
        xs = xs * xs;

        ys = y2 - y1;
        ys = ys * ys;

        return Math.sqrt( xs + ys );
    }

    Rectangle {
        id: focusBackground

        anchors.fill: parent

        color: focusColor && focusColor.a > 0
                ? Qt.rgba(0,0,0,0.2) : Qt.rgba(0,0,0,0.1)

        opacity: showFocus && focused ? 1 : 0

        Behavior on opacity {
            NumberAnimation { duration: 500; easing.type: Easing.InOutQuad }
        }
    }

    Rectangle {
        id: focusCircle

        anchors.centerIn: parent

        width: focused
                ? focusedState ? focusWidth
                               : Math.min(parent.width - Units.dp(8), focusWidth + Units.dp(12))
                : parent.width/5
        height: width

        radius: width/2

        opacity: showFocus && focused ? 1 : 0

        Behavior on opacity {
            NumberAnimation { duration: 500; easing.type: Easing.InOutQuad }
        }

        Behavior on width {
            NumberAnimation { duration: focusTimer.interval; }
        }

        color: focusColor.a === 0 ? Qt.rgba(1,1,1,0.4) : focusColor

        property bool focusedState

        Timer {
            id: focusTimer
            running: focused
            repeat: true
            interval: 800

            onTriggered: focusCircle.focusedState = !focusCircle.focusedState
        }
    }

    Component {
        id: tapCircle

        Item {
            id: circleItem

            anchors.fill: parent

            property bool done

            function removeCircle() {
                done = true

                if (fillSizeAnimation.running) {
                    fillOpacityAnimation.stop()
                    closeAnimation.start()

                    circleItem.destroy(500);
                } else {
                    showFocus = true
                    fadeAnimation.start();

                    circleItem.destroy(300);
                }
            }

            property real circleX
            property real circleY

            property bool closed

            Item {
                id: circleParent
                anchors.fill: parent
                visible: !circular

                Rectangle {
                    id: circleRectangle

                    x: circleItem.circleX - radius
                    y: circleItem.circleY - radius

                    width: radius * 2
                    height: radius * 2

                    opacity: 0
                    color: view.color

                    NumberAnimation {
                        id: fillSizeAnimation
                        running: true

                        target: circleRectangle; property: "radius"; duration: 500;
                        from: startRadius; to: endRadius; easing.type: Easing.InOutQuad

                        onStopped: {
                            if (done)
                                showFocus = true
                        }
                    }

                    NumberAnimation {
                        id: fillOpacityAnimation
                        running: true

                        target: circleRectangle; property: "opacity"; duration: 300;
                        from: 0; to: 1; easing.type: Easing.InOutQuad
                    }

                    NumberAnimation {
                        id: fadeAnimation

                        target: circleRectangle; property: "opacity"; duration: 300;
                        from: 1; to: 0; easing.type: Easing.InOutQuad
                    }

                    SequentialAnimation {
                        id: closeAnimation

                        NumberAnimation {
                            target: circleRectangle; property: "opacity"; duration: 250;
                            to: 1; easing.type: Easing.InOutQuad
                        }

                        NumberAnimation {
                            target: circleRectangle; property: "opacity"; duration: 250;
                            from: 1; to: 0; easing.type: Easing.InOutQuad
                        }
                    }
                }
            }

            CircleMask {
                anchors.fill: parent
                source: circleParent
                visible: circular
            }
        }
    }
}                           

    steps: import QtQuick 2.0
import QtQuick.Controls 1.3
import QtGraphicalEffects 1.0

ApplicationWindow {
    id: applicationWindow
    title: qsTr("PC Suite")
    width: 650
    height: 600

    Theme {
        id: theme
    }

//    ConnectDialog {
//        id: connectDialog
//        visible: false
//    }

    Rectangle {
        id: header

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
//        visible: false

        color: theme.headerColor
        height: 50

        property int fontSize: 20

        Text {
            text: qsTr("PC Suite")

            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.verticalCenter: parent.verticalCenter
            font.pixelSize: theme.titlePixelSize
            color: theme.headerTextColor
        }

        layer.enabled: true
        layer.effect: DropShadow {
            color: Qt.rgba(0.169, 0.169, 0.169, 0.3)
            transparentBorder: true
            horizontalOffset: 8
            verticalOffset: 8
        }

        Image {
            source: "images/play.svg"

            width: 20
            height: 20

            visible: BTManager.my_device.mm_controls_supported

            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.margins: 10

            MouseArea {
                anchors.fill: parent
                onClicked: console.log('play clicked')
            }
        }
    }

    Sidebar {
        id: sidebar
        width: 200

        anchors.left: parent.left
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
    }

    StackView {
        id: stackView
        clip: true
        anchors.left: sidebar.right
        anchors.top: header.bottom
        anchors.bottom: footer.top
        anchors.right: parent.right

        initialItem: MenuGrid {}

        function navigateTo(source, properties) {
            properties = properties || {}
            var parts = ['file:/', AppPath, source]
            console.log('Open path ', parts.join('/'))
            stackView.push(parts.join('/'), properties)
        }

        function back() {
            if (depth === 1) return

            pop()
        }
    }

    Footer {
        id: footer

        height: 60

        anchors.left: sidebar.right
        anchors.right: parent.right
        anchors.bottom: parent.bottom
    }

    Component.onCompleted: {
        visible = true
        console.info('Root window created')

        if (BTManager.my_device.host && BTManager.my_device.host.length) {
            BTManager.my_device.update()
        }
    }   
} 
     
    - uses: import QtQuick 2.4
import QtQuick.Layouts 1.2

Item {

    property var additional: {
        'nearby': {title: qsTr("Near By"), icon: "images/search.svg", source: "/ui/DiscoveryPage.qml"},
        'info': {title: qsTr("Phone info"), source: "", icon: "images/search.svg"},
        'calendar': {title: qsTr("Calendar"), source: "", icon: "images/calendar.svg"},
        'sms': {title: qsTr("Sms "), source: "/ui/SmsPage.qml", icon: "images/sms.svg"},
        'calls': {title: qsTr("Calls"), source: "", icon: "images/calls.svg"},
        'manager': {title: qsTr("File manager"), source: "", icon: "images/file_manager.svg"}
    }

    property int cellWidth: 100
    property int cellHeight: 100
    property double cellIconSize: 40

    Timer {
        id: update_cell_size_timer
        interval: 100
        onTriggered: updateCellSize()
    }

    onWidthChanged: update_cell_size_timer.start()

    function updateCellSize ()
    {
        cellWidth  = Math.max((Math.round(width / gridView.columns)), 100) - 40
        cellHeight = Math.max((Math.round(height / gridView.columns)), 100) - 40
        cellIconSize = Math.min(cellWidth, cellHeight) - 60
    }

    Grid {
        id: gridView
        anchors.fill: parent
        anchors.margins: 20

        columns: 3
        columnSpacing: 10
        rowSpacing: 10
        spacing: 10

        horizontalItemAlignment: Qt.AlignHCenter
        verticalItemAlignment: Qt.AlignVCenter

        Repeater {
            model: BTNotifier.services

            Item {
                id: delegateItem
                width: cellWidth
                height: 100

                ColumnLayout {
                    anchors.fill: parent

                    Image {
                        width: cellIconSize
                        height: cellIconSize
                        source: additional[name].icon

                        Layout.alignment: Qt.AlignHCenter
                        Layout.preferredHeight: 40
                        Layout.preferredWidth: 40
                    }

                    Text {
                        text: additional[name].title
                        horizontalAlignment:  Text.AlignHCenter
                        Layout.alignment: Qt.AlignHCenter
                    }
                }

                Rectangle {
                    width: 20
                    height: 20
                    radius: 20
                    color: 'red'

                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.margins: 10

                    visible: (value > 0)

                    Text {
                        anchors.centerIn: parent
                        text: (value < 9) ? value : '9+'
                        color: 'white'
                        font.bold: true
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    onClicked: {
                        var url = additional[name].source
                        if (url.length) {
                            console.log('Open url ', AppPath, url)
                            stackView.navigateTo(additional[name].source)
                        }
                        else
                            console.log('Page source not set')
                    }
                }
            }
        }
    }

    Component.onCompleted: {
        console.log('Menu grid completed')
    }
}     
          
    - name: import QtQuick 2.0

Image {
    id: btn
    clip: true
    smooth: true
    width: size
    height: size

    property double size: 30

    signal clicked()

    Ink {
        anchors.fill: parent
        onClicked: btn.clicked()
        circular: true
        centered: true
        cursorShape: Qt.PointingHandCursor
    }
}                      
      run: import QtQuick 2.0
import QtQuick.Controls 1.3
import QtQuick.Dialogs 1.0

Item {
    Theme {
        id: theme
    }

    Rectangle {
        anchors.fill: parent
        color: theme.sidebarColor
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a file"
        folder: shortcuts.home
        onAccepted: {
            console.log("You chose: " + fileDialog.fileUrls)
            var files = [fileDialog.fileUrls]
            BTManager.send_file(fileDialog.fileUrls)
            visible = false
        }
        onRejected: {
            console.log("Canceled")
            visible = false
        }
    }

    Image {
        id: my_device_picture
        source: MyDevice.image

        smooth: true
        fillMode: Image.PreserveAspectFit

        width: 240 / 2.5
        height: 494 / 2.5

        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 20

        BusyIndicator {
            width: 20
            height: 20
            visible: running
            running: BTManager.my_device.updating
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 10

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: BTNotifier.tooltip = qsTr("Connecting to %1").arg(BTManager.my_device.name)
                onExited: BTNotifier.tooltip = ''
            }
        }

        Image {
            source: BTManager.my_device.available ? "images/online.png" : "images/offline.png"
            width: 20
            height: 20
            smooth: true
            fillMode: Image.PreserveAspectFit
            visible: (BTManager.my_device.host && BTManager.my_device.host.length) && !BTManager.my_device.updating

            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 10

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: {
                    var text = ''
                    if (BTManager.my_device.available && BTManager.my_device.connected)
                        text = qsTr("%1 near by, and connected").arg(BTManager.my_device.name)
                    else if (BTManager.my_device.available && !BTManager.my_device.connected)
                        text = qsTr("%1 near by, but not yet connected").arg(BTManager.my_device.name)
                    else
                        text = qsTr("%1 not found. Maybe bluetooth is OFF").arg(BTManager.my_device.name)

                    BTNotifier.tooltip = text
                }

                onExited: BTNotifier.tooltip = ''
            }
        }
    }

    Label {
        text: qsTr("No device selected")
        anchors.top: my_device_picture.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 20
        color: theme.sidebarTextColor
        visible: !BTManager.my_device.name || BTManager.my_device.name.length === 0
    }

    Label {
        text: BTManager.my_device.name
        anchors.top: my_device_picture.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 20
        color: theme.sidebarTextColor
        visible: (BTManager.my_device && BTManager.my_device.name != undefined && BTManager.my_device.name.length > 0)
    }

    Column {
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        RadiusButton {
            size: 45
            source: 'images/up.svg'

            onClicked: fileDialog.visible = true
        }
    }          
}                   
    - name: import QtQuick 2.4
import QtQuick.Controls 1.2

Item {
    property string pageTitle: qsTr("Near by devices")

    Theme {
        id: theme
    }

    Item {
        id: controls

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 10

        height: 40

        RadiusButton {
            source: "images/up.svg"

            anchors.left: parent.left
            anchors.top: parent.top
            anchors.topMargin: 30

            transform: Rotation { angle: -90 }

            onClicked: {
                console.log('clicked back')
                stackView.back()
            }
        }

        RadiusButton {
            source: "images/search.svg"

            anchors.right: parent.right
            anchors.top: parent.top

            visible: !BTManager.discovering

            onClicked: {
                console.log('clicked search')
                BTManager.search()
            }
        }
    }

    TextEdit {
        id: sms_text_edit

        width: 140
        height: 50

        anchors.left: parent.left
        anchors.top: controls.bottom
        anchors.margins: 10
    }

    Button {
        anchors.top: sms_text_edit.bottom
        anchors.right: sms_text_edit.right
        onClicked: BTManager.send_sms(sms_text_edit.displayText, '060047856')
    }
}           
      run: import QtQuick 2.0

QtObject {
    property int titlePixelSize: 18
    property int subtitlePixelSize: 14
    property color hoverBackground: "lightgray"

    // header
    property color headerColor: '#2B2B2B'
    property color headerTextColor: 'white'

    // sidebar
    property color sidebarColor: '#313335'
    property color sidebarTextColor: 'white'

    // footer
    property color footerColor: '#2B2B2B'
    property color footerTextColor: 'white'
}                                           
