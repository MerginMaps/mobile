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

/*
 * Common dropdown (combobox) for forms (value relation and value map).
 * The difference with MMComboboxInput is that here we need to set the text and drawer manually.
 * Should not be used outside of the feature form!
 *
 * Disabled state can be achieved by setting `enabled: false`.
 *
 * See MMBaseSingleLineInput for more properties.
 */

MMPrivateComponents.MMBaseSingleLineInput {
  id: root

  property alias dropdownLoader: drawerLoader

  textField {
    readOnly: true

    onReleased: openDrawer()
  }

  rightContent: MMComponents.MMIcon {
    size: __style.icon24
    source: __style.arrowDownIcon
    color: root.editState === "enabled" ? __style.forestColor : __style.mediumGreenColor
  }

  onRightContentClicked: openDrawer()

  Loader {
    id: drawerLoader

    asynchronous: true
    active: false
  }

  function openDrawer() {
    drawerLoader.active = true
    drawerLoader.focus = true
  }
}
