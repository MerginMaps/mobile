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

  property var sourceIcon: ""
  property bool small: false

  height: (control.small ? 50 : 80) * __dp

  indicator: Rectangle {
    id: iconBgRectangle
    width: (control.small ? 24 : 40) * __dp
    height: (control.small ? 24 : 40) * __dp
    x: 20 * __dp
    y: control.height / 2 - height / 2
    radius: width / 2
    color: control.checked ? __style.whiteColor : __style.lightGreenColor

    MMIcon {
      id: icon
      width: (control.small ? 16 : 24) * __dp
      height: (control.small ? 16 : 24) * __dp
      anchors.centerIn: parent
      source: control.sourceIcon
      color: __style.forestColor
    }
  }

  contentItem: Text {
    text: control.text
    font: __style.t3
    color: control.checked ? __style.whiteColor : __style.nightColor
    verticalAlignment: Text.AlignVCenter
    leftPadding: control.indicator.width + 30 * __dp
    rightPadding: 20 * __dp
  }

  background: Rectangle {
    radius: __style.inputRadius
    color: control.checked ? __style.forestColor: __style.whiteColor
  }
}
