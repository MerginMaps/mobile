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

Button {
  id: control

  contentItem: Row {
    anchors.centerIn: control
    spacing: 10 * __dp

    Text {
      id: text

      font: __style.t3
      text: control.text
      color: control.enabled ? control.down || control.hovered ? __style.nightColor : __style.forestColor : __style.mediumGreenColor
      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
      elide: Text.ElideRight
      anchors.verticalCenter: parent.verticalCenter
    }

    MMIcon {
      source: __style.arrowLinkRightIcon
      color: text.color
    }
  }

  background: Rectangle {
    color: __style.transparentColor
  }
}
