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
 * Common dropdown input to use in the app.
 * Disabled state can be achieved by setting `enabled: false`.
 *
 * See MMDropdownDrawer to see required roles for dataModel.
 * See MMBaseInput for more properties.
 */

MMBaseInput {
  id: root

  property alias placeholderText: textField.placeholderText
  property alias text: textField.text
  property alias textFieldComponent: textField

  property alias dropdownLoader: drawerLoader

  // dataModel is used in the drawer.
  // It must have valueRole and textRole
  // and it must have `count` function to get the number of items
  // one can use qml's ListModel or our FeaturesModel.h
  property var dataModel

  property string dropDownTitle: ""
  property bool multiSelect: false
  property bool withSearchbar: false
  property var preselectedFeatures: []
  property string valueRole: "FeatureId"
  property string textRole: "FeatureTitle"

  hasFocus: textField.activeFocus

  signal selectionFinished( var selectedFeatures )

  content: TextField {
    id: textField

    anchors.fill: parent
    anchors.verticalCenter: parent.verticalCenter

    readOnly: true

    color: root.enabled ? __style.nightColor : __style.mediumGreenColor
    placeholderTextColor: __style.nightAlphaColor

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

  rightAction: MMIcon {
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
    sourceComponent: listComponent
  }

  Component {
    id: listComponent

    MMDropdownDrawer {
      focus: true
      model: root.dataModel
      title: root.dropDownTitle
      multiSelect: root.multiSelect
      withSearchbar: root.withSearchbar
      selectedFeatures: root.preselectedFeatures
      valueRole: root.valueRole
      textRole: root.textRole

      onClosed: drawerLoader.active = false

      onSelectionFinished: function ( selectedFeatures ) {
        root.selectionFinished( selectedFeatures )
      }

      Component.onCompleted: open()
    }
  }

  function openDrawer() {
    drawerLoader.active = true
    drawerLoader.focus = true
  }
}
