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

/*
 * QR/Barcode scanner editor for QGIS Attribute Form
 * Requires various global properties set to function, see featureform Loader section.
 * These properties are injected here via 'fieldXYZ' properties and captured with underscore `_`.
 *
 * Should be used only within feature form.
 */

MMPrivateComponents.MMBaseSingleLineInput  {
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

  signal editorValueChanged( var newValue, bool isNull )
  signal rememberValueBoxClicked( bool state )

  title: _fieldShouldShowTitle ? _fieldTitle : ""

  warningMsg: _fieldWarningMessage
  errorMsg: _fieldErrorMessage

  readOnly: _fieldIsReadOnly

  hasCheckbox: _fieldRememberValueSupported
  checkboxChecked: _fieldRememberValueState

  onCheckboxCheckedChanged: {
    root.rememberValueBoxClicked( checkboxChecked )
  }

  text: root._fieldValue === undefined || root._fieldValueIsNull ? '' : root._fieldValue

  onTextEdited: root.editorValueChanged( root.text, root.text === "" )

  rightContent: MMComponents.MMIcon {
    property bool pressed: false

    size: __style.icon24
    source: __style.qrCodeIcon
    color: root.editState === "enabled" ? __style.forestColor : __style.mediumGreenColor
  }

  onRightContentClicked: {
    if ( root.editState !== "enabled"  )
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

    MMComponents.MMCodeScanner {
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
