/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../../inputs"

/*
 * Switch (boolean) editor for QGIS Attribute Form
 * Requires various global properties set to function, see featureform Loader section.
 * These properties are injected here via 'fieldXYZ' properties and captured with underscore `_`.
 *
 * Should be used only within feature form.
 * See MMBaseSingleLineInput
 */

MMSwitchInput {
  id: root

  property var _field: parent.field
  property var _fieldValue: parent.fieldValue
  property var _fieldConfig: parent.fieldConfig

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

  title: _fieldShouldShowTitle ? _fieldTitle : ""

  warningMsg: _fieldWarningMessage
  errorMsg: _fieldErrorMessage

  readOnly: _fieldFormIsReadOnly || !_fieldIsEditable
  shouldShowValidationMsg: !_fieldFormIsReadOnly

  hasCheckbox: _fieldRememberValueSupported
  checkboxChecked: _fieldRememberValueState

  text: checked ? internal.checkedStateValue : internal.uncheckedStateValue

  checked: _fieldValue === internal.checkedStateValue

  onToggled: {
    let newVal = checked ? internal.checkedStateValue : internal.uncheckedStateValue
    editorValueChanged( newVal, false )
  }

  onCheckboxCheckedChanged: {
    rememberValueBoxClicked( checkboxChecked )
  }

  function getConfigValue( configValue, defaultValue ) {
    if ( !configValue && root._field.type + "" === internal.booleanEnum ) {
      return defaultValue
    } else return configValue
  }

  QtObject {
    id: internal

    property var checkedStateValue: getConfigValue( root._fieldConfig['CheckedState'], true )
    property var uncheckedStateValue: getConfigValue( root._fieldConfig['UncheckedState'], false )
    property string booleanEnum: "1" // QMetaType::Bool Enum of Qvariant::Type
  }
}
