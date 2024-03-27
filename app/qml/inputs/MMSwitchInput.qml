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
 * Common switch input to use in the app.
 *
 * See MMBaseInput for more properties.
 */

MMBaseInput {
  id: root

  property alias text: textField.text
  property alias switchComponent: switchComponent

  hasFocus: textField.activeFocus

  content: TextField {
    id: textField

    anchors.fill: parent

    readOnly: true
    placeholderTextColor: __style.nightAlphaColor
    color: root.enabled ? __style.nightColor : __style.mediumGreenColor

    font: __style.p5
    hoverEnabled: true

    background: Rectangle {
      color: __style.transparentColor
    }

    onTextEdited: root.textEdited( textField.text )
  }

  rightAction: MMSwitch {
    id: switchComponent

    anchors.verticalCenter: parent.verticalCenter
    x: -20 * __dp // TODO why is this needed? bacause of how baseinput works :(

    uncheckedBgColor: __style.lightGreenColor
  }
}
