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
import ".."

CheckBox {
  id: control

  checked: true

  indicator: Rectangle {
    implicitWidth: 24
    implicitHeight: 24
    x: control.leftPadding
    y: parent.height / 2 - height / 2
    radius: 5
    color: enabled ? ( control.checked ? StyleV2.grassColor: StyleV2.whiteColor ) : StyleV2.whiteColor
    border.color: enabled ? ( control.checked ? StyleV2.grassColor: StyleV2.forestColor ) : StyleV2.mediumGreenColor
    border.width: control.hovered ? 2.5 : 2

    MMIcon {
      id: icon

      anchors.centerIn: parent
      source: StyleV2.checkmarkIcon
      color: control.enabled ? StyleV2.forestColor : StyleV2.mediumGreenColor
      visible: control.checked
    }
  }

  contentItem: Text {
    text: control.text
    font: StyleV2.p5
    color: icon.color
    verticalAlignment: Text.AlignVCenter
    leftPadding: control.indicator.width + control.spacing
  }
}
