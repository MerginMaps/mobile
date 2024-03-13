/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

Item {
  id: root

  required property real xPos
  required property real yPos
  property bool hasPosition: true

  property real direction: 0
  property bool hasDirection: true
  
  property real horizontalAccuracy: 0.5
  property real accuracyRingSize: 50 * __dp
  
  property int size: 23 * __dp
  
  property bool trackingMode: false

  visible: root.hasPosition

  Rectangle {
    id: accuracyIndicator

    x: root.xPos - width / 2
    y: root.yPos - height / 2
    width: root.accuracyRingSize
    height: width
    visible: (root.horizontalAccuracy > 0) &&
             (accuracyIndicator.width > root.size / 2.0)
    color: root.trackingMode ? __style.earthColor : __style.forestColor
    radius: width / 2
    opacity: 0.2

    Behavior on width { NumberAnimation { properties: "width"; duration: 250 }}
  }

  Image {
    id: direction

    x: root.xPos - width / 2
    y: root.yPos - height
    width: root.size * 2
    height: width
    visible: root.hasDirection
    source: root.trackingMode ? __style.trackingDirectionImage : __style.directionImage
    fillMode: Image.PreserveAspectFit
    rotation: root.direction
    transformOrigin: Item.Bottom
    smooth: true

    Behavior on rotation { RotationAnimation { properties: "rotation"; direction: RotationAnimation.Shortest; duration: 500 }}
  }

  Rectangle {
    x: root.xPos - width / 2
    y: root.yPos - height / 2
    width: root.size
    height: width
    radius: width / 2
    color: __style.polarColor
  }

  Rectangle {
    x: root.xPos - width / 2
    y: root.yPos - height / 2
    width: root.size * 2 / 3
    height: width
    radius: width / 2
    color: root.trackingMode ? __style.earthColor : __style.forestColor
  }

  Rectangle {
    x: root.xPos - width / 2
    y: root.yPos - height / 2
    width: root.size / 3
    height: width
    radius: width / 2
    color: root.trackingMode ? __style.sunsetColor : __style.grassColor
  }
}

