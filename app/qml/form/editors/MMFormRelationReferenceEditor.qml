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
import "../../components/private" as MMPrivateComponents
import "../components" as MMFormComponents

MMPrivateComponents.MMBaseSingleLineInput {
  id: root

  property var _fieldValue: parent.fieldValue
  property var _fieldConfig: parent.fieldConfig
  property var _fieldActiveProject: parent.fieldActiveProject

  property bool _fieldFormIsReadOnly: parent.fieldFormIsReadOnly
  property bool _fieldIsEditable: parent.fieldIsEditable

  property bool _fieldShouldShowTitle: parent.fieldShouldShowTitle
  property string _fieldTitle: parent.fieldTitle
  property string _fieldErrorMessage: parent.fieldErrorMessage
  property string _fieldWarningMessage: parent.fieldWarningMessage

  property bool _fieldRememberValueSupported: parent.fieldRememberValueSupported
  property bool _fieldRememberValueState: parent.fieldRememberValueState

  signal openLinkedFeature( /* FeaturePair */ var linkedFeature )
  signal editorValueChanged( var newValue, bool isNull )
  signal rememberValueBoxClicked( bool state )

  on_FieldValueChanged: {
    textField.text = rModel.attributeFromForeignKey( root._fieldValue, MM.FeaturesModel.FeatureTitle ) || ""
  }

  title: _fieldShouldShowTitle ? _fieldTitle : ""

  errorMsg: _fieldErrorMessage
  warningMsg: _fieldWarningMessage

  readOnly: _fieldFormIsReadOnly || !_fieldIsEditable
  shouldShowValidation: !_fieldFormIsReadOnly

  hasCheckbox: _fieldRememberValueSupported
  checkboxChecked: _fieldRememberValueState

  onCheckboxCheckedChanged: {
    root.rememberValueBoxClicked( checkboxChecked )
  }

  textField.readOnly: true

  textField.onReleased: { // can be opened even when the field is readonly
    let featurePair = rModel.attributeFromForeignKey( root._fieldValue, MM.FeaturesModel.FeaturePair )
    if ( featurePair === null || !featurePair.valid ) return

    openLinkedFeature( featurePair )
  }

  rightContent: MMComponents.MMIcon {
    size: __style.icon24
    source: __style.linkIcon
    color: root.iconColor
  }

  onRightContentClicked: {
    listLoader.active = true
    listLoader.focus = true
  }

  MM.RelationReferenceFeaturesModel {
    id: rModel

    config: root._fieldConfig
    project: root._fieldActiveProject

    onModelReset: textField.text = rModel.attributeFromForeignKey( root._fieldValue, MM.FeaturesModel.FeatureTitle ) || ""
  }

  Loader {
    id: listLoader

    asynchronous: true
    active: false
    sourceComponent: listComponent
  }

  Component {
    id: listComponent

    MMFormComponents.MMFeaturesListPageDrawer {

      pageHeader.title: qsTr( "Change link" )

      list.model: rModel
      button.text: qsTr( "Unlink feature" )
      button.visible: rModel.allowNull

      onClosed: listLoader.active = false
      onSearchTextChanged: ( searchText ) => rModel.searchExpression = searchText
      onFeatureClicked: function( selectedFeatures ) {
        const fk = rModel.foreignKeyFromReferencedFeatureId( selectedFeatures.feature.id )
        root.editorValueChanged( fk, false )
        close()
      }

      onButtonClicked: {
        root.editorValueChanged( undefined, true )
        close()
      }

      Component.onCompleted: open()
    }
  }
}
