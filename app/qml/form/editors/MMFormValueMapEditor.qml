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

/*
 * Dropdown (value map) editor for QGIS Attribute Form
 * Requires various global properties set to function, see featureform Loader section.
 * These properties are injected here via 'fieldXYZ' properties and captured with underscore `_`.
 *
 * Should be used only within feature form.
 * See MMFormComboboxBaseEditor for more info.
 */

MMFormComboboxBaseEditor {
  id: root

  property var _fieldValue: parent.fieldValue
  property var _fieldConfig: parent.fieldConfig
  property bool _fieldValueIsNull: parent.fieldValueIsNull
  property bool _fieldHasMixedValues: parent.fieldHasMixedValues

  property bool _fieldShouldShowTitle: parent.fieldShouldShowTitle
  property bool _fieldFormIsReadOnly: parent.fieldFormIsReadOnly
  property bool _fieldIsEditable: parent.fieldIsEditable

  property string _fieldTitle: parent.fieldTitle
  property string _fieldErrorMessage: parent.fieldErrorMessage
  property string _fieldWarningMessage: parent.fieldWarningMessage

  property bool _fieldRememberValueSupported: parent.fieldRememberValueSupported
  property bool _fieldRememberValueState: parent.fieldRememberValueState

  property var preselectedItems: []

  signal editorValueChanged( var newValue, bool isNull )
  signal rememberValueBoxClicked( bool state )

  title: _fieldShouldShowTitle ? _fieldTitle : ""

  placeholderText: _fieldHasMixedValues ? _fieldValue : ""

  errorMsg: _fieldErrorMessage
  warningMsg: _fieldWarningMessage

  readOnly: _fieldFormIsReadOnly || !_fieldIsEditable
  shouldShowValidation: !_fieldFormIsReadOnly

  hasCheckbox: _fieldRememberValueSupported
  checkboxChecked: _fieldRememberValueState

  onCheckboxCheckedChanged: {
    root.rememberValueBoxClicked( checkboxChecked )
  }

  on_FieldValueChanged: {

    if ( _fieldValueIsNull || _fieldValue === undefined ) {
      text = ""
      preselectedItems = []
    }

    // let's find the new value in the model
    for ( let i = 0; i < listModel.count; i++ ) {
      let item_i = listModel.get( i )

      if ( _fieldValue && _fieldValue.toString() === item_i.value.toString() ) {
        text = item_i.text
        preselectedItems = [item_i.value]
      }
    }
  }

  dropdownLoader.sourceComponent: Component {

    MMComponents.MMListMultiselectDrawer {

      drawerHeader.title: root._fieldTitle

      emptyStateDelegate: Item {
        width: parent.width
        height: noItemsText.implicitHeight + __style.margin40
      
        MMComponents.MMText {
          id: noItemsText
          text: qsTr( "No items" )
          anchors.centerIn: parent
        }
      }

      list.model: listModel

      selected: root.preselectedItems

      showFullScreen: false
      multiSelect: false
      withSearch: false

      onClosed: dropdownLoader.active = false

      onSelectionFinished: function ( selectedItems ) {
        if ( !selectedItems || ( Array.isArray( selectedItems ) && selectedItems.length !== 1 ) ) {
          // should not happen...
          __inputUtils.log( "Value map", root._fieldTitle + " received unexpected values" )
          return
        }

        root.editorValueChanged( selectedItems[0], selectedItems[0] === null )
      }

      Component.onCompleted: open()
    }
  }

  ListModel { id: listModel }

  Component.onCompleted: {

    //
    // Parses value map options from config into ListModel.
    // This functionality should be moved to cpp model in order to support search.
    //

    if ( !root._fieldConfig['map'] ) {
      __inputUtils.log( "Value map", root._fieldTitle + " config is not configured properly" )
    }

    let config = root._fieldConfig['map']

    if ( config.length )
    {
      //it's a list (>=QGIS3.0)
      for ( var i = 0; i < config.length; i++ )
      {
        let modelItem = {
          text: Object.keys( config[i] )[0],
          value: Object.values( config[i] )[0]
        }

        listModel.append( modelItem )

        // Is this the current item? If so, set the text
        if ( !root._fieldValueIsNull ) {
          if ( root._fieldValue.toString() === modelItem.value.toString() ) {
            root.text = modelItem.text
            root.preselectedItems = [modelItem.value]
          }
        }
      }
    }
    else
    {
      //it's a map (<=QGIS2.18) <--- sorry, dropped support for that in 2024.1.0
      __inputUtils.log( "Value map", root._fieldTitle + " is using unsupported format (map, <=QGIS2.18)" )
    }
  }
}
