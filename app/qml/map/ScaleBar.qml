/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.14
import QtQuick.Controls 2.14

import lc 1.0
import ".."  // import InputStyle singleton

Item {
    id: scaleBar

    property alias mapSettings: scaleBarKit.mapSettings
    property alias preferredWidth: scaleBarKit.preferredWidth

    ScaleBarKit {
        id: scaleBarKit
    }

    property color barColor: InputStyle.fontColor
    property string barText: scaleBarKit.distance + " " + scaleBarKit.units
    property int barWidth: scaleBarKit.width
    property int lineWidth: 2 * __dp

    width: barWidth

    Rectangle {
        id: background
        color: InputStyle.clrPanelMain
        opacity: 0.25
        width: parent.width
        height: parent.height
        radius: 5
    }

    Item {
        anchors.fill: parent
        anchors.leftMargin: 5 * __dp
        anchors.rightMargin: anchors.leftMargin

        Text {
            id: text
            text: barText
            color: barColor
            font.pixelSize: InputStyle.fontPixelSizeSmall
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignTop
        }

        Rectangle {
            id: leftBar
            width: scaleBar.lineWidth
            height: scaleBar.height/3
            y: (scaleBar.height - leftBar.height) / 2
            color: barColor
            opacity: 1
            anchors.right: parent.right
            anchors.rightMargin: 0
        }

        Rectangle {
            id: bar
            anchors.left: parent.left
            anchors.right: parent.right
            height: scaleBar.lineWidth
            y: (scaleBar.height - scaleBar.lineWidth) / 2
            color: barColor
        }

        Rectangle {
            id: rightBar
            width: scaleBar.lineWidth
            height: scaleBar.height/3
            y: (scaleBar.height - leftBar.height) / 2
            color: barColor
        }
    }

    NumberAnimation on opacity {
        id: fadeOut
        to: 0.0
        duration: 1000

        onStopped: {
            scaleBar.visible = false
            scaleBar.opacity = 1.0
        }
    }

    Timer {
        id: scaleBarTimer
        interval: 3000; running: false; repeat: false
        onTriggered: {
            fadeOut.start()
        }
    }

    onVisibleChanged: {
        if (scaleBar.visible) {
            fadeOut.stop()
            scaleBarTimer.restart()
        }
    }
}
