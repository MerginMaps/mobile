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
 * Value-map editor for QGIS Attribute Form.
 *
 * When the field has 4 or fewer options the editor renders all options as
 * inline chip buttons (MMFormChipEditor), so the user can select a value
 * with a single tap without opening a drawer.
 *
 * When there are 5 or more options the original dropdown drawer
 * (MMFormComboboxBaseEditor + MMListMultiselectDrawer) is used unchanged.
 *
 * Requires various global properties set to function, see featureform Loader section.
 * These properties are injected here via 'fieldXYZ' properties and captured with
 * underscore `_`.
 *
 * Should be used only within feature form.
 */

Item {
  id: root

  // === Properties injected by MMFormPage ===

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

  // === Signals expected by MMFormPage ===

  signal editorValueChanged( var newValue, bool isNull )
  signal rememberValueBoxClicked( bool state )

  // === Layout ===

  implicitHeight: editorLoader.item ? editorLoader.item.implicitHeight : 0

  // === Internal: shared value-map model ===

  // Parsed once from _fieldConfig; both chip and dropdown sub-editors read from it.
  ListModel { id: listModel }

  // Tracks whether the model has been populated so we can choose the right sub-editor.
  property bool _modelReady: false

  // Initial text displayed in the dropdown (combobox mode only).
  property string _displayText: ""

  // Initial pre-selected items list passed to the drawer (combobox mode only).
  property var _preselectedItems: []

  // === Sub-editor loader ===

  Loader {
    id: editorLoader

    width: parent.width

    // sourceComponent remains null until Component.onCompleted has populated the
    // model; this prevents the Loader from briefly showing the wrong variant.
    sourceComponent: root._modelReady
      ? ( listModel.count <= 4 ? chipEditorComponent : comboboxEditorComponent )
      : null
  }

  // Forward signals from whichever sub-editor is active.
  Connections {
    target: editorLoader.item
    ignoreUnknownSignals: true

    function onEditorValueChanged( newValue, isNull ) {
      root.editorValueChanged( newValue, isNull )
    }

    function onRememberValueBoxClicked( state ) {
      root.rememberValueBoxClicked( state )
    }
  }

  // === Chip editor (≤ 4 options) ===

  Component {
    id: chipEditorComponent

    MMFormChipEditor {

      // Bind all form-field properties from the wrapper.
      _fieldValue:                  root._fieldValue
      _fieldConfig:                 root._fieldConfig
      _fieldValueIsNull:            root._fieldValueIsNull
      _fieldHasMixedValues:         root._fieldHasMixedValues
      _fieldShouldShowTitle:        root._fieldShouldShowTitle
      _fieldFormIsReadOnly:         root._fieldFormIsReadOnly
      _fieldIsEditable:             root._fieldIsEditable
      _fieldTitle:                  root._fieldTitle
      _fieldErrorMessage:           root._fieldErrorMessage
      _fieldWarningMessage:         root._fieldWarningMessage
      _fieldRememberValueSupported: root._fieldRememberValueSupported
      _fieldRememberValueState:     root._fieldRememberValueState
    }
  }

  // === Combobox / dropdown editor (> 4 options) ===

  Component {
    id: comboboxEditorComponent

    MMFormComboboxBaseEditor {
      id: combobox

      title:   root._fieldShouldShowTitle ? root._fieldTitle : ""

      placeholderText: root._fieldHasMixedValues ? root._fieldValue : ""

      errorMsg:   root._fieldErrorMessage
      warningMsg: root._fieldWarningMessage

      readOnly:             root._fieldFormIsReadOnly || !root._fieldIsEditable
      shouldShowValidation: !root._fieldFormIsReadOnly

      hasCheckbox:    root._fieldRememberValueSupported
      checkboxChecked: root._fieldRememberValueState

      // Initialise displayed text from the pre-computed value in the wrapper.
      text: root._displayText

      onCheckboxCheckedChanged: root.rememberValueBoxClicked( checkboxChecked )

      // Watch for field-value changes while the combobox is mounted.
      property var _watchValue: root._fieldValue
      on_WatchValueChanged: {
        if ( root._fieldValueIsNull || root._fieldValue === undefined ) {
          combobox.text = ""
          root._preselectedItems = []
          return
        }
        for ( let i = 0; i < listModel.count; i++ ) {
          let item = listModel.get( i )
          if ( root._fieldValue.toString() === item.value.toString() ) {
            combobox.text = item.text
            root._preselectedItems = [ item.value ]
            break
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

          selected: root._preselectedItems

          showFullScreen: false
          multiSelect: false
          withSearch: false

          onClosed: combobox.dropdownLoader.active = false

          onSelectionFinished: function ( selectedItems ) {
            if ( !selectedItems || ( Array.isArray( selectedItems ) && selectedItems.length !== 1 ) ) {
              __inputUtils.log( "Value map", root._fieldTitle + " received unexpected values" )
              return
            }

            root.editorValueChanged( selectedItems[0], selectedItems[0] === null )
          }

          Component.onCompleted: open()
        }
      }
    }
  }

  // === Initialisation ===

  Component.onCompleted: {

    if ( !root._fieldConfig['map'] ) {
      __inputUtils.log( "Value map", root._fieldTitle + " config is not configured properly" )
      root._modelReady = true
      return
    }

    let config = root._fieldConfig['map']

    if ( config.length ) {
      // QGIS ≥ 3.0 list format
      for ( let i = 0; i < config.length; i++ ) {
        let modelItem = {
          text:  Object.keys( config[i] )[0],
          value: Object.values( config[i] )[0]
        }

        listModel.append( modelItem )

        // Pre-compute display text and pre-selected list for the combobox variant.
        if ( !root._fieldValueIsNull && root._fieldValue !== undefined ) {
          if ( root._fieldValue.toString() === modelItem.value.toString() ) {
            root._displayText = modelItem.text
            root._preselectedItems = [ modelItem.value ]
          }
        }
      }
    }
    else {
      // QGIS ≤ 2.18 map format — no longer supported
      __inputUtils.log( "Value map", root._fieldTitle + " is using unsupported format (map, <=QGIS2.18)" )
    }

    // Setting _modelReady = true triggers the Loader to choose a sub-editor.
    // This happens synchronously within Component.onCompleted, before the first
    // frame is painted, so there is no visible flicker between the two variants.
    root._modelReady = true
  }
}
