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
import "."

Button {
  id: control

  property url leftIcon: ""
  property url rightIcon: ""

  height: 50 * __dp

  contentItem: Item {
    Row {
      anchors.centerIn: parent
      spacing: 5 * __dp

      MMIcon {
        source: control.leftIcon
        color: text.color
      }

      Text {
        id: text

        font: __style.t3
        text: control.text
        color: control.enabled ? control.down || control.hovered ? __style.nightColor : __style.forestColor : __style.mediumGreenColor
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
        anchors.verticalCenter: parent.verticalCenter
      }

      MMIcon {
        source: control.rightIcon
        color: text.color
      }
    }
  }

  background: Rectangle {
    width: control.width
    height: control.height
    color: __style.transparentColor
  }
}
