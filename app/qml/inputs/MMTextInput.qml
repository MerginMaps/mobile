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
 * Common text input to use in the app.
 * Disabled state can be achieved by setting `enabled: false`.
 *
 * See MMBaseSingleLineInput for more properties.
 */

MMPrivateComponents.MMBaseSingleLineInput {
  id: root

  property bool showClearIcon: true

  rightContent: MMComponents.MMIcon {
    size: __style.icon24
    source: __style.closeIcon
    color: root.editState === "enabled" ? __style.forestColor : __style.mediumGreenColor
  }

  rightContentVisible: root.showClearIcon && textField.activeFocus && textField.text.length > 0

  onRightContentClicked: textField.clear()
}
