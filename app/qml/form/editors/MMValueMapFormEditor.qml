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

  signal editorValueChanged( var newValue, bool isNull )

  title: _fieldShouldShowTitle ? _fieldTitle : ""

  dropDownTitle: _fieldTitle

  errorMsg: _fieldErrorMessage
  warningMsg: _fieldWarningMessage

  enabled: !_fieldIsReadOnly

  on_FieldValueChanged: {

    if ( _fieldValueIsNull ) {
      text = ""
    }

    // let's find the new value in the model
    for ( let i = 0; i < internal.modelData.count; i++ ) {
      let item_i = internal.modelData.get( i )

      if ( _fieldValue.toString() === item_i.data.toString() ) {
        text = item_i.display
      }
    }
  }

  QtObject {
    id: internal

    property ListModel modelData: ListModel { id: listModel }

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
          let modelItem = {
            display: Object.keys( config[i] )[0],
            data: Object.values( config[i] )[0]
          }

          listModel.append( modelItem )

          // Is this the current item? If so, set the text
          if ( !root._fieldValueIsNull ) {
            if ( root._fieldValue.toString() === modelItem.data.toString() ) {
              root.text = modelItem.display
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
}
