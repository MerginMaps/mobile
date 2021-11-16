/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.0
import QtGraphicalEffects 1.0
import "."  // import InputStyle singleton

Item {
  id: root
  property int maxValue: 99
  property int minValue: 0
  property real rowHeight: height
  property string suffix
  property real value: 0

  Image {
    id: imageDecrease
    fillMode: Image.PreserveAspectFit
    height: root.height
    source: InputStyle.backIcon
    sourceSize.height: height
    sourceSize.width: width
    width: height
  }
  ColorOverlay {
    anchors.fill: imageDecrease
    color: InputStyle.fontColorBright
    source: imageDecrease
  }
  Text {
    id: valueText
    anchors.left: imageDecrease.right
    color: InputStyle.fontColorBright
    font.pixelSize: InputStyle.fontPixelSizeNormal
    height: root.height
    horizontalAlignment: Text.AlignHCenter
    text: value + suffix
    verticalAlignment: Text.AlignVCenter
    width: root.width - (2 * imageIncrease.width)
  }
  Image {
    id: imageIncrease
    anchors.left: valueText.right
    fillMode: Image.PreserveAspectFit
    height: root.height
    rotation: 180
    source: InputStyle.backIcon
    sourceSize.height: height
    sourceSize.width: width
    width: height
  }
  ColorOverlay {
    anchors.fill: imageIncrease
    color: InputStyle.fontColorBright
    rotation: imageIncrease.rotation
    source: imageIncrease
  }

  //  Mouse areas enlarging clickable areas for arrows, see https://github.com/lutraconsulting/input/pull/1055
  MouseArea {
    height: imageDecrease.height * 3
    width: imageDecrease.width * 3
    x: imageDecrease.x - imageDecrease.width
    y: imageDecrease.y - imageDecrease.height

    onClicked: if (minValue <= root.value - 1)
      root.value -= 1
  }
  MouseArea {
    height: imageIncrease.height * 3
    width: imageIncrease.width * 3
    x: imageIncrease.x - imageIncrease.width
    y: imageIncrease.y - imageIncrease.height

    onClicked: if (maxValue >= root.value + 1)
      root.value += 1
  }
}
