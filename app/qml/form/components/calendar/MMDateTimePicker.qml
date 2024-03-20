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

import "../../../components" as MMComponents

Item {
  id: root

  implicitHeight: mainColumn.height

  property bool hasTimePicker: true
  property bool hasDatePicker: true
  property bool showSeconds: false

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
    topPadding: 10 * __dp

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

          MMComponents.MMIcon {
            source: __style.arrowDownIcon
            anchors.verticalCenter: parent.verticalCenter
          }
        }

        MouseArea {
          anchors.fill: monthYearRow
          onClicked: {
            dateYearTumbler.visible = true
          }
        }

        // Right icons for changing months
        Row {
          x: parent.width - spacing - previousIcon.width - nextIcon.width - 10 * __dp
          spacing: 30 * __dp

          MMComponents.MMIcon {
            id: previousIcon
            source: __style.backIcon
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

          MMComponents.MMIcon {
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
      id: weekdays

      width: parent.width
      topPadding: 2 * __dp
      visible: root.hasDatePicker
      height: 14 * __dp
    }

    // Calendar
    MMMonthGrid {
      id: monthGrid

      bottomPadding: 70 * __dp
      width: parent.width
      height: {

        // ApplicationWindow.window might not be attached when calculating this for the first time,
        // it will get recalculated automatically once the window is attached so we just need to
        // make sure this won't fail the first time.

        if ( ApplicationWindow.window?.height ?? 0 > 680 ) {
          return 330 * __dp
        }

        return 330 - ( 680 - ApplicationWindow.window?.height ?? 0 )
      }

      locale: root.locale
      visible: root.hasDatePicker

      onSelectedDate: function (date) {
        root.dateToSelect.setDate(date.getDate())
        root.dateToSelect.setMonth(date.getMonth())
        root.dateToSelect.setFullYear(date.getFullYear())
      }

      Timer {
        id: fixHeightTimer

        interval: 100
        repeat: false
        onTriggered: monthGrid.refresh()
      }

      Component.onCompleted: fixHeightTimer.start()

      // update height to fit the content
      function refresh() {
        height--
        weekdays.height--
      }

      function navigateToDate(date) {
        monthGrid.year = date.getFullYear()
        monthGrid.month = date.getMonth()
      }
    }

    Rectangle {
      width: parent.width
      height: 1 * __dp
      color: __style.greyColor
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
                timeTumbler.visible = true
              }
            }

            onTimeChanged: {
              let hours = time.getHours()
              let minutes = time.getMinutes()
              let seconds = time.getSeconds()

              if(root.showSeconds)
                timeText.text = (String(hours).padStart(2, '0') + ":" + String(minutes).padStart(2, '0') + ":" + String(seconds).padStart(2, '0'))
              else
                timeText.text = (String(hours).padStart(2, '0') + ":" + String(minutes).padStart(2, '0'))

              timeTumbler.hours = hours
              timeTumbler.minutes = minutes
              timeTumbler.seconds = seconds
            }
          }
        }
      }
    }
  }

  // Area to close tumbler when clicked away from it - does not propagate the click further to the calendar
  MouseArea {
    anchors.fill: mainColumn

    enabled: dateYearTumbler.visible || timeTumbler.visible

    onClicked: {
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
    showSeconds: root.showSeconds

    onHoursChanged: { timeText.time.setHours(hours); root.dateToSelect.setHours(hours) }
    onMinutesChanged: { timeText.time.setMinutes(minutes); root.dateToSelect.setMinutes(minutes) }
    onSecondsChanged: { timeText.time.setSeconds(seconds); root.dateToSelect.setSeconds(seconds) }
  }
}
