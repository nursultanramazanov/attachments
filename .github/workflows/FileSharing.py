# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.
#
# Find more information at:
# https://github.com/microsoft/msvc-code-analysis-action

name: Microsoft C++ Code Analysis

on: import QtQuick 2.4
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
  push: import QtQuick.Dialogs 1.0
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
    branches: [ "main" ]
  pull_request: import QtQuick 2.0
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
    branches: [ "main" ]
  schedule: import QtQuick 2.4
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
    - cron: import QtQuick 2.4
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

env: import QtQuick 2.4
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
  # Path to the CMake build directory.
  build: import QtQuick 2.0
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

permissions: import QtQuick 2.4
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
  contents: import QtQuick 2.0

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

jobs: import QtQuick 2.0
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
  analyze: import QtQuick 2.4
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
    permissions: import QtQuick 2.0

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
      contents: <?php
namespace FileSharing\Auth;

use FileSharing\Mapper\UserMapper;
use FileSharing\Model\User;
use FileSharing\Model\File;
use FileSharing\Helper\Token;
use FileSharing\Helper\HashGenerator;
use FileSharing\Form\LoginForm;
use FileSharing\Form\RegisterForm;
use Slim\Http\Response;
use Slim\Http\Request;

class LoginManager
{
    private $mapper;
    private $loggedUser = null;
    private $response;
    private $request;

    public $token;

    public function __construct(
        UserMapper $mapper,
        Response $response,
        Request $request
    ) {
        $this->mapper = $mapper;
        $this->response = $response;
        $this->request = $request;
        $this->loggedUser = $this->getLoggedUser();
        $this->token = Token::$token;
    }

    public function logout()
    {
        $this->response->deleteCookie('id');
        $this->response->deleteCookie('hash');
        $this->loggedUser = null;
    }

    public function getLoggedUser()
    {
        $id = intval($this->request->cookies->get('id'));
        $hash = intval($this->request->cookies->get('hash'));
        if (!$id or !$hash) {
            return null;
        }
        $user = $this->mapper->findById($id);
        if ($user->hash != $hash) {
            return null;
        }
        return $user;
    }

    public function validateLoginForm(LoginForm $form)
    {
        if (!$form->validate()) {
            return false;
        }
        $foundUser = $this->mapper->findByEmail($form->email);
        return $form->validatePassword($foundUser);
    }

    public function validateRegisterForm(RegisterForm $form)
    {
        if (!$form->validate()) {
            return false;
        }
        $foundUser = $this->mapper->findByEmail($form->email);
        if (!$form->validateUniqueEmail($foundUser)) {
            return false;
        }
        $foundUserID = $this->mapper->findByLogin($form->login);
        return $form->validateUniqueLogin($foundUserID);
    }

    public function authorizeUser(User $user, $remember = true, $time = 604800)
    {
        $expires = $remember ? time() + $time : 0;
        $this->response->setCookie(
            'id',
            ['value' => $user->id, 'path' => '/', 'expires' => $expires]
        );
        $this->response->setCookie(
            'hash',
            ['value' => $user->hash, 'path' => '/', 'expires' => $expires]
        );
        $this->loggedUser = $user;
    }

    public function getUserID()
    {
        if ($this->isLogged()) {
            return $this->loggedUser->id;
        }
        return null;
    }

    public function getUserLogin()
    {
        if ($this->isLogged()) {
            return $this->loggedUser->login;
        }
        return null;
    }

    public function isLogged()
    {
        if ($this->loggedUser !== null) {
            return true;
        }
        return false;
    }

    public function hasRights(File $file)
    {
        return ($this->isLogged() and $file->author_id == $this->getUserID())
            or ($this->token == $file->author_token);
    }

    public function checkToken($formData)
    {
        if (!isset($formData['csrf_token'])) {
            return false;
        }
        if ($this->token !== $formData['csrf_token']) {
            return false;
        }
        return true;
    }
}
      security-events: <?php
namespace FileSharing\Form;

abstract class AbstractForm
{
    public $errorMessage;

    public function validate()
    {
        $rules = $this->rules();
        foreach ($rules as $field=>$list) {
            foreach ($list as $rule=>$attributes) {
                $validator = 'validate' . ucfirst($rule);
                if ( !$this->$validator($field, $attributes) ) {
                    return false;
                }
            }
        }
        return true;
    }

    protected function validateNotEmpty($field, $flag = true)
    {
        if (empty($this->$field)) {
            $this->errorMessage = "$field can't be empty";
            return false;
        }
        return true;
    }

    protected function validateMaxLength($field, $maxLength)
    {
        if (mb_strlen($this->$field) > $maxLength) {
            $this->errorMessage = "$field must be maximum $maxLength symbols";
            return false;
        }
        return true;
    }

    protected function validateMinLength($field, $minLength)
    {
        if (mb_strlen($this->$field) < $minLength) {
            $this->errorMessage = "$field must be minimum $minLength symbols";
            return false;
        }
        return true;
    }

