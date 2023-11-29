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

  property bool transparent: false

  contentItem: Text {
    anchors.centerIn: control
    font: StyleV2.t3
    text: control.text
    leftPadding: 32 * __dp
    rightPadding: 32 * __dp
    topPadding: 10 * __dp
    bottomPadding: 10 * __dp
    color: control.enabled ? control.down || control.hovered ? StyleV2.grassColor : StyleV2.forestColor : StyleV2.forestColor
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    elide: Text.ElideRight
  }

  background: Rectangle {
    color: transparent ? "transparent" : control.enabled ? control.down || control.hovered ? StyleV2.forestColor : StyleV2.grassColor : StyleV2.mediumGreenColor
    radius: height / 2
  }
}
