/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Layouts
import "."

Item {
  id: root

  height: __style.mapItemHeight

  signal clicked

  property string text
  property url leftIconSource: ""

  Rectangle {
    width: parent.width
    height: parent.height

    radius: height / 2

    color: __style.whiteColor

    layer.enabled: true
    layer.effect: MMShadow {}

    RowLayout {
      id: row

      anchors {
        fill: parent
        leftMargin: 15 * __dp
        rightMargin: 15 * __dp
      }

      spacing: 0

      MMIcon {
        size: __style.icon24

        source: root.leftIconSource
      }

      Text {
        id: text

        Layout.fillWidth: true
        Layout.leftMargin: 10 * __dp
        Layout.rightMargin: 2 * __dp
        Layout.preferredHeight: parent.height

        text: root.text
        font: __style.p5
        color: __style.nightColor

        elide: Text.ElideRight
        verticalAlignment: Text.AlignVCenter
      }

      MMIcon {
        size: __style.icon24

        color: __style.forestColor
        source: __style.arrowDownIcon
      }
    }

    MouseArea {
      anchors.fill: parent
      onClicked: root.clicked()
    }
  }
}
