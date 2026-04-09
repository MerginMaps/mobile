/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../../components"

Column {
  id: root

  width: parent.width
  spacing: __style.margin8

  required property string fieldDisplayName
  required property var currentValue
  required property var currentValueTexts
  required property string fieldLayerId
  required property string fieldName
  required property bool multiSelect
  required property var vectorLayer

  signal refreshRequested()

  MMText {
    width: parent.width
    text: root.fieldDisplayName
    font: __style.p6
    color: __style.nightColor
    visible: root.fieldDisplayName !== ""
  }

  MMFilterTextInput {
    id: dropdownInput

    width: parent.width
    type: MMFilterTextInput.InputType.Dropdown
    checked: text !== ""
    placeholderText: qsTr( "Select..." )
    text: {
      let texts = root.currentValueTexts
      if ( !texts || texts.length === 0 ) return ""
      if ( root.multiSelect && texts.length > 1 ) return qsTr( "%1 selected" ).arg( texts.length )
      return texts.join( ", " )
    }

    onTextClicked: dropdownDrawerLoader.active = true
    onRightContentClicked: {
      if ( dropdownInput.text !== "" ) {
        __activeProject.filterController.setDropdownFilter(
          root.fieldLayerId,
          root.fieldName,
          [],
          root.multiSelect
        )
        root.refreshRequested()
      } else {
        dropdownDrawerLoader.active = true
      }
    }
  }

  Loader {
    id: dropdownDrawerLoader
    active: false
    sourceComponent: dropdownDrawerComponent
  }

  Component {
    id: dropdownDrawerComponent

    MMListMultiselectDrawer {
      drawerHeader.title: root.fieldDisplayName
      multiSelect: root.multiSelect
      withSearch: true
      showFullScreen: root.multiSelect

      list.model: ListModel { id: dropdownListModel }

      onSearchTextChanged: function( searchText ) {
        dropdownInternal.pendingSearchText = searchText
        searchDebounceTimer.restart()
      }

      onSelectionFinished: function( selectedItems ) {
        __activeProject.filterController.setDropdownFilter(
          root.fieldLayerId,
          root.fieldName,
          selectedItems,
          root.multiSelect
        )
        root.refreshRequested()
        close()
      }

      onClosed: dropdownDrawerLoader.active = false

      QtObject {
        id: dropdownInternal
        property string pendingSearchText: ""
      }

      Timer {
        id: searchDebounceTimer
        interval: 300
        repeat: false
        onTriggered: populateOptions( dropdownInternal.pendingSearchText )
      }

      function populateOptions( searchText ) {
        let options = __activeProject.filterController.getDropdownOptions(
          root.vectorLayer,
          root.fieldName,
          searchText,
          100
        )
        dropdownListModel.clear()
        if ( !options ) return
        for ( let i = 0; i < options.length; i++ ) {
          dropdownListModel.append( options[i] )
        }
      }

      Component.onCompleted: {
        let val = root.currentValue
        if ( val && val.length > 0 ) {
          let arr = []
          for ( let i = 0; i < val.length; i++ ) {
            arr.push( String( val[i] ) )
          }
          selected = arr
        }
        populateOptions( "" )
        open()
      }
    }
  }
}
