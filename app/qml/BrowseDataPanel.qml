/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.0
import QtQuick.Controls 2.12
import lc 1.0

import QgsQuick 0.1 as QgsQuick
/*
 * BrowseDataPanel should stay a logic component, please do not combine UI here
 */

Item {
  id: root
  visible: false
  property var selectedLayer: null

  signal featureSelectRequested( var pair )
  signal createFeatureRequested()

  onSelectedLayerChanged: {
    if ( selectedLayer )
      featuresListModel.populateFromLayer( selectedLayer )
  }

  onFocusChanged: { // pass focus to stackview
    browseDataLayout.focus = true
  }

  function refreshFeaturesData() {
    featuresListModel.loadFeaturesFromLayer()
  }

  function clearStackAndClose() {
    if ( browseDataLayout.depth > 1 )
      browseDataLayout.pop( null ) // pops everything besides an initialItem
    root.visible = false
  }

  function popOnePageOrClose() {
    if ( browseDataLayout.depth > 1 )
    {
      browseDataLayout.pop()
    }
    else clearStackAndClose()
  }

  function loadFeaturesFromLayerIndex( index ) {
    let modelIndex = __browseDataLayersModel.index( index, 0 )
    let layer = __browseDataLayersModel.data( modelIndex, LayersModel.VectorLayerRole )

    selectedLayer = layer
  }

  function pushFeaturesPanelWithParams( index ) {
    let modelIndex = __browseDataLayersModel.index( index, 0 )
    let hasGeometry = __browseDataLayersModel.data( modelIndex, LayersModel.HasGeometryRole )
    let layerName = __browseDataLayersModel.data( modelIndex, LayersModel.LayerNameRole )

    browseDataLayout.push( browseDataFeaturesPanel, {
                            layerHasGeometry: hasGeometry,
                            layerName: layerName,
                            featuresModel: featuresListModel
                          } )
  }

  function searchTextEdited( text ) {
    featuresListModel.filterExpression = text
  }

  StackView {
    id: browseDataLayout
    initialItem: browseDataLayersPanel
    anchors.fill: parent
    focus: true

    Keys.onReleased: {
      if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
        event.accepted = true;
        popOnePageOrClose()
      }
    }

    onVisibleChanged: {
      if ( browseDataLayout.visible )
        browseDataLayout.forceActiveFocus()
    }
  }

  Component {
    id: browseDataLayersPanel

    BrowseDataLayersPanel {
      onBackButtonClicked: popOnePageOrClose()
      onLayerClicked: {
        loadFeaturesFromLayerIndex( index )
        pushFeaturesPanelWithParams( index )
      }
    }
  }

  Component {
    id: browseDataFeaturesPanel

    BrowseDataFeaturesPanel {
      id: dataFeaturesPanel
      onBackButtonClicked: popOnePageOrClose()
      onFeatureClicked: {
        let featurePair = featuresListModel.featureLayerPair( featureIdx )

        if ( !featurePair.feature.geometry.isNull ) {
          clearStackAndClose() // close view if feature has geometry
          deactivateSearch()
        }

        root.featureSelectRequested( featurePair )
      }
      onAddFeatureClicked: createFeatureRequested()
      onSearchTextChanged: searchTextEdited( text )
    }
  }

  QgsQuick.FeaturesListModel {
    id: featuresListModel
    modelType: featuresListModel.FeatureListing
  }
}