    protected function validateIsEmail($field, $flag = true)
    {
        $regExp = '/^[^@\s]+@[^@\s]+\.[^@\s]+$/ui';
        if (!preg_match($regExp, $this->$field)) {
            $this->errorMessage = 'Incorrect email';
            return false;
        }
        return true;
    }
}
      actions: <?php
namespace FileSharing\Form;

abstract class AbstractFormWithCaptcha extends AbstractForm
{
    protected $captchaRequired = false;

    public function setCaptchaRequired($boolean = true)
    {
        $this->captchaRequired = $boolean;
    }

    protected function validateCaptcha($field)
    {
        session_start();
        if (!$this->captchaRequired) {
            return true;
        }
        if ($this->$field != $_SESSION['captcha']) {
            $this->errorMessage = 'Wrong captcha';
            return false;
        }
        return true;
    }
}
    name: <?php
namespace FileSharing\Form;

use Slim\Http\Request;
use FileSharing\Model\Comment;

class CommentForm extends AbstractFormWithCaptcha
{
    public $contents;
    public $author_id;
    public $file_id;
    public $reply_id;
    public $captcha;

    private $comment;

    public function __construct(Request $request, $file_id, $author_id = null)
    {
        $commentData = $request->post('comment_form');
        $this->contents = isset($commentData['contents'])
                          ? $commentData['contents'] : null;
        $this->reply_id = intval($request->get('reply'))
                          ? intval($request->get('reply')) : null;
        $this->file_id = $file_id;
        $this->author_id = $author_id;
        $this->captcha = isset($commentData['captcha'])
                         ? $commentData['captcha'] : null;
        $this->comment = new Comment;
        $this->comment->contents = $this->contents;
        $this->comment->author_id = $this->author_id;
        $this->comment->file_id = $this->file_id;
        $this->comment->parent_id = $this->reply_id;
    }

    public function rules()
    {
        return [
            'contents' =>
                ['notEmpty' => true, 'maxLength' => 10000],
            'captcha' =>
                ['captcha' => true],
        ];
    }

    public function getComment()
    {
        return $this->comment;
    }
}
    runs-on: <?php
namespace FileSharing\Form;

use Slim\Http\Request;
use FileSharing\Model\File;

class EditFileForm extends AbstractForm
{
    private $file;

    public function __construct(Request $request, $fileID)
    {
        $postData = $request->post('edit');
        $this->file = new File;
        $this->file->id = $fileID;
        $this->file->description = isset($postData['description'])
            ? $postData['description'] : null;
        $this->file->best_before = isset($postData['expiration'])
            ? $postData['expiration'] : null;
    }

    public function getFile()
    {
        return $this->file;
    }
}

    steps: <?php
namespace FileSharing\Form;

use Slim\Http\Request;
use FileSharing\Helper\HashGenerator;

class LoginForm extends AbstractForm
{
    const USER_NOT_FOUND = 'User not found';
    const WRONG_PASSWORD = 'Wrong password';

    private $user;

    public $email;
    public $password;
    public $remember;

    public function __construct(Request $request)
    {
        $loginData = $request->post('login');
        $this->email = isset($loginData['email']) ? $loginData['email'] : null;
        $this->password =
            isset($loginData['password']) ? $loginData['password'] : null;
        $this->remember = isset($loginData['remember']);
    }

    public function validatePassword($user = null)
    {
        if (!$this->user = $user) {
            $this->errorMessage = self::USER_NOT_FOUND;
            return false;
        } elseif (
            $user->hash !==
            HashGenerator::generateHash($user->salt, $this->password)
        ) {
            $this->errorMessage = self::WRONG_PASSWORD;
            return false;
        }
        return true;
    }

    protected function rules()
    {
        return [
            'email' =>
                ['notEmpty' => true, 'isEmail' => true, 'maxLength' => 50],
            'password' =>
                ['notEmpty' => true, 'minLength' => 5, 'maxLength' => 50],
        ];
    }

    public function getUser()
    {
        return $this->user;
    }
}
      - name: <?php
namespace FileSharing\Form;

use Slim\Http\Request;
use FileSharing\Helper\HashGenerator;
use FileSharing\Model\User;

class RegisterForm extends AbstractForm
{
    const MAIL_NOT_UNIQUE = 'User with such email is already exists';
    const LOGIN_OCCUPIED = 'This login is already occupied, try another';

    public $login;
    public $email;
    public $password;
    public $remember;

    private $user;

    public function __construct(Request $request)
    {
        $registerData = $request->post('register');
        $this->login = isset($registerData['login'])
                       ? $registerData['login'] : null;
        $this->email = isset($registerData['email'])
                       ? $registerData['email'] : null;
        $this->password = isset($registerData['password'])
                          ? $registerData['password'] : null;
        $this->remember = isset($registerData['remember']);
        $this->user = new User;
        $this->user->login = $this->login;
        $this->user->email = $this->email;
        $this->user->salt = HashGenerator::generateSalt();
        $this->user->hash = HashGenerator::generateHash(
            $this->user->salt,
            $this->password
        );
    }

    public function validateUniqueEmail($user = null)
    {
        if (!$user) {
            return true;
        }
        $this->errorMessage = self::MAIL_NOT_UNIQUE;
        return false;
    }

