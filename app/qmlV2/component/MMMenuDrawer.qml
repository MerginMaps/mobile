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
import "../Style.js" as Style
import "."

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
    radius: Style.commonSpacing
  }

  Rectangle {
    id: roundedRect

    anchors.fill: parent
    color: Style.white

    Column {
      id: mainColumn

      width: parent.width
      spacing: Style.commonSpacing
      leftPadding: Style.commonSpacing
      rightPadding: Style.commonSpacing
      bottomPadding: Style.commonSpacing

      Image {
        id: closeButton

        source: Style.closeButtonIcon
        anchors.right: parent.right
        anchors.rightMargin: Style.commonSpacing

        MouseArea {
          anchors.fill: parent
          onClicked: control.visible = false
        }
      }

      Text {
        id: title

        anchors.horizontalCenter: parent.horizontalCenter
        font: Qt.font(Style.t1)
        width: parent.width - 2*Style.commonSpacing
        color: Style.forest
        visible: text.length > 0
        horizontalAlignment: Text.AlignHCenter
      }

      GridView {
        id: menuView

        width: parent.width - 2 * Style.commonSpacing
        height: model ? model.count * Style.menuDrawerHeight : 0
        cellWidth: width
        cellHeight: Style.menuDrawerHeight
      }
    }
  }
}
