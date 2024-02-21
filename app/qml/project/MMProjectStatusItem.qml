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

import "../components"
import lc 1.0

Rectangle {
  id: root

  height: __style.row36
  required property var /* MerginProjectStatusModel */ type
  property int count: -1 // negative number: do not show count
  property string title: "" // empty string: show Added, Removed or Edited based on type

  color: internal.bgColor
  radius: height / 4

  RowLayout {
    anchors.fill: parent

    Item { // spacer
      width: __style.margin8
      height: parent.height
    }

    MMIcon {
      Layout.alignment: Qt.AlignVCenter
      source: internal.icon
      size: __style.icon16
      color: internal.fgColor
    }

    Item { // spacer
      width: __style.margin8
      height: parent.height
    }

    Text {
      Layout.alignment: Qt.AlignVCenter
      text: internal.text
      font: __style.t4
      color: internal.fgColor
      Layout.fillWidth: true
    }

    Rectangle {
      Layout.alignment: Qt.AlignVCenter
      visible: root.count > 0
      height: root.height - __style.margin8
      width: Math.max(countText.width + __style.margin8, height)
      radius: height / 2
      color: internal.fgColor

      Text {
        id: countText
        anchors.centerIn: parent
        color: internal.bgColor
        text: root.count
      }
    }

    Item {
      width: __style.margin8
      height: parent.height
    }
  }

  QtObject {
    id: internal

    property var icon: {
      if (root.type === MerginProjectStatusModel.Added ) return __style.plusIcon
      else if (root.type === MerginProjectStatusModel.Deleted) return __style.deleteIcon
      else if (root.type === MerginProjectStatusModel.Updated) return __style.editIcon
      else return ""
    }

    property var text: {
      if (root.title)
        return root.title

      if (root.type === MerginProjectStatusModel.Added ) return qsTr("Added")
      else if (root.type === MerginProjectStatusModel.Deleted) return qsTr("Removed")
      else if (root.type === MerginProjectStatusModel.Updated) return qsTr("Edited")
      else return ""
    }
    property color bgColor: {
      if (root.type === MerginProjectStatusModel.Added ) return __style.positiveColor
      else if (root.type === MerginProjectStatusModel.Deleted) return __style.negativeColor
      else if (root.type === MerginProjectStatusModel.Updated) return __style.warningColor
      else return __style.transparentColor
    }

    property color fgColor: {
      if (root.type === MerginProjectStatusModel.Added ) return __style.forestColor
      else if (root.type === MerginProjectStatusModel.Deleted) return __style.earthColor
      else if (root.type === MerginProjectStatusModel.Updated) return __style.grapeColor
      else return __style.transparentColor
    }
  }
}
