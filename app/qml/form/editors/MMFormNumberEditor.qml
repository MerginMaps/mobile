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
 * Number (range editable) editor for QGIS Attribute Form
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

  property alias placeholderText: numberInput.placeholderText

  signal editorValueChanged( var newValue, var isNull )
  signal rememberValueBoxClicked( bool state )


  title: _fieldShouldShowTitle ? _fieldTitle : ""

  errorMsg: _fieldErrorMessage
  warningMsg: _fieldWarningMessage

  enabled: !_fieldIsReadOnly
  hasFocus: numberInput.activeFocus

  hasCheckbox: _fieldRememberValueSupported
  checkboxChecked: _fieldRememberValueState

  onCheckboxCheckedChanged: {
    root.rememberValueBoxClicked( checkboxChecked )
  }

  leftAction: MMIcon {
    id: leftIcon

    height: parent.height

    source: __style.minusIcon
    color: enabled ? __style.forestColor : __style.mediumGreenColor
    enabled: Number( numberInput.text ) - internal.step >= internal.from
  }

  onLeftActionClicked: {
    if ( leftIcon.enabled )
    {
      let decremented = Number( numberInput.text ) - internal.step
      root.editorValueChanged( decremented.toFixed( internal.precision ), false )
    }
  }

  content: Item {
    anchors.fill: parent

    Row {
      height: parent.height
      anchors.horizontalCenter: parent.horizontalCenter
      clip: true

      TextField {
        id: numberInput

        height: parent.height

        text: root._fieldValue === undefined || root._fieldValueIsNull ? '' : root._fieldValue

        placeholderTextColor: __style.nightAlphaColor
        color: root.enabled ? __style.nightColor : __style.mediumGreenColor

        font: __style.p5

        clip: true
        hoverEnabled: true

        verticalAlignment: Qt.AlignVCenter
        inputMethodHints: Qt.ImhFormattedNumbersOnly

        onTextEdited: {
          let val = text.replace( ",", "." ).replace( / /g, '' ) // replace comma with dot

          root.editorValueChanged( val, val  === "" )
        }

        background: Rectangle {
          color: __style.transparentColor
        }
      }

      Text {
        id: suffix

        text: internal.suffix ? ' ' + internal.suffix : "" // to make sure there is a space between the number and the suffix

        visible: internal.suffix !== "" && numberInput.text !== ""

        height: parent.height
        verticalAlignment: Qt.AlignVCenter

        font: __style.p5
        color: numberInput.color
      }
    }
  }

  rightAction: MMIcon {
    id: rightIcon

    height: parent.height

    source: __style.plusIcon
    color: enabled ? __style.forestColor : __style.mediumGreenColor
    enabled: Number( numberInput.text ) + internal.step <= internal.to
  }

  onRightActionClicked: {
    if ( rightIcon.enabled )
    {
      let incremented = Number( numberInput.text ) + internal.step
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
  }

  // on press and hold behavior can be used from here:
  // https://github.com/mburakov/qt5/blob/93bfa3874c10f6cb5aa376f24363513ba8264117/qtquickcontrols/src/controls/SpinBox.qml#L306-L309
}
