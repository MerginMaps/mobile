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

import "../../components/private" as MMPrivateComponents

/*
 * Text Edit for QGIS Attribute Form
 * Requires various global properties set to function, see featureform Loader section.
 * These properties are injected here via 'fieldXYZ' properties and captured with underscore `_`.
 *
 * Should be used only within feature form.
 * See MMBaseSingleLineInput
 */

MMPrivateComponents.MMBaseSingleLineInput {
  id: root

  property var _field: parent.field
  property var _fieldValue: parent.fieldValue
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

  signal editorValueChanged( var newValue, bool isNull )
  signal rememberValueBoxClicked( bool state )

  text: _fieldValue === undefined || _fieldValueIsNull || _fieldHasMixedValues ? '' : _fieldValue
  placeholderText: _fieldHasMixedValues ? _fieldValue : ""

  readOnly: _fieldFormIsReadOnly || !_fieldIsEditable
  shouldShowValidation: !_fieldFormIsReadOnly

  title: _fieldShouldShowTitle ? _fieldTitle : ""

  warningMsg: _fieldWarningMessage
  errorMsg: _fieldErrorMessage

  hasCheckbox: _fieldRememberValueSupported
  checkboxChecked: _fieldRememberValueState

  textField.maximumLength: {
    if ( ( !root._field.isNumeric ) && ( root._field.length > 0 ) ) {
      return root._field.length
    }
    return internal.textMaxCharactersLimit
  }

  onCheckboxCheckedChanged: {
    root.rememberValueBoxClicked( checkboxChecked )
  }

  onTextEdited: function ( text ) {
    let val = text
    if ( root._field.isNumeric )
    {
      val = val.replace( ",", "." ).replace( / /g, '' ) // replace comma with dot and remove spaces
    }

    root.editorValueChanged( val, val === "" )
  }

  Component.onCompleted: {
    // Use numerical keyboard for number fields configured as texts
    if ( root._field.isNumeric ) {
      textField.inputMethodHints |= Qt.ImhFormattedNumbersOnly
    }
  }

  QtObject {
    id: internal

    property int textMaxCharactersLimit: 32767 // Qt default
  }
}
