/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtGraphicalEffects 1.14

import "../components" as Components

AbstractEditor {
  id: root

  property var parentField: parent.field
  property var parentValue: parent.value
  property bool parentValueIsNull: parent.valueIsNull

  property bool isReadOnly: parent.readOnly

  property bool fieldIsDate: __inputUtils.fieldType( field ) === 'QDate'
  property var typeFromFieldFormat: __inputUtils.dateTimeFieldFormat( config['field_format'] )

  property bool includesTime: typeFromFieldFormat.includes("Time")
  property bool includesDate: typeFromFieldFormat.includes("Date")

  signal editorValueChanged(var newValue, bool isNull)

  enabled: !isReadOnly

  function timeToString(attrValue) {
    if (attrValue === undefined)
    {
      return ''
    }
    else
    {
      return Qt.formatDateTime(attrValue, config['display_format'])
    }
  }

  function formatText(v) {
    if ( v === undefined || root.parentValueIsNull )
    {
      return ''
    }
    else
    {
      if ( root.parentField.isDateOrTime )
      {
        // if the field is a QDate, the automatic conversion to JS date [1]
        // leads to the creation of date time object with the time zone.
        // For instance shapefiles has support for dates but not date/time or time.
        // So a date coming from a shapefile as 2001-01-01 will become 2000-12-31 19:00:00 -05 in QML/JS in UTC -05 zone.
        // And when formatting this with the display format, this is shown as 2000-12-31.
        // So we detect if the field is a date only and revert the time zone offset.
        // [1] http://doc.qt.io/qt-5/qtqml-cppintegration-data.html#basic-qt-data-types
        if (root.fieldIsDate)
        {
          return Qt.formatDateTime( new Date(v.getTime() + v.getTimezoneOffset() * 60000), config['display_format'])
        }
        else
        {
          return Qt.formatDateTime(v, config['display_format'])
        }
      }
      else
      {
        let date = Date.fromLocaleString(Qt.locale(), v, config['field_format'])
        return Qt.formatDateTime(date, config['display_format'])
      }
    }
  }

  function openPicker(requestedDate)
  {
    // open calendar for today when no date is set
    if (!requestedDate)
      requestedDate = new Date()

    dateTimeDrawerLoader.active = true
    dateTimeDrawerLoader.item.dateToOpen = requestedDate
  }

  onContentClicked: {
    if (root.parentValueIsNull)
    {
      root.openPicker()
    }
    else
    {
      root.openPicker(root.parentValue)
    }
  }

  onRightActionClicked: {
    let newDate = new Date()
    let newValue = field.isDateOrTime ? newDate : Qt.formatDateTime(newDate, config['field_format'])
    editorValueChanged(newValue, false)
  }

  content: Text {
    id: dateText

    text: formatText(root.parentValue)

    onTextChanged: console.log( "TIMEDATE:", labelAlias, text, root.parentField, root.fieldIsDate, root.typeFromFieldFormat, root.parentValue, __inputUtils.fieldType( field ), __inputUtils.dateTimeFieldFormat( config['field_format'] )
 )

    anchors.fill: parent

    verticalAlignment: Text.AlignVCenter

    topPadding: customStyle.fields.height * 0.25
    bottomPadding: customStyle.fields.height * 0.25
    leftPadding: customStyle.fields.sideMargin
    rightPadding: customStyle.fields.sideMargin

    color: customStyle.fields.fontColor
    font.pointSize: customStyle.fields.fontPointSize
  }

  rightAction: Item {
    id: todayBtnContainer

    anchors.fill: parent

    Image {
      id: todayBtn

      anchors.centerIn: parent
      width: parent.width / 2
      sourceSize.width: parent.width / 2

      source: customStyle.icons.today
    }

    ColorOverlay {
      anchors.fill: todayBtn
      source: todayBtn
      color: todayBtn.enabled ? customStyle.toolbutton.activeButtonColor : customStyle.toolbutton.backgroundColorInvalid
    }
  }

  Loader {
    id: dateTimeDrawerLoader

    asynchronous: true
    active: false
    sourceComponent: dateTimeDrawerBlueprint
  }

  Component {
    id: dateTimeDrawerBlueprint

    Drawer {
      id: dateTimeDrawer

      property alias dateToOpen: picker.dateToSelect

      dim: true
      edge: Qt.BottomEdge
      interactive: false
      dragMargin: 0
      closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

      height: root.includesDate ? parent.height * 2/3 : parent.height * 1/3
      width: formView.width

      onOpened: console.log( labelAlias, "W:", width, "H:", height )
      onClosed: dateTimeDrawerLoader.active = false

      Component.onCompleted: open()

      Components.DateTimePicker {
        id: picker

        width: parent.width
        height: parent.height

        hasDatePicker: root.includesDate
        hasTimePicker: root.includesTime

        onSelected: {
          if ( selectedDate )
            root.editorValueChanged(selectedDate, false)

          dateTimeDrawer.close()
        }

        onCanceled: {
          dateTimeDrawer.hide()
        }
      }
    }
  }
}

