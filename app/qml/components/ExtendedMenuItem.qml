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
import QtQuick.Controls 2.2
import "."  // import InputStyle singleton
import ".."

Item {
    id: root
    property string contentText: ""
    property string imageSource: ""
    property real rowHeight: InputStyle.rowHeight
    property real panelMargin: InputStyle.panelMargin
    property bool overlayImage: true
    property bool highlight: false
    property bool showBorder: true

    property color fontColor: InputStyle.fontColor
    property color panelColor: InputStyle.clrPanelMain
    property color highlightColor: InputStyle.fontColorBright
    property color imageColor: root.highlight ? root.panelColor : root.fontColor

    anchors.fill: parent

    Item {
        id: row
        anchors.fill: parent
        width: parent.width
        height: parent.height

        Item {
            id: iconContainer
            height: rowHeight
            width: rowHeight
            anchors.verticalCenter: parent ? parent.verticalCenter : undefined

            Image {
                id: icon
                anchors.fill: parent
                anchors.margins: rowHeight/4
                source: imageSource
                sourceSize.width: width
                sourceSize.height: height
                fillMode: Image.PreserveAspectFit
            }

            ColorOverlay {
                anchors.fill: icon
                source: icon
                color: imageColor
                visible: overlayImage
            }
        }

        Item {
            id: textContainer
            y: 0
            x: iconContainer.width + panelMargin
            width: parent.width - rowHeight
            height: rowHeight
            anchors.verticalCenter: parent ? parent.verticalCenter : undefined

            Text {
                id: mainText
                text: root.contentText
                height: parent.height
                width: parent.width

                font.pixelSize: InputStyle.fontPixelSizeNormal
                font.weight: Font.Bold
                color: root.highlight ? root.panelColor : root.fontColor
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
            }
        }
    }

    Rectangle {
        id: borderLine
        color: root.highlightColor
        width: row.width
        height: 1
        visible: root.showBorder
        anchors.bottom: parent ? parent.bottom : undefined
    }
}
