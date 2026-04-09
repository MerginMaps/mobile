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
import "../../components"

Column {
  id: root

  required property string filterName
  required property string filterId
  required property var currentValue

  required property var widgetConfig

  property bool isMultiSelect: false

  width: parent.width

  spacing: __style.margin8

  MMText {
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
      if ( root.currentValue && root.currentValue.length )
      {
        root.currentValue = []
        root.currentValueChanged()
      }
      else
      {
        openDrawer()
      }
    }
  }

  Loader {
    id: dropdownDrawerLoader

    active: false

    // TODO: add indication that model is loading features
    // TODO: add animation when drawer height is changed

    sourceComponent: MMListMultiselectDrawer {
      drawerHeader.title: root.filterName

      withSearch: true
      multiSelect: root.isMultiSelect

      list.model: MM.ValueRelationFeaturesModel {
        id: vrDropdownModel

        config: root.widgetConfig
      }

      textRole: "FeatureTitle"
      valueRole: "FeatureId"

      onSelectionFinished: function( selectedItems ) {

        if ( root.isMultiSelect )
        {
          let isNull = selectedItems.length === 0

          if ( !isNull )
          {
            // We need to convert feature id to string prior to sending it to C++ in order to
            // avoid conversion to scientific notation.
            selectedItems = selectedItems.map( function(x) { return x.toString() } )
          }

          root.currentValue = vrDropdownModel.convertToQgisType( selectedItems ).replace( "{","" ).replace( "}","" ).split( ',' ).filter( x => x )
        }
        else
        {
          // We need to convert feature id to string prior to sending it to C++ in order to
          // avoid conversion to scientific notation.
          selectedItems = selectedItems.toString()

          root.currentValue = [vrDropdownModel.convertToKey( selectedItems )]
        }

        console.log("--> selected items:", selectedItems)

        // TODO: this is just a hack, we need to add a dedicated signal instead, Qt does not always get that array length has changed
        root.currentValueChanged()

        close()
      }

      onClosed: dropdownDrawerLoader.active = false

      Component.onCompleted: {
        if ( root.currentValue )
        {
          // preselect choices if any are set

          // TODO: this is expecting FIDs, but we fill in Key-column values, what to do?
          // selected = root.currentValue
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
