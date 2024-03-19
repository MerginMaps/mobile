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
import QtQuick.Layouts

Item {
  id: control

  signal clicked(var type)

  property var type
  property string text
  property string description
  property var iconSource

  property bool isActive

  width: control.width
  height: __style.menuDrawerHeight

  Rectangle {
    anchors.top: parent.top
    width: parent.width
    height: 1 * __dp
    color: __style.greyColor
  }

  RowLayout {
    height: parent.height
    width: parent.width
    spacing: 10 * __dp

    MMIcon {
      Layout.alignment: Qt.AlignVCenter
      size: __style.icon24
      color: __style.forestColor
      source: control.iconSource ?? ""
    }

    Column {
      height: parent.height
      width: parent.width

      Text {
        width: parent.width
        text: control.text
        color: __style.nightColor
        font: __style.t3
      }

      Text {
        width: parent.width
        text: control.description
        color: __style.nightColor
        font: __style.p6
        visible: control.description
      }
    }

    MMIcon {
      id: icon

      Layout.alignment: Qt.AlignVCenter
      size: __style.icon24
      color: __style.forestColor
      source: __style.doneCircleIcon
      visible: control.isActive
    }
  }

  MouseArea {
    anchors.fill: parent
    onClicked: control.clicked(control.type)
  }
}
