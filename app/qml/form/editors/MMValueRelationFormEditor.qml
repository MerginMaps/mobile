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
import "../../inputs"
import lc 1.0

/*
 * Dropdown (value relation) editor for QGIS Attribute Form
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
  property var _fieldFeatureLayerPair: parent.fieldFeatureLayerPair

  property bool _fieldShouldShowTitle: parent.fieldShouldShowTitle
  property bool _fieldIsReadOnly: parent.fieldIsReadOnly

  property string _fieldTitle: parent.fieldTitle
  property string _fieldErrorMessage: parent.fieldErrorMessage
  property string _fieldWarningMessage: parent.fieldWarningMessage

  signal editorValueChanged( var newValue, bool isNull )

  title: _fieldShouldShowTitle ? _fieldTitle : ""

  errorMsg: _fieldErrorMessage
  warningMsg: _fieldWarningMessage

  enabled: !_fieldIsReadOnly

  on_FieldValueChanged: {
    vrModel.pair = root._fieldFeatureLayerPair
  }

  dropdownLoader.sourceComponent: Component {
    MMDropdownDrawer {
      focus: true

      title: root._fieldTitle

      multiSelect: internal.allowMultivalue
      withSearchbar: vrModel.count > 5

      selectedFeatures: {
        if ( internal.allowMultivalue ) {
          root.preselectedFeatures = vrModel.convertFromQgisType( root._fieldValue, FeaturesModel.FeatureId )
        }
        else {
          root.preselectedFeatures = [root._fieldValue]
        }
      }

      model: ValueRelationFeaturesModel {
        id: vrDropdownModel

        config: root._fieldConfig
        pair: root._fieldFeatureLayerPair
      }

      onClosed: dropdownLoader.active = false

      onSelectionFinished: function ( selectedFeatures ) {

        if ( internal.allowMultivalue )
        {
          let isNull = selectedFeatures.length === 0

          if ( !isNull )
          {
            // We need to convert feature id to string prior to sending it to C++ in order to
            // avoid conversion to scientific notation.
            selectedFeatures = selectedFeatures.map( function(x) { return x.toString() } )
          }
          root.editorValueChanged( vrModel.convertToQgisType( selectedFeatures ), isNull )
        }
        else
        {
          // We need to convert feature id to string prior to sending it to C++ in order to
          // avoid conversion to scientific notation.
          selectedFeatures = selectedFeatures.toString()

          root.editorValueChanged( vrModel.convertToKey( selectedFeatures ), false )
        }
      }

      Component.onCompleted: open()
    }
  }

  ValueRelationFeaturesModel {
    id: vrModel

    config: root._fieldConfig
    pair: root._fieldFeatureLayerPair

    onInvalidate: {
      if ( root._fieldValueIsNull )
      {
        return // ignore invalidate signal if value is already NULL
      }
      if ( root._fieldIsReadOnly )
      {
        return // ignore invalidate signal if form is not in edit mode
      }
      root.editorValueChanged( "", true )
    }

    onFetchingResultsChanged: function ( isFetching ) {
      if ( !isFetching )
      {
        setText()
      }
    }
  }

  function reload()
  {
    if ( !root.isReadOnly )
    {
      vrModel.pair = root._fieldFeatureLayerPair
    }
  }

  function setText()
  {
    root.text = vrModel.convertFromQgisType( root._fieldValue, FeaturesModel.FeatureTitle ).join( ', ' )
  }

  QtObject {
    id: internal

    property bool allowMultivalue: root._fieldConfig["AllowMulti"]
  }
}
