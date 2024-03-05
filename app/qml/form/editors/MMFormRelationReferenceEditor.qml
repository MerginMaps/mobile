/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Controls

import mm 1.0 as MM

import "../../components"
import "../../inputs"

MMBaseInput {
  id: root

  property var _fieldValue: parent.fieldValue
  property var _fieldConfig: parent.fieldConfig
  property var _fieldActiveProject: parent.fieldActiveProject
  property bool _fieldValueIsNull: parent.fieldValueIsNull
  property bool _fieldIsReadOnly: parent.fieldIsReadOnly
  property string _fieldTitle: parent.fieldTitle
  property bool _fieldShouldShowTitle: parent.fieldShouldShowTitle

  signal openLinkedFeature( /* FeaturePair */ var linkedFeature )
  signal editorValueChanged( var newValue, bool isNull )

  on_FieldValueChanged: {
    title.text = rModel.attributeFromForeignKey( root._fieldValue, MM.FeaturesModel.FeatureTitle ) || ""
  }

  title: _fieldShouldShowTitle ? _fieldTitle : ""

  MM.RelationReferenceFeaturesModel {
    id: rModel

    config: root._fieldConfig
    project: root._fieldActiveProject

    onModelReset: title.text = rModel.attributeFromForeignKey( root._fieldValue, MM.FeaturesModel.FeatureTitle ) || ""
  }

  content: Text {
    id: title

    anchors.fill: parent
    font: __style.p5
    text: root._fieldValue
    color: root.enabled ? __style.nightColor : __style.mediumGreenColor
    verticalAlignment: Text.AlignVCenter
  }

  onContentClicked: {
    let featurePair = rModel.attributeFromForeignKey( root._fieldValue, MM.FeaturesModel.FeaturePair )

    if ( featurePair == null || !featurePair.valid ) return

    openLinkedFeature( featurePair )
  }

  rightAction: MMIcon {
    id: rightIcon

    anchors.verticalCenter: parent.verticalCenter

    size: __style.icon24
    source: __style.linkIcon
    color: __style.forestColor
  }

  onRightActionClicked: {
    if ( root._fieldIsReadOnly )
      return

    listLoader.active = true
    listLoader.focus = true
  }

  Loader {
    id: listLoader

    asynchronous: true
    active: false
    sourceComponent: listComponent
  }

  Component {
    id: listComponent

    MMFeaturesListDrawer {
      id: featuresDrawer
      focus: true
      model: rModel
      title: qsTr( "Change link" )
      buttonText: qsTr( "Unlink feature" )
      buttonVisible: rModel.allowNull
      withSearch: false
      Component.onCompleted: open()

      onClosed: listLoader.active = false

      onFeatureClicked: function(selectedFeatures) {
        let fk = rModel.foreignKeyFromAttribute( MM.FeaturesModel.FeatureId, selectedFeatures.feature.id )
        root.editorValueChanged( fk, false )
        featuresDrawer.close()

      }

      onButtonClicked: {
        root.editorValueChanged( undefined, true )
        featuresDrawer.close()
      }
    }
  }
}
