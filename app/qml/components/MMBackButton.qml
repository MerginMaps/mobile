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

  implicitWidth: icon.width * 2
  implicitHeight: icon.height * 2

  contentItem: MMIcon {
    id: icon

    source: __style.backIcon
    color: __style.forestColor
  }

  background: Rectangle {
    color: control.down || control.hovered ? __style.mediumGreenColor : __style.lightGreenColor
    radius: control.implicitHeight / 2
  }
}
