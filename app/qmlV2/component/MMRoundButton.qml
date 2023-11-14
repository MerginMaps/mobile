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

  contentItem: MMIcon {
    id: icon

    source: StyleV2.arrowLinkRightIcon
    color: control.enabled ? control.down || control.hovered ? StyleV2.grassColor : StyleV2.forestColor : StyleV2.forestColor
  }

  background: Rectangle {
    color: control.enabled ? control.down || control.hovered ? StyleV2.forestColor : StyleV2.grassColor : StyleV2.mediumGreenColor
    radius: control.implicitHeight / 2
  }
}
