/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Layouts

import "../../../components" as MMComponents
import "../../../components/private" as MMPrivateComponents

/**
 * Single-line input for the filters panel. Encapsulates either one or two inputs in one Row.
 */
Row {
  id: root

  property bool doubleInput: false
  property bool disabledKeyboardInput: false
  property url iconSource
  property string errorMsg

  property alias primaryPlaceholderText: primaryInput.placeholderText
  property alias secondaryPlaceholderText: secondaryInput.placeholderText
  property alias primaryText: primaryInput.text
  property alias secondaryText: secondaryInput.text

  signal primaryTextClicked()
  signal secondaryTextClicked()
  signal primaryRightContentClicked( bool isChecked )
  signal secondaryRightContentClicked( bool isChecked )

  spacing: __style.margin12

  MMPrivateComponents.MMBaseSingleLineInput {
    id: primaryInput

    property bool checked: false

    textField.readOnly: root.disabledKeyboardInput

    shouldShowValidation: !root.doubleInput

    textField.color: {
      if (primaryInput.editState === "readOnly") return __style.nightColor
      if (primaryInput.editState === "enabled") return __style.nightColor
      if (primaryInput.editState === "disabled") return __style.mediumGreyColor
      return __style.forestColor
    }

    // error state takes priority over checked, checked over default
    textFieldBackground.color: {
      if (primaryInput.validationState === "error") return __style.negativeUltraLightColor
      if (primaryInput.checked) return __style.positiveColor
      return __style.lightGreenColor
    }

    textFieldBackground.border.color: {
      if (primaryInput.validationState === "error") return __style.negativeColor
      if (primaryInput.checked) return __style.darkGreenColor
      return __style.polarColor
    }

    textFieldBackground.border.width: {
      if (primaryInput.validationState === "error") return __style.width2
      if (primaryInput.checked) return 1 * __dp
      return 0
    }

    // close icon when checked, type-specific icon otherwise
    rightContent: MMComponents.MMIcon
    {
      size: __style.icon24
      source: {
        if (primaryInput.checked) return __style.closeIcon
        return root.iconSource
      }
      color: __style.forestColor
    }

    rightContentVisible: {
      if (root.iconSource) return primaryInput.checked
      return true
    }

    // keep checked in sync with whether the field has a value
    onTextEdited: (newText) => {
      primaryInput.checked = newText.length
    }

    onTextClicked: {
      root.primaryTextClicked()
    }

    // clear the field when tapping the close icon
    onRightContentClicked: {
      if ( primaryInput.checked ) {
        textField.clear()
        root.primaryRightContentClicked( true )
        primaryInput.checked = false
      } else {
        root.primaryRightContentClicked( false )
      }
    }

    Component.onCompleted: {
      if (primaryInput.text) {
        primaryInput.checked = true
      }
    }
  }

  MMPrivateComponents.MMBaseSingleLineInput {
    id: secondaryInput

    property bool checked: false

    visible: root.doubleInput

    shouldShowValidation: false
    textField.readOnly: root.disabledKeyboardInput

    textField.color: {
      if (secondaryInput.editState === "readOnly") return __style.nightColor
      if (secondaryInput.editState === "enabled") return __style.nightColor
      if (secondaryInput.editState === "disabled") return __style.mediumGreyColor
      return __style.forestColor
    }

    // error state takes priority over checked, checked over default
    textFieldBackground.color: {
      if (secondaryInput.validationState === "error") return __style.negativeUltraLightColor
      if (secondaryInput.checked) return __style.positiveColor
      return __style.lightGreenColor
    }

    textFieldBackground.border.color: {
      if (secondaryInput.validationState === "error") return __style.negativeColor
      if (secondaryInput.checked) return __style.darkGreenColor
      return __style.polarColor
    }

    textFieldBackground.border.width: {
      if (secondaryInput.validationState === "error") return __style.width2
      if (secondaryInput.checked) return 1 * __dp
      return 0
    }

    // close icon when checked, type-specific icon otherwise
    rightContent: MMComponents.MMIcon
    {
      size: __style.icon24
      source: {
        if (secondaryInput.checked) return __style.closeIcon
        return root.iconSource
      }
      color: __style.forestColor
    }

    rightContentVisible: {
      if (root.iconSource) return secondaryInput.checked
      return true
    }

    // keep checked in sync with whether the field has a value
    onTextEdited: (newText) => {
      secondaryInput.checked = newText.length
    }

    onTextClicked: {
      root.secondaryTextClicked()
    }

    // clear the field when tapping the close icon
    onRightContentClicked: {
      if ( secondaryInput.checked ) {
        textField.clear()
        root.secondaryRightContentClicked( true )
        secondaryInput.checked = false
      } else {
        root.secondaryRightContentClicked( false )
      }
    }

    Component.onCompleted: {
      if (secondaryInput.text) {
        secondaryInput.checked = true
      }
    }
  }

  Item {
    // light version of validation message from MMBaseInput

    width: root.width
    height: validationMessagegroup.implicitHeight

    visible: root.errorMsg && root.doubleInput

    RowLayout {
      id: validationMessagegroup

      width: root.width

      MMComponents.MMIcon {
        source: __style.errorCircleIcon
        size: __style.icon16
        color: __style.negativeColor
      }

      MMComponents.MMText {
        Layout.fillWidth: true

        text: root.errorMsg
        color:  __style.grapeColor
        font: __style.t4

        wrapMode: Text.Wrap
        maximumLineCount: 10
      }
    }
  }
}
