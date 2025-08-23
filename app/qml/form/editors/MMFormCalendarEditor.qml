/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../../components" as MMComponents
import "../../components/private" as MMPrivateComponents

import "../components" as MMFormComponents

/*
 * Calendar (datetime) editor for QGIS Attribute Form
 * Requires various global properties set to function, see featureform Loader section.
 * These properties are injected here via 'fieldXYZ' properties and captured with underscore `_`.
 *
 * Should be used only within feature form.
 */

MMPrivateComponents.MMBaseSingleLineInput {
  id: root

  property var _field: parent.field
  property var _fieldValue: parent.fieldValue
  property var _fieldConfig: parent.fieldConfig
  property bool _fieldValueIsNull: parent.fieldValueIsNull
  property bool _fieldHasMixedValues: parent.fieldHasMixedValues

  property bool _fieldShouldShowTitle: parent.fieldShouldShowTitle
  property bool _fieldFormIsReadOnly: parent.fieldFormIsReadOnly
  property bool _fieldIsEditable: parent.fieldIsEditable

  property string _fieldTitle: parent.fieldTitle
  property string _fieldErrorMessage: parent.fieldErrorMessage
  property string _fieldWarningMessage: parent.fieldWarningMessage

  property bool _fieldRememberValueSupported: parent.fieldRememberValueSupported
  property bool _fieldRememberValueState: parent.fieldRememberValueState

  signal editorValueChanged( var newValue, var isNull )
  signal rememberValueBoxClicked( bool state )

  property bool fieldIsDate: __inputUtils.fieldType( _field ) === 'QDate'
  property var typeFromFieldFormat: __inputUtils.dateTimeFieldFormat( _fieldConfig['field_format'] )
  property bool includesTime: typeFromFieldFormat.includes("Time")
  property bool includesDate: typeFromFieldFormat.includes("Date")
  property bool showSeconds: true

  title: _fieldShouldShowTitle ? _fieldTitle : ""
  text: formatText( root._fieldValue )
  placeholderText: _fieldHasMixedValues ? _fieldValue : ""

  warningMsg: _fieldWarningMessage
  errorMsg: _fieldErrorMessage

  hasCheckbox: _fieldRememberValueSupported
  checkboxChecked: _fieldRememberValueState

  readOnly: _fieldFormIsReadOnly || !_fieldIsEditable
  shouldShowValidation: !_fieldFormIsReadOnly

  onCheckboxCheckedChanged: {
    root.rememberValueBoxClicked( checkboxChecked )
  }

  textField.readOnly: true

  onTextClicked: root.openCalendar()
  onRightContentClicked: root.newDateSelected( new Date() )

  rightContent: MMComponents.MMIcon {
    id: rightIcon

    size: __style.icon24
    source: __style.calendarIcon
    color: root.iconColor
  }

  Loader {
    id: dateTimeDrawerLoader

    asynchronous: true
    active: false
    sourceComponent: dateTimeDrawerBlueprint
  }

  Component {
    id: dateTimeDrawerBlueprint

    MMFormComponents.MMCalendarDrawer {
      id: dateTimeDrawer

      title: root._fieldTitle
      dateTime: root._fieldValueIsNull || root._fieldHasMixedValues ? new Date() : dateTransformer.toJsDate( root._fieldValue )
      hasDatePicker: root.includesDate
      hasTimePicker: root.includesTime
      showSeconds: root.showSeconds

      onPrimaryButtonClicked: {
        root.newDateSelected( dateTime )
      }

      onClosed: dateTimeDrawerLoader.active = false

      Component.onCompleted: open()
    }
  }

  function openCalendar() {
    if (root._fieldValueIsNull || _fieldHasMixedValues) {
      root.openPicker( new Date() )
    }
    else {
      root.openPicker( dateTransformer.toJsDate(root._fieldValue) )
    }
  }

  QtObject {
    id: dateTransformer
    // When changing this function, test with various timezones!
    // On desktop, use environment variable TZ, e.g. TZ=America/Mexico_City (UTC-5)
    function toJsDate(qtDate) {
      if ( root._field.isDateOrTime ) {
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
        return Date.fromLocaleString(Qt.locale(), qtDate, root._fieldConfig['field_format'])
      }
    }
  }

  function newDateSelected( jsDate ) {

    if ( jsDate ) {
      if ( root._field.isDateOrTime ) {
        // For QDate, the year, month and day is clipped based on
        // the local timezone in QgsFeature.convertCompatible
        root.editorValueChanged( jsDate, false )
      }
      else {
        let qtDate = jsDate.toLocaleString(Qt.locale(), root._fieldConfig['field_format'])
        root.editorValueChanged(qtDate, false)
      }
    }
  }

  function formatText( qtDate ) {
    if ( qtDate === undefined || root._fieldValueIsNull || root._fieldHasMixedValues) {
      return ''
    }
    else {
      let jsDate = dateTransformer.toJsDate(qtDate)
      console.log("DATETETETETTE: ", jsDate);
      return Qt.formatDateTime(jsDate, root._fieldConfig['display_format'])
    }
  }

  function openPicker(requestedDate) {
    dateTimeDrawerLoader.active = true
    dateTimeDrawerLoader.focus = true
  }
}
