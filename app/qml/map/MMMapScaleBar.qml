/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import Qt5Compat.GraphicalEffects
import lc 1.0
import "."

Item {
  id: root

  property alias mapSettings: scaleBarKit.mapSettings
  property alias preferredWidth: scaleBarKit.preferredWidth
  // Item to blur (map)
  property alias sourceItem: effect.sourceItem

  ScaleBarKit {
    id: scaleBarKit
  }

  width: scaleBarKit.width + 48 * __dp

  Rectangle {
    anchors.fill: fastBlur
    color: __style.forestColor
    opacity: 0.8
    radius: fastBlur.height / 2
  }

  FastBlur {
    id: fastBlur

    width: parent.width
    height: 45 * __dp

    radius: 32
    opacity: 0.8

    source: ShaderEffectSource {
      id: effect

      sourceRect: Qt.rect(root.x, root.y, fastBlur.width, fastBlur.height)
    }

    Column {
      width: scaleBarKit.width
      anchors.centerIn: parent

      Rectangle {
        width: parent.width
        height: 2 * __dp
        color: __style.forestColor

        Rectangle {
          anchors.top: parent.top
          anchors.left: parent.left
          anchors.topMargin: (width - height) / 2
          width: parent.height
          height: 6 * __dp
          color: parent.color
          radius: width / 2
        }

        Rectangle {
          anchors.top: parent.top
          anchors.right: parent.right
          anchors.topMargin: (width - height) / 2
          width: parent.height
          height: 6 * __dp
          color: parent.color
          radius: width / 2
        }
      }

      Text {
        id: text

        height: 26 * __dp
        anchors.horizontalCenter: parent.horizontalCenter

        text: scaleBarKit.distance + " " + scaleBarKit.units
        color: __style.forestColor
        font: __style.t3
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
      }
    }
  }

  NumberAnimation on opacity {
    id: fadeOut

    from: 0.4
    to: 0.0
    duration: 1000

    onStopped: {
      root.visible = false
      root.opacity = 1.0
    }
  }

  Timer {
    id: scaleBarTimer
    interval: 3000; running: false; repeat: false
    onTriggered: {
      fadeOut.start()
    }
  }

  onVisibleChanged: {
    if (root.visible) {
      fadeOut.stop()
      scaleBarTimer.restart()
    }
  }
}
