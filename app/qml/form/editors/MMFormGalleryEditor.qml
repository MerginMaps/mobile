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
import MMInput

import "../../components" as MMComponents
import "../../components/private" as MMPrivateComponents

MMPrivateComponents.MMBaseInput {
  id: root

  property MM.AttributeController _fieldController: parent.fieldController
  property var _fieldAssociatedRelation: parent.fieldAssociatedRelation
  property var _fieldActiveProject: parent.fieldActiveProject

  property bool _fieldShouldShowTitle: parent.fieldShouldShowTitle
  property string _fieldTitle: parent.fieldTitle

  signal openLinkedFeature( var linkedFeature )
  signal createLinkedFeature( var parentFeature, var relation )

  title: _fieldShouldShowTitle ? _fieldTitle : ""

  infoMsg: root._fieldAssociatedRelation && __activeProject.filterController?.filteringAvailable && __activeProject.filterController?.hasActiveFilterOnLayer( rmodel?.layer?.id )
           ? qsTr( "Some features may be hidden by active filters" )
           : ""

  property real _photoCardSize: rowView.height

  inputContent: MMComponents.MMListView {
    id: rowView

    width: parent.width
    height: __style.row120

    clip: true
    spacing: __style.spacing12
    orientation: ListView.Horizontal

    model: RelationFeaturesModel {
      id: rmodel

      relation: root._fieldAssociatedRelation
      parentFeatureLayerPair: root._fieldController.featureLayerPair
      homePath: root._fieldActiveProject.homePath
    }

    delegate: MMComponents.MMPhotoCard{
      size: root._photoCardSize

      imageSource: {
        let absolutePath = model.PhotoPath

        if ( absolutePath !== '' && __inputUtils.fileExists( absolutePath ) ) {
          return __inputUtils.localFileToUrl(absolutePath)
        }
        return ''
      }

      textVisible: false

      text: model.FeatureTitle

      onClicked: function( path ) {
        root.forceActiveFocus()
        root.openLinkedFeature( model.FeaturePair )
      }
    }

    header: {
      if ( __activeProject.projectRole !== "reader" && !rmodel?.layer?.readOnly )
        return addFeatureComponent

      if ( rowView.count === 0 )
        return emptyComponent

      return null
    }
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
          onSingleClicked: {
            root.forceActiveFocus()
            root.createLinkedFeature( root._fieldController.featureLayerPair, root._fieldAssociatedRelation )
          }
        }
      }

      MMComponents.MMListSpacer {
        width: rowView.spacing
      }
    }
  }

  Component {
    id: emptyComponent

    Rectangle {
      height: root._photoCardSize
      width: root._photoCardSize

      radius: __style.radius20
      color: __style.polarColor

      Rectangle {
        width: parent.width * 0.8
        height: parent.height * 0.8

        anchors.centerIn: parent
        radius: parent.radius
        color: __style.lightGreenColor

        MMComponents.MMText {
          anchors.centerIn: parent
          text: qsTr( "No images" )
          font: __style.t4Italic
          color: __style.darkGreyColor
        }
      }
    }
  }
}
