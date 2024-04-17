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
  property bool _fieldIsReadOnly: parent.fieldIsReadOnly

  property string _fieldTitle: parent.fieldTitle
  property string _fieldErrorMessage: parent.fieldErrorMessage
  property string _fieldWarningMessage: parent.fieldWarningMessage

  property bool _fieldRememberValueSupported: parent.fieldRememberValueSupported
  property bool _fieldRememberValueState: parent.fieldRememberValueState

  signal editorValueChanged( var newValue, var isNull )
  signal rememberValueBoxClicked( bool state )

  title: _fieldShouldShowTitle ? _fieldTitle : ""
  readOnly: _fieldIsReadOnly

  errorMsg: _fieldErrorMessage
  warningMsg: _fieldWarningMessage

  hasCheckbox: _fieldRememberValueSupported
  checkboxChecked: _fieldRememberValueState

  onCheckboxCheckedChanged: {
    root.rememberValueBoxClicked( checkboxChecked )
  }

  leftContentMouseArea.enabled: root.editState === "enabled" && internal.canSubtractStep
  leftContent: MMComponents.MMIcon {
    id: leftIcon

    size: __style.icon24
    source: __style.minusIcon
    color: root.editState === "enabled" && internal.canSubtractStep ? __style.forestColor : __style.mediumGreenColor
  }

  textField {
    text: root._fieldValue === undefined || root._fieldValueIsNull ? '' : root._fieldValue

    clip: true
    horizontalAlignment: Qt.AlignHCenter

    inputMethodHints: Qt.ImhFormattedNumbersOnly

    background: Rectangle {
      color: "transparent"

      // add suffix
      Text {
        anchors.centerIn: parent
        anchors.horizontalCenterOffset: textField.contentWidth / 2 + __style.margin10

        color: __style.nightColor
        font: __style.p5

        text: internal.suffix

        visible: internal.suffix && textField.text.length > 0
      }
    }
  }

  rightContentMouseArea.enabled: root.editState === "enabled" && internal.canAddStep

  rightContent: MMComponents.MMIcon {
    id: rightIcon

    size: __style.icon24
    source: __style.plusIcon
    color: root.editState === "enabled" && internal.canAddStep ? __style.forestColor : __style.mediumGreenColor
  }

  onLeftContentClicked: {
    let decremented = Number( textField.text ) - internal.step
    root.editorValueChanged( decremented.toFixed( internal.precision ), false )
  }

  onTextEdited: ( text ) => {
    let val = text.replace( ",", "." ).replace( / /g, '' ) // replace comma with dot

    root.editorValueChanged( val, val  === "" )
  }

  onRightContentClicked: {
    let incremented = Number( textField.text ) + internal.step
    root.editorValueChanged( incremented.toFixed( internal.precision ), false )
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
