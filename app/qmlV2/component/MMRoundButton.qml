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

  contentItem: MMIcon {
    id: icon

    source: Style.arrowLinkRight
    color: control.enabled ? control.down || control.hovered ? Style.grass : Style.forest : Style.forest
    implicitWidth: width + 5*__dp
    implicitHeight: height + 5*__dp
  }

  background: Rectangle {
    color: control.enabled ? control.down || control.hovered ? Style.forest : Style.grass : Style.mediumGreen
    radius: icon.implicitHeight
  }
}
