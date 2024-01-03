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
  id: root

  implicitHeight: mainColumn.height

  property bool hasTimePicker: true
  property bool hasDatePicker: true

  property date dateToSelect: new Date()
  property var locale: Qt.locale()

  signal selected(date selectedDate)
  signal canceled()

  onDateToSelectChanged: setDate(dateToSelect)

  function setDate(toDate) {
    monthGrid.date = toDate
    monthGrid.navigateToDate(toDate)
    timeText.time = toDate
  }

  Column {
    id: mainColumn

    width: root.width - 40 * __dp
    spacing: 20 * __dp
    topPadding: 20 * __dp

    // Mount and Year
    Row {
      width: parent.width

      visible: root.hasDatePicker

      Item {
        width: parent.width
        height: monthYearRow.height

        Row {
          id: monthYearRow

          spacing: 5 * __dp

          Text {
            text: root.locale.standaloneMonthName( monthGrid.month, Locale.LongFormat )
            font: __style.t1
            color: __style.nightColor
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
          }

          Text {
            text: monthGrid.year
            font: __style.t1
            color: __style.nightColor
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
          }

          MMIcon {
            source: __style.arrowDownIcon
            anchors.verticalCenter: parent.verticalCenter
          }
        }

        MouseArea {
          anchors.fill: monthYearRow
          onClicked: {
            tumblerBgArea.visible = true
            dateYearTumbler.visible = true
          }
        }

        // Right icons for changing months
        Row {
          x: parent.width - spacing - previousIcon.width - nextIcon.width - 10 * __dp
          spacing: 30 * __dp

          MMIcon {
            id: previousIcon
            source: __style.previousIcon
            anchors.verticalCenter: parent.verticalCenter

            MouseArea {
              anchors.fill: parent
              onClicked: {
                if (monthGrid.month !== Calendar.January) {
                  monthGrid.month -= 1
                } else {
                  monthGrid.year -= 1
                  monthGrid.month = Calendar.December
                }
              }
            }
          }

          MMIcon {
            id: nextIcon
            source: __style.nextIcon
            anchors.verticalCenter: parent.verticalCenter

            MouseArea {
              anchors.fill: parent
              onClicked: {
                if (monthGrid.month !== Calendar.December) {
                  monthGrid.month += 1
                } else {
                  monthGrid.month = Calendar.January
                  monthGrid.year += 1
                }
              }
            }
          }
        }
      }
    }

    // Weekdays
    MMDayOfWeekRow {
      width: parent.width
      topPadding: 2 * __dp
      visible: root.hasDatePicker
    }

    // Calendar
    MMMonthGrid {
      id: monthGrid

      bottomPadding: 70 * __dp
      width: parent.width
      height: 330 * __dp

      locale: root.locale
      visible: root.hasDatePicker

      onSelectedDate: function (date) {
        root.dateToSelect.setDate(date.getDate())
        root.dateToSelect.setMonth(date.getMonth())
        root.dateToSelect.setFullYear(date.getFullYear())
      }

// FIXME: Trying to refresh GUI, sometimes contains wrong inplicit size and set it explicit doesn't work
//      Timer {
//        id: timer

//        interval: 1000
//        repeat: false
//        onTriggered: monthGrid.refresh()
//      }

//      Component.onCompleted: {
//        //Qt.callLater(monthGrid.refresh)
//        console.log("Ela implicitContentWidth = " + implicitContentWidth + " (parent width = " + parent.width + ")")
//        timer.start()
//        contentWidth = parent.width
//      }

//      function refresh() {
//        console.log("Hop implicitContentWidth = " + implicitContentWidth + " (parent width = " + parent.width + ")")
//        contentWidth = parent.width
//        monthGrid.update()
//      }

      function navigateToDate(date) {
        monthGrid.year = date.getFullYear()
        monthGrid.month = date.getMonth()
      }
    }

    Rectangle {
      width: parent.width
      height: 1 * __dp
      color: __style.grayColor
      visible: root.hasDatePicker && root.hasTimePicker
    }

    // Time
    Item {
      id: timeRow

      width: parent.width + 20 * __dp
      height: 56 * __dp
      visible: root.hasTimePicker

      Text {
        anchors.verticalCenter: parent.verticalCenter
        text: qsTr("Time")
        font: __style.t1
        color: __style.nightColor
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
      }

      Row {
        anchors.right: parent.right
        anchors.rightMargin: 20 * __dp
        spacing: 10 * __dp

        Rectangle {
          width: timeText.width
          height: timeRow.height
          color: __style.lightGreenColor
          radius: height / 2

          Text {
            id: timeText

            property date time

            anchors.centerIn: parent
            leftPadding: 20 * __dp
            rightPadding: 20 * __dp

            font:  __style.h3
            color: __style.forestColor

            MouseArea {
              anchors.fill: parent
              onClicked: {
                tumblerBgArea.visible = true
                timeTumbler.visible = true
              }
            }

            onTimeChanged: {
              let hours = time.getHours()
              let minutes = time.getMinutes()
              let seconds = time.getSeconds()

              timeText.text = (String(hours).padStart(2, '0') + ":" + String(minutes).padStart(2, '0') + ":" + String(seconds).padStart(2, '0'))

              timeTumbler.hours = hours
              timeTumbler.minutes = minutes
              timeTumbler.seconds = seconds
            }
          }
        }
      }
    }
  }

  // visible area when a tumbler is visible. Waits for a click to cloase the tumbler
  MouseArea {
    id: tumblerBgArea

    anchors.fill: mainColumn
    visible: false
    onClicked: {
      visible = false
      dateYearTumbler.visible = false
      timeTumbler.visible = false
    }
  }

  MMDateTumbler {
    id: dateYearTumbler

    visible: false
    y: 50 * __dp

    initMonthIndex: root.dateToSelect.getMonth()
    initYear: root.dateToSelect.getFullYear()

    onMonthIndexChanged: function(monthIndex) {
      monthGrid.month = monthIndex
    }

    onYearChanged: function(year) {
      monthGrid.year = year
    }
  }

  MMTimeTumbler {
    id: timeTumbler

    x: mainColumn.width - timeTumbler.width
    y: mainColumn.height - timeTumbler.height - timeRow.height - 10 * __dp
    visible: false

    onHoursChanged: { timeText.time.setHours(hours); root.dateToSelect.setHours(hours) }
    onMinutesChanged: { timeText.time.setMinutes(minutes); root.dateToSelect.setMinutes(minutes) }
    onSecondsChanged: { timeText.time.setSeconds(seconds); root.dateToSelect.setSeconds(seconds) }
  }
}
