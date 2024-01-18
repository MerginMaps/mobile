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

Item {
  id: control

  signal clicked(type: string)

  required property string type
  required property string text
  required property var iconSource

  width: control.width
  height: __style.menuDrawerHeight

  Rectangle {
    anchors.top: parent.top
    width: parent.width
    height: 1 * __dp
    color: __style.greyColor
  }

  Row {
    height: parent.height
    width: parent.width
    spacing: 10 * __dp

    MMIcon {
      height: parent.height
      width: 20 * __dp
      color: __style.forestColor
      source: control.iconSource ?? ""
    }

    Text {
      height: parent.height
      verticalAlignment: Text.AlignVCenter
      text: control.text
      color: __style.nightColor
      font: __style.t3
    }
  }

  MouseArea {
    anchors.fill: parent
    onClicked: control.clicked(control.type)
  }
}
