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
    //timepicker.setTime(toDate)
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

          width: parent.width
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

//        MouseArea {
//          anchors.fill: parent
//          onClicked: dateYearTumbler.visible = !dateYearTumbler.visible
//        }

//        MMTimeTumbler {
//          id: dateYearTumbler

//          anchors.left: monthYearRow.left
//          anchors.top: monthYearRow.bottom
//          anchors.topMargin: 15 * __dp
//          visible: false
//        }

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

      onSelectedDate: function (date) { root.dateToSelect = date }

      function navigateToDate(ddate) {
        monthGrid.year = ddate.getFullYear()
        monthGrid.month = ddate.getMonth()
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
        text: "Time"
        font: __style.t1
        color: __style.nightColor
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
      }

      Row {
        anchors.right: parent.right
        spacing: 10 * __dp

        Rectangle {
          width: timeText.width
          height: timeRow.height
          color: __style.lightGreenColor
          radius: height / 2

          Text {
            id: timeText

            anchors.centerIn: parent
            leftPadding: 20 * __dp
            rightPadding: 20 * __dp

            font:  __style.h3
            color: __style.forestColor
            text: "11:38:00"

            MouseArea {
              anchors.fill: parent
              onClicked: timeTumbler.visible = !timeTumbler.visible
            }

            function setTime(ddatetime) {
              let hour = ddatetime.getHours()
              let minutes = ddatetime.getMinutes()
              let seconds = ddatetime.getSeconds()

              text = (hour + ":" + minutes + ":" + seconds)

              //                hoursTumbler.currentIndex = hour
              //                minutesTumbler.currentIndex = minutes
              //                secondsTumbler.currentIndex = seconds
            }
          }
          MMTimeTumbler {
            id: timeTumbler

            anchors.right: timeText.right
            anchors.bottom: timeText.top
            anchors.bottomMargin: 25 * __dp
            visible: false
          }
        }

        MMAmPmSwitch {}
      }
    }

  }
}
