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
  property color color: InputStyle.fontColor
  property real circleWidth: 8 * InputStyle.dp

  background: Rectangle {
      implicitWidth: root.width
      implicitHeight: root.height
      color: InputStyle.clrPanelMain
  }

  contentItem: Item
  {
      id: contentRoot
      width: root.width
      height: root.height

          Rectangle
          {
             id: outerRing
             z: 0
             width: root.width
             height: width
             anchors.centerIn: contentRoot
             radius: width / 2
             antialiasing: true
             color: InputStyle.panelBackgroundDark
             transform: Rotation { origin.x: outerRing.radius; origin.y: outerRing.radius; angle: 90}
             border.color: InputStyle.clrPanelMain
             border.width: 1

             ConicalGradient
             {
                source: outerRing
                anchors.fill: parent
                gradient: Gradient
                {
                   GradientStop { position: 0.00; color: InputStyle.fontColor }
                   GradientStop { position: root.value; color: InputStyle.fontColor }
                   GradientStop { position: root.value + 1e-6; color: InputStyle.panelBackgroundDark  }
                   GradientStop { position: 1.00; color: InputStyle.panelBackgroundDark }
                }
             }
          }


          Rectangle
          {
             id: innerRing
             z: 1
             anchors.centerIn: contentRoot
             width: root.width - 2* root.circleWidth
             height: width
             radius: width / 2
             color: InputStyle.clrPanelMain
             antialiasing: true
          }
    }
}