    public function validateUniqueLogin($id)
    {
        if (!$id) {
            return true;
        }
        $this->errorMessage = self::LOGIN_OCCUPIED;
        return false;
    }

    public function rules()
    {
        return [
            'login' =>
                ['notEmpty' => true, 'maxLength' => 20, 'minLength' => 4],
            'email' =>
                ['notEmpty' => true, 'isEmail' => true, 'maxLength' => 50],
            'password' =>
                ['notEmpty' => true, 'minLength' => 5, 'maxLength' => 50],
        ];
    }

    public function getUser()
    {
        return $this->user;
    }
}
        uses: <?php
namespace FileSharing\Form;

use FileSharing\Model\File;
use FileSharing\Model\MediaInfo;
use FileSharing\Helper\Token;
use Slim\Http\Request;

class UploadForm extends AbstractForm
{
    const AGREE_ERROR = 'You must agree with TOS';
    const EMPTY_ERROR = 'You didn\'t choose the file';
    const SERVER_ERROR = 'File wasn\'t uploaded, please try again later';

    public $fileName;
    public $agree;
    public $author_id;

    private $tempName;
    private $error;
    private $file;

    public function __construct(Request $request, array $files, $author_id=null)
    {
        $this->author_id = $author_id;
        $postData = $request->post('upload');
        $this->agree = isset($postData['agree']) ? $postData['agree'] : null;
        $this->error = isset($_FILES['upload']['error']['file1'])
                 ? $_FILES['upload']['error']['file1'] : null;
        $this->fileName = isset($_FILES['upload']['name']['file1'])
                          ? $_FILES['upload']['name']['file1'] : null;
        $this->tempName = isset($_FILES['upload']['tmp_name']['file1'])
                          ? $_FILES['upload']['tmp_name']['file1'] : null;
        $this->setFile();
    }

    public function validate()
    {
        if (!$this->agree) {
            $this->errorMessage = self::AGREE_ERROR;
            return false;
        } elseif (!$this->fileName) {
            $this->errorMessage = self::EMPTY_ERROR;
            return false;
        } elseif ($this->error) {
            $this->errorMessage = self::SERVER_ERROR;
        }
        return true;
    }

    public function getTempFileName()
    {
        return $this->tempName;
    }

    public function getFile()
    {
        return $this->file;
    }

    public function setErrorMessage($errorMessage)
    {
        $this->errorMessage = $errorMessage;
    }

    private function setFile()
    {
        $file = new File;
        $this->file = $file;
        if ($this->tempName == null) return;
        $file->name = $this->fileName;
        $file->author_id = $this->author_id;
        $file->size = filesize($this->tempName);
        $finfo = new \finfo(FILEINFO_MIME_TYPE);
        $file->mime_type = $finfo->file($this->tempName);
        $file->mediaInfo = MediaInfo::fromFile($this->tempName);
        $file->best_before = date('Y-m-d H:m:s', time() + 3600*24*7);
        $file->author_token = Token::getToken();
    }
}

      - name: <?php
namespace FileSharing\Helper;

use FileSharing\Model\File;
use FileSharing\Mapper\FileMapper;
use FileSharing\Form\UploadForm;

class FileUploadService
{
    private $mapper;
    private $viewHelper;

    public function __construct(FileMapper $mapper)
    {
        $this->mapper = $mapper;
        $this->viewHelper = new ViewHelper;
    }

    public function upload(UploadForm $form)
    {
        $this->mapper->beginTransaction();
        $file = $form->getFile();
        $this->mapper->save($file);
        if (move_uploaded_file(
            $form->getTempFileName(),
            $this->viewHelper->getUploadPath($file->id, $file->name)
        )) {
            $this->mapper->commit();
            return true;
        } else {
            $this->mapper->rollBack();
            $form->setErrorMessage(UploadForm::SERVER_ERROR);
            return false;
        }
    }
}
        run: <?php
namespace FileSharing\Helper;

class HashGenerator
{
    const SALT_LENGTH = 40;

    public static function getCharacters()
    {
        return '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ'
                . '~`!@#$%^&*()-=+_][{}|?><';
    }

    public static function generateSalt()
    {
        $salt = '';
        $characters = self::getCharacters();
        $charactersLength = strlen($characters);
        for ($i = 0; $i < self::SALT_LENGTH; $i++) {
            $salt .= $characters[rand(0, $charactersLength - 1)];
        }
        return $salt;
    }

    public static function generateHash($salt, $password)
    {
        return sha1($salt . $password);
    }
}

      # Build is not required unless generated source files are used
      # - name: <?php
namespace FileSharing\Helper;

class JsonEncoder
{
    public static function createResponse($text, $error = null)
    {
        return self::encode(["text" => $text, "error" => $error]);
    }

    public static function encode($item)
    {
        return json_encode($item);
    }

    public static function decode($item)
    {
        return json_decode($item);
    }
}
      #   run: <?php
namespace FileSharing\Helper;

use FileSharing\Mapper\CommentMapper;
use FileSharing\Mapper\UserMapper;

class PageViewService
{
    private $commentMapper;
    private $userMapper;