//  Components.DatePicker {
//    id: datePicker

//  }

//    ColumnLayout {
//        id: main
//        property var currentValue: value
//        property bool fieldIsDate: __inputUtils.fieldType( field ) === 'QDate'
//        property var typeFromFieldFormat: __inputUtils.dateTimeFieldFormat( config['field_format'] )
//        property var rowHeight: customStyle.fields.height * 0.75

//        anchors { right: parent.right; left: parent.left }

//        Item {
//            Layout.fillWidth: true
//            Layout.minimumHeight: customStyle.fields.height

//            TextField {
//                id: label

//                anchors.fill: parent
//                verticalAlignment: Text.AlignVCenter
//                font.pointSize: customStyle.fields.fontPointSize
//                color: customStyle.fields.fontColor
//                padding: 0
//                topPadding: 10 * __dp
//                bottomPadding: 10 * __dp
//                leftPadding: customStyle.fields.sideMargin
//                background: Rectangle {
//                    radius: customStyle.fields.cornerRadius

//                    border.color: popup.opened ? customStyle.fields.activeColor : customStyle.fields.normalColor
//                    border.width: popup.opened ? 2 : 1
//                    color: customStyle.fields.backgroundColor
//                }

//                text: if ( value === undefined )
//                        {
//                          qsTr('(no date)')
//                        }
//                        else
//                        {
//                          if ( field.isDateOrTime )
//                          {
//                            // if the field is a QDate, the automatic conversion to JS date [1]
//                            // leads to the creation of date time object with the time zone.
//                            // For instance shapefiles has support for dates but not date/time or time.
//                            // So a date coming from a shapefile as 2001-01-01 will become 2000-12-31 19:00:00 -05 in QML/JS in UTC -05 zone.
//                            // And when formatting this with the display format, this is shown as 2000-12-31.
//                            // So we detect if the field is a date only and revert the time zone offset.
//                            // [1] http://doc.qt.io/qt-5/qtqml-cppintegration-data.html#basic-qt-data-types
//                            if (main.fieldIsDate) {
//                              Qt.formatDateTime( new Date(value.getTime() + value.getTimezoneOffset() * 60000), config['display_format'])
//                            } else {
//                              Qt.formatDateTime(value, config['display_format'])
//                            }
//                          }
//                          else
//                          {
//                            var date = Date.fromLocaleString(Qt.locale(), value, config['field_format'])
//                            Qt.formatDateTime(date, config['display_format'])
//                          }
//                        }

//                inputMethodHints: Qt.ImhDate

//                MouseArea {
//                  anchors.fill: parent
//                  onClicked: {
//                    var usedDate = new Date();
//                    if (value !== undefined && value !== '') {
//                      usedDate = field.isDateOrTime ? value : Date.fromLocaleString(Qt.locale(), value, config['field_format'])
//                    }

//                    calendar.selectedDate = usedDate
//                    if (main.typeFromFieldFormat === "Time" || main.typeFromFieldFormat === "Date Time") {
//                      hoursSpinBox.value = usedDate.getHours()
//                      minutesSpinBox.value = usedDate.getMinutes()
//                      secondsSpinBox.value = usedDate.getSeconds()
//                    }

//                    popup.open()
//                  }
//                }
//            }

//            Image {
//                id: todayBtn2
//                height: fieldItem.iconSize
//                sourceSize.height: fieldItem.iconSize
//                autoTransform: true
//                fillMode: Image.PreserveAspectFit
//                source: customStyle.icons.today
//                anchors.right: parent.right
//                anchors.verticalCenter: parent.verticalCenter
//                visible: fieldItem.enabled
//                anchors.rightMargin: customStyle.fields.sideMargin

//                MouseArea {
//                    anchors.fill: parent
//                    onClicked: {
//                      var newDate = new Date()
//                      var newValue = field.isDateOrTime ? newDate : Qt.formatDateTime(newDate, config['field_format'])
//                      editorValueChanged(newValue, false)
//                    }
//                }
//            }

//            ColorOverlay {
//                anchors.fill: todayBtn
//                source: todayBtn
//                color: customStyle.toolbutton.activeButtonColor
//                visible: todayBtn.visible
//            }
//        }

//        Popup {
//          id: popup
//          modal: true
//          focus: true
//          closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
//          parent: ApplicationWindow.overlay
//          anchors.centerIn: parent

//            ScrollView {
//              clip: true
//              width: parent.width
//              height: parent.height

