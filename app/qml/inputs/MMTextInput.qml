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
import "../components"

/*
 * Common text input to use in the app.
 * Disabled state can be achieved by setting `enabled: false`.
 *
 * See MMBaseInput for more properties.
 */

MMBaseInput {
  id: root

  property bool showClearIcon: true
  property alias text: textField.text
  property alias placeholderText: textField.placeholderText

  property alias textFieldComponent: textField

  signal textEdited( string text )

  hasFocus: textField.activeFocus

  content: TextField {
    id: textField

    anchors.fill: parent

    placeholderTextColor: __style.nightAlphaColor
    color: root.enabled ? __style.nightColor : __style.mediumGreenColor

    font: __style.p5
    hoverEnabled: true

    background: Rectangle {
      color: __style.transparentColor
    }

    onTextEdited: root.textEdited( textField.text )
  }

  rightAction: MMIcon {
    id: rightIcon

    anchors.verticalCenter: parent.verticalCenter

    size: __style.icon24
    source: __style.xMarkIcon
    color: root.enabled ? __style.forestColor : __style.mediumGreenColor
    visible: root.showClearIcon && textField.activeFocus && textField.text.length > 0
  }

  onRightActionClicked: {
    if (root.showClearIcon) {
      textField.clear()
    }
    else {
      // if the clear button should not be there, let's open keyboard instead
      textField.forceActiveFocus()
    }
  }
}
