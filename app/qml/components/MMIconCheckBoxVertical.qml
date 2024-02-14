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

CheckBox {
  id: control

  property string sourceIcon: ""
  property color bgColorIcon: __style.forestColor

  width: 170 * __dp
  height: 158 * __dp

  indicator: Item {}

  contentItem: Column {
    padding: 20 * __dp
    spacing: 10 * __dp

    Item {
      width: parent.width
      height: 50 * __dp
      anchors.horizontalCenter: parent.horizontalCenter

      Rectangle {
        id: iconBgRectangle

        width: 50 * __dp
        height: width
        radius: width / 2
        anchors.horizontalCenter: parent.horizontalCenter
        color: control.bgColorIcon

        MMIcon {
          id: icon

          size: control.small ? __style.icon16 : __style.icon24
          anchors.centerIn: parent
          source: control.sourceIcon
        }
      }
    }
    Text {
      width: parent.width - 2 * parent.padding
      height: control.height - 2 * parent.padding - parent.spacing - iconBgRectangle.height
      anchors.horizontalCenter: parent.horizontalCenter

      text: control.text
      font: __style.t3
      color: control.checked ? __style.whiteColor : __style.nightColor
      verticalAlignment: Text.AlignVCenter
      horizontalAlignment: Text.AlignHCenter
      wrapMode: Text.WordWrap
      lineHeight: 1.5
    }
  }

  background: Rectangle {
    radius: __style.inputRadius
    color: control.checked ? __style.forestColor: __style.whiteColor
  }
}
