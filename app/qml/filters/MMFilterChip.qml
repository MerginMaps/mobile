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

import "../components"

Rectangle {
  id: root

  property string text: ""
  property bool selected: false

  signal clicked()

  implicitWidth: chipText.implicitWidth + __style.margin24
  implicitHeight: __style.row36

  radius: __style.radius20
  color: selected ? __style.grassColor : __style.lightGreenColor
  border.color: selected ? __style.grassColor : __style.mediumGreenColor
  border.width: 1

  MMText {
    id: chipText

    anchors.centerIn: parent

    text: root.text
    font: __style.p5
    color: selected ? __style.forestColor : __style.nightColor
  }

  MouseArea {
    anchors.fill: parent
    cursorShape: Qt.PointingHandCursor

    onClicked: root.clicked()
  }
}
