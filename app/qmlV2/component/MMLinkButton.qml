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

  StyleV2 { id: styleV2 }

  contentItem: Text {
    anchors.centerIn: control
    font: styleV2.t3
    text: control.text
    leftPadding: 32 * __dp
    rightPadding: 32 * __dp
    topPadding: 10 * __dp
    bottomPadding: 10 * __dp
    color: control.enabled ? styleV2.forestColor : styleV2.mediumGreenColor
    horizontalAlignment: Text.AlignHCenter
    verticalAlignment: Text.AlignVCenter
    elide: Text.ElideRight
  }

  background: Rectangle {
    color: control.enabled ? control.down || control.hovered ? styleV2.grassColor : styleV2.whiteColor : styleV2.whiteColor
    border.color: control.enabled ? control.down || control.hovered ? styleV2.transparentColor : styleV2.forestColor : styleV2.mediumGreenColor
    border.width: 2 * __dp
    radius: height / 2
  }
}
