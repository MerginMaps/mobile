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

import "../../components/private" as MMPrivateComponents

/*
 * Number slider editor for QGIS Attribute Form
 * Requires various global properties set to function, see featureform Loader section.
 * These properties are injected here via 'fieldXYZ' properties and captured with underscore `_`.
 *
 * Should be used only within feature form.
 */

MMPrivateComponents.MMBaseSingleLineInput {
  id: root

  property var _fieldValue: parent.fieldValue
  property var _fieldConfig: parent.fieldConfig

  property bool _fieldShouldShowTitle: parent.fieldShouldShowTitle
  property bool _fieldFormIsReadOnly: parent.fieldIsReadOnly
  property bool _isAttributeEditable: parent.isAttributeEditable

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

  readOnly: _fieldFormIsReadOnly

  hasCheckbox: _fieldRememberValueSupported
  checkboxChecked: _fieldRememberValueState

  onCheckboxCheckedChanged: {
    root.rememberValueBoxClicked( checkboxChecked )
  }

  textField {
    text: Number( slider.value ).toFixed( internal.precision ).toLocaleString( root.locale ) + ' ' + internal.suffix
    readOnly: true
  }

  rightContentMouseArea.enabled: false

  rightContent: Slider {
    id: slider

    width: root.width / 2

    to: internal.to
    from: internal.from
    stepSize: internal.step

    enabled: root.editState === "enabled"
    value: root._fieldValue ? root._fieldValue : 0

    onPressedChanged: textField.focus = true
    onValueChanged: root.editorValueChanged( slider.value, false )

    background: Rectangle {
      x: slider.leftPadding
      y: slider.topPadding + slider.availableHeight / 2 - height / 2
      width: slider.availableWidth
      height: __style.row4
      radius: __style.radius2

      color: {
        if ( root.validationState === "error" ) return __style.negativeColor
        if ( root.validationState === "warning" ) return __style.warningColor
        return __style.lightGreenColor
      }

      Rectangle {
        // fill indicator
        height: parent.height
        width: slider.visualPosition * parent.width

        color: root.iconColor

        radius: __style.radius2
      }
    }

    handle: Rectangle {
      x: slider.leftPadding + slider.visualPosition * (slider.availableWidth - width)
      y: slider.topPadding + slider.availableHeight / 2 - height / 2
      width: 20 * __dp
      height: width

      radius: height / 2

      color: root.iconColor
    }
  }

  QtObject {
    id: internal

    property var locale: Qt.locale()

    property real from: fixRange( _fieldConfig["Min"] )
    property real to: fixRange( _fieldConfig["Max"] )

    property int precision: _fieldConfig["Precision"]
    property real step: _fieldConfig["Step"] ? _fieldConfig["Step"] : 1
    property string suffix: _fieldConfig["Suffix"] ? _fieldConfig["Suffix"] : ""

    readonly property int intMax: 2000000000 // https://doc.qt.io/qt-5/qml-int.html

    function fixRange( rangeValue ) {
      if ( typeof rangeValue !== 'undefined' ) {

        if ( rangeValue >= -internal.intMax && rangeValue <= internal.intMax ) {
          return rangeValue
        }
      }

      return internal.intMax
    }
  }
}
