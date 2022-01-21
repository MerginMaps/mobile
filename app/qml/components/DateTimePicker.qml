/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.14
import QtQuick.Layouts 1.14
import QtQuick.Controls 2.14
import Qt.labs.calendar 1.0
import QtGraphicalEffects 1.14

import ".."

Item {
  id: root

  property bool hasTimePicker: true
  property bool hasDatePicker: true

  property date dateToSelect: new Date()
  property var locale: Qt.locale()

  signal selected(date selectedDate)
  signal canceled()

  function getSelectedTime() {
    if (hasDatePicker && hasTimePicker) {
      let ddate = calendar.selectedDate
      let time = timepicker.time
      var newDate = new Date(
            ddate.getFullYear(),
            ddate.getMonth(),
            ddate.getDate(),
            time.getHours(),
            time.getMinutes(),
            time.getSeconds(),
            time.getMilliseconds()
            )
      return newDate
    } else if (hasDatePicker) {
      return calendar.selectedDate
    } else if (hasTimePicker) {
      return timepicker.time
    }
    return new Date()
  }

  function setDate(toDate) {
    calendar.selectDate(toDate)
    calendar.navigateToDate(toDate)
    timepicker.setTime(toDate)
  }

  onDateToSelectChanged: {
    setDate(dateToSelect)
  }

  focus: true

  Keys.onReleased: {
    if ( event.key === Qt.Key_Back || event.key === Qt.Key_Escape ) {
      event.accepted = true
      root.canceled()
    }
  }

  height: 400
  width: 400

  ColumnLayout {
    anchors.fill: parent

    spacing: 0

    Rectangle {
      id: header

      Layout.fillWidth: true
      Layout.minimumHeight: 40
      Layout.preferredHeight: 50
      Layout.maximumHeight: 55

      color: InputStyle.clrPanelBackground

      Rectangle {
        id: cancelBtn

        height: parent.height
        width: cancelBtnText.contentWidth + 15

        anchors {
          left: parent.left
          leftMargin: 10
          verticalCenter: parent.verticalCenter
        }

        color: InputStyle.clrPanelBackground
        Text {
          id: cancelBtnText
          anchors.centerIn: parent
          font.pixelSize: InputStyle.fontPixelSizeBig
          font.bold: true
          color: "#FD9626"
          text: qsTr("Cancel")
        }
        MouseArea {
          anchors.fill: parent
          onClicked: {
            root.canceled();
          }
        }
      }

      Column {
        id: yearDayContainer

        width: parent.width / 4
        height: parent.height

        anchors {
          horizontalCenter: parent.horizontalCenter
        }

        Rectangle {
          id: selectedYear

          visible: root.hasDatePicker
          height: parent.height / 2
          width: parent.width
          color: InputStyle.clrPanelBackground
          Text {
            id: yearTitle

            anchors.centerIn: parent

            font.pixelSize: InputStyle.fontPixelSizeBig
            opacity: yearsList.visible ? 1 : 0.7
            color: "white"
            text: calendar.selectedDate.getFullYear()
          }
          MouseArea {
            anchors.fill: parent
            onClicked: {
              yearsList.show();
            }
          }
        }

        Text {
          id: selectedWeekDayMonth

          visible: root.hasDatePicker

          height: parent.height / 2
          width: parent.width

          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter

          font.pixelSize: InputStyle.fontPixelSizeNormal
          text: root.locale.dayName( calendar.selectedDate.getDay(), Locale.ShortFormat ) + ", "
                + calendar.selectedDate.getDate() + " "
                + root.locale.monthName( calendar.selectedDate.getMonth(), Locale.ShortFormat )
          color: "white"
          opacity: yearsList.visible ? 0.7 : 1

          MouseArea {
            anchors.fill: parent
            onClicked: {
              yearsList.hide();
            }
          }
        }
      }

      Rectangle {
        id: okBtn

        height: parent.height
        width: okBtnText.contentWidth + 15

        anchors {
          right: parent.right
          rightMargin: 10
          verticalCenter: parent.verticalCenter
        }

        color: InputStyle.clrPanelBackground

        Text {
          id: okBtnText

          anchors.centerIn: parent

          font.pixelSize: InputStyle.fontPixelSizeBig
          font.bold: true

          color: InputStyle.activeButtonColorOrange
          text: qsTr("OK")
        }
        MouseArea {
          anchors.fill: parent
          onClicked: {
            root.selected(getSelectedTime())
          }
        }
      }
    }

    Rectangle {
      id: placeholder // placeholder that consumes all white space when we only show timepicker

      property bool isActive: !root.hasDatePicker && hasTimePicker

      color: "transparent"
      Layout.fillWidth: isActive ? true : false
      Layout.fillHeight: isActive ? true : false
    }

    Rectangle {
      id: datepickerContainer

      Layout.fillWidth: root.hasDatePicker ? true : false
      Layout.fillHeight: root.hasDatePicker ? true : false

      visible: root.hasDatePicker

      Item {
        id: calendar

        property date selectedDate: new Date()

        property int startYear: 1940
        property int endYear: 2040

        function selectDate(ddate) {
          calendar.selectedDate = ddate
        }

        function navigateToDate(ddate) {
          monthGrid.year = ddate.getFullYear()
          monthGrid.month = ddate.getMonth()
        }

        anchors.fill: parent

        ColumnLayout {

          anchors.fill: parent

          spacing: 5

          Rectangle {
            id: monthYearTitle

            Layout.preferredHeight: parent.height / 10
            Layout.maximumHeight: parent.height / 9
            Layout.fillWidth: true

            Image {
              id: leftArrow

              anchors.left: parent.left
              anchors.leftMargin: 20
              anchors.verticalCenter: parent.verticalCenter

              source: customStyle.icons.valueRelationMore

              width: parent.height * 0.5
              sourceSize.width: parent.height * 0.5

              rotation: 180

              MouseArea {
                anchors.fill: parent
                onClicked: {
                  if (monthGrid.month !== Calendar.January) {
                    monthGrid.month -= 1;
                  } else {
                    monthGrid.year -= 1;
                    monthGrid.month = Calendar.December;
                  }
                }
              }
            }

            ColorOverlay {
              anchors.fill: leftArrow
              source: leftArrow
              rotation: 180
              color: customStyle.fields.fontColor
            }

            Text {
              anchors.centerIn: parent
              font.pixelSize: InputStyle.fontPixelSizeNormal
              text: root.locale.standaloneMonthName( monthGrid.month, Locale.LongFormat ) + " " + monthGrid.year;
            }

            Image {
              id: rightArrow

              anchors.right: parent.right
              anchors.rightMargin: 20
              anchors.verticalCenter: parent.verticalCenter

              source: customStyle.icons.valueRelationMore

              width: parent.height * 0.5
              sourceSize.width: parent.height * 0.5

              MouseArea {
                anchors.fill: parent
                onClicked: {
                  if (monthGrid.month != Calendar.December) {
                    monthGrid.month += 1;
                  } else {
                    monthGrid.month = Calendar.January
                    monthGrid.year += 1;
                  }
                }
              }
            }

            ColorOverlay {
              anchors.fill: rightArrow
              source: rightArrow
              color: customStyle.fields.fontColor
            }
          }

          DayOfWeekRow {
            id: weekTitles

            Layout.fillWidth: true
            Layout.preferredHeight: parent.height / 10
            Layout.maximumHeight: parent.height / 9

            locale: root.locale

            delegate: Text {
              text: model.shortName
              horizontalAlignment: Text.AlignHCenter
              verticalAlignment: Text.AlignVCenter
              font.pixelSize: InputStyle.fontPixelSizeSmall
            }
          }

          MonthGrid {
            id: monthGrid

            Layout.fillWidth: true
            Layout.fillHeight: true

            month: 10
            year: 2020

            spacing: 0

            locale: root.locale

            delegate: Rectangle {
              property bool highlighted: enabled
                                         && model.day === calendar.selectedDate.getDate()
                                         && model.month === calendar.selectedDate.getMonth()
                                         && model.year === calendar.selectedDate.getFullYear()

              height: 10
              width: 10
              radius: height * 0.5

              enabled: model.month === monthGrid.month
              color: enabled && highlighted ? InputStyle.fontColor : "white"

              Text {
                anchors.centerIn: parent
                text: model.day
                font.pixelSize: InputStyle.fontPixelSizeNormal
                scale: highlighted ? 1.25 : 1
                Behavior on scale {
                  NumberAnimation {
                    duration: 150
                  }
                }
                visible: parent.enabled
                color: parent.highlighted ? "white" : "black"
              }

              MouseArea {
                anchors.fill: parent
                onClicked: {
                  calendar.selectDate(model.date)
                }
              }
            }
          }
        }
      }

      ListView {
        id: yearsList

        property int currentYear
        property int startYear: calendar.startYear
        property int endYear: calendar.endYear

        anchors.fill: calendar

        orientation: ListView.Vertical
        visible: false
        clip: true
        spacing: 10

        model: ListModel {
          id: yearsModel
        }

        delegate: Rectangle {
          width: parent.width
          height: 30
          Text {
            anchors.centerIn: parent
            font.pixelSize: InputStyle.fontPixelSizeNormal
            text: name
            scale: index === (yearsList.currentYear - yearsList.startYear) ? 1.5 : 1
            color: InputStyle.fontColor
          }
          MouseArea {
            anchors.fill: parent
            onClicked: {
              calendar.navigateToDate(new Date(yearsList.startYear + index, calendar.selectedDate.getMonth()));
              yearsList.hide();
            }
          }
        }

        Component.onCompleted: {
          for (var year = startYear; year <= endYear; year ++)
            yearsModel.append({
                                name: year
                              });
        }

        function show() {
          visible = true
          calendar.visible = false
          currentYear = calendar.selectedDate.getFullYear()
          yearsList.positionViewAtIndex(currentYear - startYear, ListView.SnapToItem)
        }

        function hide() {
          visible = false;
          calendar.visible = true;
        }
      }
    }

    Rectangle {
      id: separator

      color: InputStyle.clrPanelBackground

      visible: root.hasDatePicker

      Layout.preferredWidth: parent.width
      Layout.maximumHeight: 1
      Layout.minimumHeight: 1
    }

    Rectangle {
      id: timepicker

      property date time: new Date()

      function setTime(ddatetime) {
        let hour = ddatetime.getHours()
        let minutes = ddatetime.getMinutes()
        let seconds = ddatetime.getSeconds()

        hoursTumbler.currentIndex = hour
        minutesTumbler.currentIndex = minutes
        secondsTumbler.currentIndex = seconds
      }

      function constructTime() {
        let ddate = new Date (
              2000, 1, 1,
              hoursTumbler.currentIndex,
              minutesTumbler.currentIndex,
              secondsTumbler.currentIndex
              )
        timepicker.time = ddate
      }

      Layout.fillWidth: true
      Layout.minimumHeight: root.hasTimePicker ? parent.height / 6 : 0
      Layout.preferredHeight: {
        if (!root.hasTimePicker) return 0
        if (root.hasDatePicker) return parent.height * 3 / 10
        return parent.height
      }
      Layout.maximumHeight: {
        if (!root.hasTimePicker) return 0
        if (root.hasDatePicker) return parent.height / 4
        return 200
      }

      visible: root.hasTimePicker

      FontMetrics {
        id: fontMetrics
      }

      Component {
        id: delegateComponent

        Label {
          property bool isActive: modelData === Tumbler.tumbler.currentIndex

          text: modelData.toString().length < 2 ? "0" + modelData : modelData;

          opacity: 1.0 - Math.abs(Tumbler.displacement) / (Tumbler.tumbler.visibleItemCount / 2)

          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          font.pixelSize: InputStyle.fontPixelSizeNormal

          color: isActive ? InputStyle.fontColor : "black"
        }
      }

      Item {
        id: frame

        width: parent.width
        height: parent.height

        Row {

          anchors.centerIn: parent

          Tumbler {
            id: hoursTumbler

            onCurrentIndexChanged: timepicker.constructTime()

            width: frame.width/8
            height: frame.height * .8
            anchors.verticalCenter: parent.verticalCenter

            model: 24
            visibleItemCount: 3

            delegate: delegateComponent
          }

          Text {
            // separator
            text: ":"
            anchors.verticalCenter: parent.verticalCenter

            font.bold: true
            font.pixelSize: InputStyle.fontPixelSizeNormal
            color: "black"
          }

          Tumbler {
            id: minutesTumbler

            onCurrentIndexChanged: timepicker.constructTime()

            model: 60
            visibleItemCount: 3
            width: frame.width/8
            height: frame.height * .8
            delegate: delegateComponent
            anchors.verticalCenter: parent.verticalCenter
          }

          Text {
            // separator
            text: ":"
            anchors.verticalCenter: parent.verticalCenter

            font.bold: true
            font.pixelSize: InputStyle.fontPixelSizeNormal
            color: "black"
          }

          Tumbler {
            id: secondsTumbler

            onCurrentIndexChanged: timepicker.constructTime()

            model: 60
            visibleItemCount: 3

            width: frame.width/8
            height: frame.height * .8

            delegate: delegateComponent

            anchors.verticalCenter: parent.verticalCenter
          }
        }
      }
    }
  }
}
