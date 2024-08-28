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
import "../../components"


Item {
  id: control

  implicitWidth: row.width
  implicitHeight: __style.mapItemHeight

  signal clicked

  required property string text

  property real maxWidth: implicitWidth
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
        property real spacing:  5 * __dp
        anchors.verticalCenter: parent.verticalCenter
        color: control.textBgColorInverted ? control.textColor : control.bgColor
        height: text.height + spacing
        width: text.width + 3 * spacing
        radius: height / 2

        Text {
          id: text

          property real textSurroundingItemsWidth: textBg.spacing + icon.width + row.spacing + 2 * row.leftPadding

          width: ( implicitWidth + textSurroundingItemsWidth ) > control.maxWidth ? control.maxWidth - textSurroundingItemsWidth : implicitWidth
          anchors.centerIn: parent
          color: control.textBgColorInverted ? control.bgColor : control.textColor
          text: control.text
          font: __style.t3
          elide: Text.ElideRight
        }
      }
    }

    MouseArea {
      anchors.fill: parent
      onClicked: control.clicked()
    }
  }
}
