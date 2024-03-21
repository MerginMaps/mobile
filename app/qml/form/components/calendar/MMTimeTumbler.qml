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

import "../../../components" as MMComponents

Item {
  id: root

  width: row.width + 40 * __dp
  height: row.height

  property alias hours: hoursTumbler.currentIndex
  property alias minutes: minutesTumbler.currentIndex
  property alias seconds: secondsTumbler.currentIndex
  property bool showSeconds: false

  Rectangle {
    width: parent.width
    height: parent.height
    anchors.horizontalCenter: parent.horizontalCenter

    color: __style.polarColor
    radius: 20 * __dp

    layer.enabled: true
    layer.effect: MMComponents.MMShadow {
      radius: 20 * __dp
    }

    MouseArea {
      anchors.fill: parent
    }
  }

  Rectangle {
    anchors {
      left: parent.left
      leftMargin: 12 * __dp
      right: parent.right
      rightMargin: 12 * __dp
      verticalCenter: parent.verticalCenter
    }

    height: 54 * __dp
    radius: 8 * __dp

    color: __style.lightGreenColor
  }

  Row {
    id: row

    anchors.horizontalCenter: parent.horizontalCenter

    MMTumbler {
      id: hoursTumbler
      model: 24
    }

    MMTumbler {
      id: minutesTumbler
      model: 60
    }

    MMTumbler {
      id: secondsTumbler
      model: 60
      visible: root.showSeconds
    }
  }
}
