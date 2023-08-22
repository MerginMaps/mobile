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
import "../Style.js" as Style
import "."

RoundButton {
  id: control

  implicitWidth: icon.width * 2
  implicitHeight: icon.height * 2

  contentItem: MMIcon {
    id: icon

    source: Style.arrowLinkRightIcon
    color: control.enabled ? control.down || control.hovered ? Style.grass : Style.forest : Style.forest
  }

  background: Rectangle {
    color: control.enabled ? control.down || control.hovered ? Style.forest : Style.grass : Style.mediumGreen
    radius: control.implicitHeight / 2
  }
}
