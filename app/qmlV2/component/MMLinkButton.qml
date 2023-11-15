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
import ".."

Button {
  id: control

  contentItem: Text {
    anchors.centerIn: control
    font: StyleV2.t3
    text: control.text
    leftPadding: 32 * __dp
    rightPadding: 32 * __dp
    topPadding: 10 * __dp
    bottomPadding: 10 * __dp
    color: control.enabled ? StyleV2.forestColor : StyleV2.mediumGreenColor
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    elide: Text.ElideRight
  }

  background: Rectangle {
    color: control.enabled ? control.down || control.hovered ? StyleV2.grassColor : StyleV2.whiteColor : StyleV2.whiteColor
    border.color: control.enabled ? control.down || control.hovered ? StyleV2.transparentColor : StyleV2.forestColor : StyleV2.mediumGreenColor
    border.width: 2 * __dp
    radius: height / 2
  }
}
