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

CheckBox {
  id: control

  required property var sourceIcon
  required property var bgColorIcon

  width: (control.small ? 90 : 220) * __dp
  height: (control.small ? 30 : 50) * __dp

  indicator: Rectangle {
    id: iconBgRectangle
    width: (control.small ? 24 : 40) * __dp
    height: (control.small ? 24 : 40) * __dp
    y: control.height / 2 - height / 2
    radius: width / 2
    color: control.bgColorIcon

    MMIcon {
      id: icon
      width: (control.small ? 16 : 24) * __dp
      height: (control.small ? 16 : 24) * __dp
      anchors.centerIn: parent
      source: control.sourceIcon
    }
  }

  contentItem: Text {
    text: control.text
    font: __style.t3
    color: control.checked ? __style.whiteColor : __style.nightColor
    verticalAlignment: Text.AlignVCenter
    leftPadding: control.indicator.width
  }

  background: Rectangle {
    radius: 12 * __dp
    color: control.checked ? __style.forestColor: __style.whiteColor
  }
}
