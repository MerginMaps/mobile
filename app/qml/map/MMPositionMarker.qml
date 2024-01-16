/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import lc 1.0

Item {
  id: control

  required property real xPos
  required property real yPos
  property real direction: 0
  property int size: 24 * __dp
  property real screenAccuracy: 50

  property bool trackingMode: false
  property bool withAccuracy: true
  property bool hasPosition: true
  property bool hasDirection: true
  property real horizontalAccuracy: 0.5

  Rectangle {
    id: accuracyIndicator

    x: control.xPos - width / 2
    y: control.yPos - height / 2
    width: control.screenAccuracy
    height: width
    visible: withAccuracy &&
             control.hasPosition &&
             (control.horizontalAccuracy > 0) &&
             (accuracyIndicator.width > control.size / 2.0)
    color: control.trackingMode ? __style.earthColor : __style.forestColor
    radius: width / 2
    opacity: 0.2

    Behavior on width { NumberAnimation { properties: "width"; duration: 250 }}
  }

  Image {
    id: direction

    x: control.xPos - width / 2
    y: control.yPos - height
    width: control.size * 2
    height: width
    visible: control.hasPosition && control.hasDirection
    source: control.trackingMode ? __style.trackingDirectionIcon : __style.directionIcon
    fillMode: Image.PreserveAspectFit
    rotation: control.direction
    transformOrigin: Item.Bottom
    smooth: true

    Behavior on rotation { RotationAnimation { properties: "rotation"; direction: RotationAnimation.Shortest; duration: 500 }}
  }

  Rectangle {
    id: navigation

    anchors.centerIn: accuracyIndicator
    width: control.size
    height: width
    visible: control.hasPosition
    radius: width / 2
    color: __style.whiteColor

    Rectangle {
      anchors.centerIn: parent
      width: control.size * 2 / 3
      height: width
      radius: width / 2
      color: __style.earthColor

      Rectangle {
        anchors.centerIn: parent
        width: control.size / 3
        height: width
        radius: width / 2
        color: control.trackingMode ? __style.sunsetColor : __style.grassColor
      }
    }
  }
}

