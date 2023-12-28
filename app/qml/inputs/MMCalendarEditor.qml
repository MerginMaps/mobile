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
import "../components"

MMAbstractEditor {
  id: root

  property var parentField: parent.field ?? ""
  property var parentValue: parent.value ?? ""
  property bool parentValueIsNull: parent.valueIsNull ?? true
  property bool isReadOnly: parent.readOnly ?? false

  // TODO: Uncomment to use it in Input app
  property bool fieldIsDate //__inputUtils.fieldType( field ) === 'QDate'
  property var typeFromFieldFormat //__inputUtils.dateTimeFieldFormat( config['field_format'] )
  property bool includesTime //typeFromFieldFormat.includes("Time")
  property bool includesDate //typeFromFieldFormat.includes("Date")

  property alias placeholderText: textField.placeholderText
  property alias text: textField.text

  signal editorValueChanged( var newValue, var isNull )
  signal selected(date dateTime)

  enabled: !isReadOnly
  hasFocus: textField.activeFocus

  Component.onCompleted: dateTimeDrawerLoader.active = true

  content: TextField {
    id: textField

    anchors.fill: parent

    text: root.parentValue
    color: root.enabled ? __style.nightColor : __style.mediumGreenColor
    placeholderTextColor: __style.nightAlphaColor
    font: __style.p5
    hoverEnabled: true

    background: Rectangle {
      color: __style.transparentColor
    }
  }

  rightAction: MMIcon {
    id: rightIcon

    height: parent.height

    source: __style.calendarIcon
    color: root.enabled ? __style.forestColor : __style.mediumGreenColor
  }

  onRightActionClicked: {
    if (root.parentValueIsNull) {
      // open calendar for today when no date is set
      root.openPicker( new Date() )
    }
    else {
      root.openPicker( dateTransformer.toJsDate(root.parentValue) )
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

    MMCalendarDrawer {
      id: dateTimeDrawer

//      property alias dateToOpen: picker.dateToSelect

      title: root.fieldIsDate ? qsTr("Date") : qsTr("Date & Time")
      primaryButton: qsTr("Confirm")

      onPrimaryButtonClicked: root.selected(dateTimeDrawer.dateTime)
      onClosed: dateTimeDrawerLoader.active = false
      Component.onCompleted: open()
    }
  }

  QtObject {
    id: dateTransformer
    // When changing this function, test with various timezones!
    // On desktop, use environment variable TZ, e.g. TZ=America/Mexico_City (UTC-5)
    function toJsDate(qtDate) {
      if ( root.parentField.isDateOrTime ) {
        if (root.fieldIsDate) {
          if (qtDate.getUTCHours() === 0)
          {
            // on cold start of this editor widget, the JS date coming from C++ QDate is shifted.
            // As [1] docs say: "converting a QDate will result in UTC's start of the
            // day, which falls on a different date in some other time-zones"
            // So for example if 2001-01-01 is stored in date file,
            // it will become 2000-12-31 19:00:00 -05 in QML/JS in UTC -05 zone.
            // However, we need 2001-01-01 00:00:00 in local timezone.
            // [1] https://doc.qt.io/qt-6/qml-date.html
            let date = new Date(qtDate.getUTCFullYear(), qtDate.getUTCMonth(), qtDate.getUTCDate() )
            return date
          } else {
            //
            // Other issue is that when we already set NEW value by our calendar picker,
            // the JS date coming from C++ already has correct (local) timezone...
            // We can distinguish between these two by checking if the UTC hour is midnight
            // or not and based on that apply or not apply the timezone shift
            //
            return qtDate
          }
        }
        else {
          return qtDate
        }
      }
      else {
        // This is the case when the date coming from C++ is pure string, so we
        // need to convert it to JS Date ourselves
        //        return Date.fromLocaleString(Qt.locale(), qtDate, config['field_format'])
      }
    }
  }

  function newDateSelected( jsDate ) {
    if ( jsDate ) {
      if ( root.parentField.isDateOrTime ) {
        // For QDate, the year, month and day is clipped based on
        // the local timezone in QgsFeature.convertCompatible
        root.editorValueChanged( jsDate, false  )
      }
      else {
        //        let qtDate = jsDate.toLocaleString(Qt.locale(), config['field_format'])
        //        root.editorValueChanged(qtDate, false)
      }
    }
  }

  function formatText( qtDate ) {
    if ( qtDate === undefined || root.parentValueIsNull ) {
      return ''
    }
    else {
      let jsDate = dateTransformer.toJsDate(qtDate)
      //      return Qt.formatDateTime(jsDate, config['display_format'])
    }
  }

  function openPicker(requestedDate) {
    dateTimeDrawerLoader.active = true
    dateTimeDrawerLoader.focus = true
//    dateTimeDrawerLoader.item.dateToOpen = requestedDate
  }
}
