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

/*
 * BrowseDataPanel should stay a logic component, please do not combine UI here
 */

Item {
  id: root
  visible: false
  property var selectedLayer: null

  signal featureSelectRequested( var pair )
  signal createFeatureRequested()

  function clearStackAndClose() {
    if ( browseDataLayout.depth > 1 )
      browseDataLayout.pop( null ) // pops everything besides an initialItem
    root.visible = false
  }

  onSelectedLayerChanged: {
    if ( selectedLayer )
      __featuresModel.reloadDataFromLayer( selectedLayer )
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
    let featuresCount = __featuresModel.rowCount()

    browseDataLayout.push( browseDataFeaturesPanel, { layerHasGeometry: hasGeometry, layerName: layerName } )
  }

  function searchTextEdited( text ) {
    __featuresModel.filterExpression = text
  }

  StackView {
    id: browseDataLayout
    initialItem: browseDataLayersPanel
    anchors.fill: parent
  }

  Component {
    id: browseDataLayersPanel

    BrowseDataLayersPanel {
      onBackButtonClicked: clearStackAndClose()
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
      onBackButtonClicked: browseDataLayout.pop()
      onFeatureClicked: {
        let featurePair = __featuresModel.featureLayerPair( featureId )

        if ( !featurePair.feature.geometry.isNull )
          clearStackAndClose() // close view if feature has geometry

        root.featureSelectRequested( featurePair )
      }
      onAddFeatureClicked: createFeatureRequested()
      onSearchTextChanged: searchTextEdited( text )
    }
  }

  Connections {
    target: __featuresModel
    onTooManyFeaturesInLayer: {
      __inputUtils.showNotification( qsTr( "Too many features in layer, showing first %1" ).arg( limitCount ) )
    }
  }
}
