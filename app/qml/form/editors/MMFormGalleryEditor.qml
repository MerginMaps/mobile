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

import "../../components" as MMComponents
import "../../components/private" as MMPrivateComponents

MMPrivateComponents.MMBaseInput {
  id: root

  property var _fieldAssociatedRelation: parent.fieldAssociatedRelation
  property var _fieldFeatureLayerPair: parent.fieldFeatureLayerPair
  property var _fieldActiveProject: parent.fieldActiveProject

  property bool _fieldShouldShowTitle: parent.fieldShouldShowTitle
  property string _fieldTitle: parent.fieldTitle

  signal openLinkedFeature( var linkedFeature )
  signal createLinkedFeature( var parentFeature, var relation )

  title: _fieldShouldShowTitle ? _fieldTitle : ""

  inputContent: MMComponents.MMListView {
    id: rowView

    width: parent.width
    height: __style.row120

    clip: true
    spacing: __style.spacing12
    orientation: ListView.Horizontal

    model: MM.RelationFeaturesModel {
      id: rmodel

      relation: root._fieldAssociatedRelation
      parentFeatureLayerPair: root._fieldFeatureLayerPair
      homePath: root._fieldActiveProject.homePath
    }

    delegate: MMComponents.MMPhotoCard{
      width: rowView.height
      height: rowView.height

      imageSource: {
        let absolutePath = model.PhotoPath

        if ( absolutePath !== '' && __inputUtils.fileExists( absolutePath ) ) {
          return "file:///" + absolutePath
        }
        return ''
      }

      textVisible: model.FeatureTitle ? true : false

      text: model.FeatureTitle

      onClicked: function( path ) {
        root.openLinkedFeature( model.FeaturePair )
      }
    }

    header: __activeProject.projectRole !== "reader" ? addFeatureComponent : null
  }

  Component {
    id: addFeatureComponent

    Row {

      Rectangle {
        height: rowView.height
        width: height

        radius: __style.radius20
        color: __style.polarColor

        MMComponents.MMIcon {
          anchors.centerIn: parent
          source: __style.addImageIcon
          color: root.errorMsg.length > 0 ? __style.grapeColor : root.warningMsg.length > 0 ? __style.earthColor : __style.forestColor
          size: __style.icon32
        }

        MMComponents.MMSingleClickMouseArea {
          anchors.fill: parent
          onSingleClicked: root.createLinkedFeature( root._fieldFeatureLayerPair, root._fieldAssociatedRelation )
        }
      }

      MMComponents.MMListSpacer {
        width: rowView.spacing
      }
    }
  }
}
