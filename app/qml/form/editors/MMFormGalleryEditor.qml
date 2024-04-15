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

Item {
  id: root

  width: parent.width
  height: column.height

  property var _fieldAssociatedRelation: parent.fieldAssociatedRelation
  property var _fieldFeatureLayerPair: parent.fieldFeatureLayerPair
  property var _fieldActiveProject: parent.fieldActiveProject

  property string _fieldTitle: parent.fieldTitle
  property bool _fieldShouldShowTitle: parent.fieldShouldShowTitle

  property var model
  property string title: _fieldShouldShowTitle ? _fieldTitle : ""
  property string warningMsg
  property string errorMsg
  property int maxVisiblePhotos: -1 // -1 for showing all photos
  property bool showAddImage: true

  signal showAll()
  signal clicked( var path )
  signal addImage()

  signal openLinkedFeature( var linkedFeature )
  signal createLinkedFeature( var parentFeature, var relation )

  Column {
    id: column

    padding: 20 * __dp
    spacing: 10 * __dp
    width: parent.width - 40 * __dp

    Item {
      width: parent.width
      height: 15 * __dp

      Text {
        width: column.width - showAllButton.width - 10 * __dp

        text: root.title
        font: __style.p6
        elide: Text.ElideRight
        color: __style.nightColor
      }

      Text {
        id: showAllButton

        anchors.right: parent.right

        visible: false // for now

        text: qsTr("Show all")
        font: __style.t4
        color: __style.forestColor

        MouseArea {
          anchors.fill: parent
          onClicked: root.showAll()
        }
      }
    }

    ListView {
      id: rowView

      height: 120 * __dp
      width: parent.width
      spacing: root.maxVisiblePhotos !== 0 ? __style.spacing12 : 0
      orientation: ListView.Horizontal

//      model: {
//        if(root.maxVisiblePhotos >= 0 && root.model.length > root.maxVisiblePhotos) {
//          return root.model.slice(0, root.maxVisiblePhotos)
//        }
//        return root.model
//      }

      model: MM.RelationFeaturesModel {
        id: rmodel

        relation: root._fieldAssociatedRelation
        parentFeatureLayerPair: root._fieldFeatureLayerPair
        homePath: root._fieldActiveProject.homePath
      }

      delegate: MMPhoto {
        width: rowView.height

        fillMode: Image.PreserveAspectCrop

        photoUrl: {
          let absolutePath = model.PhotoPath

          if ( absolutePath !== '' && __inputUtils.fileExists( absolutePath ) ) {
            return "file://" + absolutePath
          }
          return ''
        }

        onClicked: function(path) {
          root.clicked(path)
          root.openLinkedFeature( model.FeaturePair )
        }
      }

      header: Row {
        visible: root.showAddImage

        Rectangle {
          width: visible ? height : 0
          height: rowView.height
          radius: 20 * __dp
          border.width: 2 * __dp
          border.color: root.errorMsg.length > 0 ? __style.negativeColor : root.warningMsg.length > 0 ? __style.warningColor : __style.polarColor
          color: (root.errorMsg.length > 0 || root.warningMsg.length > 0) ? __style.negativeUltraLightColor : __style.polarColor

          MMIcon {
            anchors.centerIn: parent
            source: __style.addImageIcon
            color: root.errorMsg.length > 0 ? __style.grapeColor : root.warningMsg.length > 0 ? __style.earthColor : __style.forestColor
            size: __style.icon32
          }

          MouseArea {
            anchors.fill: parent
            onClicked: {
              root.addImage()
              root.createLinkedFeature( root._fieldFeatureLayerPair, root._fieldAssociatedRelation )
            }
          }
        }

        Item {
          width: visible ? rowView.spacing : 0
          height: rowView.height
        }
      }

      footer: MMMorePhoto {
        width: visible ? rowView.height + rowView.spacing: 0

//        hiddenPhotoCount: root.model.length - root.maxVisiblePhotos
//        visible: root.maxVisiblePhotos >= 0 && root.model.length > root.maxVisiblePhotos
//        photoUrl: visible ? model[root.maxVisiblePhotos] : ""
//        space: visible ? rowView.spacing : 0

        visible: false

        onClicked: root.showAll()
      }
    }

    Row {
      id: msgRow

      spacing: 4 * __dp

      MMIcon {
        id: msgIcon

        source: visible ? __style.errorCircleIcon : ""
        color: root.errorMsg.length > 0 ? __style.negativeColor : __style.warningColor
        size: __style.icon16
        visible: root.errorMsg.length > 0 || root.warningMsg.length > 0
      }

      Text {
        width: column.width - msgRow.spacing - msgIcon.size

        text: root.errorMsg.length > 0 ? root.errorMsg : root.warningMsg
        font: __style.t4
        wrapMode: Text.WordWrap
        visible: root.errorMsg.length > 0 || root.warningMsg.length > 0
      }
    }
  }
}
