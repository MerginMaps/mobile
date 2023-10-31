/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import "../Style.js" as Style
import "."

Item {
  id: control

  width: text.width
  height: Style.mapItemHeight

  signal clicked

  required property string text
  property url iconSource: ""
  property color bgColor: Style.positive
  property color textColor: Style.forest

  Rectangle {
    width: row.width
    height: parent.height
    radius: control.height / 2
    color: control.bgColor

    layer.enabled: true
    layer.effect: MMShadow {}

    Row {
      id: row

      anchors.centerIn: parent
      leftPadding: 20 * __dp
      rightPadding: leftPadding
      spacing: 4 * __dp
      height: parent.height

      MMIcon {
        id: icon

        source: control.iconSource ? control.iconSource : ""
        color: text.color
        height: parent.height
      }

      Text {
        id: text

        color: control.textColor
        text: control.text
        font: Qt.font(Style.t3)
        verticalAlignment: Text.AlignVCenter
        height: parent.height
      }
    }

    MouseArea {
      anchors.fill: parent
      onClicked: control.clicked()
    }
  }
}
