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

/**
 * Single-line input for the filters panel.
 * See MMBaseSingleLineInput for additional properties.
 */
MMPrivateComponents.MMBaseSingleLineInput {
  id: root

  enum InputType { Text, Number, Date, Dropdown }

  property bool checked: false
  property int type: MMFilterTextInput.InputType.Text

  // date and dropdown types open pickers instead of accepting keyboard input
  textField.readOnly: root.type === MMFilterTextInput.InputType.Date || root.type === MMFilterTextInput.InputType.Dropdown
  textField.inputMethodHints: root.type === MMFilterTextInput.InputType.Number ? Qt.ImhFormattedNumbersOnly : Qt.ImhNone
  textField.color: {
    if ( root.editState === "readOnly" ) return __style.nightColor
    if ( root.editState === "enabled" ) return __style.nightColor
    if ( root.editState === "disabled" ) return __style.mediumGreyColor
    return __style.forestColor
  }

  // error state takes priority over checked, checked over default
  textFieldBackground.color: {
    if ( root.validationState === "error" ) return __style.negativeUltraLightColor
    if ( root.checked ) return __style.positiveColor
    return __style.lightGreenColor
  }

  textFieldBackground.border.color: {
    if ( root.validationState === "error" ) return __style.negativeColor
    if ( root.checked ) return __style.darkGreenColor
    return __style.polarColor
  }

  textFieldBackground.border.width: {
    if ( root.validationState === "error" ) return __style.width2
    if ( root.checked ) return 1 * __dp
    return 0
  }

  // close icon when checked, type-specific icon otherwise
  rightContent: MMComponents.MMIcon {
    size: __style.icon24
    source: {
      if ( root.checked ) return __style.closeIcon
      if ( root.type === MMFilterTextInput.InputType.Date ) return __style.calendarIcon
      if ( root.type === MMFilterTextInput.InputType.Dropdown ) return __style.arrowDownIcon
      return __style.closeIcon
    }
    color: __style.forestColor
  }

  // picker types always show the icon, editable types only when there is a value
  rightContentVisible: {
    if ( root.type === MMFilterTextInput.InputType.Text || root.type === MMFilterTextInput.InputType.Number ) return root.checked
    return true
  }

  // keep checked in sync with whether the field has a value
  onTextChanged: {
    root.checked = root.text.length > 0
  }

  // clear the field when tapping the close icon
  onRightContentClicked: {
    if ( ( root.type === MMFilterTextInput.InputType.Text || root.type === MMFilterTextInput.InputType.Number ) && root.checked ) {
      textField.clear()
      root.checked = false
    }
  }

  Component.onCompleted: {
    if ( root.text ) {
      root.checked = true
    }
  }
}
