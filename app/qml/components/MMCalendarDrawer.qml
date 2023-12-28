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

  property alias title: title.text
  property alias primaryButton: primaryButton.text
  property alias dateTime: dateTimePicker.dateToSelect

  signal primaryButtonClicked

  width: window.width
  height: mainColumn.height
  edge: Qt.BottomEdge
  dim: true
  interactive: false
  dragMargin: 0
  closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

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
      spacing: 20 * __dp
      leftPadding: 20 * __dp
      rightPadding: 20 * __dp
      bottomPadding: 20 * __dp

      Row {
        width: parent.width
        spacing: 10 * __dp

        Item {
          id: emptyItem

          width: closeButton.width
          height: 1
        }

        Text {
          id: title

          anchors.verticalCenter: parent.verticalCenter
          width: parent.width - emptyItem.width - closeButton.width - mainColumn.leftPadding - mainColumn.rightPadding - 2 * parent.spacing
          horizontalAlignment: Text.AlignHCenter
          wrapMode: Text.WordWrap
          font: __style.t2
          color: __style.forestColor
        }

        Image {
          id: closeButton

          source: __style.closeButtonIcon

          MouseArea {
            anchors.fill: parent
            onClicked: control.visible = false
          }
        }
      }

      MMDateTimePicker {
        id: dateTimePicker
        width: parent.width
      }

      MMButton {
        id: primaryButton

        width: parent.width - 2 * 20 * __dp
        visible: text.length > 0

        onClicked: {
          primaryButtonClicked()
          close()
        }
      }
    }
  }
}
