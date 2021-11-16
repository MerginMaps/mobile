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
import "../"

Item {
  id: root
  property string contentText: ""
  property color fontColor: InputStyle.fontColor
  property bool highlight: false
  property color highlightColor: InputStyle.fontColorBright
  property color imageColor: root.highlight ? root.panelColor : root.fontColor
  property string imageSource: ""
  property bool overlayImage: true
  property color panelColor: InputStyle.clrPanelMain
  property real panelMargin: InputStyle.panelMargin
  property real rowHeight: InputStyle.rowHeight
  property bool showBorder: true

  anchors.fill: parent

  Item {
    id: row
    anchors.fill: parent
    height: parent.height
    width: parent.width

    Item {
      id: iconContainer
      anchors.verticalCenter: parent ? parent.verticalCenter : undefined
      height: rowHeight
      width: rowHeight

      Image {
        id: icon
        anchors.fill: parent
        anchors.margins: rowHeight / 4
        fillMode: Image.PreserveAspectFit
        source: imageSource
        sourceSize.height: height
        sourceSize.width: width
      }
      ColorOverlay {
        anchors.fill: icon
        color: imageColor
        source: icon
        visible: overlayImage
      }
    }
    Item {
      id: textContainer
      anchors.verticalCenter: parent ? parent.verticalCenter : undefined
      height: rowHeight
      width: parent.width - rowHeight
      x: iconContainer.width + panelMargin
      y: 0

      Text {
        id: mainText
        color: root.highlight ? root.panelColor : root.fontColor
        elide: Text.ElideRight
        font.pixelSize: InputStyle.fontPixelSizeNormal
        font.weight: Font.Bold
        height: parent.height
        horizontalAlignment: Text.AlignLeft
        text: root.contentText
        verticalAlignment: Text.AlignVCenter
        width: parent.width
      }
    }
  }
  Rectangle {
    id: borderLine
    anchors.bottom: parent ? parent.bottom : undefined
    color: root.highlightColor
    height: 1
    visible: root.showBorder
    width: row.width
  }
}
