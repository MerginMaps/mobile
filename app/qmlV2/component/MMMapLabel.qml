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

  width: text.width
  height: Style.commonItemHeight
  visible: control.text.length > 0

  signal clicked

  required property string text
  property color bgColor: Style.positive
  property color textColor: Style.forest

  Rectangle {
    width: parent.width
    height: parent.height
    radius: control.height / 2
    color: control.bgColor

    layer.enabled: true
    layer.effect: MMShadow {}

    Text {
      id: text

      anchors.centerIn: parent
      color: control.textColor
      text: control.text
      font: Qt.font(Style.t3)
      leftPadding: Style.commonSpacing
      rightPadding: Style.commonSpacing
    }

    MouseArea {
      anchors.fill: parent
      onClicked: control.clicked()
    }
  }
}
