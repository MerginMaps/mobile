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
import "."
import ".."

Drawer {
  id: control

  property alias title: title.text
  property alias model: menuView.model

  signal clicked(var button)

  width: window.width
  height: mainColumn.height
  edge: Qt.BottomEdge

  Rectangle {
    color: roundedRect.color
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    height: 2 * radius
    anchors.topMargin: -radius
    radius: 20 * __dp
  }

  Rectangle {
    id: roundedRect

    anchors.fill: parent
    color: StyleV2.whiteColor

    Column {
      id: mainColumn

      width: parent.width
      spacing: 20 * __dp
      leftPadding: 20 * __dp
      rightPadding: 20 * __dp
      bottomPadding: 20 * __dp

      Image {
        id: closeButton

        source: StyleV2.closeButtonIcon
        anchors.right: parent.right
        anchors.rightMargin: 20 * __dp

        MouseArea {
          anchors.fill: parent
          onClicked: control.visible = false
        }
      }

      Text {
        id: title

        anchors.horizontalCenter: parent.horizontalCenter
        font: StyleV2.t1
        width: parent.width - 40 * __dp
        color: StyleV2.forestColor
        visible: text.length > 0
        horizontalAlignment: Text.AlignHCenter
      }

      GridView {
        id: menuView

        width: parent.width - 40 * __dp
        height: model ? model.count * StyleV2.menuDrawerHeight : 0
        cellWidth: width
        cellHeight: StyleV2.menuDrawerHeight
        interactive: false
      }
    }
  }
}
