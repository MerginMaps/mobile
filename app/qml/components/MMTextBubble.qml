
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
import QtQuick.Layouts

Item {
  id: root

  height: 100
  width: 300

  required property var title
  required property var description

  Rectangle {
      width: root.width
      height: root.height
      color: __style.whiteColor
      radius: 15
  }

  Row {
      padding: 10
      spacing: 10
      anchors.fill: parent

      Rectangle {
          implicitWidth: radius * 2
          implicitHeight: radius * 2
          // x: control.leftPadding
          y: parent.height / 2 - height / 2
          radius: 20
          color: __style.forestColor

          MMIcon {
            id: icon
            anchors.centerIn: parent
            source: __style.bubbleIcon
          }
        }

      Column {
          id: column
          width: 2* root.width / 3
          spacing: 10
          Label {
              text: root.title
              font: __style.t1
              color: __style.nightColor
              wrapMode: Label.WordWrap
          }

          Label {
              text:root.description
              font: __style.p5
              color: __style.forestColor
              wrapMode: Label.WordWrap
              width: column.width
          }
      }
  }
}