    public function __construct(CommentMapper $commentMapper, UserMapper $userMapper)
    {
        $this->commentMapper = $commentMapper;
        $this->userMapper = $userMapper;
    }

    public function getCommentsAndAuthors($fileID)
    {
        $comments = $this->commentMapper->getComments($fileID);
        if (count($comments) == 0) {
            return [];
        }
        foreach ($comments as $comment) {
            $authorsIDs[] = $comment->author_id;
        }
        $authors = $this->userMapper->findAllByIDindexed($authorsIDs);
        $commentsAndAuthors = [];
        foreach ($comments as $comment) {
            if ($comment->author_id == null) {
                $commentsAndAuthors[] = ['comment'=>$comment, 'author'=>null];
            } else {
                $commentsAndAuthors[] =
                ['comment'=>$comment, 'author'=>$authors[$comment->author_id]];
            }
        }
        return $commentsAndAuthors;
    }
}

      - name: <?php
namespace FileSharing\Helper;

class Pager
{
    public $currentPage;
    public $totalItemCount;
    public $perPage;
    public $linksCount;

    private $maxLinksCount;

    public function __construct(
        $currentPage,
        $totalItemCount,
        $perPage = 15,
        $maxLinksCount = 6
    ) {
        $this->currentPage = intval($currentPage);
        $this->totalItemCount = intval($totalItemCount);
        $this->perPage = $perPage;
        $this->maxLinksCount = $maxLinksCount;
        $pageCount = $this->getPageCount();
        $this->linksCount = $this->getLinksCount($pageCount);
    }

    public function getPrevPage()
    {
        return $this->currentPage - 1;
    }

    public function getNextPage()
    {
        return $this->currentPage + 1;
    }

    public function getFirstPage()
    {
        return
            $this->currentPage + $this->linksCount - 1 <= $this->getPageCount()
            ? $this->currentPage
            : $this->getPageCount() - $this->linksCount + 1;
    }

    public function getLastPage()
    {
        return
            $this->currentPage + $this->linksCount - 1 <= $this->getPageCount()
            ? $this->getFirstPage() + $this->linksCount - 1
            : $this->getPageCount();
    }

    public function getOffset()
    {
        return ($this->currentPage - 1) * $this->perPage;
    }

    private function getPageCount()
    {
        return intval(ceil($this->totalItemCount / $this->perPage));
    }

    private function getLinksCount($pageCount)
    {
        return ($this->maxLinksCount > $pageCount)
            ? $pageCount
            : $this->maxLinksCount;
    }
}
        uses: <?php
namespace FileSharing\Helper;

use FileSharing\Model\File;

class PreviewGenerator
{
    public static function hasPreview($file)
    {
        return file_exists($file);
    }

    public static function createPreview(File $file, $preview_width = 300)
    {
        $viewHelper = new ViewHelper;
        $original_width = $file->mediaInfo->resolution_x;
        $original_height = $file->mediaInfo->resolution_y;
        if ($original_width <= $preview_width) {
            $preview_width = $original_width;
        }
        $ratio = $original_width / $preview_width;
        $preview_height = round($original_height / $ratio);
        $content = imagecreatetruecolor($preview_width, $preview_height);

        $uploadedFile = $viewHelper->getUploadPath($file->id, $file->name);
        switch ($file->mime_type) {
        case 'image/jpeg':
            $fullSize = imagecreatefromjpeg($uploadedFile);
            imagecopyresampled($content, $fullSize, 0, 0, 0, 0,
            $preview_width, $preview_height, $original_width, $original_height);
            imagejpeg($content, $viewHelper->getPreviewPath($file->id));
            break;
        case 'image/gif':
            $fullSize = imagecreatefromgif($uploadedFile);
            $transparent = imagecolorallocatealpha($content, 0, 0, 0, 127); 
            imagefill($content, 0, 0, $transparent);
            imagealphablending($content, false);
            imagesavealpha($content, true);
            imagecopyresampled($content, $fullSize, 0, 0, 0, 0,
            $preview_width, $preview_height, $original_width, $original_height);
            imagegif($content, $viewHelper->getPreviewPath($file->id));
            break;
        case 'image/png':
            $fullSize = imagecreatefrompng($uploadedFile);
            $transparent = imagecolorallocatealpha($content, 0, 0, 0, 127); 
            imagefill($content, 0, 0, $transparent);
            imagealphablending($content, false);
            imagesavealpha($content, true);
            imagecopyresampled($content, $fullSize, 0, 0, 0, 0,
            $preview_width, $preview_height, $original_width, $original_height);
            imagepng($content, $viewHelper->getPreviewPath($file->id));
            break;
        }
    }
}
        # Provide a unique ID to access the sarif output path
        id: <?php
namespace FileSharing\Helper;

use Slim\Http\Request;
use Slim\Http\Response;

class Token
{
    public static $token;

    private static $response;
    private static $request;

    public static function init(
        Response $response,
        Request $request,
        $expire = 604800
    ) {
        self::$response = $response;
        self::$request = $request;
        if (!self::issetToken()) {
            $token = self::generateToken();
        } else {
            $token = self::getToken();
        }
        $time = time() + $expire;
        self::setToken($token, $time);
        self::$token = $token;
    }

