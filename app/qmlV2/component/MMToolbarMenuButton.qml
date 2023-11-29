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
import ".."

Item {
  id: control

  signal clicked

  required property var iconSource
  required property string text

  StyleV2 { id: styleV2 }

  Item {
    id: menuButton

    width: control.width
    height: styleV2.menuDrawerHeight

    Rectangle {
      anchors.top: parent.top
      width: parent.width
      height: 1 * __dp
      color: styleV2.grayColor
    }

    Row {
      height: parent.height
      width: parent.width
      spacing: 20 * __dp
      MMIcon {
        height: parent.height
        color: styleV2.forestColor
        source: control.iconSource
      }
      Text {
        text: control.text
        color: styleV2.forestColor
        font: styleV2.t3
        verticalAlignment: Text.AlignVCenter
        height: parent.height
      }
    }

    MouseArea {
      anchors.fill: parent
      onClicked: control.clicked()
    }
  }
}
