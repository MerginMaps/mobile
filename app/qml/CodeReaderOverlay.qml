/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.7
import QtQuick.Layouts 1.3
import QgsQuick 0.1 as QgsQuick

Item {
  id: root
  property real rectSize

  Item {
    id: captureZoneCorners
    anchors.centerIn: root
    height: root.rectSize
    width: root.rectSize

    GridLayout {
      columns: 2
      height: root.rectSize
      width: root.rectSize

      Repeater {
        model: [0, 1, 3, 2]

        delegate: Item {
          id: cornerItem
          property var animationDuration: 1000
          property var borderWidth: 5 * QgsQuick.Utils.dp
          property var length: 30 * QgsQuick.Utils.dp
          property var primaryColor: InputStyle.fontColor
          property var secondaryColor: InputStyle.fontColorBright

          Layout.fillHeight: true
          Layout.fillWidth: true
          rotation: modelData * 90

          Rectangle {
            color: InputStyle.fontColor
            height: cornerItem.borderWidth
            radius: width / 2
            width: cornerItem.length

            anchors {
              left: parent.left
              top: parent.top
            }

            SequentialAnimation on color  {
              loops: Animation.Infinite

              ColorAnimation {
                duration: cornerItem.animationDuration
                from: cornerItem.primaryColor
                to: cornerItem.secondaryColor
              }
              ColorAnimation {
                duration: cornerItem.animationDuration
                from: cornerItem.secondaryColor
                to: cornerItem.primaryColor
              }
            }
          }
          Rectangle {
            color: InputStyle.fontColor
            height: cornerItem.length
            radius: width / 2
            width: cornerItem.borderWidth

            anchors {
              left: parent.left
              top: parent.top
            }

            SequentialAnimation on color  {
              loops: Animation.Infinite

              ColorAnimation {
                duration: cornerItem.animationDuration
                from: cornerItem.primaryColor
                to: cornerItem.secondaryColor
              }
              ColorAnimation {
                duration: cornerItem.animationDuration
                from: cornerItem.secondaryColor
                to: cornerItem.primaryColor
              }
            }
          }
        }
      }
    }
  }
}
