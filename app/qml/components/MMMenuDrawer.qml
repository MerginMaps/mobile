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

Drawer {
  id: control

  property alias title: header.title
  property alias model: menuView.model

  signal clicked(var button)

  width: window.width
  height: mainColumn.height
  edge: Qt.BottomEdge
  dragMargin: 0 // do not open by drag gesture

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
    color: __style.whiteColor

    Column {
      id: mainColumn

      width: parent.width
      spacing: __style.pageMargins
      leftPadding: __style.pageMargins
      rightPadding:__style.pageMargins
      bottomPadding: __style.pageMargins

      MMPageHeader {
        id: header
        backVisible: false
        width: parent.width
        titleFont: __style.t2
        // TODO
        // rightMarginShift: closeButton.width + __style.pageMargins

        MMRoundButton {
          id: closeButton
          iconSource: __style.closeIcon
          bgndColor: __style.lightGreenColor
          anchors.right: parent.right
          anchors.rightMargin: 2 * __style.pageMargins
          onClicked: control.visible = false
        }
      }

      GridView {
        id: menuView

        width: parent.width - 40 * __dp
        height: model ? model.count * __style.menuDrawerHeight : 0
        cellWidth: width
        cellHeight: __style.menuDrawerHeight
        interactive: false
      }
    }
  }
}
