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
 * Common text input to use in the app, with button on right
 * Disabled state can be achieved by setting `enabled: false`.
 *
 * See MMBaseInput for more properties.
 */

MMBaseInput {
  id: root

  property alias placeholderText: textField.placeholderText
  property alias text: textField.text
  property alias buttonText: buttonText.text
  property alias buttonEnabled: rightButton.enabled

  signal textEdited( string text )
  signal buttonClicked()

  hasFocus: textField.activeFocus

  content: TextField {
    id: textField

    anchors.verticalCenter: parent.verticalCenter
    width: parent.width + rightButton.x

    color: root.enabled ? __style.nightColor : __style.mediumGreenColor
    placeholderTextColor: __style.darkGreyColor
    font: __style.p5
    hoverEnabled: true

    background: Rectangle {
      color: __style.transparentColor
    }

    onTextEdited: root.textEdited( textField.text )
  }

  rightAction: Button {
    id: rightButton

    property bool transparent: false

    x: 10 * __dp - buttonText.width
    height: 34 * __dp
    anchors.verticalCenter: parent.verticalCenter

    contentItem: Text {
      id: buttonText

      anchors.centerIn: rightButton
      font: __style.t3
      color: rightButton.enabled ? rightButton.down || rightButton.hovered ? __style.grassColor : __style.forestColor : __style.deepOceanColor
      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
    }

    background: Rectangle {
      color: rightButton.enabled ? rightButton.down || rightButton.hovered ? __style.forestColor : __style.grassColor : __style.mediumGreenColor
      radius: height / 2
    }

    onClicked: {
      textField.forceActiveFocus()
      root.buttonClicked()
    }
  }
}
