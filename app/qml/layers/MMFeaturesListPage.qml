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
  property bool hasToolbar: false
  property bool layerIsReadOnly: selectedLayer?.readOnly ?? false

  signal featureClicked( var featurePair )
  signal addFeatureClicked( var toLayer )

  pageHeader.title: root.selectedLayer ? root.selectedLayer.name + " (" + featuresModel.layerFeaturesCount + ")": ""
  pageBottomMargin: 0

  pageContent: Item {
    width: parent.width
    height: parent.height

    Rectangle {
      id: filterNotification

      anchors.top: parent.top
      anchors.topMargin: __style.spacing20

      width: parent.width
      height: filterRow.implicitHeight + 2 * __style.margin8
      radius: __style.radius12

      visible: root.selectedLayer && globalFilterController.filteredLayerIds.indexOf(root.selectedLayer.id) >= 0

      color: __style.sandColor
      border.width: 1 * __dp
      border.color: __style.sunsetColor

      Row {
        id: filterRow

        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: __style.margin12
        anchors.rightMargin: __style.margin12

        spacing: __style.margin4

        MMComponents.MMText {
          width: parent.width - resetButton.width - parent.spacing
          text: qsTr("Some features are hidden by a filter.")
          font: __style.p6
          color: __style.nightColor
          anchors.verticalCenter: parent.verticalCenter
          wrapMode: Text.Wrap
        }

        MMComponents.MMButton {
          id: resetButton

          type: MMButton.Types.Tertiary
          text: qsTr("Reset")
          fontColor: __style.earthColor
          size: MMButton.Sizes.Small
          anchors.verticalCenter: parent.verticalCenter

          onClicked: {
            globalFilterController.clearAllFilters()
            globalFilterController.applyFiltersToAllLayers()
            featuresModel.reloadFeatures()
          }
        }
      }
    }

    MMSearchInput {
      id: searchBar

      anchors.top: filterNotification.visible ? filterNotification.bottom : parent.top
      anchors.topMargin: __style.spacing20

      width: parent.width

      delayedSearch: true
      onSearchTextChanged: featuresModel.searchExpression = searchBar.text
    }

    MMComponents.MMListView {
      id: listView

      width: parent.width

      anchors {
        top: searchBar.bottom
        bottom: parent.bottom
        topMargin: __style.spacing20
      }

      model: MM.LayerFeaturesModel {
        id: featuresModel

        useAttributeTableSortOrder: true
        layer: root.selectedLayer
        attributeList: __inputUtils.referencedAttributeIndexes( layer, layer.displayExpression )
      }

      clip: true

      delegate: MMComponents.MMListDelegate {
        text: model.display?.toString()?.replace(/\n/g, ' ') ?? ''
        secondaryText: model.Description + ( model.SearchResult ? ", " + model.SearchResult.replace(/\n/g, ' ') : "" )

        onClicked: root.featureClicked( model.FeaturePair )
      }

      footer: MMComponents.MMListSpacer {
        height: __style.margin20 + ( root.hasToolbar ? 0 : __style.safeAreaBottom ) + ( addButton.visible ? addButton.height : 0 )
      }
    }

    MMComponents.MMBusyIndicator {
      anchors.centerIn: parent
      running: featuresModel.fetchingResults
    }

    MMComponents.MMButton {
      id: addButton

      width: parent.width
      anchors.bottom: parent.bottom
      anchors.bottomMargin: root.hasToolbar ? __style.margin20 : ( __style.safeAreaBottom + __style.margin8 )

      visible: __inputUtils.isNoGeometryLayer( root.selectedLayer ) && !root.layerIsReadOnly && __activeProject.projectRole !== "reader"

      text: qsTr("Add feature")

      onClicked: root.addFeatureClicked( root.selectedLayer )
    }
  }

  Component.onCompleted: {
    featuresModel.reloadFeatures()
  }
}
