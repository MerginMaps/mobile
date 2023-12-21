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
import Qt5Compat.GraphicalEffects
import "../components"

MMAbstractEditor {
  id: root

  property alias placeholderText: textField.placeholderText
  property alias text: textField.text

  hasFocus: textField.activeFocus

  content: TextField {
    id: textField

    anchors.fill: parent
    anchors.verticalCenter: parent.verticalCenter

    color: root.enabled ? __style.nightColor : __style.mediumGreenColor
    placeholderTextColor: __style.nightAlphaColor
    font: __style.p5
    hoverEnabled: true
    echoMode: eyeButton.pressed ? TextInput.Normal : TextInput.Password
    background: Rectangle {
      color: __style.transparentColor
    }
  }

  rightAction: MMIcon {
    id: eyeButton

    property bool pressed: false

    height: parent.height

    source: pressed ? __style.hideIcon : __style.showIcon
    color: root.enabled ? __style.forestColor : __style.mediumGreenColor
  }

  onRightActionClicked: eyeButton.pressed = !eyeButton.pressed
}
