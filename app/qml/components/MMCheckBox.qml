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
  id: root

  property bool small: false

  width: (root.small ? 16 : 24) * __dp
  height: root.width

  indicator: Rectangle {
    width: root.width
    height: root.height
    y: root.height / 2 - height / 2
    radius: 5 * __dp
    color: (enabled && root.checked) ? __style.grassColor: __style.polarColor
    border.color: {
      if(enabled) {
        if(checked) {
          return __style.grassColor
        }
        return __style.forestColor
      }
      return __style.mediumGreenColor
    }
    border.width: (root.hovered ? 2.5 : 2) * __dp

    MMIcon {
      id: icon

      anchors.centerIn: parent
      source: __style.checkmarkIcon
      color: root.enabled ? __style.forestColor : __style.mediumGreenColor
      visible: root.checked
      size: root.small ? __style.icon16 : __style.icon24
    }
  }

  contentItem: Text {
    text: root.text
    font: __style.p5
    color: icon.color
    verticalAlignment: Text.AlignVCenter
    leftPadding: root.indicator.width
  }
}
