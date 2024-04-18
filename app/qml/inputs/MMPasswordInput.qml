/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../components" as MMComponents
import "../components/private" as MMPrivateComponents

MMPrivateComponents.MMBaseSingleLineInput {
  id: root

  textField.echoMode: eyeButton.pressed ? TextInput.Normal : TextInput.Password

  textField.inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase | Qt.ImhSensitiveData | Qt.ImhHiddenText

  rightContent: MMComponents.MMIcon {
    id: eyeButton

    property bool pressed: false

    size: __style.icon24
    source: pressed ? __style.hideIcon : __style.showIcon
    color: root.editState === "enabled" ? __style.forestColor : __style.mediumGreenColor
  }

  onRightContentClicked: eyeButton.pressed = !eyeButton.pressed
}
