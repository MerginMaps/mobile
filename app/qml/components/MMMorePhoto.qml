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

Row {
  id: control

  property int hiddenPhotoCount: 0
  property alias source: image.source
  property int space: 0

  signal clicked()

  // left space
  Item { width: control.space; height: 1 }

  Image {
    id: image

    width: control.width - control.space
    height: width
    asynchronous: true
    layer.enabled: true
    layer {
      effect: OpacityMask {
        maskSource: Item {
          width: image.width
          height: width
          Rectangle {
            anchors.centerIn: parent
            width: image.width
            height: parent.height
            radius: 20 * __dp
          }
        }
      }
    }

    MouseArea {
      anchors.fill: parent
      onClicked: control.clicked()
    }

    Rectangle {
      anchors.centerIn: parent
      width: image.width
      height: parent.height
      radius: 20 * __dp
      color: __style.transparentColor
      border.color: __style.forestColor
      border.width: 1 * __dp
    }

    Image {
      id: bluredImage

      anchors.fill: parent
      source: image.source
      asynchronous: true
      layer.enabled: true

      layer {
        effect: FastBlur {
          anchors.fill: bluredImage
          source: bluredImage
          radius: 32
        }
      }
    }

    Column {
      anchors.centerIn: parent

      Image {
        source: __style.morePhotosIcon
        anchors.horizontalCenter: parent.horizontalCenter
      }

      Text {
        font: __style.t4
        text: qsTr("+%1 more").arg(control.hiddenPhotoCount)
        color: __style.whiteColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
      }
    }

    onStatusChanged: {
      if (status === Image.Error) {
        console.error("MMMorePhoto: Error loading image");
      }
    }
  }
}
