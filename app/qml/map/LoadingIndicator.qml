/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import ".."

Rectangle {
  id: loadingIndicator
  anchors.top: parent.top
  color: InputStyle.fontColor

  Rectangle {
    id: bar
    width: parent.width
    height: parent.height

    PropertyAnimation {
      running: loadingIndicator.visible
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
        GradientStop { position: 0.0; color: InputStyle.fontColor }
        GradientStop { position: 0.5; color: Qt.lighter(InputStyle.fontColor, 2) }
        GradientStop { position: 1.0; color: InputStyle.fontColor }
      }
    }
  }
}
