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

  property bool multiSelect: false
  property var model
  property var selected

  onClicked: {
    if(root.model?.count > 0) {
      dropdownDrawer.visible = true
    }
  }

  MMDropdownDrawer {
    id: dropdownDrawer

    focus: true
    model: root.model
    title: root.valueDescription
    multiSelect: root.multiSelect
    selectedFeatures: root.selected
    withSearchbar: false
    valueRole: "value"
    textRole: "text"

    onSelectionFinished: function ( selectedFeatures ) {
      root.valueWasChanged( selectedFeatures )
    }
  }
}
