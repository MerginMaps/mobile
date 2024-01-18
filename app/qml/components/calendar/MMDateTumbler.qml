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
  id: root

  width: row.width + 40 * __dp
  height: row.height

  signal monthIndexChanged(var monthIndex)
  signal yearChanged(var year)
  property int initMonthIndex
  property int initYear

  readonly property int calendarYearFrom: 1900
  readonly property int calendarYearTo: 2050

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

      model: root.monthList()
      currentIndex: root.initMonthIndex
      width: 120 * __dp
      onCurrentIndexChanged: root.monthIndexChanged(currentIndex)
    }

    MMTumbler {
      id: yearsTumble

      model: root.yearList()
      currentIndex: root.initYear - root.calendarYearFrom
      onCurrentItemChanged: root.yearChanged(parseInt(currentItem.text))
    }
  }

  function monthList() {
    const monthList = Array(12).keys();
    const getMonthName = (monthIndex) => Qt.locale().monthName(monthIndex)
    return Array.from(monthList, getMonthName)
  }

  function yearList() {
    var years = []
    for (var i = root.calendarYearFrom; i <= root.calendarYearTo; ++i)
      years.push(i)
    return years
  }
}
