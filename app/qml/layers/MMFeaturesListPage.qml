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
import QtQuick.Layouts

import mm 1.0 as MM

import "../inputs"
import "../components" as MMComponents

MMComponents.MMPage {
  id: root

  property var selectedLayer: null

  signal close()
  signal featureClicked( var featurePair )
  signal addFeatureClicked( var toLayer )

  pageHeader.title: root.selectedLayer ? root.selectedLayer.name + " (" + featuresModel.layerFeaturesCount + ")": ""
  onBackClicked: root.close()

  pageContent: Item {
    width: parent.width
    height: parent.height

    MMSearchInput {
      id: searchbox

      anchors.topMargin: __style.spacing20
      width: parent.width
      onSearchTextChanged: featuresModel.searchExpression = searchbox.text
      allowTimer: true
    }

    ListView {
      id: listView

      width: parent.width
      anchors {
        top: searchbox.bottom
        bottom: addButton.visible ? addButton.top : parent.bottom
        bottomMargin: __style.spacing20
        topMargin: __style.spacing20
      }

      model: MM.FeaturesModel {
        id: featuresModel

        layer: root.selectedLayer
      }

      clip: true

      delegate: MMComponents.MMListDelegate {
        text: model.display?.toString()?.replace(/\n/g, ' ') ?? ''
        secondaryText: model.Description + ( model.SearchResult ? ", " + model.SearchResult.replace(/\n/g, ' ') : "" )

        onClicked: root.featureClicked( model.FeaturePair )
      }
    }

    MMComponents.MMBusyIndicator {
      id: busyIndicator

      anchors.centerIn: parent
      running: featuresModel.fetchingResults
    }

    MMComponents.MMButton {
      id: addButton

      width: parent.width
      anchors.bottom: parent.bottom
      anchors.bottomMargin: __style.margin20
      visible: __inputUtils.isNoGeometryLayer( root.selectedLayer )

      text: qsTr("Add feature")

      onClicked: root.addFeatureClicked( root.selectedLayer )
    }
  }

  Component.onCompleted: {
    featuresModel.reloadFeatures()
  }
}
