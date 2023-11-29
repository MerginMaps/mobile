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

RoundButton {
  id: control

  implicitWidth: icon.width * 2
  implicitHeight: icon.height * 2

  StyleV2 { id: styleV2 }

  contentItem: MMIcon {
    id: icon

    source: styleV2.arrowLinkRightIcon
    color: control.enabled ? styleV2.forestColor : styleV2.mediumGreenColor
  }

  background: Rectangle {
    color: control.enabled ? control.down || control.hovered ? styleV2.grassColor : styleV2.whiteColor : styleV2.whiteColor
    border.color: control.enabled ? control.down || control.hovered ? styleV2.transparentColor : styleV2.forestColor : styleV2.mediumGreenColor

    border.width: 2 * __dp
    radius: control.implicitHeight / 2
  }
}
