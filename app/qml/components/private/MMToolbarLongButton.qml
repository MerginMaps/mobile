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

import "../" as MMComponents

Item {
  id: root

  signal clicked

  property var iconSource
  property color iconColor
  property color iconColorDisabled
  property color bgColor
  property string text

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

        MMComponents.MMIcon {
          id: icon

          source: root.iconSource
          color: root.enabled ? root.iconColor : root.iconColorDisabled
          anchors.verticalCenter: parent.verticalCenter
        }

        Text {
          id: text

          visible: button.width > 130 * __dp
          text: root.text
          color: root.enabled ? root.iconColor : root.iconColorDisabled
          font: __style.t3
          anchors.verticalCenter: parent.verticalCenter
        }
      }
    }

    background: Rectangle {
      color: root.bgColor
      radius: height / 2
    }

    onClicked: root.clicked()
  }
}
