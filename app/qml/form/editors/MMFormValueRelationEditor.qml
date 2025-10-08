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

/*
 * Dropdown (value relation) editor for QGIS Attribute Form
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
  property var _fieldFeatureLayerPair: parent.fieldFeatureLayerPair

  property bool _fieldShouldShowTitle: parent.fieldShouldShowTitle
  property bool _fieldFormIsReadOnly: parent.fieldFormIsReadOnly
  property bool _fieldIsEditable: parent.fieldIsEditable

  property string _fieldTitle: parent.fieldTitle
  property string _fieldErrorMessage: parent.fieldErrorMessage
  property string _fieldWarningMessage: parent.fieldWarningMessage

  property bool _fieldRememberValueSupported: parent.fieldRememberValueSupported
  property bool _fieldRememberValueState: parent.fieldRememberValueState

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

  on_FieldValueChanged: {
    vrModel.pair = root._fieldFeatureLayerPair
  }

  onCheckboxCheckedChanged: {
    root.rememberValueBoxClicked( checkboxChecked )
  }

  dropdownLoader.sourceComponent: Component {

    MMComponents.MMListMultiselectDrawer {
      drawerHeader.title: root._fieldTitle

      //Incase when list is empty (ex: no relation value fields)
      emptyStateDelegate: MMComponents.MMText {
        text: qsTr( "No items" )
        topPadding: __style.margin20
        bottomPadding: __style.margin20
      }

      multiSelect: internal.allowMultivalue
      withSearch: vrModel.count > 5
      showFullScreen: multiSelect || withSearch

      valueRole: "FeatureId"
      textRole: "FeatureTitle"

      list.model: MM.ValueRelationFeaturesModel {
        id: vrDropdownModel

        config: root._fieldConfig
        pair: root._fieldFeatureLayerPair
      }

      onSearchTextChanged: ( searchText ) => vrDropdownModel.searchExpression = searchText

      onClosed: dropdownLoader.active = false

      onSelectionFinished: function ( selectedItems ) {

        if ( internal.allowMultivalue )
        {
          let isNull = selectedItems.length === 0

          if ( !isNull )
          {
            // We need to convert feature id to string prior to sending it to C++ in order to
            // avoid conversion to scientific notation.
            selectedItems = selectedItems.map( function(x) { return x.toString() } )
          }
          root.editorValueChanged( vrModel.convertToQgisType( selectedItems ), isNull )
        }
        else
        {
          // We need to convert feature id to string prior to sending it to C++ in order to
          // avoid conversion to scientific notation.
          selectedItems = selectedItems.toString()

          root.editorValueChanged( vrModel.convertToKey( selectedItems ), false )
        }

        close()
      }

      Component.onCompleted: {
        // We want to set the initial value of 'selected' property but not bind it so we avoid a binding loop
        if ( internal.allowMultivalue ) {
          selected = vrModel.convertFromQgisType( root._fieldValue, MM.FeaturesModel.FeatureId )
        }
        else {
          selected = [root._fieldValue]
        }
        open()
      }
    }
  }

  MM.ValueRelationFeaturesModel {
    id: vrModel

    config: root._fieldConfig
    pair: root._fieldFeatureLayerPair

    onInvalidate: {
      if ( root._fieldHasMixedValues )
      {
        return // ignore invalidate signal if value is MixedAttributeValue
      }
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
    root.text = vrModel.convertFromQgisType( root._fieldValue, MM.FeaturesModel.FeatureTitle ).join( ', ' )
  }

  QtObject {
    id: internal

    property bool allowMultivalue: root._fieldConfig["AllowMulti"]
  }
}
