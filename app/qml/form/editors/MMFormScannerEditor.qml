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
 * QR/Barcode scanner editor for QGIS Attribute Form
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

  property alias placeholderText: textField.placeholderText
  property alias text: textField.text

  signal editorValueChanged( var newValue, bool isNull )
  signal rememberValueBoxClicked( bool state )

  title: _fieldShouldShowTitle ? _fieldTitle : ""

  warningMsg: _fieldWarningMessage
  errorMsg: _fieldErrorMessage

  hasFocus: textField.activeFocus
  enabled: !_fieldIsReadOnly

  hasCheckbox: _fieldRememberValueSupported
  checkboxChecked: _fieldRememberValueState

  onCheckboxCheckedChanged: {
    root.rememberValueBoxClicked( checkboxChecked )
  }

  content: TextField {
    id: textField

    anchors.fill: parent
    anchors.verticalCenter: parent.verticalCenter

    readOnly: !root.enabled

    text: root._fieldValue === undefined || root._fieldValueIsNull ? '' : root._fieldValue

    color: root.enabled ? __style.nightColor : __style.mediumGreenColor
    placeholderTextColor: __style.nightAlphaColor

    font: __style.p5
    hoverEnabled: true

    background: Rectangle { color: __style.transparentColor }

    onTextEdited: root.editorValueChanged( textField.text, textField.text === "" )
  }

  rightAction: MMIcon {
    property bool pressed: false

    anchors.verticalCenter: parent.verticalCenter

    size: __style.icon24
    source: __style.qrCodeIcon
    color: root.enabled ? __style.forestColor : __style.mediumGreenColor
  }

  onRightActionClicked: {
    if ( !root.enabled )
      return

    if (!__inputUtils.acquireCameraPermission())
      return

    codeScannerLoader.active = true
    codeScannerLoader.focus = true
  }

  Loader {
    id: codeScannerLoader

    asynchronous: true
    active: false
    sourceComponent: readerComponent
  }

  Component {
    id: readerComponent

    MMCodeScanner {
      focus: true

      onClosed: codeScannerLoader.active = false

      Component.onCompleted: open()

      onScanFinished: function( captured ) {
        root.editorValueChanged( captured, false )
        close()
      }
    }
  }
}