//              ColumnLayout {
//                Rectangle {
//                  id: calendarOverlay
//                  color: "transparent"
//                  implicitWidth: Math.min(popup.parent.width, popup.parent.height) * 0.8
//                  implicitHeight: implicitWidth
//                  visible: main.typeFromFieldFormat === "Date" || main.typeFromFieldFormat === "Date Time"

//                  MouseArea {
//                      anchors.fill: parent
//                      onClicked: mouse.accepted = true
//                      onWheel: wheel.accepted = true
//                  }

//                  GridLayout {
//                      id: calendarGrid
//                      anchors.left: parent.left
//                      anchors.right: parent.right
//                      columns: 1
//                      implicitWidth: calendarOverlay.width
//                      implicitHeight: calendarOverlay.height

//                        Controls1.Calendar {
//                          id: calendar
//                          selectedDate: {
//                            var date = field.isDateOrTime ? main.currentValue : Date.fromLocaleString(Qt.locale(), value, config['field_format'])
//                            date || new Date()
//                          }
//                          weekNumbersVisible: true
//                          focus: false
//                          implicitWidth: calendarOverlay.width
//                          implicitHeight: calendarOverlay.height
//                      }
//                  }
//              }

//              RowLayout {
//                visible: main.typeFromFieldFormat === "Time" || main.typeFromFieldFormat === "Date Time"
//                Layout.alignment: Qt.AlignHCenter

//                  GridLayout {
//                      id: timeGrid
//                      Layout.alignment: Qt.AlignHCenter
//                      Layout.leftMargin: 20
//                      rows: 3
//                      columns: 2

//                      Label {
//                          verticalAlignment: Text.AlignVCenter
//                          Layout.preferredHeight: main.rowHeight
//                          Layout.fillWidth: true
//                          Layout.row: 0
//                          Layout.column: 0
//                          text: qsTr( "Hours" )

//                      }
//                      SpinBox {
//                          id: hoursSpinBox
//                          Layout.preferredHeight: main.rowHeight
//                          Layout.minimumWidth: main.rowHeight * 3
//                          Layout.fillWidth: true
//                          Layout.row: 0
//                          Layout.column: 1
//                          editable: true
//                          from: 0
//                          to: 23
//                          value: 12
//                          inputMethodHints: Qt.ImhTime
//                          down.indicator.width: main.rowHeight
//                          up.indicator.width: main.rowHeight
//                      }
//                      Label {
//                          verticalAlignment: Text.AlignVCenter
//                          Layout.preferredHeight: main.rowHeight
//                          Layout.fillWidth: true
//                          Layout.row: 1
//                          Layout.column: 0
//                          text: qsTr( "Minutes" )
//                      }
//                      SpinBox {
//                          id: minutesSpinBox
//                          Layout.preferredHeight: main.rowHeight
//                          Layout.minimumWidth: main.rowHeight * 3
//                          Layout.fillWidth: true
//                          Layout.row: 1
//                          Layout.column: 1
//                          editable: true
//                          from: 0
//                          to: 59
//                          value: 30
//                          inputMethodHints: Qt.ImhTime
//                          down.indicator.width: main.rowHeight
//                          up.indicator.width: main.rowHeight
//                      }
//                      Label {
//                          verticalAlignment: Text.AlignVCenter
//                          Layout.preferredHeight: main.rowHeight
//                          Layout.fillWidth: true
//                          Layout.row: 2
//                          Layout.column: 0
//                          text: qsTr( "Seconds" )
//                      }
//                      SpinBox {
//                          id: secondsSpinBox
//                          Layout.preferredHeight: main.rowHeight
//                          Layout.minimumWidth: main.rowHeight * 3
//                          Layout.fillWidth: true
//                          Layout.row: 2
//                          Layout.column: 1
//                          editable: true
//                          from: 0
//                          to: 59
//                          value: 30
//                          inputMethodHints: Qt.ImhTime
//                          down.indicator.width: main.rowHeight
//                          up.indicator.width: main.rowHeight
//                      }
//                  }
//              }

//              RowLayout {
//                  Button {
//                      text: qsTr( "OK" )
//                      Layout.fillWidth: true
//                      Layout.preferredHeight: main.rowHeight

//                      onClicked: {
//                          var newDate = calendar.selectedDate

//                          if (main.typeFromFieldFormat === "Time" || main.typeFromFieldFormat === "Date Time") {
//                            newDate.setHours(hoursSpinBox.value);
//                            newDate.setMinutes(minutesSpinBox.value);
//                            newDate.setSeconds(secondsSpinBox.value);
//                          }

//                          var newValue = field.isDateOrTime ? newDate : Qt.formatDateTime(newDate, config['field_format'])
//                          editorValueChanged(newValue, newValue === undefined)
//                          popup.close()
//                      }
//                  }
//                }
//             }
//          }
//        }

//        onCurrentValueChanged: {
//            label.text = field.isDateOrTime ? timeToString(main.currentValue) : main.currentValue
//        }
//    }

