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
 * Number (range editable) editor for QGIS Attribute Form
 * Requires various global properties set to function, see featureform Loader section.
 * These properties are injected here via 'fieldXYZ' properties and captured with underscore `_`.
 *
 * Should be used only within feature form.
 */

MMPrivateComponents.MMBaseSingleLineInput {
  id: root

  property var _fieldValue: parent.fieldValue
  property var _fieldConfig: parent.fieldConfig
  property bool _fieldValueIsNull: parent.fieldValueIsNull

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
  readOnly: _fieldFormIsReadOnly || !_fieldIsEditable
  shouldShowValidation: !_fieldFormIsReadOnly

  errorMsg: _fieldErrorMessage
  warningMsg: _fieldWarningMessage

  hasCheckbox: _fieldRememberValueSupported
  checkboxChecked: _fieldRememberValueState

  onCheckboxCheckedChanged: {
    root.rememberValueBoxClicked( checkboxChecked )
  }

  leftContent: MMComponents.MMIcon {
    id: leftIcon

    size: __style.icon24
    source: __style.minusIcon
    color: {
      if ( root.editState !== "enabled" ) return __style.mediumGreyColor
      if ( internal.canSubtractStep ) {
        if ( root.validationState === "error" ) return __style.grapeColor
        if ( root.validationState === "warning" ) return __style.earthColor
        return __style.forestColor
      }
      else {
        if ( root.validationState === "error" ) return __style.negativeColor
        if ( root.validationState === "warning" ) return __style.warningColor
        return __style.mediumGreyColor
      }
    }
  }

  textField {
    text: root._fieldValue === undefined || root._fieldValueIsNull ? '' : root._fieldValue

    clip: true

    // AlignHCenter with optional suffix
    leftPadding: Math.max( 0, ( textField.width / 2 - textField.contentWidth / 2 ) - ( internal.suffix ? suffixText.width / 2 : 0 ) )

    inputMethodHints: Qt.ImhFormattedNumbersOnly

    background: Rectangle {
      color: "transparent"

      // Suffix is added as a part of the background property in order to not block clicks to the textField
      MMComponents.MMText {
        id: suffixText

        property real maxWidth: textField.width / 2

        width: Math.min( implicitWidth + __style.margin4, maxWidth )
        x: textField.leftPadding + textField.contentWidth + __style.margin4
        anchors.verticalCenter: parent.verticalCenter

        color: __style.nightColor
        font: __style.p5

        text: internal.suffix

        visible: internal.suffix && textField.text.length > 0
      }
    }
  }

  rightContent: MMComponents.MMIcon {
    id: rightIcon

    size: __style.icon24
    source: __style.plusIcon
    color: {
      if ( root.editState !== "enabled" ) return __style.mediumGreyColor
      if ( internal.canAddStep ) {
        if ( root.validationState === "error" ) return __style.grapeColor
        if ( root.validationState === "warning" ) return __style.earthColor
        return __style.forestColor
      }
      else {
        if ( root.validationState === "error" ) return __style.negativeColor
        if ( root.validationState === "warning" ) return __style.warningColor
        return __style.mediumGreyColor
      }
    }
  }

  onLeftContentClicked: {
    if ( internal.canSubtractStep ) {
      let decremented = Number( textField.text ) - internal.step
      root.editorValueChanged( decremented.toFixed( internal.precision ), false )
    }
  }

  onTextEdited: ( text ) => {
    let val = text.replace( ",", "." ).replace( / /g, '' ) // replace comma with dot

    root.editorValueChanged( val, val  === "" )
  }

  onRightContentClicked: {
    if ( internal.canAddStep ) {
      let incremented = Number( textField.text ) + internal.step
      root.editorValueChanged( incremented.toFixed( internal.precision ), false )
    }
  }

  QtObject {
    id: internal

    property real to: _fieldConfig["Max"]
    property real from: _fieldConfig["Min"]
    property string suffix: _fieldConfig['Suffix'] ? _fieldConfig['Suffix'] : ''
    property real precision: _fieldConfig['Precision'] ? _fieldConfig['Precision'] : 0


    // don't ever use a step smaller than would be visible in the widget
    // i.e. if showing 2 decimals, smallest increment will be 0.01
    // https://github.com/qgis/QGIS/blob/a038a79997fb560e797daf3903d94c7d68e25f42/src/gui/editorwidgets/qgsdoublespinbox.cpp#L83-L87
    property real step: Math.max(_fieldConfig["Step"], Math.pow( 10.0, 0.0 - precision ))

    property bool canSubtractStep: Number( root.textField.text ) - internal.step >= internal.from
    property bool canAddStep: Number( root.textField.text ) + internal.step <= internal.to
  }

  // on press and hold behavior can be used from here:
  // https://github.com/mburakov/qt5/blob/93bfa3874c10f6cb5aa376f24363513ba8264117/qtquickcontrols/src/controls/SpinBox.qml#L306-L309
}
