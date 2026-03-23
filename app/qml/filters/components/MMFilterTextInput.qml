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
 * */ 
MMPrivateComponents.MMBaseSingleLineInput {
  id: root

  property bool checked: false
  property string type: "text"

  // date and dropdown types open pickers instead of accepting keyboard input
  textField.readOnly: root.type === "date" || root.type === "dropdown"

  // error state takes priority over checked, checked over default
  textFieldBackground.color: {
    if (root.validationState === "error") return __style.negativeUltraLightColor
    if (root.checked) return __style.filterGreenColor
    return __style.lightGreenColor
  }

  textFieldBackground.border.color: {
    if (root.validationState === "error") return __style.negativeColor
    if (root.checked) return __style.grassColor
    return __style.polarColor
  }

  textFieldBackground.border.width: {
    if (root.validationState === "error") return __style.width2
    if (root.checked) return 1 * __dp
    return 0
  }

  // close icon when checked, type-specific icon otherwise
  rightContent: MMComponents.MMIcon {
    size: __style.icon24
    source: {
      if (root.checked) return __style.closeIcon
      if (root.type === "date") return __style.calendarIcon
      if (root.type === "dropdown") return __style.arrowDownIcon
      return __style.closeIcon
    }
    color: root.iconColor
  }

  // picker types always show the icon, editable types only when there is a value
  rightContentVisible: {
    if (root.type === "text" || root.type === "number") return root.checked
    return true
  }

  // keep checked in sync with whether the field has a value
  onTextChanged: {
    if (root.type === "text" || root.type === "number") {
      root.checked = (root.text !== "")
    }
  }

  // clear the field when tapping the close icon
  onRightContentClicked: {
    if ((root.type === "text" || root.type === "number") && root.checked) {
      textField.clear()
      root.checked = false
    }
  }
}
