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

Item {
  id: root

  signal clicked

  required property var iconSource
  property color iconColor: __style.polarColor

  required property string text

  height: __style.toolbarHeight

  Button {
    id: button

    readonly property double maximumToolbarLongButtonWidth: 353 * __dp

    width: parent.width - 2 * __style.pageMargins < maximumToolbarLongButtonWidth ? parent.width - 2 * __style.pageMargins : maximumToolbarLongButtonWidth
    height: root.height - 2 * __style.margin10

    anchors.centerIn: parent

    contentItem: Item {
      width: parent.width
      anchors.centerIn: parent

      Row {
        id: row

        spacing: __style.margin6
        height: parent.height
        anchors.centerIn: parent

        MMIcon {
          id: icon

          source: root.iconSource
          color: root.iconColor
          anchors.verticalCenter: parent.verticalCenter
        }

        Text {
          id: text

          visible: button.width > 130 * __dp
          text: root.text
          color: __style.forestColor
          font: __style.t3
          anchors.verticalCenter: parent.verticalCenter
        }
      }
    }

    background: Rectangle {
      color: __style.grassColor
      radius: height / 2
    }

    onClicked: root.clicked()
  }
}
