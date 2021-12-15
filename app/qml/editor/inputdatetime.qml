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
import QtQuick.Window 2.14

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
    dateTimeDrawerLoader.focus = true
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

    anchors.fill: parent

    verticalAlignment: Text.AlignVCenter

    topPadding: customStyle.fields.height * 0.25
    bottomPadding: customStyle.fields.height * 0.25
    leftPadding: customStyle.fields.sideMargin
    rightPadding: customStyle.fields.sideMargin

    color: customStyle.fields.fontColor
    font.pixelSize: customStyle.fields.fontPixelSize
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

      height: {
        if (root.includesDate) {
          if ( Screen.primaryOrientation === Qt.PortraitOrientation ) {
            return parent.height * 2/3
          }
          return parent.height // for landscape mode
        }

        return parent.height * 1/3
      }
      width: formView.width

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
          dateTimeDrawer.close()
        }
      }
    }
  }
}
