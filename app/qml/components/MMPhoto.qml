/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "."

Image {
  id: control

  property url photoUrl

  signal clicked( var path )

  height: width
  source: control.photoUrl
  asynchronous: true
  layer.enabled: true
  layer {
    effect: OpacityMask {
      maskSource: Item {
        width: control.width
        height: control.height
        Rectangle {
          anchors.centerIn: parent
          width: parent.width
          height: parent.height
          radius: 20 * __dp
        }
      }
    }
  }

  MouseArea {
    anchors.fill: parent
    onClicked: control.clicked(control.photoUrl)
  }

  Rectangle {
    anchors.centerIn: parent
    width: parent.width
    height: parent.height
    radius: 20 * __dp
    color: __style.transparentColor
    border.color: __style.forestColor
    border.width: 1 * __dp
  }

  onStatusChanged: {
    if (status === Image.Error) {
      console.error("MMPhoto: Error loading image: " + control.photoUrl);
    }
  }
}