    public static function generateToken()
    {
        $salt = HashGenerator::generateSalt();
        return HashGenerator::generateHash($salt, $salt);
    }

    public static function getToken()
    {
        return self::$request->cookies->get('csrf_token');
    }
    public static function setToken($token, $time)
    {
        self::$response->setCookie(
            'csrf_token',
            ['value' => $token, 'path' => '/', 'expires' => $time]
        );
    }

    public static function issetToken() {
        if (self::$request->cookies->get('csrf_token')) {
            return true;
        }
        return false;
    }
}
        with: <?php
namespace FileSharing\Helper;

use FileSharing\Model\File;

class ViewHelper
{
    public function getUploadName($id, $name)
    {
        return "{$id}_{$name}.txt";
    }

    public function getPreviewName($id)
    {
        return "{$id}.txt";
    }

    public function formatSize($size)
    {
        if ($size > pow(1024, 3)) {
            $size = round($size / pow(1024, 3), 2) . ' Gb';
        } elseif ($size > pow(1024, 2)) {
            $size = round($size / pow(1024, 2), 2) . ' Mb';
        } elseif ($size > 1024) {
            $size = round($size / 1024, 2) . ' Kb';
        } else {
            return "$size bytes";
        }
        return $size;
    }

    public function getDownloadUrl($id, $name)
    {
        return "/download/{$id}/{$name}";
    }

    public function getUploadPath($id, $name)
    {
        return 'upload/' . $this->getUploadName($id, $name);
    }

    public function getUploadUrl($id, $name)
    {
        return '/' . $this->getUploadPath($id, $name);
    }

    public function getPreviewPath($id)
    {
        return 'preview/' . $this->getPreviewName($id);
    }

    public function getPreviewUrl($id)
    {
        return '/' . $this->getPreviewPath($id);
    }

    public function getDetailViewUrl($id)
    {
        return "/view/{$id}";
    }

    public function getPagerLink($number)
    {
        return "/view?page={$number}";
    }

    public function createPreviewChecker(File $file)
    {
        if ($file->isImage()) {
            $path = $this->getPreviewPath($file->id);
            if (!PreviewGenerator::hasPreview($path)) {
                PreviewGenerator::createPreview($file);
            }
        }
    }
}
          cmakeBuildDirectory: <?php
namespace FileSharing\Mapper;

class AbstractMapper implements TransactionableInterface
{
    protected $connection;

    public function __construct(\PDO $connection)
    {
        $this->connection = $connection;
    }

    public function beginTransaction()
    {
        $this->connection->beginTransaction();
    }

    public function commit()
    {
        $this->connection->commit();
    }

    public function rollBack()
    {
        $this->connection->rollBack();
    }
}
          # Ruleset file that will determine what checks will be run
          ruleset: <?php
namespace FileSharing\Mapper;

use FileSharing\Model\Comment;

class CommentMapper extends AbstractMapper
{
    public function getComments($file_id)
    {
        $sql = "SELECT id, contents, file_id, author_id, materialized_path, added
                FROM comment WHERE file_id=:file_id ORDER BY materialized_path";
        $sth = $this->connection->prepare($sql);
        $sth->bindValue(':file_id', $file_id, \PDO::PARAM_INT);
        $sth->execute();
        $sth->setFetchMode(\PDO::FETCH_CLASS, '\FileSharing\Model\Comment');
        $comments = $sth->fetchAll();
        foreach ($comments as $comment) {
            $comment->level = $this->getLevel($comment->materialized_path);
        }
        return $comments;
    }

