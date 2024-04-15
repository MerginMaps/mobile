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

import "../../components" as MMComponents
import "../../inputs" as MMInputs

/*
 * Common dropdown (combobox) for forms (value relation and value map).
 * The difference with MMComboboxInput is that here we need to set the text and drawer manually.
 * Should not be used outside of the feature form!
 *
 * Disabled state can be achieved by setting `enabled: false`.
 *
 * See MMBaseInput for more properties.
 */

MMInputs.MMBaseInput {
  id: root

  property alias placeholderText: textField.placeholderText
  property alias text: textField.text
  property alias textFieldComponent: textField

  property alias dropdownLoader: drawerLoader

  hasFocus: textField.activeFocus

  content: TextField {
    id: textField

    anchors.fill: parent
    anchors.verticalCenter: parent.verticalCenter

    readOnly: true

    color: root.enabled ? __style.nightColor : __style.mediumGreenColor
    placeholderTextColor: __style.darkGreyColor

    font: __style.p5
    hoverEnabled: true

    background: Rectangle {
      color: __style.transparentColor
    }

    MouseArea {
      anchors.fill: parent
      onClicked: function( mouse ) {
        mouse.accepted = true
        openDrawer()
      }
    }
  }

  rightAction: MMComponents.MMIcon {
    property bool pressed: false

    anchors.verticalCenter: parent.verticalCenter

    size: __style.icon24
    source: __style.arrowDownIcon
    color: root.enabled ? __style.forestColor : __style.mediumGreenColor
  }

  onRightActionClicked: {
    if ( !root.enabled )
      return

    openDrawer()
  }

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
