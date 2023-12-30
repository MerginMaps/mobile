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

Item {
  width: row.width + 40 * __dp
  height: row.height

  Rectangle {
    width: parent.width
    height: parent.height
    anchors.horizontalCenter: parent.horizontalCenter

    color: __style.whiteColor
    radius: 20 * __dp

    layer.enabled: true
    layer.effect: MMShadow {
      radius: 20 * __dp
    }
  }

  Row {
    id: row

    anchors.horizontalCenter: parent.horizontalCenter

    MMTumbler {
      id: hoursTumbler
      model: 12
      currentIndex: 10
    }

    MMTumbler {
      id: minutesTumbler
      model: 60
      currentIndex: 20
    }

    MMTumbler {
      id: secundesTumbler
      model: 60
      currentIndex: 40
    }
  }
}
