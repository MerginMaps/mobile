/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.7
import QtGraphicalEffects 1.0
import ".."  // import InputStyle singleton

Rectangle {
    id: header
    color: InputStyle.clrPanelMain

    property real rowHeight
    property string titleText: ""
    property string backText: qsTr("Cancel")
    property bool withBackButton: true
    property bool backTextVisible: false
    property bool backIconVisible: true
    property color fontBtnColor: InputStyle.fontColorBright

    signal back()

    Item {
        id: backButton
        height: header.rowHeight
        width: height * 2
        anchors.left: header.left
        anchors.leftMargin: InputStyle.panelMargin
        z: title.z + 1
        visible: withBackButton

        Image {
            id: image
            height: backIconVisible ? 20 : 0
            width: height
            source: InputStyle.backIcon
            sourceSize.width: width
            sourceSize.height: height
            fillMode: Image.PreserveAspectFit
            anchors.bottomMargin: (parent.height - height)/2
            anchors.topMargin: anchors.bottomMargin
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            visible: false
        }

        ColorOverlay {
            anchors.fill: image
            source: image
            color: InputStyle.fontColorBright
            visible: header.backIconVisible
        }

        Text {
            id: backButtonText
            text: header.backText
            color: header.fontBtnColor
            font.pixelSize: InputStyle.fontPixelSizeNormal
            height: header.rowHeight
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            anchors.left: image.right
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.top: parent.top
            anchors.leftMargin: backIconVisible ? header.rowHeight/4 : 0
            visible: backTextVisible
        }

        MouseArea {
          anchors.fill: parent
          anchors.leftMargin: -InputStyle.panelMargin
          onClicked: back()
        }
    }

    Text {
        id: title
        anchors.fill: parent
        text: header.titleText
        color: InputStyle.fontColor
        font.pixelSize: InputStyle.fontPixelSizeBig
        font.bold: true
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

}
