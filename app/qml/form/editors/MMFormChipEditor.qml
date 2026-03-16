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
import "../../components/private" as MMPrivateComponents

/*
 * Chip-button selector for QGIS Attribute Form value-map fields.
 *
 * Displays all options as horizontally-wrapping pill buttons instead of opening
 * a drawer. Intended for fields with a small number of options (≤ 4) where
 * showing all choices inline is more ergonomic than a dropdown.
 *
 * Requires various global properties set to function, see featureform Loader section.
 * These properties are injected here via 'fieldXYZ' properties and captured with
 * underscore `_`.
 *
 * Should be used only within feature form.
 * See MMBaseInput for base class properties (title, errorMsg, warningMsg, etc.).
 */

MMPrivateComponents.MMBaseInput {
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

  // === Internal state ===

  // The raw field value of the currently selected option (from the value-map's value column).
  property var _currentValue: undefined

  // === Signals expected by MMFormPage ===

  signal editorValueChanged( var newValue, bool isNull )
  signal rememberValueBoxClicked( bool state )

  // === MMBaseInput bindings ===

  title: _fieldShouldShowTitle ? _fieldTitle : ""

  errorMsg: _fieldErrorMessage
  warningMsg: _fieldWarningMessage

  readOnly: _fieldFormIsReadOnly || !_fieldIsEditable
  shouldShowValidation: !_fieldFormIsReadOnly

  hasCheckbox: _fieldRememberValueSupported
  checkboxChecked: _fieldRememberValueState

  onCheckboxCheckedChanged: root.rememberValueBoxClicked( checkboxChecked )

  on_FieldValueChanged: {
    root._currentValue = _fieldValueIsNull ? undefined : _fieldValue
  }

  // === Chip grid ===

  inputContent: Rectangle {
    id: chipContainer

    width: parent.width
    // Height grows with the wrapped chips + top/bottom padding
    height: chipsFlow.implicitHeight + 2 * __style.margin12

    radius: __style.radius12
    color: __style.polarColor

    Flow {
      id: chipsFlow

      anchors {
        top: parent.top
        left: parent.left
        right: parent.right
        margins: __style.margin12
      }

      spacing: __style.margin8

      Repeater {
        id: chipsRepeater

        model: chipsModel

        delegate: Item {
          id: chipItem

          readonly property bool isSelected: {
            if ( root._fieldValueIsNull || root._currentValue === undefined ) return false
            if ( model.value === undefined || model.value === null ) return false
            return model.value.toString() === root._currentValue.toString()
          }

          width: chipBackground.width
          height: chipBackground.height

          Rectangle {
            id: chipBackground

            height: chipRow.implicitHeight + 2 * __style.margin8
            width: chipRow.implicitWidth + 2 * __style.margin16

            radius: __style.radius30

            // Selected: solid grassColor fill.
            // Unselected: white with a muted grey border so selected chip
            //             stands out clearly even when only 2 options exist.
            color: chipItem.isSelected ? __style.grassColor : __style.polarColor

            border.width: __style.width2
            border.color: chipItem.isSelected ? __style.grassColor : __style.greyColor

            Row {
              id: chipRow

              anchors.centerIn: parent
              spacing: __style.margin4

              // Checkmark shown only on the selected chip for unambiguous feedback.
              MMComponents.MMIcon {
                id: chipCheckmark

                visible: chipItem.isSelected
                source: __style.checkmarkIcon
                size: __style.icon16
                color: __style.forestColor
                anchors.verticalCenter: parent.verticalCenter
              }

              MMComponents.MMText {
                id: chipLabel

                text: model.text || ""
                font: __style.p5
                color: __style.nightColor
                anchors.verticalCenter: parent.verticalCenter
              }
            }
          }

          MouseArea {
            anchors.fill: parent

            enabled: !root.readOnly
            cursorShape: enabled ? Qt.PointingHandCursor : Qt.ArrowCursor

            onClicked: function ( mouse ) {
              mouse.accepted = true
              root._currentValue = model.value
              root.editorValueChanged( model.value, false )
            }
          }
        }
      }
    }
  }

  // === Model ===

  ListModel { id: chipsModel }

  // === Initialisation ===

  Component.onCompleted: {
    // Set the initial selection from the current field value
    root._currentValue = _fieldValueIsNull ? undefined : _fieldValue

    if ( !root._fieldConfig['map'] ) {
      __inputUtils.log( "Chip editor", root._fieldTitle + " config is not configured properly" )
      return
    }

    let config = root._fieldConfig['map']

    if ( !config.length ) {
      __inputUtils.log( "Chip editor", root._fieldTitle + " is using unsupported format (map, <=QGIS2.18)" )
      return
    }

    for ( let i = 0; i < config.length; i++ ) {
      chipsModel.append( {
        text:  Object.keys( config[i] )[0],
        value: Object.values( config[i] )[0]
      } )
    }
  }
}
