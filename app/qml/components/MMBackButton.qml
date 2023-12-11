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

RoundButton {
  id: control

  implicitWidth: 40 * __dp
  implicitHeight: 40 * __dp

  property color color: __style.whiteColor
  property color hoverColor: __style.mediumGreenColor

  contentItem: MMIcon {
    id: icon

    source: __style.backIcon
    color: __style.forestColor
  }

  background: Rectangle {
    color: control.down || control.hovered ? control.hoverColor : control.color
    radius: control.implicitHeight / 2
  }
}
