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

  Rectangle {
    anchors.fill: parent
    color: __style.whiteColor
    z: -1

    MMIcon {
      anchors.centerIn: parent
      source: __style.morePhotosIcon
      color: __style.mediumGreenColor
      size: __style.icon32
    }
  }

  MouseArea {
    anchors.fill: parent
    onClicked: control.clicked(control.photoUrl)
  }

  onStatusChanged: {
    if (status === Image.Error) {
      console.error("MMPhoto: Error loading image: " + control.photoUrl);
    }
  }
}
