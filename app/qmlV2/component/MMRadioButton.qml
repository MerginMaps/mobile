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

RadioButton {
  id: control

  checked: true

  StyleV2 { id: styleV2 }

  indicator: Rectangle {
    implicitWidth: 24
    implicitHeight: 24
    x: control.leftPadding
    y: parent.height / 2 - height / 2
    radius: 12
    color: enabled ? ( control.checked ? styleV2.grassColor: styleV2.whiteColor ) : styleV2.whiteColor
    border.color: enabled ? ( control.checked ? styleV2.grassColor: styleV2.forestColor ) : styleV2.mediumGreenColor
    border.width: control.hovered ? 2.5 : 2

    MMIcon {
      id: icon

      anchors.centerIn: parent
      source: styleV2.checkmarkIcon
      color: control.enabled ? styleV2.forestColor : styleV2.mediumGreenColor
      visible: control.checked
    }
  }

  contentItem: Text {
    text: control.text
    font: styleV2.p5
    color: icon.color
    verticalAlignment: Text.AlignVCenter
    leftPadding: control.indicator.width + control.spacing
  }
}
