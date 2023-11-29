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

  contentItem: Row {
    anchors.centerIn: control
    spacing: 10 * __dp

    Text {
      id: text

      font: styleV2.t3
      text: control.text
      color: control.enabled ? control.down || control.hovered ? styleV2.nightColor : styleV2.forestColor : styleV2.mediumGreenColor
      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
      elide: Text.ElideRight
      anchors.verticalCenter: parent.verticalCenter
    }

    MMIcon {
      source: styleV2.arrowLinkRightIcon
      color: text.color
    }
  }

  background: Rectangle {
    color: styleV2.transparentColor
  }
}
