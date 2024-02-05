/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../../inputs"

/*
 * Dropdown (value map) editor for QGIS Attribute Form
 * Requires various global properties set to function, see featureform Loader section.
 * These properties are injected here via 'fieldXYZ' properties and captured with underscore `_`.
 *
 * Should be used only within feature form.
 * See MMDropdownInput for more info.
 */
MMDropdownInput {
  id: root

  property var _fieldValue: parent.fieldValue
  property var _fieldConfig: parent.fieldConfig
  property bool _fieldValueIsNull: parent.fieldValueIsNull

  property bool _fieldShouldShowTitle: parent.fieldShouldShowTitle
  property bool _fieldIsReadOnly: parent.fieldIsReadOnly

  property string _fieldTitle: parent.fieldTitle
  property string _fieldErrorMessage: parent.fieldErrorMessage
  property string _fieldWarningMessage: parent.fieldWarningMessage

  property bool _fieldRememberValueSupported: parent.fieldRememberValueSupported
  property bool _fieldRememberValueState: parent.fieldRememberValueState

  signal editorValueChanged( var newValue, bool isNull )
  signal rememberValueBoxClicked( bool state )

  title: _fieldShouldShowTitle ? _fieldTitle : ""

  dropDownTitle: _fieldTitle

  errorMsg: _fieldErrorMessage
  warningMsg: _fieldWarningMessage

  enabled: !_fieldIsReadOnly

  hasCheckbox: _fieldRememberValueSupported
  checkboxChecked: _fieldRememberValueState

  dataModel: listModel
  multiSelect: false
  withSearchbar: false

  onCheckboxCheckedChanged: {
    root.rememberValueBoxClicked( checkboxChecked )
  }

  onSelectionFinished: function ( selectedFeatures ) {

    if ( !selectedFeatures || ( Array.isArray( selectedFeatures ) && selectedFeatures.length !== 1 ) ) {
      // should not happen...
      __inputUtils.log( "Value map", root._fieldTitle + " received unexpected values" )
      return
    }

    root.editorValueChanged( selectedFeatures[0], selectedFeatures[0] === null )
  }

  on_FieldValueChanged: {

    if ( _fieldValueIsNull ) {
      text = ""
      preselectedFeatures = []
    }

    // let's find the new value in the model
    for ( let i = 0; i < listModel.count; i++ ) {
      let item_i = listModel.get( i )

      if ( _fieldValue.toString() === item_i.FeatureId.toString() ) {
        text = item_i.FeatureTitle
        preselectedFeatures = [item_i.FeatureId]
      }
    }
  }

  ListModel { id: listModel }

  Component.onCompleted: {

    //
    // Parses value map options from config into ListModel.
    // This functionality should be moved to FeaturesListModel(?) in order to support search.
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
        // Intentionally using roles "FeatureXYZ" here so that it mimics
        // the FeaturesListModel and can be used in the DropdownDrawer
        let modelItem = {
          FeatureTitle: Object.keys( config[i] )[0],
          FeatureId: Object.values( config[i] )[0]
        }

        listModel.append( modelItem )

        // Is this the current item? If so, set the text
        if ( !root._fieldValueIsNull ) {
          if ( root._fieldValue.toString() === modelItem.FeatureId.toString() ) {
            root.text = modelItem.FeatureTitle
            root.preselectedFeatures = [modelItem.FeatureId]
          }
        }
      }
    }
    else
    {
      //it's a map (<=QGIS2.18) <--- sorry, dropped support for that in 2024.1.0
      __inputUtils.log( "Value map", root._fieldTitle + " is using unsupported format (list, <=QGIS2.18)" )
    }
  }
}
