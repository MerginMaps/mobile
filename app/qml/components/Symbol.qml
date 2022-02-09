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
import "./.." // import InputStyle singleton

Rectangle {

  property real iconSize: InputStyle.rowHeight
  property string source: ""
  property color bgColor: "transparent"
  property color iconColor: InputStyle.fontColor

  id: iconContainer
  height: iconContainer.iconSize
  width: iconContainer.iconSize
  color: iconContainer.bgColor

  Image {
    id: icon
    anchors.centerIn: parent
    source: iconContainer.source
    width: iconContainer.iconSize
    height: iconContainer.iconSize
    sourceSize.width: width
    sourceSize.height: height
    fillMode: Image.PreserveAspectFit
  }

  ColorOverlay {
    anchors.fill: icon
    source: icon
    color: iconContainer.iconColor
  }
}
