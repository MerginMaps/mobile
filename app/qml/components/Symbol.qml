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
import "../" // import InputStyle singleton

Rectangle {
  id: iconContainer
  property color bgColor: "transparent"
  property color fontColor: InputStyle.fontColor
  property real iconSize: InputStyle.rowHeight
  property string source: ""

  color: iconContainer.bgColor
  height: iconContainer.iconSize
  width: iconContainer.iconSize

  Image {
    id: icon
    anchors.centerIn: parent
    fillMode: Image.PreserveAspectFit
    height: iconContainer.iconSize
    source: iconContainer.source
    sourceSize.height: height
    sourceSize.width: width
    width: iconContainer.iconSize
  }
  ColorOverlay {
    anchors.fill: icon
    color: iconContainer.fontColor
    source: icon
  }
}
