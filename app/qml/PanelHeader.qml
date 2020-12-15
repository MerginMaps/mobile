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
import "."  // import InputStyle singleton

Rectangle {
    id: header
    color: InputStyle.clrPanelMain

    property real rowHeight
    property string titleText: ""
    property bool withBackButton: true
    property bool backTextVisible: true

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
            height: InputStyle.fontPixelSizeNormal
            width: height
            source: "back.svg"
            sourceSize.width: width
            sourceSize.height: height
            fillMode: Image.PreserveAspectFit
            anchors.bottomMargin: (parent.height - height)/2
            anchors.topMargin: anchors.bottomMargin
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
        }

        ColorOverlay {
            anchors.fill: image
            source: image
            color: InputStyle.fontColorBright
        }

        Text {
            id: backButtonText
            text: qsTr("Back")
            color: InputStyle.fontColorBright
            font.pixelSize: InputStyle.fontPixelSizeNormal
            height: header.rowHeight
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            anchors.left: image.right
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.top: parent.top
            anchors.leftMargin: header.rowHeight/4
            visible: ((title.contentWidth + backButton.width * 2) > header.width) || !backTextVisible ? false : true
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
        font.pixelSize: InputStyle.fontPixelSizeTitle
        font.bold: true
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

}
