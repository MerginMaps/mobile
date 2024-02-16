/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import "."

Item {
  id: control

  width: text.width
  height: __style.mapItemHeight

  signal clicked

  required property string text
  property url iconSource: ""
  property color bgColor: __style.positiveColor
  property color textColor: __style.forestColor

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
        anchors.verticalCenter: parent.verticalCenter
        source: control.iconSource ? control.iconSource : ""
        color: text.color
        size: __style.icon24
      }

      Text {
        id: text

        color: control.textColor
        text: control.text
        font: __style.t3
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
