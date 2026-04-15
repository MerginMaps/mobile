/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import mm 1.0 as MM
import "../../components" as MMComponents

Column {
  id: root

  required property string filterName
  required property var currentValue

  required property var widgetConfig

  property bool isMultiSelect: false

  width: parent.width

  spacing: __style.margin8

  MMComponents.MMText {
    width: parent.width

    text: root.filterName

    visible: text
    font: __style.p6
    color: __style.nightColor
  }

  MMFilterTextInput {
    id: dropdownInput

    width: parent.width

    type: MMFilterTextInput.InputType.Dropdown

    checked: text !== ""

    placeholderText: qsTr( "Select..." )
    text: root.currentValue && root.currentValue.length ? qsTr( "%1 selected" ).arg( root.currentValue.length ) : ""

    onTextClicked: openDrawer()
    onRightContentClicked: {
      if ( root.currentValue && root.currentValue.length ) {
        root.currentValue = undefined
        root.currentValueChanged()
      }
      else {
        openDrawer()
      }
    }
  }

  Loader {
    id: dropdownDrawerLoader

    active: false


    sourceComponent: MMComponents.MMListMultiselectDrawer {
      drawerHeader.title: root.filterName

      withSearch: vrDropdownModel.count > 5
      multiSelect: root.isMultiSelect

      isLoading: vrDropdownModel.isLoading

      list.model: MM.ValueRelationFeaturesModel {
        id: vrDropdownModel

        config: root.widgetConfig
      }

      textRole: "FeatureTitle"
      valueRole: "Key"

      onSelectionFinished: function( selectedItems ) {

        //
        // Large fids could be converted to scientific notation on their way to cpp,
        // so we convert them to string first in JS.
        //
        selectedItems = selectedItems.map( x => x.toString() )

        root.currentValue = selectedItems

        // TODO: this is just a hack, we need to add a dedicated signal instead, Qt does not always get that array length has changed
        root.currentValueChanged()

        close()
      }

      onSearchTextChanged: ( searchText ) => vrDropdownModel.searchExpression = searchText

      onClosed: dropdownDrawerLoader.active = false

      Component.onCompleted: {
        if ( root.currentValue ) {
          // preselect choices if any are set
          selected = root.currentValue
        }

        open()
        vrDropdownModel.reloadFeatures()
      }
    }
  }

  function openDrawer() {
    dropdownDrawerLoader.active = true
    dropdownDrawerLoader.focus = true
  }
}