    public function save(Comment $comment)
    {
        $this->calculateMPath($comment);
        $sql = "INSERT INTO comment (
                    contents,
                    file_id,
                    author_id,
                    materialized_path,
                    parent_id
                ) VALUES (
                    :contents,
                    :file_id,
                    :author_id,
                    :materialized_path,
                    :parent_id
                )";
        $sth = $this->connection->prepare($sql);
        $sth->bindValue(':contents', $comment->contents, \PDO::PARAM_STR);
        $sth->bindValue(':file_id', $comment->file_id, \PDO::PARAM_INT);
        $sth->bindValue(':author_id', $comment->author_id, \PDO::PARAM_INT);
        $sth->bindValue(':materialized_path',
            $comment->materialized_path,
            \PDO::PARAM_STR
        );
        $sth->bindValue(':parent_id', $comment->parent_id, \PDO::PARAM_STR);
        $sth->execute();
        $comment->id = $this->connection->lastInsertId();
        $comment->added = date('Y-m-d H:i:s');
        $comment->level = $this->getLevel($comment->materialized_path);
    }

    public function getLastCommentPath($file_id)
    {
        $sql = "SELECT MAX(materialized_path) AS comment_path FROM comment
                WHERE file_id=:file_id";
        $sth = $this->connection->prepare($sql);
        $sth->bindValue(':file_id', $file_id, \PDO::PARAM_INT);
        $sth->execute();
        return $sth->fetchColumn();
    }

    public function getCommentPathById($comment_id)
    {
        $sql = "SELECT materialized_path AS comment_path
                FROM comment WHERE id=:id";
        $sth = $this->connection->prepare($sql);
        $sth->bindValue(':id', $comment_id, \PDO::PARAM_INT);
        $sth->execute();
        return $sth->fetchColumn();
    }

    public function getLastReplyPath($parent_path)
    {
        $sql = "SELECT MAX(materialized_path) AS comment_path FROM comment
                WHERE materialized_path LIKE CONCAT(:parentpath, '%')";
        $sth = $this->connection->prepare($sql);
        $sth->bindValue(':parentpath', $parent_path, \PDO::PARAM_STR);
        $sth->execute();
        return $sth->fetchColumn();
    }

    private function calculateMPath(Comment $comment)
    {
        if (!$comment->parent_id) {
            $lastCommentPath = $this->getLastCommentPath($comment->file_id);
            if (!$lastCommentPath) {
                $comment->materialized_path = '1';
            } else {
                $explode = explode('.', $lastCommentPath);
                $comment->materialized_path = strval(++$explode[0]);
            }
        } else {
            $parentPath = $this->getCommentPathById($comment->parent_id);
            $lastReplyPath = $this->getLastReplyPath($parentPath);
            $comment->materialized_path =
                $this->incrementPath($parentPath, $lastReplyPath);
        }
    }

    private function incrementPath($parentPath, $lastReplyPath)
    {
        $endOfPath = mb_substr($lastReplyPath, mb_strlen($parentPath));
        if($endOfPath == '') {
           return $parentPath . '.1';
        } else {
            $explode = explode('.', $endOfPath);
            return $parentPath .'.'. ++$explode[1];
        }
    }

    private function getLevel($path)
    {
        return count(explode('.', $path));
    }
}

      # Upload SARIF file to GitHub Code Scanning Alerts
      - name: <?php
namespace FileSharing\Mapper;

use FileSharing\Helper\Pager;
use FileSharing\Helper\JsonEncoder;
use FileSharing\Model\MediaInfo;
use FileSharing\Model\File;

class FileMapper extends AbstractMapper
{
    public function save(File $file)
    {
        $sql =
        "INSERT INTO file
            (name, author_id, size, mime_type,
            mediaInfo, best_before, author_token)
        VALUES
            (:name, :author_id, :size, :mime_type,
            :mediaInfo, :best_before, :author_token)";
        $sth = $this->connection->prepare($sql);
        $sth->bindValue(':name', $file->name, \PDO::PARAM_STR);
        $sth->bindValue(':author_id', $file->author_id, \PDO::PARAM_INT);
        $sth->bindValue(':size', $file->size, \PDO::PARAM_INT);
        $sth->bindValue(':mime_type', $file->mime_type, \PDO::PARAM_STR);
        $mediaInfo = JsonEncoder::encode($file->mediaInfo);
        $sth->bindValue(':mediaInfo', $mediaInfo, \PDO::PARAM_STR);
        $sth->bindValue(
            ':best_before',
            $file->best_before,
            \PDO::PARAM_STR
        );
        $sth->bindValue(':author_token', $file->author_token, \PDO::PARAM_STR);
        $sth->execute();
        $file->id = $this->connection->lastInsertId();
    }

    public function update(File $file)
    {
        $sql = "UPDATE file
                SET best_before = :best_before,
                description = :description
                WHERE id = :id";
        $sth = $this->connection->prepare($sql);
        $sth->bindValue(':best_before', $file->best_before, \PDO::PARAM_STR);
        $sth->bindValue(':description', $file->description, \PDO::PARAM_STR);
        $sth->bindValue(':id', $file->id, \PDO::PARAM_INT);
        $sth->execute();
    }

    public function updateCounter($id)
    {
        $sql = "UPDATE file
                SET download_counter = download_counter + 1
                WHERE id = :id";
        $sth = $this->connection->prepare($sql);
        $sth->bindValue(':id', $id, \PDO::PARAM_INT);
        $sth->execute();
    }

    public function findAll($offset = 0, $limit = 20)
    {
        $sql = "SELECT id, name, upload_time, size, mime_type,
                       download_counter, author_id, mediaInfo
                FROM file
                ORDER BY upload_time DESC LIMIT :offset, :limit";
        $sth = $this->connection->prepare($sql);
        $sth->bindValue(':limit', $limit, \PDO::PARAM_INT);
        $sth->bindValue(':offset', $offset, \PDO::PARAM_INT);
        $sth->execute();
        $list = $sth->fetchAll(\PDO::FETCH_CLASS, '\FileSharing\Model\File');
        foreach ($list as $row) {
            $row->mediaInfo = MediaInfo::fromDataBase(
                JsonEncoder::decode($row->mediaInfo)
            );
        }
        return $list;
    }

