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

Item {
  id: root

  // Item to blur (map)
  property alias sourceItem: effect.sourceItem

  property alias fadeOutDuration: fadeOut.duration // in ms
  property alias timerInterval: timer.interval // in ms

  visible: false

  // Blurred map
  FastBlur {
    id: fastBlur

    width: parent.width
    height: parent.height

    radius: 32
    opacity: 0.8

    source: ShaderEffectSource {
      id: effect

      sourceRect: Qt.rect(root.x, root.y, fastBlur.width, fastBlur.height)
    }
   }

  // Colored background rectangle
  Rectangle {
    anchors.fill: parent
    color: __style.forestColor
    opacity: 0.1
    radius: parent.height / 2
  }

  NumberAnimation on opacity {
    id: fadeOut

    from: 1
    to: 0.0

    onStopped: {
      root.visible = false
      root.opacity = 1.0
    }
  }

  Timer {
    id: timer
    running: false
    repeat: false
    onTriggered: {
      fadeOut.start()
    }
  }

  onVisibleChanged: {
    if (root.visible) {
      restartAnimation()
    }
  }

  function restartAnimation() {
    fadeOut.stop()
    timer.restart()
  }
}
