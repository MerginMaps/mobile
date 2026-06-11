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
  property MM.AttributeController _fieldController: parent.fieldController

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

  on_FieldValueChanged: lookupDisplayText()
  on_FieldHasMixedValuesChanged: lookupDisplayText()

  onCheckboxCheckedChanged: root.rememberValueBoxClicked( checkboxChecked )

  dropdownLoader.sourceComponent: Component {

    MMComponents.MMListMultiselectDrawer {
      id: listDrawer

      drawerHeader.title: root._fieldTitle
      drawerHeader.titleFont: __style.t2

      drawerHeader.topLeftItem.visible: !root._fieldValueIsNull
      drawerHeader.topLeftItemContent: MMComponents.MMButton {
        text: qsTr( "Clear" )

        type: MMButton.Types.Tertiary

        fontColor: __style.darkGreyColor
        fontColorHover: __style.nightColor

        onClicked: {
          root.editorValueChanged( "", true )
          close()
        }
      }

      withSearch: false
      multiSelect: _controller.isMultiSelection

      valueRole: "KeyColumn"
      textRole: "ValueColumn"

      isLoading: vrDropdownModel.fetchingResults

      list.model: MM.ValueRelationFeaturesModel {
        id: vrDropdownModel

        property bool firstFetchFinished: false

        config: root._fieldConfig
        pair: root._fieldController.featureLayerPair

        // We show search for lists with more then 8 features.
        // We need to intentionally break the binding here because "count" changes
        // when users search for something and that would hide the search bar
        onFetchingResultsChanged: {
          if ( !fetchingResults && !firstFetchFinished )
          {
            if ( count > 8 )
            {
              listDrawer.withSearch = true

              // Additionally, focus the searchbar immediately in case "UseCompleter" is enabled
              if ( internal.useCompleter )
              {
                listDrawer.focusSearchBar()
              }
            }
            else
            {
              listDrawer.withSearch = false
            }

            firstFetchFinished = true
          }
        }

        Component.onCompleted: reloadFeatures()
      }

      onSearchTextChanged: ( searchText ) => vrDropdownModel.searchExpression = searchText

      onClosed: dropdownLoader.active = false

      onSelectionFinished: function ( selectedItems ) {
        const keys = _controller.arrayToQgisFormat( selectedItems )
        const isNull = selectedItems.length === 0

        root.editorValueChanged( keys, isNull )
        close()
      }

      Component.onCompleted: {
        // Pre-select the currently stored keys so the drawer opens with the
        // right items highlighted.
        selected = _controller.qgisFormatToArray( root._fieldValue )

        open()
      }
    }
  }

  MM.ValueRelationController {
    id: _controller

    config: root._fieldConfig

    isEditable: !root._fieldFormIsReadOnly && root._fieldIsEditable

    onDisplayTextChanged: root.text = _controller.displayText
    onInvalidateSelection: root.editorValueChanged( "", true )
    onPresentRawValue: root.text = root._fieldValue
  }

  QtObject {
    id: internal

    property bool useCompleter: root?._fieldConfig?.["UseCompleter"] ?? false
  }

  function hotReload()
  {
    if ( !root._fieldHasMixedValues )
    {
      _controller.lookupDisplayTextOnHotreload( root._fieldValue, root._fieldController.featureLayerPair.feature )
    }
  }

  function lookupDisplayText()
  {
    if ( !root._fieldHasMixedValues )
    {
      _controller.lookupDisplayTextOnValueChanged( root._fieldValue )
    }
  }
}
