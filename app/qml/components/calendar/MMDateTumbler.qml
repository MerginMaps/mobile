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
import ".."

Item {
  id: control

  width: row.width + 40 * __dp
  height: row.height

  signal monthIndexChanged(var monthIndex)
  signal yearChanged(var year)
  property int initMonthIndex
  property int initYear

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

    MouseArea {
      anchors.fill: parent
    }
  }

  Row {
    id: row

    anchors.horizontalCenter: parent.horizontalCenter

    MMTumbler {
      id: monthsTumbler

      model: control.monthList()
      currentIndex: control.initMonthIndex
      width: 120 * __dp
      onCurrentIndexChanged: control.monthIndexChanged(currentIndex)
    }

    MMTumbler {
      id: yearsTumble

      model: control.yearList()
      currentIndex: control.initYear - __style.calendarYearFrom
      onCurrentItemChanged: control.yearChanged(parseInt(currentItem.text))
    }
  }

  function monthList() {
    const monthList = Array(12).keys();
    const getMonthName = (monthIndex) => Qt.locale().monthName(monthIndex)
    return Array.from(monthList, getMonthName)
  }

  function yearList() {
    var years = []
    for (var i = __style.calendarYearFrom; i <= __style.calendarYearTo; ++i)
      years.push(i)
    return years
  }
}
