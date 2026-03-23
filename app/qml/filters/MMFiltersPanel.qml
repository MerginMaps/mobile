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

import "../components" as MMComponents

MMComponents.MMDrawer {
  id: root

  required property var filterController

  modal: false
  interactive: false
  closePolicy: Popup.CloseOnEscape

  dropShadow: true

  background: Rectangle {
    color: __style.polarColor
    radius: __style.radius20

    layer.enabled: root.dropShadow
    layer.effect: MMComponents.MMShadow {}

    Rectangle {
      color: __style.polarColor
      width: parent.width
      height: parent.height / 2
      y: parent.height / 2
    }
  }

  property bool filtersApplied: false

  Component.onCompleted: {
    internal.refreshLayers()
    root.open()
  }

  onClosed: {
    if ( !filtersApplied ) {
      // discard pending changes when closed without applying
      filterController.discardPendingChanges()
    }
    filtersApplied = false
  }

  // Cache of vector layers
  QtObject {
    id: internal
    property var vectorLayers: []

    function refreshLayers() {
      // clear first to force a full rebuild
      vectorLayers = []
      vectorLayers = filterController.getVectorLayers()
    }
  }

  drawerHeader.title: qsTr("Filters")
  drawerHeader.titleFont: __style.t2

  drawerHeader.topLeftItemContent: MMComponents.MMButton {
    type: MMButton.Types.Primary
    size: MMButton.Sizes.Small
    text: qsTr("Reset")
    fontColor: root.filterController.hasActiveFilters ? __style.grapeColor : __style.forestColor
    fontColorHover: root.filterController.hasActiveFilters ? __style.grapeColor : __style.forestColor
    bgndColor: root.filterController.hasActiveFilters ? __style.negativeLightColor : __style.transparentColor
    bgndColorHover: root.filterController.hasActiveFilters ? __style.negativeLightColor : __style.transparentColor

    anchors {
      left: parent.left
      leftMargin: __style.pageMargins + __style.safeAreaLeft
      verticalCenter: parent.verticalCenter
    }

    onClicked: {
      filterController.clearAllFilters()
      filterController.applyFiltersToAllLayers()
      root.filtersApplied = true
      // rebuild to reset input fields
      internal.refreshLayers()
    }
  }

  drawerContent: Item {
    width: parent.width
    height: root.maxHeightHit ? root.drawerContentAvailableHeight : contentColumn.implicitHeight

    MMComponents.MMScrollView {
      id: scrollView

      width: parent.width
      height: parent.height

      Column {
        id: contentColumn

        width: scrollView.availableWidth
        spacing: __style.margin20

        // no layers message
        MMComponents.MMText {
          width: parent.width
          visible: internal.vectorLayers.length === 0
          text: qsTr("No layers available for filtering")
          font: __style.p4
          color: __style.mediumGreyColor
          horizontalAlignment: Text.AlignHCenter
        }

        Repeater {
          id: layerRepeater

          model: internal.vectorLayers

          delegate: Item {
            required property var model

            width: contentColumn.width
            height: layerSection.implicitHeight

            MMFilterLayerSection {
              id: layerSection
              width: parent.width

              layerId: model.layerId
              layerName: model.layerName
              filterController: root.filterController
              vectorLayer: model.layer
            }
          }
        }

        // space for the floating button
        Item {
          width: parent.width
          height: showResultsButton.height + __style.margin12
        }
      }
    }

    MMComponents.MMButton {
      id: showResultsButton

      z: 1
      width: parent.width

      anchors {
        bottom: parent.bottom
        left: parent.left
        right: parent.right
      }

      text: qsTr("Apply filters")   

      onClicked: {
        filterController.applyFiltersToAllLayers()
        root.filtersApplied = true
        root.close()
      }
    }
  }

  Connections {
    target: __activeProject

    function onProjectReloaded(qgsProject) {
      internal.refreshLayers()
    }
  }
}
