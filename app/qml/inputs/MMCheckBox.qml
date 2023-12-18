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
import "../components"

CheckBox {
  id: control

  property bool small: false

  width: (control.small ? 16 : 24) * __dp
  height: control.width

  indicator: Rectangle {
    width: control.width
    height: control.height
    y: control.height / 2 - height / 2
    radius: 5 * __dp
    color: (enabled && control.checked) ? __style.grassColor: __style.whiteColor
    border.color: {
      if(enabled) {
        if(checked) {
          return __style.grassColor
        }
        return __style.forestColor
      }
      return __style.mediumGreenColor
    }
    border.width: (control.hovered ? 2.5 : 2) * __dp

    MMIcon {
      id: icon

      anchors.centerIn: parent
      source: __style.checkmarkIcon
      color: control.enabled ? __style.forestColor : __style.mediumGreenColor
      visible: control.checked
      scale: control.width / (24 * __dp)
    }
  }

  contentItem: Text {
    text: control.text
    font: __style.p5
    color: icon.color
    verticalAlignment: Text.AlignVCenter
    leftPadding: control.indicator.width
  }
}
