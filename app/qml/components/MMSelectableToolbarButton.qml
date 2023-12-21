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
  id: control

  signal clicked

  required property url iconSource
  required property url selectedIconSource
  required property string text

  property bool checked: false

  height: __style.toolbarHeight

  Rectangle {
    width: parent.width - 10 * __dp
    height: parent.height - 10 * __dp
    anchors.centerIn: parent

    clip: true
    color: __style.transparentColor

    MMIcon {
      id: icon

      anchors.horizontalCenter: parent.horizontalCenter
      anchors.bottom: parent.bottom
      anchors.bottomMargin: 44 * __dp

      source: control.checked ? control.selectedIconSource : control.iconSource
      color: control.checked ? __style.whiteColor : __style.mediumGreenColor
    }

    Text {
      id: text

      width: parent.width
      anchors.horizontalCenter: parent.horizontalCenter
      anchors.bottom: parent.bottom
      anchors.bottomMargin: 20 * __dp

      text: control.text
      color: icon.color
      font: __style.t4
      horizontalAlignment: Text.AlignHCenter
      elide: Text.ElideMiddle
    }

    MouseArea {
      anchors.fill: parent
      onClicked: control.clicked()
    }
  }
}
