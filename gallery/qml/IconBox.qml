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

import "../app/qml/components"

//******
// TODO REMOVE!

Column {
  spacing: 5
  id: root

  required property var source
  required property var text
  property bool colorise: false

  Text {
    text: root.text
  }

  Rectangle {
    width: 50
    height: 50
    border.color: "gray"
    color: __style.greyColor

    MMIcon {
      id: icon

      width: parent.width
      height: parent.height
      source: root.source
    }
  }

  Component.onCompleted: {
    if (root.colorise) {
      icon.color = "black"
    }
  }
}