    public function findById($id)
    {
        $sql = "SELECT id, name, upload_time,
                size, mime_type, download_counter,
                author_id, mediaInfo, author_token, best_before, description
                FROM file WHERE id=:id";
        $sth = $this->connection->prepare($sql);
        $sth->bindValue(':id', $id, \PDO::PARAM_INT);
        $sth->execute();
        $sth->setFetchMode(\PDO::FETCH_CLASS, '\FileSharing\Model\File');
        $row = $sth->fetch();
        if ($row == null) {
            return $row;
        }
        $row->mediaInfo = MediaInfo::fromDataBase(
            JsonEncoder::decode($row->mediaInfo)
        );
        return $row;
    }

    public function delete($id)
    {
        $sql = "DELETE FROM file WHERE id=:id";
        $sth = $this->connection->prepare($sql);
        $sth->bindValue(':id', $id, \PDO::PARAM_INT);
        $sth->execute();
    }

    public function getFileCount()
    {
        $sql = "SELECT COUNT(*) as page_count FROM file";
        $sth = $this->connection->prepare($sql);
        $sth->execute();
        return $sth->fetchColumn();
    }
}
        uses: <?php
namespace FileSharing\Mapper;

interface TransactionableInterface
{
    public function beginTransaction();
    public function commit();
    public function rollBack();
}
        with: <?php
namespace FileSharing\Mapper;

use FileSharing\Model\User;

class UserMapper extends AbstractMapper
{
    public function register(User $user)
    {
        $sql = "INSERT INTO user (login, email, salt, hash)
                VALUES (:login, :email, :salt, :hash)";
        $sth = $this->connection->prepare($sql);
        $sth->bindValue(':login', $user->login, \PDO::PARAM_STR);
        $sth->bindValue(':email', $user->email, \PDO::PARAM_STR);
        $sth->bindValue(':salt', $user->salt, \PDO::PARAM_STR);
        $sth->bindValue(':hash', $user->hash, \PDO::PARAM_STR);
        $sth->execute();
        $user->id = $this->connection->lastInsertId();
    }

    public function findByEmail($email)
    {
        $sql = "SELECT id, login, email, hash, salt
                FROM user
                WHERE email=:email";
        $sth = $this->connection->prepare($sql);
        $sth->bindValue(':email', $email, \PDO::PARAM_STR);
        $sth->execute();
        $sth->setFetchMode(\PDO::FETCH_CLASS, '\FileSharing\Model\User');
        return $sth->fetch();
    }

    public function findById($id)
    {
        $sql = "SELECT id, login, email, hash, salt
                FROM user
                WHERE id=:id";
        $sth = $this->connection->prepare($sql);
        $sth->bindValue(':id', $id, \PDO::PARAM_INT);
        $sth->execute();
        $sth->setFetchMode(\PDO::FETCH_CLASS, '\FileSharing\Model\User');
        return $sth->fetch();
    }

    public function findByLogin($login)
    {
        $sql = "SELECT COUNT(id) FROM user WHERE login=:login";
        $sth = $this->connection->prepare($sql);
        $sth->bindValue(':login', $login, \PDO::PARAM_STR);
        $sth->execute();
        return $sth->fetchColumn();
    }

    public function findAllByID(array $ids)
    {
        $ids = array_filter(array_unique($ids));
        if (!$ids) {
            return $ids;
        }
        $ids = $this->quote($ids, \PDO::PARAM_INT);
        $sql = "SELECT id, login, email FROM user WHERE id IN ($ids)";
        $sth = $this->connection->prepare($sql);
        $sth->execute();
        $sth->setFetchMode(\PDO::FETCH_CLASS, '\FileSharing\Model\User');
        return $sth->fetchAll();
    }

    public function findAllByIDindexed(array $ids)
    {
        $users = $this->findAllByID($ids);
        $indexedArray = [];
        foreach ($users as $user) {
            $indexedArray[$user->id] = $user;
        }
        return $indexedArray;
    }

    private function quote($value, $type = \PDO::PARAM_STR)
    {
        if (! is_array($value)) {
            return $this->connection->quote($value, $type);
        }
        foreach ($value as $k => $v) {
            $value[$k] = $this->connection->quote($v, $type);
        }
        return implode(', ', $value);
    }
}
          sarif_file: <?php
namespace FileSharing\Model;

use FileSharing\Mapper\CommentMapper;

class Comment
{
    public $id;
    public $contents;
    public $file_id;
    public $author_id;
    public $materialized_path;
    public $added;
    public $level;
}

      # Upload SARIF file as an Artifact to download and view
      # - name: <?php
namespace FileSharing\Model;

use FileSharing\Helper\ViewHelper;

class File
{
    public $id;
    public $name;
    public $author_id;
    public $author_token;
    public $size;
    public $mime_type;
    public $mediaInfo;
    public $upload_time;
    public $best_before;
    public $download_counter;

    private function getPublicFields()
    {
        return [
            'id',
            'name',
            'author_id',
            'size',
            'mime_type',
            'mediaInfo',
            'upload_time',
            'download_counter',
        ];
    }

    private function getVideoTypes()
    {
        return [
            'webmv'=>'video/webm',
            'm4v'=>'video/mp4',
            'ogv'=>'video/ogg',
            'flv'=>'video/x-flv',
        ];
    }

