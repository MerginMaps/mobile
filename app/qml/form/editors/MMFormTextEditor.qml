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

import "../../inputs"

/*
 * Text Edit for QGIS Attribute Form
 * Requires various global properties set to function, see featureform Loader section.
 * These properties are injected here via 'fieldXYZ' properties and captured with underscore `_`.
 *
 * Should be used only within feature form.
 * See MMTextInput
 */

MMTextInput {
  id: root

  property var _field: parent.field
  property var _fieldValue: parent.fieldValue
  property bool _fieldValueIsNull: parent.fieldValueIsNull

  property bool _fieldShouldShowTitle: parent.fieldShouldShowTitle
  property bool _fieldIsReadOnly: parent.fieldIsReadOnly

  property string _fieldTitle: parent.fieldTitle
  property string _fieldErrorMessage: parent.fieldErrorMessage
  property string _fieldWarningMessage: parent.fieldWarningMessage

  property bool _fieldRememberValueSupported: parent.fieldRememberValueSupported
  property bool _fieldRememberValueState: parent.fieldRememberValueState

  signal editorValueChanged( var newValue, bool isNull )
  signal rememberValueBoxClicked( bool state )

  text: _fieldValue === undefined || _fieldValueIsNull ? '' : _fieldValue

  showClearIcon: false

  readOnly: _fieldIsReadOnly
  textFieldComponent.readOnly: _fieldIsReadOnly
  textFieldComponent.inputMethodHints: root._field.isNumeric ? Qt.ImhFormattedNumbersOnly : Qt.ImhNone
  textFieldComponent.color: __style.nightColor

  title: _fieldShouldShowTitle ? _fieldTitle : ""

  warningMsg: _fieldWarningMessage
  errorMsg: _fieldErrorMessage

  hasCheckbox: _fieldRememberValueSupported
  checkboxChecked: _fieldRememberValueState

  textFieldComponent.maximumLength: {
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

  // Avoid Android's uncommited text
  // Could in theory be fixed with `inputMethodComposing` TextInput property instead
  textFieldComponent.onPreeditTextChanged: if ( __androidUtils.isAndroid ) Qt.inputMethod.commit()

  QtObject {
    id: internal

    property int textMaxCharactersLimit: 32767 // Qt default
  }
}
