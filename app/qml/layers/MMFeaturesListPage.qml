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

Page {
  id: root

  property var selectedLayer: null

  signal close()
  signal featureClicked( var featurePair )
  signal addFeatureClicked( var toLayer )

  header: MMComponents.MMPageHeader {
    width: parent.width
    color: __style.lightGreenColor
    title: root.selectedLayer ? root.selectedLayer.name + " (" + featuresModel.layerFeaturesCount + ")": ""
    onBackClicked: root.close()
  }

  Rectangle {
    anchors.fill: parent
    color: __style.lightGreenColor
  }

  MMSearchInput {
    id: searchbox
    anchors {
      left: parent.left
      leftMargin: __style.pageMargins
      right: parent.right
      rightMargin: __style.pageMargins
      top: parent.top
      topMargin: __style.margin20
    }

    onSearchTextChanged: searchDelay.restart()
  }

  MMComponents.MMMessage {
    anchors.fill: parent

    image: __style.positiveMMSymbolImage
    title: qsTr("No features found")
    visible: listView.model.count === 0
  }

  ListView {
    id: listView

    model: MM.FeaturesModel {
      id: featuresModel

      layer: root.selectedLayer
    }

    anchors {
      top: searchbox.bottom
      topMargin: __style.margin20
      left: parent.left
      leftMargin: __style.pageMargins
      right: parent.right
      rightMargin: __style.pageMargins
      bottom: parent.bottom
    }

    clip: true

    delegate: MMComponents.MMListDelegate {
      width: ListView.view.width
      height: __style.row63

      text: model.display?.toString()?.replace(/\n/g, ' ') ?? ''
      secondaryText: model.Description + ( model.SearchResult ? ", " + model.SearchResult.replace(/\n/g, ' ') : "" )

      onClicked: root.featureClicked( model.FeaturePair )
    }
  }

  Timer {
    id: searchDelay
    interval: 500
    running: false
    repeat: false
    onTriggered: featuresModel.searchExpression = searchbox.text
  }

  MMComponents.MMBusyIndicator {
    id: busyIndicator
    running: featuresModel.fetchingResults
    anchors.centerIn: parent
  }

  MMComponents.MMButton {
    width: root.width - 2 * __style.pageMargins
    visible: __inputUtils.isNoGeometryLayer( root.selectedLayer )
    anchors {
      left: parent.left
      leftMargin: __style.pageMargins
      right: parent.right
      rightMargin: __style.pageMargins
      bottom: parent.bottom
      bottomMargin: __style.margin20
    }

    text: qsTr("Add feature")
    onClicked: root.addFeatureClicked( root.selectedLayer )
  }

  Component.onCompleted: {
    featuresModel.reloadFeatures()
  }
}