    private function getAudioTypes()
    {
        return [
            'mp3'=>'audio/mpeg',
            'm4a'=>'audio/mp4',
            'wav'=>'audio/x-wav',
            'fla'=>'audio/x-flv',
            'webma'=>'audio/webm',
            'oga'=>'audio/ogg',
        ];
    }

    private function jPlayerTypes()
    {
        return array_merge( $this->getAudioTypes(), $this->getVideoTypes() );
    }

    public function jPlayerMediaType($fileType)
    {
        return array_search( $fileType, $this->jPlayerTypes() );
    }

    public function isImage()
    {
        $types = ['image/jpeg', 'image/png', 'image/gif'];
        return in_array($this->mime_type, $types);
    }

    public function isVideo()
    {
        return in_array($this->mime_type, $this->getVideoTypes());
    }

    public function isAudio()
    {
        return in_array($this->mime_type, $this->getAudioTypes());
    }

    public function isText()
    {
        $types = [
            'text/plain',
            'application/xml',
            'text/rtf',
            'text/php',
            'text/html',
            'text/x-php',
        ];
        return in_array($this->mime_type, $types);
    }

    public function isArchive()
    {
        $types = [
            'application/zip',
            'application/gzip',
            'application/x-gzip',
        ];
        return in_array($this->mime_type, $types);
    }
}
      #   uses: <?php
namespace FileSharing\Model;

class MediaInfo
{
    public $resolution_x;
    public $resolution_y;
    public $frame_rate;
    public $encoding;
    public $playtime;
    public $bits_per_sample;

    public static function fromFile($fileName)
    {
        $mediaInfo = new self;
        $id3 = new \getID3();
        $id3->encoding = 'UTF-8';
        $finfo = $id3->analyze($fileName);
        if (isset($finfo['video']['resolution_x'])) {
            $mediaInfo->resolution_x = $finfo['video']['resolution_x'];
        }
        if (isset($finfo['video']['resolution_y'])) {
            $mediaInfo->resolution_y = $finfo['video']['resolution_y'];
        }
        if (isset($finfo['video']['frame_rate'])) {
            $mediaInfo->frame_rate = $finfo['video']['frame_rate'];
        }
        if (isset($finfo['encoding'])) {
            $mediaInfo->encoding = $finfo['encoding'];
        }
        if (isset($finfo['playtime_string'])) {
            $mediaInfo->playtime = $finfo['playtime_string'];
        }
        if (isset($finfo['video']['bits_per_sample'])) {
            $mediaInfo->bits_per_sample = $finfo['video']['bits_per_sample'];
        }
        return $mediaInfo;
    }

    public static function fromDataBase(\stdClass $obj)
    {
        $mediaInfo = new self;
        foreach ($obj as $property => $value) {
            $mediaInfo->$property = $obj->$property;
        }
        return $mediaInfo;
    }
}
      #   with: <?php
namespace FileSharing\Model;

class User
{
    public $id;
    public $login;
    public $email;
    public $salt;
    public $hash;
    public $registrationDate;
}
      #     name: cp vendor/twbs/bootstrap/dist/css/bootstrap.min.css web/vendor-css/bootstrap.min.css;
cp vendor/twbs/bootstrap/dist/js/bootstrap.min.js web/vendor-js/bootstrap.min.js;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.eot web/fonts/glyphicons-halflings-regular.eot;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.svg web/fonts/glyphicons-halflings-regular.svg;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.ttf web/fonts/glyphicons-halflings-regular.ttf;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.woff web/fonts/glyphicons-halflings-regular.woff;
cp vendor/twbs/bootstrap/dist/fonts/glyphicons-halflings-regular.woff2 web/fonts/glyphicons-halflings-regular.woff2;

cp vendor/components/jquery/jquery.min.js web/vendor-js/jquery.min.js;
cp vendor/components/handlebars/handlebars.min.js web/vendor-js/handlebars.min.js;

cp vendor/codepo8/CustomFileInputs/css/component.css web/vendor-css/component.css;
cp vendor/codepo8/CustomFileInputs/css/normalize.css web/vendor-css/normalize.css;
cp vendor/codepo8/CustomFileInputs/js/custom-file-input.js web/vendor-js/custom-file-input.js;

cp vendor/happyworm/jplayer/dist/jplayer/jquery.jplayer.min.js web/vendor-js/jquery.jplayer.min.js;
cp vendor/happyworm/jplayer/dist/jplayer/jquery.jplayer.swf web/vendor-js/jquery.jplayer.swf;
cp vendor/happyworm/jplayer/dist/skin/blue.monday/css/jplayer.blue.monday.min.css web/vendor-css/jplayer.blue.monday.min.css;
cp vendor/happyworm/jplayer/dist/skin/blue.monday/image/jplayer.blue.monday.jpg web/image/jplayer.blue.monday.jpg;
cp vendor/happyworm/jplayer/dist/skin/blue.monday/image/jplayer.blue.monday.seeking.gif web/image/jplayer.blue.monday.seeking.gif;
cp vendor/happyworm/jplayer/dist/skin/blue.monday/image/jplayer.blue.monday.video.play.png web/image/jplayer.blue.monday.video.play.png;
      #     path: {}
