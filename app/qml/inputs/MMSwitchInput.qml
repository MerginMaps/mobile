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

/*
 * Common switch input to use in the app.
 *
 * See MMBaseSingleLineInput for more properties.
 */

MMPrivateComponents.MMBaseSingleLineInput {
  id: root

  property alias checked: switchComponent.checked
  //! emitted when interactively toggled by the user via touch, mouse, or keyboard.
  signal toggled()

  textField.readOnly: true

  rightContent: MMComponents.MMSwitch {
    id: switchComponent

    uncheckedBgColor: __style.lightGreenColor
  }

  textField.onReleased: toggleSwitchComponent()

  onRightContentClicked: toggleSwitchComponent()

  function toggleSwitchComponent() {
    if ( !readOnly) {
      switchComponent.toggle()
      toggled()
    }
  }
}
