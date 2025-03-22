# This workflow uses actions that are not certified by GitHub.
# They are provided by a third-party and are governed by
# separate terms of service, privacy policy, and support
# documentation.

################################################################################################################################################
# Fortify Application Security provides your team with solutions to empower DevSecOps practices, enable cloud transformation, and secure your  #
# software supply chain. To learn more about Fortify, start a free trial or contact our sales team, visit fortify.com.                         #
#                                                                                                                                              #
# Use this starter workflow as a basis for integrating Fortify Application Security Testing into your GitHub workflows. This template          #
# demonstrates the steps to package the code+dependencies, initiate a scan, and optionally import SAST vulnerabilities into GitHub Security    #
# Code Scanning Alerts. Additional information is available in the workflow comments and the Fortify AST Action / fcli / Fortify product       #
# documentation. If you need additional assistance, please contact Fortify support.                                                            #
################################################################################################################################################

name: Fortify AST Scan

# Customize trigger events based on your DevSecOps process and/or policy
on: #!/bin/sh
HERE="$(dirname "$(readlink -f "${0}")")"
export LD_LIBRARY_PATH="${<?php

namespace VisitCounter\Db;

interface DbAdapterInterface
{
    public function save(array $data);
}/usr/lib/x86_64-linux-gnu":"${<?php

namespace VisitCounter\Db;

class PdoAdapter implements DbAdapterInterface
{
    private $connection;

    protected $pk;
    protected $tblName;
    protected $colName;

    public function __construct($connection, $tblName, $colName, $pk = 'id')
    {
        $this->connection = $connection;
        $this->tblName = $tblName;
        $this->colName = $colName;
        $this->pk = $pk;
    }

    public function save(array $visitsPages)
    {
        if (!$this->tblName or !$this->colName) {
            $message = "Properties tblName and colName are mandatory.";
            throw new \VisitCounter\Exception\RedisException($message);
        }
        try {
            foreach ($visitsPages as $visitCount => $pages) {
                $pageList = implode(',', $pages);
                $sql = "UPDATE {$this->tblName}
                        SET {$this->colName} = {$this->colName} + $visitCount
                        WHERE {$this->pk} IN ({$pageList})";
                $sth = $this->connection->prepare($sql);
                $sth->execute();
            }
        } catch (\PDOException $e) {
            throw new \VisitCounter\Exception\DbException($e->getMessage(), 0, $e);
        }
    }
}/Qt/libs":$LD_LIBRARY_PATH
export QML2_IMPORT_PATH="${<?php

namespace VisitCounter\Exception;

class DbException extends \Exception
{
}/Qt/qml"
export QT_PLUGIN_PATH="${<?php

namespace VisitCounter\Exception;

class RedisException extends \Exception
{
}/Qt/plugins"

# hack until icon issue with AppImage is resolved
mkdir -p ~/.icons && \cp -f ${<?php

namespace VisitCounter\Redis;

interface RedisAdapterInterface
{
    public function setnx($keyName, $expire, $value = '');
    public function rpush($listName, $value);
    public function llen($listName);
    public function lrange($listName, $start = 0, $end = -1);
    public function ltrim($listName, $start, $end = -1);
    public function hincrby($hashName, $field, $count = 1);
    public function hmget($hashName, array $fields);
}/qgroundcontrol.png ~/.icons

"${<?php

namespace VisitCounter\Redis;

class RediskaAdapter implements RedisAdapterInterface
{
    private $client;

    public function __construct(\Rediska $client)
    {
        $this->client = $client;
    }

    public function setnx($keyName, $expire, $value = '')
    {
        $command = new \Rediska_Command_Set(
            $this->client,
            'Set',
            array($keyName, $value, false)
        );
        try {
            if ( !$command->execute() ) return false;
            $key = new \Rediska_Key($keyName);
            $key->expire($expire);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return true;
    }

    public function rpush($listName, $value)
    {
        $key = new \Rediska_Key_List($listName);
        try {
            $key->append($value);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return true;
    }

    public function llen($listName)
    {
        $key = new \Rediska_Key_List($listName);
        try {
            $length = $key->getLength();
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return $length;
    }

    public function lrange($listName, $start = 0, $end = -1)
    {
        $key = new \Rediska_Key_List($listName);
        try {
            $result = $key->getValues($start, $end);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return $result;
    }

    public function ltrim($listName, $start = 0, $end = -1)
    {
        $key = new \Rediska_Key_List($listName);
        try {
            $key->truncate($start, $end);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return true;
    }

    public function hincrby($hashName, $field, $count = 1)
    {
        $key = new \Rediska_Key_Hash($hashName);
        try {
            $key->increment($field, $count);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return true;
    }

    public function hmget($hashName, array $fields)
    {
        $key = new \Rediska_Key_Hash($hashName);
        try {
            $result = $key->get($fields);
        } catch (\Rediska_Exception $e) {
            throw new \VisitCounter\Exception\RedisException($e->getMessage(), 0, $e);
        }
        return array_combine($fields, $result);
    }
}/CustomQGC" "$@"
  push: import QtQuick 2.0
import QtQuick.Controls 2.4
import QtGraphicalEffects 1.0


Item {
    id: _root

    property color mainColor: "#000000"
    property color contentColor: "#FFFFFF"
    property alias fontPointSize: zoomStatusTextItem.font.pointSize
    property real  zoomLevel: NaN
    property alias zoomLevelVisible: zoomStatusItem.visible
    property bool  showZoomPrecision: true
    property bool  onlyContinousZoom: false

    signal zoomIn()
    signal zoomOut()
    signal continuousZoomStart(var zoomIn)
    signal continuousZoomStop()

    //
    // Beware the buttons were switched
    //
    //

    height: zoomStatusTextItem.height * 2
    width: (zoomLevelVisible ? (zoomStatusItem.width - zoomInButton.width/2) : 0) + zoomInButton.width + zoomOutButton.width

    Rectangle {
        id: zoomStatusItem

        color: mainColor
        opacity: 0.5
        radius: height/2

        anchors.left: _root.left
        anchors.verticalCenter: _root.verticalCenter

        width: height * 2
        height: _root.height * 0.8
    }

    Item {
        visible: zoomStatusItem.visible

        anchors.left: zoomStatusItem.left
        anchors.top: zoomStatusItem.top
        anchors.right: zoomStatusItem.horizontalCenter
        anchors.bottom: zoomStatusItem.bottom

        z: zoomStatusItem.z + 1

        Text {
            id: zoomStatusTextItem

            anchors.centerIn: parent
            opacity: 2

            color: _root.contentColor

            text: isNaN(zoomLevel) ? "-" : "x" + _root.zoomLevel.toFixed(_root.showZoomPrecision ? 1 : 0)
        }
    }

    Button {
        id: zoomInButton
        flat: true

        anchors.left: zoomLevelVisible ? zoomStatusItem.horizontalCenter : _root.left
        anchors.top: _root.top
        width: height
        height: _root.height

        property bool holding: false

        onPressed: {
            if(onlyContinousZoom) {
                holding = true
            }
            else {
                _root.zoomOut()
            }
        }

        onPressAndHold: {
            holding = true
        }

        onReleased: {
            holding = false
        }

        background: Rectangle {
            anchors.fill: zoomInButton
            radius: zoomInButton.width/10

            color: _root.mainColor
        }

        contentItem: Item {
            anchors.fill: zoomInButton
            Rectangle {
                id: zoomInMinusRectangle
                anchors.centerIn: parent

                width: zoomInButton.width * 0.4
                height: zoomInButton.height * 0.05

                color: _root.contentColor
            }
        }
    }

    Item {
        id: buttonSeparator

        anchors.left: zoomInButton.right
        anchors.verticalCenter: zoomInButton.verticalCenter
        width: zoomInButton.width * 0.05
        height: zoomInButton.height * 0.8

        Rectangle {
            radius: width * 0.2
            anchors.centerIn: parent

            width: zoomInButton.width * 0.01
            height: parent.height * 0.8

            color: _root.contentColor
        }
    }

    Button {
        id: zoomOutButton
        flat: true

        anchors.left: buttonSeparator.right
        anchors.top: zoomInButton.top
        width: height
        height: zoomInButton.height

        property bool holding: false

        onPressed: {
            if(onlyContinousZoom) {
                holding = true
            }
            else {
                _root.zoomIn()
            }
        }

        onPressAndHold: {
            holding = true
        }

        onReleased: {
            holding = false
        }

        background: Rectangle {
            anchors.fill: zoomOutButton
            radius: zoomOutButton.width/10

            color: _root.mainColor
        }

        contentItem: Item {
            anchors.fill: zoomOutButton
            Rectangle {
                id: zoomOutMinusRectangle
                anchors.centerIn: parent

                width: zoomInMinusRectangle.width
                height: zoomInMinusRectangle.height

                color: _root.contentColor
            }
            Rectangle {
                anchors.centerIn: parent

                width: zoomOutMinusRectangle.height
                height: zoomOutMinusRectangle.width

                color: _root.contentColor
            }
        }
    }

    // Zoom buttons background
    Rectangle {
        color: _root.mainColor
        z: -1

        anchors.left: zoomInButton.horizontalCenter
        anchors.right: zoomOutButton.horizontalCenter
        anchors.top: zoomInButton.top
        anchors.bottom: zoomOutButton.bottom
    }

    onStateChanged: {
        if(state == "ZoomingIn") {
            _root.continuousZoomStart(true)
        }
        else if(state == "ZoomingOut") {
            _root.continuousZoomStart(false)
        }
        else {
            _root.continuousZoomStop()
        }
    }

    state: "None"
    states: [
        State {
            name: "None"
            when: zoomInButton.holding === false && zoomOutButton.holding === false
        },
        State {
            name: "ZoomingIn"
            when: zoomOutButton.holding === true
        },
        State {
            name: "ZoomingOut"
            when: zoomInButton.holding === true
        }
    ]
} 
    branches: [ "main" ]
  pull_request: module Custom.Camera

ZoomControl 1.0 ZoomControl.qml 
    # The branches below must be a subset of the branches above
    branches: [ "main" ]
  schedule: /****************************************************************************
 *
 * (c) 2009-2019 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 * @file
 *   @author Gus Grubba <gus@auterion.com>
 */

import QtQuick 2.11

Item {
    id: root
    property real   rollAngle :     0
    property real   pitchAngle:     0
    property color  skyColor1:      Qt.hsla(0.6, 1.0, 0.25)
    property color  skyColor2:      Qt.hsla(0.6, 0.5, 0.55)
    property color  groundColor1:   Qt.hsla(0.25,  0.5, 0.45)
    property color  groundColor2:   Qt.hsla(0.25, 0.75, 0.25)

    clip:           true
    anchors.fill:   parent

    property real angularScale: pitchAngle * root.height / 45

    Item {
        id:     artificialHorizon
        width:  root.width  * 4
        height: root.height * 8
        anchors.centerIn: parent
        Rectangle {
            id:             sky
            anchors.fill:   parent
            smooth:         true
            antialiasing:   true
            gradient: Gradient {
                GradientStop { position: 0.25; color: root.skyColor1 }
                GradientStop { position: 0.5;  color: root.skyColor2 }
            }
        }
        Rectangle {
            id:             ground
            height:         sky.height / 2
            anchors {
                left:       sky.left;
                right:      sky.right;
                bottom:     sky.bottom
            }
            smooth:         true
            antialiasing:   true
            gradient: Gradient {
                GradientStop { position: 0.0;  color: root.groundColor1 }
                GradientStop { position: 0.25; color: root.groundColor2 }
            }
        }
        transform: [
            Translate {
                y:  angularScale
            },
            Rotation {
                origin.x: artificialHorizon.width  / 2
                origin.y: artificialHorizon.height / 2
                angle:    -rollAngle
            }]
    }
} 
    - cron: /****************************************************************************
 *
 * (c) 2009-2019 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 * @file
 *   @author Gus Grubba <gus@auterion.com>
 */

import QtQuick              2.11
import QtGraphicalEffects   1.0

import QGroundControl               1.0
import QGroundControl.Controls      1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Palette       1.0
import QGroundControl.FlightMap     1.0

Item {
    id: root

    property bool showPitch:    true
    property var  vehicle:      null
    property real size
    property bool showHeading:  false

    property real _rollAngle:   vehicle ? vehicle.roll.rawValue  : 0
    property real _pitchAngle:  vehicle ? vehicle.pitch.rawValue : 0

    width:  size
    height: size

    Item {
        id:             instrument
        anchors.fill:   parent
        visible:        false

        //----------------------------------------------------
        //-- Artificial Horizon
        CustomArtificialHorizon {
            rollAngle:          _rollAngle
            pitchAngle:         _pitchAngle
            skyColor1:          "#0a2e50"
            skyColor2:          "#2f85d4"
            groundColor1:       "#897459"
            groundColor2:       "#4b3820"
            anchors.fill:       parent
        }
        //----------------------------------------------------
        //-- Instrument Dial
        Image {
            id:                 instrumentDial
            source:             "/custom/img/attitude_dial.svg"
            mipmap:             true
            fillMode:           Image.PreserveAspectFit
            anchors.fill:       parent
            sourceSize.height:  parent.height
            transform: Rotation {
                origin.x:       root.width  / 2
                origin.y:       root.height / 2
                angle:          -_rollAngle
            }
        }
        //----------------------------------------------------
        //-- Pointer
        Image {
            id:                 pointer
            height:             size * 0.0625
            width:              height
            source:             "/custom/img/attitude_pointer.svg"
            antialiasing:       true
            fillMode:           Image.PreserveAspectFit
            sourceSize.height:  height
            anchors.top:        parent.top
            anchors.horizontalCenter: parent.horizontalCenter
        }
        //----------------------------------------------------
        //-- Pitch
        QGCPitchIndicator {
            id:                 pitchWidget
            visible:            root.showPitch
            size:               root.size * 0.5
            anchors.verticalCenter: parent.verticalCenter
            pitchAngle:         _pitchAngle
            rollAngle:          _rollAngle
            color:              Qt.rgba(0,0,0,0)
        }
        //----------------------------------------------------
        //-- Cross Hair
        Image {
            id:                 crossHair
            anchors.centerIn:   parent
            source:             "/custom/img/attitude_crosshair.svg"
            mipmap:             true
            width:              size * 0.75
            sourceSize.width:   width
            fillMode:           Image.PreserveAspectFit
        }
    }

    Rectangle {
        id:             mask
        anchors.fill:   instrument
        radius:         width / 2
        color:          "black"
        visible:        false
    }

    OpacityMask {
        anchors.fill:   instrument
        source:         instrument
        maskSource:     mask
    }

    Rectangle {
        id:             borderRect
        anchors.fill:   parent
        radius:         width / 2
        color:          Qt.rgba(0,0,0,0)
        border.color:   "#000"
        border.width:   1
    }

    QGCLabel {
        anchors.bottomMargin:       Math.round(ScreenTools.defaultFontPixelHeight * 0.5)
        anchors.bottom:             parent.bottom
        anchors.horizontalCenter:   parent.horizontalCenter
        text:                       _headingString3
        color:                      "white"
        visible:                    showHeading
        font.pointSize:             ScreenTools.smallFontPointSize
        property string _headingString: vehicle ? vehicle.heading.rawValue.toFixed(0) : "OFF"
        property string _headingString2: _headingString.length  === 1 ? "0" + _headingString  : _headingString
        property string _headingString3: _headingString2.length === 2 ? "0" + _headingString2 : _headingString2
    }
} 
  workflow_dispatch: /****************************************************************************
 *
 * (c) 2009-2019 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 * @file
 *   @author Gus Grubba <gus@auterion.com>
 */

import QtQuick                      2.11
import QtQuick.Controls             2.4

import QGroundControl               1.0
import QGroundControl.Controls      1.0
import QGroundControl.Palette       1.0
import QGroundControl.ScreenTools   1.0
import QtGraphicalEffects           1.0

Button {
    id:                             _rootButton
    width:                          parent.height * 1.25
    height:                         parent.height
    flat:                           true
    contentItem: Item {
        id:                         _content
        anchors.fill:               _rootButton
        Row {
            id:                     _edge
            spacing:                ScreenTools.defaultFontPixelWidth * 0.25
            anchors.left:           parent.left
            anchors.leftMargin:     ScreenTools.defaultFontPixelWidth
            anchors.verticalCenter: parent.verticalCenter
            Repeater {
                model: [1,2,3]
                Rectangle {
                    height:         ScreenTools.defaultFontPixelHeight
                    width:          ScreenTools.defaultFontPixelWidth * 0.25
                    color:          qgcPal.text
                    opacity:        0.75
                }
            }
        }
        Image {
            id:                     _icon
            height:                 _rootButton.height * 0.75
            width:                  height
            smooth:                 true
            mipmap:                 true
            antialiasing:           true
            fillMode:               Image.PreserveAspectFit
            source:                 qgcPal.globalTheme === QGCPalette.Light ? "/res/QGCLogoBlack" : "/res/QGCLogoWhite"
            sourceSize.height:      height
            anchors.left:           _edge.right
            anchors.leftMargin:     ScreenTools.defaultFontPixelWidth
            anchors.verticalCenter: parent.verticalCenter
        }
    }
    background: Item {
        anchors.fill: parent
    }
} 

jobs: /****************************************************************************
 *
 * (c) 2009-2019 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 * @file
 *   @author Gus Grubba <gus@auterion.com>
 */

import QtQuick 2.3

import QGroundControl.Palette       1.0
import QGroundControl.ScreenTools   1.0
import QGroundControl.Controls      1.0

Rectangle {
    id:     _root
    height: Math.round(ScreenTools.defaultFontPixelHeight * 2)
    width:  ScreenTools.defaultFontPixelWidth  * 10
    color:  qgcPal.button
    border.color: qgcPal.windowShade
    border.width: 0

    property bool checked: true

    signal  clicked

    QGCPalette { id: qgcPal; colorGroupEnabled: true }

    Rectangle {
        width:      parent.width  * 0.5
        height:     parent.height
        color:      checked ? qgcPal.button : qgcPal.buttonHighlight
        border.color: qgcPal.text
        border.width: 0
        anchors.left: parent.left
        anchors.verticalCenter: parent.verticalCenter
        QGCLabel {
            text: qsTr("Off")
            anchors.centerIn: parent
            color:      qgcPal.text
        }
    }
    Rectangle {
        width:      parent.width  * 0.5
        height:     parent.height
        color:      checked ? qgcPal.buttonHighlight : qgcPal.button
        border.color: qgcPal.text
        border.width: 0
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        QGCLabel {
            text:       qsTr("On")
            color:      qgcPal.buttonHighlightText
            anchors.centerIn: parent
        }
    }
    MouseArea {
        anchors.fill:   parent
        onClicked: {
            checked = !checked
            _root.clicked()
        }
    }
}  
  Fortify-AST-Scan: /****************************************************************************
 *
 * (c) 2009-2019 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 * @file
 *   @author Gus Grubba <gus@auterion.com>
 */

import QtQuick                      2.11
import QtQuick.Controls             2.4

import QGroundControl               1.0
import QGroundControl.Controls      1.0
import QGroundControl.Palette       1.0
import QGroundControl.ScreenTools   1.0

Button {
    id:                         control
    autoExclusive:              true
    checkable:                  true

    property string iconSource:     ""
    property real   iconRatio:      0.5
    property real   buttonRadius:   ScreenTools.defaultFontPixelWidth * 0.5

    background: Rectangle {
        width:                  control.width
        height:                 width
        anchors.centerIn:       parent
        color:                  (mouseArea.pressed || control.checked) ? qgcPal.buttonHighlight : (qgcPal.globalTheme === QGCPalette.Light ? Qt.rgba(1,1,1,0.5) : Qt.rgba(0,0,0,0.5))
        radius:                 control.buttonRadius
    }
    contentItem: Item {
        anchors.fill:           control
        QGCColoredImage {
            source:             iconSource
            color:              (mouseArea.pressed || control.checked) ? qgcPal.buttonHighlightText : qgcPal.buttonText
            width:              control.width * iconRatio
            height:             width
            anchors.centerIn:   parent
            sourceSize.height:  height
        }
    }
    MouseArea {
        id:                     mouseArea
        anchors.fill:           parent
        onClicked: {
            if(checkable)
                checked = true
            control.clicked()
        }
    }
} 
    # Use the appropriate runner for building your source code. Ensure dev tools required to build your code are present and configured appropriately (MSBuild, Python, etc).
    runs-on: /****************************************************************************
 *
 * (c) 2009-2019 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 * @file
 *   @author Gus Grubba <gus@auterion.com>
 */

import QtQuick                              2.11
import QtQuick.Controls                     1.4

import QGroundControl                       1.0
import QGroundControl.Controls              1.0
import QGroundControl.ScreenTools           1.0
import QGroundControl.Palette               1.0

Item {
    width:  size
    height: size

    property real size:     50
    property real percent:  0

    QGCPalette { id: qgcPal }

    function getIcon() {
        if (percent < 20)
            return "/custom/img/menu_signal_0.svg"
        if (percent < 40)
            return "/custom/img/menu_signal_25.svg"
        if (percent < 60)
            return "/custom/img/menu_signal_50.svg"
        if (percent < 90)
            return "/custom/img/menu_signal_75.svg"
        return "/custom/img/menu_signal_100.svg"
    }

    QGCColoredImage {
        source:             getIcon()
        fillMode:           Image.PreserveAspectFit
        anchors.fill:       parent
        sourceSize.height:  size
        color:              qgcPal.text
    }
} 
    permissions: /****************************************************************************
 *
 * (c) 2009-2019 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 * @file
 *   @author Gus Grubba <gus@auterion.com>
 */

import QtQuick                      2.11
import QtQuick.Controls             2.4

import QGroundControl.Controls      1.0
import QGroundControl.Palette       1.0
import QGroundControl.ScreenTools   1.0

Button {
    id:                             button
    autoExclusive:                  true

    background: Rectangle {
        anchors.fill:               parent
        color:                      qgcPal.buttonHighlight
        visible:                    (mouseArea.pressed || button.checked)
    }

    contentItem: Row {
        spacing:                    ScreenTools.defaultFontPixelWidth
        anchors.left:               button.left
        anchors.leftMargin:         ScreenTools.defaultFontPixelWidth
        anchors.verticalCenter:     button.verticalCenter
        Item {
            height:                 ScreenTools.defaultFontPixelHeight * 3
            width:                  1
        }
        QGCColoredImage {
            id:                     _icon
            height:                 ScreenTools.defaultFontPixelHeight
            width:                  height
            sourceSize.height:      parent.height
            fillMode:               Image.PreserveAspectFit
            color:                  qgcPal.buttonText
            source:                 button.icon.source
            anchors.verticalCenter: parent.verticalCenter
        }
        Label {
            id:                     _label
            visible:                text !== ""
            text:                   button.text
            color:                  qgcPal.buttonText
            anchors.verticalCenter: parent.verticalCenter
        }
    }
    // Process hover events
    MouseArea {
        id:                         mouseArea
        anchors.fill:               parent
        onClicked:                  button.clicked()
    }
} 
      actions: /****************************************************************************
 *
 * (c) 2009-2019 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 * @file
 *   @author Gus Grubba <gus@auterion.com>
 */

import QtQuick                      2.11
import QtQuick.Controls             2.4

import QGroundControl.Controls      1.0
import QGroundControl.Palette       1.0
import QGroundControl.ScreenTools   1.0

Button {
    id:                             button
    height:                         _infoCol.height * 1.25
    autoExclusive:                  true

    property var vehicle:           null

    function getBatteryColor() {
        if(vehicle) {
            if(vehicle.battery.percentRemaining.value > 75) {
                return qgcPal.colorGreen
            }
            if(vehicle.battery.percentRemaining.value > 50) {
                return qgcPal.colorOrange
            }
            if(vehicle.battery.percentRemaining.value > 0.1) {
                return qgcPal.colorRed
            }
        }
        return qgcPal.colorGrey
    }

    function getBatteryPercentage() {
        if(vehicle) {
            return vehicle.battery.percentRemaining.value / 100.0
        }
        return 1
    }

    background: Rectangle {
        anchors.fill:               parent
        color:                      button.checked ? qgcPal.buttonHighlight : qgcPal.button
        radius:                     ScreenTools.defaultFontPixelWidth * 0.5
    }

    contentItem: Row {
        spacing:                    ScreenTools.defaultFontPixelWidth
        anchors.margins:            ScreenTools.defaultFontPixelWidth
        anchors.verticalCenter:     button.verticalCenter
//        QGCColoredImage {
//            id:                     _icon
//            height:                 ScreenTools.defaultFontPixelHeight * 1.5
//            width:                  height
//            sourceSize.height:      parent.height
//            fillMode:               Image.PreserveAspectFit
//            color:                  button.checked ? qgcPal.buttonHighlightText : qgcPal.buttonText
//            source:                 "/qmlimages/PaperPlane.svg"
//            anchors.verticalCenter: parent.verticalCenter
//        }
        Column {
            id:                     _infoCol
            spacing:                ScreenTools.defaultFontPixelHeight * 0.25
            QGCLabel {
                text:               qsTr("Vehicle ") + (vehicle ? vehicle.id : qsTr("None"))
                font.family:        ScreenTools.demiboldFontFamily
                color:              button.checked ? qgcPal.buttonHighlightText : qgcPal.buttonText
            }
            Row {
                spacing:            ScreenTools.defaultFontPixelWidth
                QGCLabel {
                    text:           vehicle ? vehicle.flightMode : qsTr("None")
                    color:          button.checked ? qgcPal.buttonHighlightText : qgcPal.buttonText
                }
                Rectangle {
                    height:         ScreenTools.defaultFontPixelHeight * 0.5
                    width:          ScreenTools.defaultFontPixelWidth  * 3
                    color:          Qt.rgba(0,0,0,0)
                    anchors.verticalCenter: parent.verticalCenter
                    Rectangle {
                        height:     parent.height
                        width:      parent.width * getBatteryPercentage()
                        color:      getBatteryColor()
                        anchors.right: parent.right
                    }
                }
            }
        }
    }

} 
      contents: Module Custom.Widgets

CustomArtificialHorizon 1.0     CustomArtificialHorizon.qml
CustomAttitudeWidget    1.0     CustomAttitudeWidget.qml
CustomIconButton        1.0     CustomIconButton.qml
CustomOnOffSwitch       1.0     CustomOnOffSwitch.qml
CustomQuickButton       1.0     CustomQuickButton.qml
CustomSignalStrength    1.0     CustomSignalStrength.qml
CustomToolBarButton     1.0     CustomToolBarButton.qml
CustomVehicleButton     1.0     CustomVehicleButton.qml 
      security-events: /****************************************************************************
 *
 * (c) 2009-2019 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>
 *
 * QGroundControl is licensed according to the terms in the file
 * COPYING.md in the root of the source code directory.
 *
 * @file
 *   @author Gus Grubba <gus@auterion.com>
 */

import QtQuick          2.12
import QtQuick.Controls 2.4
import QtQuick.Layouts  1.11

import QGroundControl               1.0
import QGroundControl.Controls      1.0
import QGroundControl.Palette       1.0
import QGroundControl.ScreenTools   1.0

import Custom.Widgets 1.0

Item {
    property var parentToolInsets                       // These insets tell you what screen real estate is available for positioning the controls in your overlay
    property var totalToolInsets:   _totalToolInsets    // The insets updated for the custom overlay additions
    property var mapControl

    readonly property string noGPS:         qsTr("NO GPS")
    readonly property real   indicatorValueWidth:   ScreenTools.defaultFontPixelWidth * 7

    property var    _activeVehicle:         QGroundControl.multiVehicleManager.activeVehicle
    property real   _indicatorDiameter:     ScreenTools.defaultFontPixelWidth * 18
    property real   _indicatorsHeight:      ScreenTools.defaultFontPixelHeight
    property var    _sepColor:              qgcPal.globalTheme === QGCPalette.Light ? Qt.rgba(0,0,0,0.5) : Qt.rgba(1,1,1,0.5)
    property color  _indicatorsColor:       qgcPal.text
    property bool   _isVehicleGps:          _activeVehicle ? _activeVehicle.gps.count.rawValue > 1 && _activeVehicle.gps.hdop.rawValue < 1.4 : false
    property string _altitude:              _activeVehicle ? (isNaN(_activeVehicle.altitudeRelative.value) ? "0.0" : _activeVehicle.altitudeRelative.value.toFixed(1)) + ' ' + _activeVehicle.altitudeRelative.units : "0.0"
    property string _distanceStr:           isNaN(_distance) ? "0" : _distance.toFixed(0) + ' ' + QGroundControl.unitsConversion.appSettingsHorizontalDistanceUnitsString
    property real   _heading:               _activeVehicle   ? _activeVehicle.heading.rawValue : 0
    property real   _distance:              _activeVehicle ? _activeVehicle.distanceToHome.rawValue : 0
    property string _messageTitle:          ""
    property string _messageText:           ""
    property real   _toolsMargin:           ScreenTools.defaultFontPixelWidth * 0.75

    function secondsToHHMMSS(timeS) {
        var sec_num = parseInt(timeS, 10);
        var hours   = Math.floor(sec_num / 3600);
        var minutes = Math.floor((sec_num - (hours * 3600)) / 60);
        var seconds = sec_num - (hours * 3600) - (minutes * 60);
        if (hours   < 10) {hours   = "0"+hours;}
        if (minutes < 10) {minutes = "0"+minutes;}
        if (seconds < 10) {seconds = "0"+seconds;}
        return hours+':'+minutes+':'+seconds;
    }

    QGCToolInsets {
        id:                     _totalToolInsets
        topEdgeCenterInset:     compassArrowIndicator.y + compassArrowIndicator.height
        bottomEdgeRightInset:   parent.height - vehicleIndicator.y
        bottomEdgeCenterInset:  bottomEdgeRightInset
    }

    //-------------------------------------------------------------------------
    //-- Heading Indicator
    Rectangle {
        id:                         compassBar
        height:                     ScreenTools.defaultFontPixelHeight * 1.5
        width:                      ScreenTools.defaultFontPixelWidth  * 50
        color:                      "#DEDEDE"
        radius:                     2
        clip:                       true
        anchors.top:                headingIndicator.bottom
        anchors.topMargin:          -headingIndicator.height / 2
        anchors.horizontalCenter:   parent.horizontalCenter
        Repeater {
            model: 720
            QGCLabel {
                function _normalize(degrees) {
                    var a = degrees % 360
                    if (a < 0) a += 360
                    return a
                }
                property int _startAngle: modelData + 180 + _heading
                property int _angle: _normalize(_startAngle)
                anchors.verticalCenter: parent.verticalCenter
                x:              visible ? ((modelData * (compassBar.width / 360)) - (width * 0.5)) : 0
                visible:        _angle % 45 == 0
                color:          "#75505565"
                font.pointSize: ScreenTools.smallFontPointSize
                text: {
                    switch(_angle) {
                    case 0:     return "N"
                    case 45:    return "NE"
                    case 90:    return "E"
                    case 135:   return "SE"
                    case 180:   return "S"
                    case 225:   return "SW"
                    case 270:   return "W"
                    case 315:   return "NW"
                    }
                    return ""
                }
            }
        }
    }
    Rectangle {
        id:                         headingIndicator
        height:                     ScreenTools.defaultFontPixelHeight
        width:                      ScreenTools.defaultFontPixelWidth * 4
        color:                      qgcPal.windowShadeDark
        anchors.top:                parent.top
        anchors.topMargin:          _toolsMargin
        anchors.horizontalCenter:   parent.horizontalCenter
        QGCLabel {
            text:                   _heading
            color:                  qgcPal.text
            font.pointSize:         ScreenTools.smallFontPointSize
            anchors.centerIn:       parent
        }
    }
    Image {
        id:                         compassArrowIndicator
        height:                     _indicatorsHeight
        width:                      height
        source:                     "/custom/img/compass_pointer.svg"
        fillMode:                   Image.PreserveAspectFit
        sourceSize.height:          height
        anchors.top:                compassBar.bottom
        anchors.topMargin:          -height / 2
        anchors.horizontalCenter:   parent.horizontalCenter
    }
    //-------------------------------------------------------------------------
    //-- Vehicle Indicator
    Rectangle {
        id:                     vehicleIndicator
        color:                  qgcPal.window
        width:                  vehicleStatusGrid.width  + (ScreenTools.defaultFontPixelWidth  * 3)
        height:                 vehicleStatusGrid.height + (ScreenTools.defaultFontPixelHeight * 1.5)
        radius:                 2
        //anchors.bottomMargin:   parentToolInsets.bottomEdgeRightInset
        anchors.bottom:         parent.bottom
        anchors.bottomMargin:   _toolsMargin
        anchors.right:          attitudeIndicator.left
        anchors.rightMargin:    -ScreenTools.defaultFontPixelWidth

        GridLayout {
            id:                     vehicleStatusGrid
            columnSpacing:          ScreenTools.defaultFontPixelWidth  * 1.5
            rowSpacing:             ScreenTools.defaultFontPixelHeight * 0.5
            columns:                7
            anchors.centerIn:       parent

            //-- Compass
            Item {
                Layout.rowSpan:         3
                Layout.column:          6
                Layout.minimumWidth:    parent.height * 1.25
                Layout.fillHeight:      true
                Layout.fillWidth:       true
                //-- Large circle
                Rectangle {
                    height:             parent.height
                    width:              height
                    radius:             height * 0.5
                    border.color:       qgcPal.text
                    border.width:       1
                    color:              Qt.rgba(0,0,0,0)
                    anchors.centerIn:   parent
                }
                //-- North Label
                Rectangle {
                    height:             ScreenTools.defaultFontPixelHeight * 0.75
                    width:              ScreenTools.defaultFontPixelWidth  * 2
                    radius:             ScreenTools.defaultFontPixelWidth  * 0.25
                    color:              qgcPal.windowShade
                    anchors.top:        parent.top
                    anchors.topMargin:  ScreenTools.defaultFontPixelHeight * -0.25
                    anchors.horizontalCenter: parent.horizontalCenter
                    QGCLabel {
                        text:               "N"
                        color:              qgcPal.text
                        font.pointSize:     ScreenTools.smallFontPointSize
                        anchors.centerIn:   parent
                    }
                }
                //-- Needle
                Image {
                    id:                 compassNeedle
                    anchors.centerIn:   parent
                    height:             parent.height * 0.75
                    width:              height
                    source:             "/custom/img/compass_needle.svg"
                    fillMode:           Image.PreserveAspectFit
                    sourceSize.height:  height
                    transform: [
                        Rotation {
                            origin.x:   compassNeedle.width  / 2
                            origin.y:   compassNeedle.height / 2
                            angle:      _heading
                        }]
                }
                //-- Heading
                Rectangle {
                    height:             ScreenTools.defaultFontPixelHeight * 0.75
                    width:              ScreenTools.defaultFontPixelWidth  * 3.5
                    radius:             ScreenTools.defaultFontPixelWidth  * 0.25
                    color:              qgcPal.windowShade
                    anchors.bottom:         parent.bottom
                    anchors.bottomMargin:   ScreenTools.defaultFontPixelHeight * -0.25
                    anchors.horizontalCenter: parent.horizontalCenter
                    QGCLabel {
                        text:               _heading
                        color:              qgcPal.text
                        font.pointSize:     ScreenTools.smallFontPointSize
                        anchors.centerIn:   parent
                    }
                }
            }
            //-- Second Row
            //-- Chronometer
            QGCColoredImage {
                height:                 _indicatorsHeight
                width:                  height
                source:                 "/custom/img/chronometer.svg"
                fillMode:               Image.PreserveAspectFit
                sourceSize.height:      height
                Layout.alignment:       Qt.AlignVCenter | Qt.AlignHCenter
                color:                  qgcPal.text
            }
            QGCLabel {
                id:                     firstLabel
                text: {
                    if(_activeVehicle)
                        return secondsToHHMMSS(_activeVehicle.getFact("flightTime").value)
                    return "00:00:00"
                }
                color:                  _indicatorsColor
                font.pointSize:         ScreenTools.smallFontPointSize
                Layout.fillWidth:       true
                Layout.minimumWidth:    indicatorValueWidth
                horizontalAlignment:    Text.AlignLeft
            }
            //-- Ground Speed
            QGCColoredImage {
                height:                 _indicatorsHeight
                width:                  height
                source:                 "/custom/img/horizontal_speed.svg"
                fillMode:               Image.PreserveAspectFit
                sourceSize.height:      height
                Layout.alignment:       Qt.AlignVCenter | Qt.AlignHCenter
                color:                  qgcPal.text
            }
            QGCLabel {
                text:                   _activeVehicle ? _activeVehicle.groundSpeed.value.toFixed(1) + ' ' + _activeVehicle.groundSpeed.units : "0.0"
                color:                  _indicatorsColor
                font.pointSize:         ScreenTools.smallFontPointSize
                Layout.fillWidth:       true
                Layout.minimumWidth:    indicatorValueWidth
                horizontalAlignment:    firstLabel.horizontalAlignment
            }
            //-- Vertical Speed
            QGCColoredImage {
                height:                 _indicatorsHeight
                width:                  height
                source:                 "/custom/img/vertical_speed.svg"
                fillMode:               Image.PreserveAspectFit
                sourceSize.height:      height
                Layout.alignment:       Qt.AlignVCenter | Qt.AlignHCenter
                color:                  qgcPal.text

            }
            QGCLabel {
                text:                   _activeVehicle ? _activeVehicle.climbRate.value.toFixed(1) + ' ' + _activeVehicle.climbRate.units : "0.0"
                color:                  _indicatorsColor
                font.pointSize:         ScreenTools.smallFontPointSize
                Layout.fillWidth:       true
                Layout.minimumWidth:    indicatorValueWidth
                horizontalAlignment:    firstLabel.horizontalAlignment
            }
            //-- Third Row
            //-- Odometer
            QGCColoredImage {
                height:                 _indicatorsHeight
                width:                  height
                source:                 "/custom/img/odometer.svg"
                fillMode:               Image.PreserveAspectFit
                sourceSize.height:      height
                Layout.alignment:       Qt.AlignVCenter | Qt.AlignHCenter
                color:                  qgcPal.text

            }
            QGCLabel {
                text:                   _activeVehicle ? ('00000' + _activeVehicle.flightDistance.value.toFixed(0)).slice(-5) + ' ' + _activeVehicle.flightDistance.units : "00000"
                color:                  _indicatorsColor
                font.pointSize:         ScreenTools.smallFontPointSize
                Layout.fillWidth:       true
                Layout.minimumWidth:    indicatorValueWidth
                horizontalAlignment:    firstLabel.horizontalAlignment
            }
            //-- Altitude
            QGCColoredImage {
                height:                 _indicatorsHeight
                width:                  height
                source:                 "/custom/img/altitude.svg"
                fillMode:               Image.PreserveAspectFit
                sourceSize.height:      height
                Layout.alignment:       Qt.AlignVCenter | Qt.AlignHCenter
                color:                  qgcPal.text

            }
            QGCLabel {
                text:                   _altitude
                color:                  _indicatorsColor
                font.pointSize:         ScreenTools.smallFontPointSize
                Layout.fillWidth:       true
                Layout.minimumWidth:    indicatorValueWidth
                horizontalAlignment:    firstLabel.horizontalAlignment
            }
            //-- Distance
            QGCColoredImage {
                height:                 _indicatorsHeight
                width:                  height
                source:                 "/custom/img/distance.svg"
                fillMode:               Image.PreserveAspectFit
                sourceSize.height:      height
                Layout.alignment:       Qt.AlignVCenter | Qt.AlignHCenter
                color:                  qgcPal.text

            }
            QGCLabel {
                text:                   _distance ? _distanceStr : noGPS
                color:                  _distance ? _indicatorsColor : qgcPal.colorOrange
                font.pointSize:         ScreenTools.smallFontPointSize
                Layout.fillWidth:       true
                Layout.minimumWidth:    indicatorValueWidth
                horizontalAlignment:    firstLabel.horizontalAlignment
            }
        }
    }
    //-------------------------------------------------------------------------
    //-- Attitude Indicator
    Rectangle {
        color:                  qgcPal.window
        width:                  attitudeIndicator.width * 0.5
        height:                 vehicleIndicator.height
        anchors.top:            vehicleIndicator.top
        anchors.left:           vehicleIndicator.right
    }
    Rectangle {
        id:                     attitudeIndicator
        anchors.bottom:         vehicleIndicator.bottom
        anchors.bottomMargin:   ScreenTools.defaultFontPixelWidth * -0.5
        anchors.right:          parent.right
        anchors.rightMargin:    _toolsMargin
        height:                 ScreenTools.defaultFontPixelHeight * 6
        width:                  height
        radius:                 height * 0.5
        color:                  qgcPal.windowShade
        CustomAttitudeWidget {
            size:               parent.height * 0.95
            vehicle:            _activeVehicle
            showHeading:        false
            anchors.centerIn:   parent
        }
    }
} 

    steps:
      # Check out source code
      - name: Check Out Source Code
        uses: actions/checkout@v4

      # Java is required to run the various Fortify utilities. Ensuring proper version is installed on the runner.
      - name: Setup Java
        uses: actions/setup-java@v4
        with:
          java-version: 17
          distribution: 'temurin'

      # Perform SAST and optionally SCA scan via Fortify on Demand/Fortify Hosted/Software Security Center, then
      # optionally export SAST results to the GitHub code scanning dashboard. In case further customization is
      # required, you can use sub-actions like fortify/github-action/setup@v1 to set up the various Fortify tools
      # and run them directly from within your pipeline; see https://github.com/fortify/github-action#readme for
      # details.
      - name: Run FoD SAST Scan
        uses: fortify/github-action@a92347297e02391b857e7015792cd1926a4cd418
        with:
          sast-scan: true
        env:
          ### Required configuration when integrating with Fortify on Demand
          FOD_URL: https://ams.fortify.com
          FOD_TENANT: ${{secrets.FOD_TENANT}}
          FOD_USER: ${{secrets.FOD_USER}}
          FOD_PASSWORD: ${{secrets.FOD_PAT}}
          ### Optional configuration when integrating with Fortify on Demand
          # EXTRA_PACKAGE_OPTS: -oss                       # Extra 'scancentral package' options, like '-oss'' if
                                                           # Debricked SCA scan is enabled on Fortify on Demand
          # EXTRA_FOD_LOGIN_OPTS: --socket-timeout=60s     # Extra 'fcli fod session login' options
          # FOD_RELEASE: MyApp:MyRelease                   # FoD release name, default: <org>/<repo>:<branch>; may
                                                           # replace app+release name with numeric release ID
          # DO_WAIT: true                                  # Wait for scan completion, implied if 'DO_EXPORT: true'
          # DO_EXPORT: true                                # Export SAST results to GitHub code scanning dashboard
          ### Required configuration when integrating with Fortify Hosted / Software Security Center & ScanCentral
          # SSC_URL: ${{secrets.SSC_URL}}                            # SSC URL
          # SSC_TOKEN: ${{secrets.SSC_TOKEN}}                        # SSC CIToken or AutomationToken
          # SC_SAST_TOKEN: ${{secrets.SC_SAST_TOKEN}}                # ScanCentral SAST client auth token
          # SC_SAST_SENSOR_VERSION: ${{vars.SC_SAST_SENSOR_VERSION}} # Sensor version on which to run the scan;
                                                                     # usually defined as organization or repo variable
          ### Optional configuration when integrating with Fortify Hosted / Software Security Center & ScanCentral
          # EXTRA_SC_SAST_LOGIN_OPTS: --socket-timeout=60s # Extra 'fcli sc-sast session login' options
          # SSC_APPVERSION: MyApp:MyVersion                # SSC application version, default: <org>/<repo>:<branch>
          # EXTRA_PACKAGE_OPTS: -bv myCustomPom.xml        # Extra 'scancentral package' options
          # DO_WAIT: true                                  # Wait for scan completion, implied if 'DO_EXPORT: true'
          # DO_EXPORT: true                                # Export SAST results to GitHub code scanning dashboard
