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
import QtQuick.Layouts

Item {
  id: root

  height: row.height

  required property string title
  required property string description

  Rectangle {
    width: root.width
    height: row.height
    color: __style.whiteColor
    radius: __style.inputRadius
  }

  Row {
    id: row

    padding: 20 * __dp
    spacing: 10 * __dp

    Rectangle {
      width: 50 * __dp
      height: width
      radius: height / 2
      color: __style.forestColor

      Image {
        id: icon
        anchors.centerIn: parent
        source: __style.bubbleIcon
      }
    }

    Column {
      id: column

      width: root.width - icon.width - 4 * row.spacing - 2 * row.padding
      spacing: 10 * __dp

      Text {
        width: parent.width

        text: root.title
        font: __style.t3
        color: __style.deepOceanColor
        wrapMode: Label.WordWrap
      }

      Text {
        width: column.width

        text: root.description
        font: __style.p6
        color: __style.deepOceanColor
        wrapMode: Label.WordWrap
        lineHeight: 1.5
      }
    }
  }
}
