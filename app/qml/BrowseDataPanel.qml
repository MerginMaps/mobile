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

  onFocusChanged: { // pass focus to stackview
    browseDataLayout.focus = true
  }

  function refreshFeaturesData() {
    featuresListModel.reloadFeatures()
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

  function setSelectedLayer( layerId ) {
    let layer = __browseDataLayersModel.layerFromLayerId( layerId )
    selectedLayer = layer
  }

  function pushFeaturesListPage() {
    let geometry = __inputUtils.geometryFromLayer( root.selectedLayer )
    browseDataLayout.push( featuresListComponent, {
                            toolbarVisible: geometry === "nullGeo",
                            resetAfterSelection: geometry !== "nullGeo"
                          } )
    featuresListModel.reloadFeatures()
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
        setSelectedLayer( layerId )
        pushFeaturesListPage()
      }
    }
  }

  Component {
    id: featuresListComponent

    FeaturesListPage {
      id: featuresListPage

      featuresModel: featuresListModel

      toolbarButtons: ["add"]
      onBackButtonClicked: popOnePageOrClose()
      onSelectionFinished: {
        let featurePair = featuresListModel.convertRoleValue( FeaturesModel.FeatureId, featureIds, FeaturesModel.FeaturePair )

        if ( !featurePair.feature.geometry.isNull ) {
          clearStackAndClose() // close view if feature has geometry
        }

        root.featureSelectRequested( featurePair )
      }
      onAddFeatureClicked: createFeatureRequested()
    }
  }

  FeaturesModel {
    id: featuresListModel

    layer: root.selectedLayer
  }
}
