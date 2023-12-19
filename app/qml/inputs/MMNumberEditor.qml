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

MMAbstractEditor {
  id: root

  property alias placeholderText: textField.placeholderText
  property int number: 0

  signal editorValueChanged( var newValue, var isNull )

  hasFocus: textField.activeFocus

  leftAction: MMIcon {
    id: leftIcon

    height: parent.height

    source: __style.minusIcon
    color: root.enabled ? __style.forestColor : __style.mediumGreenColor
  }

  content: TextField {
    id: textField

    anchors.fill: parent

    text: root.number
    color: root.enabled ? __style.nightColor : __style.mediumGreenColor
    placeholderTextColor: __style.nightAlphaColor
    font: __style.p5
    hoverEnabled: true
    horizontalAlignment: TextInput.AlignHCenter
    inputMethodHints: Qt.ImhFormattedNumbersOnly

    // taking care to show numbers only
    onTextChanged: {
      const newNumber = parseInt(textField.text, 10)

      if(textField.text === "" || textField.text === "-") {
        root.number = 0
        textField.text = ""
        return
      }

      if(Number.isInteger(newNumber)) {
        root.number = newNumber
      }
      textField.text = root.number
      console.log(newNumber)
    }

    background: Rectangle {
      color: __style.transparentColor
    }
  }

  rightAction: MMIcon {
    id: rightIcon

    height: parent.height

    source: __style.plusIcon
    color: root.enabled ? __style.forestColor : __style.mediumGreenColor
  }

  onLeftActionClicked: { textField.forceActiveFocus(); textField.text = --root.number }
  onRightActionClicked: { textField.forceActiveFocus(); textField.text = ++root.number }
}
