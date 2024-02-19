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

import lc 1.0

Item {
  id: control

  signal clicked

  required property var iconSource
  required property string text
  property string rightText

  Item {
    id: menuButton

    width: control.width
    height: __style.menuDrawerHeight

    Rectangle {
      anchors.top: parent.top
      width: parent.width
      height: 1 * __dp
      color: __style.greyColor
    }

    Row {
      id: row
      height: parent.height
      width: parent.width - 2 * rightTextBg.width
      spacing: 12 * __dp

      MMIcon {
        anchors.verticalCenter: parent.verticalCenter
        color: __style.forestColor
        source: control.iconSource
        size: __style.icon24
      }

      Text {
        text: control.text
        color: __style.forestColor
        font: __style.t3
        verticalAlignment: Text.AlignVCenter
        height: parent.height
      }
    }

    Rectangle {
      // TODO: the layout of this active to right needs to be fixed!
      id: rightTextBg
      visible: control.rightText && row.width > 100
      property real spacing: 5
      anchors.left: row.right
      anchors.verticalCenter: parent.verticalCenter
      color: __style.forestColor
      height: rightText.height + spacing
      width: rightText.width + 3 * spacing
      radius: height / 2

      Text {
        id: rightText
        anchors.centerIn: parent
        color: __style.whiteColor
        text: control.rightText
        font: __style.t4
      }
    }

    MouseArea {
      anchors.fill: parent
      onClicked: control.clicked()
    }
  }
}
