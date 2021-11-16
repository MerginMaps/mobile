/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.0
import "."  // import InputStyle singleton

Rectangle {
    id: root
    height: InputStyle.rowHeight
    width: parent.width
    color: InputStyle.clrPanelMain

    property string text: ""
    property string text2: ""
    property bool bold: false

    Text {
        text: root.text
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.leftMargin: InputStyle.panelMargin
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        color: InputStyle.fontColor
        font.bold: bold
        font.pixelSize: InputStyle.fontPixelSizeNormal
        elide: Text.ElideRight
    }

    Text {
        text: text2
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.leftMargin: InputStyle.panelMargin
        anchors.rightMargin: InputStyle.panelMargin
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
        color: InputStyle.fontColor
        font.bold: bold
        font.pixelSize: InputStyle.fontPixelSizeNormal
        elide: Text.ElideRight
        visible: text2
    }
}
