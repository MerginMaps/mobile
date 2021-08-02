/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import ".."  // import InputStyle singleton

Rectangle {
    id: root
    height: width
    anchors.centerIn: parent
    property string imageSource: ""
    property string imageSource2: ""
    property bool imageSourceCondition: true
    property string text
    property color fontColor: "white"
    property color backgroundColor: InputStyle.fontColor
    property bool isHighlighted: false
    property bool enabled: true
    property bool handleClicks: true // enable property is used also for color
    property bool faded: false

    signal activated()
    signal activatedOnHold()

    color: root.isHighlighted ? fontColor : backgroundColor
    opacity: root.faded ? 0.6 : 1

    MouseArea {
        anchors.fill: parent
        enabled: root.enabled && handleClicks
        onClicked: {
            root.activated()
        }
        onPressAndHold: {
            root.activatedOnHold()
        }
    }

    Image {
        id: image
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.fill: parent
        source: root.imageSourceCondition ? root.imageSource : root.imageSource2
        sourceSize.width: width
        sourceSize.height: height
        visible: source
        anchors.margins: root.width/4
        anchors.topMargin: 0
        fillMode: Image.PreserveAspectFit
    }

    ColorOverlay {
        anchors.fill: image
        source: image
        color: {
            if (!root.enabled) return InputStyle.fontColorBright
            return root.isHighlighted ? backgroundColor : fontColor
        }
    }

    Text {
        anchors.fill: parent
        color: {
            if (!root.enabled) return InputStyle.fontColorBright
            return root.isHighlighted ? backgroundColor : fontColor
        }
        text: root.text
        font.bold: true
        font.pixelSize: InputStyle.fontPixelSizeSmall
        verticalAlignment: Text.AlignBottom
        horizontalAlignment: Text.AlignHCenter
        z: image.z + 1
    }
}
