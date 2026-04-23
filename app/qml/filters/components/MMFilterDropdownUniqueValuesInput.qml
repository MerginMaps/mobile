/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import MMInput as MM
import "../../components" as MMComponents

Column {
  id: root

  required property string filterName
  required property var currentValue

  required property string vectorLayerId
  required property string fieldName

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

  MMFilterBaseInput {
    id: dropdownInput

    width: parent.width

    type: MMFilterBaseInput.InputType.Dropdown

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

      withSearch: uniqueValuesModel.count > 5
      multiSelect: root.isMultiSelect

      emptyStateDelegate: Component {
        MMComponents.MMListEmptyLoaderDelegate {
          isLoading: uniqueValuesModel.isLoading
        }
      }

      list.model: MM.SearchProxyModel {
        id: searchProxyModel

        sourceModel: uniqueValuesModel
      }

      textRole: "display"
      secondaryTextRole: ""
      valueRole: "value"

      onSelectionFinished: function( selectedItems ) {
        root.currentValue = selectedItems

        // TODO: this is just a hack, we need to add a dedicated signal instead, Qt does not always get that array length has changed
        root.currentValueChanged()

        close()
      }

      onSearchTextChanged: ( searchText ) => searchProxyModel.searchString = searchText

      onClosed: dropdownDrawerLoader.active = false

      Component.onCompleted: {
        if ( root.currentValue ) {
          // preselect choices if any are set
          selected = root.currentValue
        }

        uniqueValuesModel.populate( __activeProject.filterController )
        open()
      }
    }
  }

  //
  // Intentionally keeping the model outside of loader to keep it instantiated while the
  // filtering drawer is opened to avoid reloading features on each dropdown open.
  //
  // Values are not loaded unless .populate() is called
  //
  MM.UniqueValuesFilterModel {
    id: uniqueValuesModel

    layerId: root.vectorLayerId
    fieldName: root.fieldName
  }

  function openDrawer() {
    dropdownDrawerLoader.active = true
    dropdownDrawerLoader.focus = true
  }
}
