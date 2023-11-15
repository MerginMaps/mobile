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

Item {
  id: control

  signal clicked

  required property var iconSource
  required property string text

  Item {
    id: menuButton

    width: control.width
    height: Style.menuDrawerHeight

    Rectangle {
      anchors.top: parent.top
      width: parent.width
      height: 1 * __dp
      color: Style.gray
    }

    Row {
      height: parent.height
      width: parent.width
      spacing: Style.commonSpacing
      MMIcon {
        height: parent.height
        color: Style.forest
        source: control.iconSource
      }
      Text {
        text: control.text
        color: Style.forest
        font: Qt.font(Style.t3)
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
