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

// To ignore the warning "The current style does not support customization"
// see from https://stackoverflow.com/questions/76625756/the-current-style-does-not-support-customization-of-this-control
import QtQuick.Controls.Basic

import "../../components/private" as MMPrivateComponents

/*
 * Text multiline editor for QGIS Attribute Form
 * Requires various global properties set to function, see featureform Loader section.
 * These properties are injected here via 'fieldXYZ' properties and captured with underscore `_`.
 *
 * Should be used only within feature form.
 */
MMPrivateComponents.MMBaseInput {
  id: root

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
  property string _fieldHomePath: parent.fieldHomePath

  property bool _fieldRememberValueSupported: parent.fieldRememberValueSupported
  property bool _fieldRememberValueState: parent.fieldRememberValueState

  signal editorValueChanged( var newValue, var isNull )
  signal rememberValueBoxClicked( bool state )

  title: _fieldShouldShowTitle ? _fieldTitle : ""

  warningMsg: _fieldWarningMessage
  errorMsg: _fieldErrorMessage

  readOnly: _fieldFormIsReadOnly || !_fieldIsEditable
  shouldShowValidation: !_fieldFormIsReadOnly

  hasCheckbox: _fieldRememberValueSupported
  checkboxChecked: _fieldRememberValueState

  onCheckboxCheckedChanged: {
    root.rememberValueBoxClicked( checkboxChecked )
  }

  inputContent: TextArea {
    id: textArea

    width: parent.width
    height: Math.max( implicitHeight, internal.minHeight )

    text: _fieldValue === undefined || _fieldHasMixedValues ? '' : _fieldValue
    textFormat: root._fieldConfig['UseHtml'] ? TextEdit.RichText : TextEdit.PlainText
    placeholderText: _fieldHasMixedValues ? _fieldValue : ""

    topPadding: __style.margin12
    bottomPadding: __style.margin12
    leftPadding: __style.margin20
    rightPadding: __style.margin20

    wrapMode: TextEdit.Wrap

    font: __style.p5
    color: {
      if ( root.editState === "readOnly" ) return __style.nightColor
      if ( root.editState === "enabled" ) return __style.nightColor
      if ( root.editState === "disabled" ) return __style.mediumGreyColor
      return __style.nightColor
    }
    placeholderTextColor: __style.darkGreyColor

    readOnly: root.editState !== "enabled"

    // same as in MMBaseSingleLineInput, native selection in read-only fields is broken (QTBUG-141929),
    // so read-only fields don't take focus, have no native selection and copy the whole value on long press instead
    activeFocusOnPress: !readOnly
    inputMethodHints: readOnly ? Qt.ImhNoTextHandles : Qt.ImhNone

    // same as in MMBaseSingleLineInput, focus the field before Android's native long press comes,
    // so it can select the word, and restore the selection that TextArea's held-back press clears on release
    property bool androidLongPressed: false

    onPressed: androidLongPressed = false

    onReleased: {
      if ( androidLongPressed && selectedText !== "" ) {
        const start = selectionStart
        const end = selectionEnd
        Qt.callLater( () => select( start, end ) )
      }
      androidLongPressed = false
    }

    TapHandler {
      enabled: __androidUtils.isAndroid && !textArea.readOnly
      longPressThreshold: 0.3 // Android's native long press comes after 0.4 s

      onLongPressed: {
        textArea.forceActiveFocus()
        textArea.androidLongPressed = true
      }
    }

    // TapHandler cancels the long press once the finger moves (e.g. scrolling the form), TextArea's pressAndHold does not
    TapHandler {
      enabled: textArea.readOnly

      onLongPressed: {
        if ( textArea.text.trim() === "" ) return

        textArea.selectAll()
        textArea.copy()
        textArea.deselect()
        __notificationModel.addSuccess( qsTr( "Copied to clipboard" ) )
      }
    }

    background: Rectangle {

      color: {
        if ( root.editState !== "enabled" ) return __style.polarColor
        if ( root.validationState === "error" ) return __style.negativeUltraLightColor
        if ( root.validationState === "warning" ) return __style.negativeUltraLightColor

        return __style.polarColor
      }

      border.width: {
        if ( root.validationState === "error" ) return __style.width2
        if ( root.validationState === "warning" ) return __style.width2
        if ( textArea.activeFocus ) return __style.width2
        if ( textArea.hovered ) return __style.width1
        return 0
      }

      border.color: {
        if ( root.editState !== "enabled" ) return __style.polarColor
        if ( root.validationState === "error" ) return __style.negativeColor
        if ( root.validationState === "warning" ) return __style.warningColor
        if ( textArea.activeFocus ) return __style.forestColor
        if ( textArea.hovered ) return __style.forestColor

        return __style.polarColor
      }

      radius: __style.radius12
    }

    onLinkActivated: function ( link ) {
      if ( !__inputUtils.openLink( root._fieldHomePath, link.toString( ) ) )
      {
        __notificationModel.addError( qsTr( "Could not open the file. It may not exist, could be invalid, or there might be no application available to open it." ) )
      }
    }

    onTextChanged: root.editorValueChanged( textArea.text, textArea.text === "" )
    onPreeditTextChanged: if( __androidUtils.isAndroid ) Qt.inputMethod.commit() // to avoid Android's unconfirmed text
  }

  FontMetrics {
    id: metrics
    font: textArea.font
  }

  QtObject {
    id: internal

    // Minimum height for multiline is 3 lines + paddings
    property real minHeight: metrics.height * 3 + textArea.topPadding + textArea.bottomPadding
  }
}
