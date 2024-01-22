/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

Row {
  id: root

  required property string title
  property color lineColor: __style.greyColor
  property color textColor: __style.nightColor

  spacing: 15

  Rectangle {
    id: leftLine

    width: (root.width - text.width) / 2 - root.spacing
    height: 1
    anchors.verticalCenter: parent.verticalCenter
    color: root.lineColor
  }

  Text {
    id: text

    text: root.title
    font: __style.t3
    color: root.textColor
    wrapMode: Text.WordWrap
  }

  Rectangle {
    width: leftLine.width
    height: leftLine.height
    color: leftLine.color
    anchors.verticalCenter: parent.verticalCenter
  }
}
