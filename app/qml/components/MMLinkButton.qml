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

  contentItem: Text {
    anchors.centerIn: control
    font: __style.t3
    text: control.text
    leftPadding: 32 * __dp
    rightPadding: 32 * __dp
    topPadding: 10 * __dp
    bottomPadding: 10 * __dp
    color: control.enabled ? __style.forestColor : __style.mediumGreenColor
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    elide: Text.ElideRight
  }

  background: Rectangle {
    color: control.enabled ? control.down || control.hovered ? __style.grassColor : __style.whiteColor : __style.whiteColor
    border.color: control.enabled ? control.down || control.hovered ? __style.transparentColor : __style.forestColor : __style.mediumGreenColor
    border.width: 2 * __dp
    radius: height / 2
  }
}
