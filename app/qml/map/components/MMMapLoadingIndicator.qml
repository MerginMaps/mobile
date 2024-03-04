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

Rectangle {
  id: root
  color: __style.forestColor

  height: root.visible ? 7 * __dp : 0
  property alias running: root.visible

  Rectangle {
    id: bar
    width: parent.width
    height: parent.height

    PropertyAnimation {
      running: root.visible
      target: bar
      property: "x"
      from: -bar.width
      to: bar.width
      duration: 1500
      loops: Animation.Infinite
    }

    LinearGradient {
      anchors.fill: bar
      start: Qt.point(0, 0)
      end: Qt.point(bar.width, 0)
      source: bar
      gradient: Gradient {
        GradientStop { position: 0.0; color: __style.forestColor }
        GradientStop { position: 0.5; color: Qt.lighter(__style.forestColor, 2) }
        GradientStop { position: 1.0; color: __style.forestColor }
      }
    }
  }
}
