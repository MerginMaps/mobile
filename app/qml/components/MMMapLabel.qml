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
  property bool textBgColorInverted: false

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
        color: control.textColor
        size: __style.icon24
      }

      Rectangle {
        id: textBg
        property real spacing: 5
        anchors.verticalCenter: parent.verticalCenter
        color: control.textBgColorInverted ? control.textColor : control.bgColor
        height: text.height + spacing
        width: text.width + 3 * spacing
        radius: height / 2

        Text {
          id: text
          anchors.centerIn: parent
          color: control.textBgColorInverted ? control.bgColor : control.textColor
          text: control.text
          font: __style.t3
        }
      }
    }

    MouseArea {
      anchors.fill: parent
      onClicked: control.clicked()
    }
  }
}
