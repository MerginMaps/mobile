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

Item {
  id: root
  property real rectSize

  Item {
    id: captureZoneCorners
    anchors.centerIn: root
    width: root.rectSize
    height: root.rectSize

    GridLayout {
      columns: 2
      width: root.rectSize
      height: root.rectSize

      Repeater {
        model: [0, 1, 3, 2]

        delegate: Item {
          property var length: 30  * __dp
          property var borderWidth: 5 * __dp
          property var animationDuration: 1000
          property var primaryColor: InputStyle.fontColor
          property var secondaryColor: InputStyle.fontColorBright

          id: cornerItem
          Layout.fillHeight: true
          Layout.fillWidth: true
          rotation: modelData * 90

          Rectangle {
            anchors {
              top: parent.top
              left: parent.left
            }

            width: cornerItem.length
            height: cornerItem.borderWidth
            color: InputStyle.fontColor
            radius: width / 2

            SequentialAnimation on color {
              loops: Animation.Infinite
              ColorAnimation {
                from: cornerItem.primaryColor
                to: cornerItem.secondaryColor
                duration: cornerItem.animationDuration
              }
              ColorAnimation {
                from: cornerItem.secondaryColor
                to: cornerItem.primaryColor
                duration: cornerItem.animationDuration
              }
            }
          }

          Rectangle {
            anchors {
              top: parent.top
              left: parent.left
            }

            width: cornerItem.borderWidth
            height: cornerItem.length
            color: InputStyle.fontColor
            radius: width / 2

            SequentialAnimation on color {
              loops: Animation.Infinite
              ColorAnimation {
                from: cornerItem.primaryColor
                to: cornerItem.secondaryColor
                duration: cornerItem.animationDuration
              }
              ColorAnimation {
                from: cornerItem.secondaryColor
                to: cornerItem.primaryColor
                duration: cornerItem.animationDuration
              }
            }
          }
        }
      }
    }
  }
}
