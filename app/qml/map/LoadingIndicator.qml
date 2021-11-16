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
import "../"

Rectangle {
  id: loadingIndicator
  anchors.top: parent.top
  color: InputStyle.fontColor

  Rectangle {
    id: bar
    height: parent.height
    width: parent.width

    PropertyAnimation {
      duration: 1500
      from: -bar.width
      loops: Animation.Infinite
      property: "x"
      running: loadingIndicator.visible
      target: bar
      to: bar.width
    }
    LinearGradient {
      anchors.fill: bar
      end: Qt.point(bar.width, 0)
      source: bar
      start: Qt.point(0, 0)

      gradient: Gradient {
        GradientStop {
          color: InputStyle.fontColor
          position: 0.0
        }
        GradientStop {
          color: Qt.lighter(InputStyle.fontColor, 2)
          position: 0.5
        }
        GradientStop {
          color: InputStyle.fontColor
          position: 1.0
        }
      }
    }
  }
}
