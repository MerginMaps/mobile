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
import "../../components"
import "../../inputs"

/*
 * Text multiline editor for QGIS Attribute Form
 * Requires various global properties set to function, see featureform Loader section.
 * These properties are injected here via 'fieldXYZ' properties and captured with underscore `_`.
 *
 * Should be used only within feature form.
 */
MMBaseInput {
  id: root

  property var _fieldValue: parent.fieldValue
  property var _fieldConfig: parent.fieldConfig
  property bool _fieldValueIsNull: parent.fieldValueIsNull

  property bool _fieldShouldShowTitle: parent.fieldShouldShowTitle
  property bool _fieldIsReadOnly: parent.fieldIsReadOnly

  property string _fieldTitle: parent.fieldTitle
  property string _fieldErrorMessage: parent.fieldErrorMessage
  property string _fieldWarningMessage: parent.fieldWarningMessage

  property bool _fieldRememberValueSupported: parent.fieldRememberValueSupported
  property bool _fieldRememberValueState: parent.fieldRememberValueState

  property alias placeholderText: textArea.placeholderText
  property string text: _fieldValue === undefined || _fieldValueIsNull ? '' : _fieldValue

  property int minimumRows: 3

  signal editorValueChanged( var newValue, var isNull )
  signal rememberValueBoxClicked( bool state )

  title: _fieldShouldShowTitle ? _fieldTitle : ""

  warningMsg: _fieldWarningMessage
  errorMsg: _fieldErrorMessage

  enabled: !_fieldIsReadOnly

  hasFocus: textArea.activeFocus

  hasCheckbox: _fieldRememberValueSupported
  checkboxChecked: _fieldRememberValueState

  onCheckboxCheckedChanged: {
    root.rememberValueBoxClicked( checkboxChecked )
  }

  contentItemHeight: {
    const minHeight = 34 * __dp + metrics.height * root.minimumRows
    var realHeight = textArea.y + textArea.contentHeight + 2 * textArea.verticalPadding
    return realHeight < minHeight ? minHeight : realHeight
  }

  content: TextArea {
    id: textArea

    property real verticalPadding: 11 * __dp

    y: textArea.verticalPadding
    height: contentHeight + textArea.verticalPadding
    width: parent.width

    text: root.text
    textFormat: root._fieldConfig['UseHtml'] ? TextEdit.RichText : TextEdit.PlainText

    hoverEnabled: true
    placeholderTextColor: __style.nightAlphaColor
    color: __style.nightColor

    font: __style.p5
    wrapMode: Text.WordWrap

    onLinkActivated: function( link ) {
      if ( link.startWith( "file://" ) ) {
        __androidUtils.openFile( link )
      }
      else {
      Qt.openUrlExternally( link )
      }
    }

    onTextChanged: root.editorValueChanged( text, text === "" )

    // Avoid Android's uncommited text
    // Could in theory be fixed with `inputMethodComposing` TextInput property instead
    onPreeditTextChanged: if ( __androidUtils.isAndroid ) Qt.inputMethod.commit()
  }

  FontMetrics {
    id: metrics
    font: textArea.font
  }
}
