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
    spacing: 10

    Text {
      font: Qt.font(Style.t3)
      text: control.text
      color: control.enabled ? control.down || control.hovered ? Style.grass : Style.forest : Style.forest
      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
      elide: Text.ElideRight
      anchors.verticalCenter: parent.verticalCenter
    }

    Image {
      source: Style.arrowLinkRight
      anchors.verticalCenter: parent.verticalCenter
    }
  }

  background: Rectangle {
    color: Style.transparent
  }
}
