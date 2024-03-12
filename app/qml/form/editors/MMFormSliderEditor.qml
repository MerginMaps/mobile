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
import QtQuick.Layouts

import "../../inputs"

/*
 * Number slider editor for QGIS Attribute Form
 * Requires various global properties set to function, see featureform Loader section.
 * These properties are injected here via 'fieldXYZ' properties and captured with underscore `_`.
 *
 * Should be used only within feature form.
 */

MMBaseInput {
  id: root

  property var _fieldValue: parent.fieldValue
  property var _fieldConfig: parent.fieldConfig

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

  warningMsg: _fieldWarningMessage
  errorMsg: _fieldErrorMessage

  hasFocus: slider.activeFocus
  enabled: !_fieldIsReadOnly

  hasCheckbox: _fieldRememberValueSupported
  checkboxChecked: _fieldRememberValueState

  onCheckboxCheckedChanged: {
    root.rememberValueBoxClicked( checkboxChecked )
  }

  content: Item {
    id: input

    anchors.fill: parent

    RowLayout {
      id: rowLayout

      anchors.fill: parent

      Text {
        id: valueLabel

        Layout.preferredWidth: rowLayout.width / 2 - root.spacing - 14 * __dp
        Layout.maximumWidth: rowLayout.width / 2 - root.spacing - 14 * __dp
        Layout.preferredHeight: input.height
        Layout.maximumHeight: input.height

        elide: Text.ElideRight
        text: Number( slider.value ).toFixed( internal.precision ).toLocaleString( root.locale ) + ' ' + internal.suffix

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        font: __style.p5
        color: root.enabled ? __style.nightColor : __style.mediumGreenColor
      }

      Slider {
        id: slider

        Layout.fillWidth: true
        Layout.maximumHeight: input.height
        Layout.preferredHeight: input.height

        to: internal.to
        from: internal.from
        stepSize: internal.step
        value: root._fieldValue ? root._fieldValue : 0

        onValueChanged: root.editorValueChanged( slider.value, false )

        background: Rectangle {
          x: slider.leftPadding
          y: slider.topPadding + slider.availableHeight / 2 - height / 2
          width: slider.availableWidth
          height: 4 * __dp
          radius: 2 * __dp

          color: __style.lightGreenColor
        }

        handle: Rectangle {
          x: slider.leftPadding + slider.visualPosition * (slider.availableWidth - width)
          y: slider.topPadding + slider.availableHeight / 2 - height / 2
          width: 20 * __dp
          height: width
          radius: height / 2

          color: root.enabled ? __style.forestColor : __style.lightGreenColor
        }
      }
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
