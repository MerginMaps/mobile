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

  property color bgColor: __style.whiteColor
  property var image: __style.bubbleImage

  Rectangle {
    width: root.width
    height: row.height
    color: root.bgColor
    radius: __style.inputRadius
  }

  RowLayout {
    id: row

    spacing: 0
    height: __style.row114

    Item {
      height: row.height
      width: __style.margin12
    }

    Image {
      id: icon

      Layout.alignment: Qt.AlignLeft
      Layout.minimumWidth: width

      width: 50 * __dp
      height: width
      source: root.image
    }

    Item {
      height: row.height
      width: __style.margin20
    }

    ColumnLayout {
      id: column

      Layout.fillWidth: true
      Layout.fillHeight: true
      spacing: __style.margin12

      Text {
        width: column.width
        height: __style.row36

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

    Item {
      height: row.height
      width: __style.margin12
    }
  }
}
