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

import "../inputs"

MMSettingItem {
  id: root

  property string valueDescription

  onClicked: {
    inputDrawer.value = root.value
    inputDrawer.visible = true
  }

  MMDrawer {
    id: inputDrawer

    property string value

    signal clicked ( string newValue )

    width: ApplicationWindow.window.width
    title: root.title
    primaryButton: qsTr("Confirm")
    visible: false
    specialComponent: MMTextInput {
      width: inputDrawer.width - 40 * __dp
      title: root.valueDescription
      bgColor: __style.lightGreenColor
      text: inputDrawer.value
      focus: true

      onTextChanged: inputDrawer.value = text
    }

    onPrimaryButtonClicked: {
      visible = false
      root.valueWasChanged(inputDrawer.value)
    }
  }
}
