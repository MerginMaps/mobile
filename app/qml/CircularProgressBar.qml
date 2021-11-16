/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.7
import QtQuick.Controls.Styles 1.2
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import "."  // import InputStyle singleton

ProgressBar {
  id: root
  property color bgColor: InputStyle.panelBackgroundDark
  property real circleWidth: 8 * InputStyle.dp
  property color color: InputStyle.fontColor

  background: Rectangle {
    color: InputStyle.clrPanelMain
    implicitHeight: root.height
    implicitWidth: root.width
  }
  contentItem: Item {
    id: contentRoot
    height: root.height
    width: root.width

    Rectangle {
      id: outerRing
      anchors.centerIn: contentRoot
      antialiasing: true
      border.color: InputStyle.clrPanelMain
      border.width: 1
      color: InputStyle.panelBackgroundDark
      height: width
      radius: width / 2
      width: root.width
      z: 0

      ConicalGradient {
        anchors.fill: parent
        source: outerRing

        gradient: Gradient {
          GradientStop {
            color: InputStyle.fontColor
            position: 0.00
          }
          GradientStop {
            color: InputStyle.fontColor
            position: root.value
          }
          GradientStop {
            color: InputStyle.panelBackgroundDark
            position: root.value + 1e-6
          }
          GradientStop {
            color: InputStyle.panelBackgroundDark
            position: 1.00
          }
        }
      }

      transform: Rotation {
        angle: 90
        origin.x: outerRing.radius
        origin.y: outerRing.radius
      }
    }
    Rectangle {
      id: innerRing
      anchors.centerIn: contentRoot
      antialiasing: true
      color: InputStyle.clrPanelMain
      height: width
      radius: width / 2
      width: root.width - 2 * root.circleWidth
      z: 1
    }
  }
}
