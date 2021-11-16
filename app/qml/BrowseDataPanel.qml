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
  property var selectedLayer: null

  visible: false

  function clearStackAndClose() {
    if (browseDataLayout.depth > 1)
      browseDataLayout.pop(null); // pops everything besides an initialItem
    root.visible = false;
  }
  signal createFeatureRequested
  signal featureSelectRequested(var pair)
  function popOnePageOrClose() {
    if (browseDataLayout.depth > 1) {
      browseDataLayout.pop();
    } else
      clearStackAndClose();
  }
  function pushFeaturesListPage() {
    let geometry = __inputUtils.geometryFromLayer(root.selectedLayer);
    browseDataLayout.push(featuresListComponent, {
        "toolbarVisible": geometry === "nullGeo",
        "resetAfterSelection": geometry !== "nullGeo"
      });
    featuresListModel.reloadFeatures();
  }
  function refreshFeaturesData() {
    featuresListModel.reloadFeatures();
  }
  function setSelectedLayer(layerId) {
    let layer = __browseDataLayersModel.layerFromLayerId(layerId);
    selectedLayer = layer;
  }

  onFocusChanged: {
    // pass focus to stackview
    browseDataLayout.focus = true;
  }

  StackView {
    id: browseDataLayout
    anchors.fill: parent
    focus: true
    initialItem: browseDataLayersPanel

    Keys.onReleased: {
      if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
        event.accepted = true;
        popOnePageOrClose();
      }
    }
    onVisibleChanged: {
      if (browseDataLayout.visible)
        browseDataLayout.forceActiveFocus();
    }
  }
  Component {
    id: browseDataLayersPanel
    BrowseDataLayersPanel {
      onBackButtonClicked: popOnePageOrClose()
      onLayerClicked: {
        setSelectedLayer(layerId);
        pushFeaturesListPage();
      }
    }
  }
  Component {
    id: featuresListComponent
    FeaturesListPage {
      id: featuresListPage
      featuresModel: featuresListModel
      toolbarButtons: ["add"]

      onAddFeatureClicked: createFeatureRequested()
      onBackButtonClicked: popOnePageOrClose()
      onSelectionFinished: {
        let featurePair = featuresListModel.convertRoleValue(FeaturesModel.FeatureId, featureIds, FeaturesModel.FeaturePair);
        if (!featurePair.feature.geometry.isNull) {
          clearStackAndClose(); // close view if feature has geometry
        }
        root.featureSelectRequested(featurePair);
      }
    }
  }
  FeaturesModel {
    id: featuresListModel
    layer: root.selectedLayer
  }
}
