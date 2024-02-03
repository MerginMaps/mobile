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
import "../../components"
import "../../inputs"

/*
 * Switch (boolean) editor for QGIS Attribute Form
 * Requires various global properties set to function, see featureform Loader section.
 * These properties are injected here via 'fieldXYZ' properties and captured with underscore `_`.
 *
 * Should be used only within feature form.
 */
MMBaseInput {
  id: root

  property var _field: parent.field
  property var _fieldValue: parent.fieldValue
  property var _fieldConfig: parent.fieldConfig

  property bool _fieldShouldShowTitle: parent.fieldShouldShowTitle
  property bool _fieldIsReadOnly: parent.fieldIsReadOnly

  property string _fieldTitle: parent.fieldTitle
  property string _fieldErrorMessage: parent.fieldErrorMessage
  property string _fieldWarningMessage: parent.fieldWarningMessage

  signal editorValueChanged( var newValue, var isNull )

  title: _fieldShouldShowTitle ? _fieldTitle : ""

  warningMsg: _fieldWarningMessage
  errorMsg: _fieldErrorMessage

  enabled: !_fieldIsReadOnly

  hasFocus: rightSwitch.focus

  content: Text {
    id: textField

    width: parent.width + rightSwitch.x
    anchors.verticalCenter: parent.verticalCenter

    text: rightSwitch.checked ? internal.checkedStateValue : internal.uncheckedStateValue

    color: root.enabled ? __style.nightColor : __style.mediumGreenColor
    font: __style.p5
    elide: Text.ElideRight
  }

  onContentClicked: {
    rightSwitch.toggle()
  }

  rightAction: MMSwitch {
    id: rightSwitch

    width: 50
    height: parent.height
    x: -30 * __dp

    checked: root._fieldValue === internal.checkedStateValue

    onCheckedChanged: {
      let newVal = rightSwitch.checked ? internal.checkedStateValue : internal.uncheckedStateValue
      root.editorValueChanged( newVal, false )
    }
  }

  function getConfigValue( configValue, defaultValue ) {
    if ( !configValue && root._field.type + "" === internal.booleanEnum ) {
      return defaultValue
    } else return configValue
  }

  QtObject {
    id: internal

    property var checkedStateValue: getConfigValue( root._fieldConfig['CheckedState'], true )
    property var uncheckedStateValue: getConfigValue( root._fieldConfig['UncheckedState'], false )
    property string booleanEnum: "1" // QMetaType::Bool Enum of Qvariant::Type
  }
}
