/***************************************************************************
 datetime.qml
  --------------------------------------
  Date                 : 2017
  Copyright            : (C) 2017 by Matthias Kuhn
  Email                : matthias@opengis.ch
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4 as Controls1
import QtGraphicalEffects 1.0
import QgsQuick 0.1 as QgsQuick

/**
 * Calendar for QGIS Attribute Form
 * Requires various global properties set to function, see featureform Loader section
 * Do not use directly from Application QML
 */
Item {
  id: fieldItem
  property real iconSize: fieldItem.height * 0.75
  property var timeToString: function timeToString(attrValue) {
    if (attrValue === undefined) {
      return qsTr('(no date)');
    } else {
      return Qt.formatDateTime(attrValue, config['display_format']);
    }
  }

  enabled: !readOnly
  height: childrenRect.height

  signal editorValueChanged(var newValue, bool isNull)

  anchors {
    left: parent.left
    right: parent.right
  }
  ColumnLayout {
    id: main
    property var currentValue: value
    property bool fieldIsDate: __inputUtils.fieldType(field) === 'QDate'
    property var rowHeight: customStyle.fields.height * 0.75
    property var typeFromFieldFormat: __inputUtils.dateTimeFieldFormat(config['field_format'])

    onCurrentValueChanged: {
      label.text = field.isDateOrTime ? timeToString(main.currentValue) : main.currentValue;
    }

    anchors {
      left: parent.left
      right: parent.right
    }
    Item {
      Layout.fillWidth: true
      Layout.minimumHeight: customStyle.fields.height

      TextField {
        id: label
        anchors.fill: parent
        bottomPadding: 10 * QgsQuick.Utils.dp
        color: customStyle.fields.fontColor
        font.pointSize: customStyle.fields.fontPointSize
        inputMethodHints: Qt.ImhDate
        leftPadding: customStyle.fields.sideMargin
        padding: 0
        text: if (value === undefined) {
          qsTr('(no date)');
        } else {
          if (field.isDateOrTime) {
            // if the field is a QDate, the automatic conversion to JS date [1]
            // leads to the creation of date time object with the time zone.
            // For instance shapefiles has support for dates but not date/time or time.
            // So a date coming from a shapefile as 2001-01-01 will become 2000-12-31 19:00:00 -05 in QML/JS in UTC -05 zone.
            // And when formatting this with the display format, this is shown as 2000-12-31.
            // So we detect if the field is a date only and revert the time zone offset.
            // [1] http://doc.qt.io/qt-5/qtqml-cppintegration-data.html#basic-qt-data-types
            if (main.fieldIsDate) {
              Qt.formatDateTime(new Date(value.getTime() + value.getTimezoneOffset() * 60000), config['display_format']);
            } else {
              Qt.formatDateTime(value, config['display_format']);
            }
          } else {
            var date = Date.fromLocaleString(Qt.locale(), value, config['field_format']);
            Qt.formatDateTime(date, config['display_format']);
          }
        }
        topPadding: 10 * QgsQuick.Utils.dp
        verticalAlignment: Text.AlignVCenter

        MouseArea {
          anchors.fill: parent

          onClicked: {
            var usedDate = new Date();
            if (value !== undefined && value !== '') {
              usedDate = field.isDateOrTime ? value : Date.fromLocaleString(Qt.locale(), value, config['field_format']);
            }
            calendar.selectedDate = usedDate;
            if (main.typeFromFieldFormat === "Time" || main.typeFromFieldFormat === "Date Time") {
              hoursSpinBox.value = usedDate.getHours();
              minutesSpinBox.value = usedDate.getMinutes();
              secondsSpinBox.value = usedDate.getSeconds();
            }
            popup.open();
          }
        }

        background: Rectangle {
          border.color: popup.opened ? customStyle.fields.activeColor : customStyle.fields.normalColor
          border.width: popup.opened ? 2 : 1
          color: customStyle.fields.backgroundColor
          radius: customStyle.fields.cornerRadius
        }
      }
      Image {
        id: todayBtn
        anchors.right: parent.right
        anchors.rightMargin: customStyle.fields.sideMargin
        anchors.verticalCenter: parent.verticalCenter
        autoTransform: true
        fillMode: Image.PreserveAspectFit
        height: fieldItem.iconSize
        source: customStyle.icons.today
        sourceSize.height: fieldItem.iconSize
        visible: fieldItem.enabled

        MouseArea {
          anchors.fill: parent

          onClicked: {
            var newDate = new Date();
            var newValue = field.isDateOrTime ? newDate : Qt.formatDateTime(newDate, config['field_format']);
            editorValueChanged(newValue, false);
          }
        }
      }
      ColorOverlay {
        anchors.fill: todayBtn
        color: customStyle.toolbutton.activeButtonColor
        source: todayBtn
        visible: todayBtn.visible
      }
    }
    Popup {
      id: popup
      anchors.centerIn: parent
      closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
      focus: true
      modal: true
      parent: ApplicationWindow.overlay

      ScrollView {
        clip: true
        height: parent.height
        width: parent.width

        ColumnLayout {
          Rectangle {
            id: calendarOverlay
            color: "transparent"
            implicitHeight: implicitWidth
            implicitWidth: Math.min(popup.parent.width, popup.parent.height) * 0.8
            visible: main.typeFromFieldFormat === "Date" || main.typeFromFieldFormat === "Date Time"

            MouseArea {
              anchors.fill: parent

              onClicked: mouse.accepted = true
              onWheel: wheel.accepted = true
            }
            GridLayout {
              id: calendarGrid
              anchors.left: parent.left
              anchors.right: parent.right
              columns: 1
              implicitHeight: calendarOverlay.height
              implicitWidth: calendarOverlay.width

              Controls1.Calendar {
                id: calendar
                focus: false
                implicitHeight: calendarOverlay.height
                implicitWidth: calendarOverlay.width
                selectedDate: {
                  var date = field.isDateOrTime ? main.currentValue : Date.fromLocaleString(Qt.locale(), value, config['field_format']);
                  date || new Date();
                }
                weekNumbersVisible: true
              }
            }
          }
          RowLayout {
            Layout.alignment: Qt.AlignHCenter
            visible: main.typeFromFieldFormat === "Time" || main.typeFromFieldFormat === "Date Time"

            GridLayout {
              id: timeGrid
              Layout.alignment: Qt.AlignHCenter
              Layout.leftMargin: 20
              columns: 2
              rows: 3

              Label {
                Layout.column: 0
                Layout.fillWidth: true
                Layout.preferredHeight: main.rowHeight
                Layout.row: 0
                text: qsTr("Hours")
                verticalAlignment: Text.AlignVCenter
              }
              SpinBox {
                id: hoursSpinBox
                Layout.column: 1
                Layout.fillWidth: true
                Layout.minimumWidth: main.rowHeight * 3
                Layout.preferredHeight: main.rowHeight
                Layout.row: 0
                down.indicator.width: main.rowHeight
                editable: true
                from: 0
                inputMethodHints: Qt.ImhTime
                to: 23
                up.indicator.width: main.rowHeight
                value: 12
              }
              Label {
                Layout.column: 0
                Layout.fillWidth: true
                Layout.preferredHeight: main.rowHeight
                Layout.row: 1
                text: qsTr("Minutes")
                verticalAlignment: Text.AlignVCenter
              }
              SpinBox {
                id: minutesSpinBox
                Layout.column: 1
                Layout.fillWidth: true
                Layout.minimumWidth: main.rowHeight * 3
                Layout.preferredHeight: main.rowHeight
                Layout.row: 1
                down.indicator.width: main.rowHeight
                editable: true
                from: 0
                inputMethodHints: Qt.ImhTime
                to: 59
                up.indicator.width: main.rowHeight
                value: 30
              }
              Label {
                Layout.column: 0
                Layout.fillWidth: true
                Layout.preferredHeight: main.rowHeight
                Layout.row: 2
                text: qsTr("Seconds")
                verticalAlignment: Text.AlignVCenter
              }
              SpinBox {
                id: secondsSpinBox
                Layout.column: 1
                Layout.fillWidth: true
                Layout.minimumWidth: main.rowHeight * 3
                Layout.preferredHeight: main.rowHeight
                Layout.row: 2
                down.indicator.width: main.rowHeight
                editable: true
                from: 0
                inputMethodHints: Qt.ImhTime
                to: 59
                up.indicator.width: main.rowHeight
                value: 30
              }
            }
          }
          RowLayout {
            Button {
              Layout.fillWidth: true
              Layout.preferredHeight: main.rowHeight
              text: qsTr("OK")

              onClicked: {
                var newDate = calendar.selectedDate;
                if (main.typeFromFieldFormat === "Time" || main.typeFromFieldFormat === "Date Time") {
                  newDate.setHours(hoursSpinBox.value);
                  newDate.setMinutes(minutesSpinBox.value);
                  newDate.setSeconds(secondsSpinBox.value);
                }
                var newValue = field.isDateOrTime ? newDate : Qt.formatDateTime(newDate, config['field_format']);
                editorValueChanged(newValue, newValue === undefined);
                popup.close();
              }
            }
          }
        }
      }
    }
  }
}
