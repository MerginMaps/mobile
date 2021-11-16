/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import "../"  // import InputStyle singleton

Row {
  id: root
  property color bgColor: "white"
  property real fieldHeight: InputStyle.rowHeight
  property color fontColor: InputStyle.fontColor
  property color iconColor: InputStyle.fontColor
  property string source: ""
  property string text: ""
  property alias textItem: textItem

  spacing: 0

  signal linkActivated(string link)

  Rectangle {
    id: iconContainer
    color: root.bgColor
    height: fieldHeight
    width: fieldHeight

    Image {
      id: icon
      anchors.fill: parent
      anchors.margins: (fieldHeight / 4)
      fillMode: Image.PreserveAspectFit
      height: fieldHeight
      source: root.source
      sourceSize.height: height
      sourceSize.width: width
      width: fieldHeight
    }
    ColorOverlay {
      anchors.fill: icon
      color: root.iconColor
      source: icon
    }
  }
  Text {
    id: textItem
    color: root.fontColor
    font.pixelSize: InputStyle.fontPixelSizeNormal
    height: fieldHeight
    text: "<style>a:link { color: " + InputStyle.highlightColor + "; text-decoration: underline; }</style>" + root.text
    textFormat: Text.RichText
    verticalAlignment: Text.AlignVCenter
    width: parent.width - iconContainer.width
    wrapMode: Text.WordWrap

    onLinkActivated: root.linkActivated(link)
  }
}
