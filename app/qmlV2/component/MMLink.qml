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

Button {
  id: control

  contentItem: Row {
    anchors.centerIn: control
    spacing: 10 * __dp

    Text {
      id: text

      font: Qt.font(Style.t3)
      text: control.text
      color: control.enabled ? control.down || control.hovered ? Style.night : Style.forest : Style.mediumGreen
      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
      elide: Text.ElideRight
      anchors.verticalCenter: parent.verticalCenter
    }

    MMIcon {
      source: Style.arrowLinkRightIcon
      color: text.color
    }
  }

  background: Rectangle {
    color: Style.transparent
  }
}
