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
    property real rowHeight: height
    property real value: 0
    property string suffix
    property int maxValue: 99
    property int minValue: 0

    Image {
        id: imageDecrease
        height: root.height
        width: height
        source: InputStyle.backIcon
        sourceSize.width: width
        sourceSize.height: height
        fillMode: Image.PreserveAspectFit
    }

    ColorOverlay {
        anchors.fill: imageDecrease
        source: imageDecrease
        color: InputStyle.fontColorBright
    }

    Text {
        id: valueText
        text: value + suffix
        color: InputStyle.fontColorBright
        font.pixelSize: InputStyle.fontPixelSizeNormal
        height: root.height
        width: root.width - (2 * imageIncrease.width)
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        anchors.left: imageDecrease.right
    }

    Image {
        id: imageIncrease
        height: root.height
        width: height
        anchors.left: valueText.right
        source: InputStyle.backIcon
        rotation: 180
        sourceSize.width: width
        sourceSize.height: height
        fillMode: Image.PreserveAspectFit
    }

    ColorOverlay {
        anchors.fill: imageIncrease
        source: imageIncrease
        color: InputStyle.fontColorBright
        rotation: imageIncrease.rotation
    }

//  Mouse areas enlarging clickable areas for arrows, see https://github.com/lutraconsulting/input/pull/1055
    MouseArea {
      y: imageDecrease.y - imageDecrease.height
      x: imageDecrease.x - imageDecrease.width
      width: imageDecrease.width * 3
      height: imageDecrease.height * 3

      onClicked: if (minValue <= root.value - 1) root.value -=1
    }

    MouseArea {
      y: imageIncrease.y - imageIncrease.height
      x: imageIncrease.x - imageIncrease.width
      width: imageIncrease.width * 3
      height: imageIncrease.height * 3

      onClicked: if (maxValue >= root.value + 1) root.value +=1
    }
}
