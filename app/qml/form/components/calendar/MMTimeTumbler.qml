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

MMComponents.MMPopup {
  id: root

  width: row.width + 2 * __style.spacing20
  height: row.height

  property alias hours: hoursTumbler.currentIndex
  property alias minutes: minutesTumbler.currentIndex
  property alias seconds: secondsTumbler.currentIndex
  property bool showSeconds: false

  contentItem: Item {
    width: parent.width
    height: parent.height

    Rectangle {
      anchors.centerIn: parent

      width: parent.width - 2 * __style.margin12
      height: __style.row54
      radius: __style.radius8

      color: __style.lightGreenColor
    }

    Row {
      id: row

      anchors.centerIn: parent

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
}
