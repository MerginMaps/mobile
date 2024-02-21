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

import "../inputs"
import "../components"

Item {
  id: root

  required property string title
  property string description
  required property bool checked

  height: mainRow.height

  signal clicked
  signal valueWasChanged ( var newValue )

  Row {
    id: mainRow

    width: parent.width

    Column {
      id: mainColumn

      width: parent.width * 0.6
      spacing: 8 * __dp

      Text {
        width: parent.width

        text: root.title
        wrapMode: Text.WordWrap
        font: __style.t3
        color: __style.nightColor
      }

      Text {
        width: parent.width

        text: root.description
        wrapMode: Text.WordWrap
        font: __style.p6
        color: __style.nightColor
        lineHeight: 1.6
      }
    }

    Item {
      width: parent.width * 0.4
      height: mainColumn.height

      MMSwitch {
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        spacing: -8 * __dp
        checked: root.checked
      }
    }
  }

  MouseArea {
    anchors.fill: parent
    onClicked: root.clicked()
  }
}
