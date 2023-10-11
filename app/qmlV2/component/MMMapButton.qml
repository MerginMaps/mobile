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
import QtQuick.Controls.Basic
import Qt5Compat.GraphicalEffects
import "../Style.js" as Style
import "."

Item {
  id: control

  width: 50
  height: 50

  Rectangle {
    id: border

    width: control.width + 3
    height: control.height + 3
    radius: width / 2
    color: "transparent"

    RadialGradient {
      id: light
      anchors.centerIn: parent

      width: control.width * 1.2
      height: control.height * 1.2

      gradient: Gradient {
        GradientStop { position: 0.3; color: "#80000000" }
        GradientStop { position: 0.5; color: "transparent" }
      }

      layer.enabled: true
      layer.effect: OpacityMask {
        id: mask
        maskSource: Rectangle {
          height: light.height
          width: light.width
          radius: width / 2
        }
      }
    }
  }

  Rectangle {
    id: rectangle

    width: parent.width
    height: parent.height
    radius: 25
    color: Style.white

    MouseArea {
      anchors.fill: parent
    }
  }
}
