/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic

RoundButton {
    id: root

    implicitWidth: __style.margin48
    implicitHeight: __style.margin48

    required property color buttonColor
    required property color bgndColor
    property bool isSelected: false

    anchors{
        verticalCenter: parent.verticalCenter
        top: __style.margin8
    }

    contentItem: Rectangle {
        color: root.buttonColor
        radius: width / 2
        height: __style.margin32
        width: __style.margin32
        border{
            color:  __style.photoSketchingWhiteColor ? __style.lightGreenColor : __style.transparentColor
            width: 2
        }
    }

    background: Rectangle {
        anchors.centerIn: root

        radius: width / 2
        width: __style.margin48
        height: __style.margin48
        color: bgndColor
        border{
            width: 2
            color: root.isSelected ? __style.forestColor : __style.transparentColor
        }
    }
}