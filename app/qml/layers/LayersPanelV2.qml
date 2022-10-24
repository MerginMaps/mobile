/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.14
import QtQuick.Controls 2.14

import lc 1.0
import "../components" as MMComponents
import ".."

Item {
  id: root

  signal close()

  signal addFeature( var targetLayer )
  signal selectFeature( var featurePair )

  LayerTreeSortFilterModel {
    id: layerTreeProxyModel

    layerTreeModel: LayerTreeModel {
      id: layerTreeModel
      qgsProject: __activeProject.qgsProject
    }
  }

  LayerTreeFlatSortFilterModel {
    id: layerTreeFlatSortFilterModel

    layerTreeFlatModel: LayerTreeFlatModel {
      id: layerTreeFlatModel
      qgsProject: __activeProject.qgsProject
    }
  }

  StackView {
    id: pagesStackView

    anchors.fill: parent
  }

  Keys.onReleased: {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true
      root.close()
    }
  }

  Component {
    id: layersListPage

    LayersListPageV2 {
      model: layerTreeProxyModel

      onNodeClicked: function ( node, nodeType, nodeName ) {
        if ( nodeType === "group" )
        {
          let index = layerTreeProxyModel.node2index( node )
          const props = {
            parentNodeIndex: index,
            pageTitle: layerTreeProxyModel.data(index, nodeName)
          }

          let item = pagesStackView.push( layersListPage, props , StackView.PushTransition )
          item.forceActiveFocus()
        }
        else if ( nodeType === "layer" )
        {
          const props = {
            layerTreeNode: node
          }

          let item = pagesStackView.push( layerDetailsPage, props, StackView.PushTransition )
          item.forceActiveFocus()
        }
      }

      onNodeVisibilityClicked: function( node ) {
        __activeProject.switchLayerTreeNodeVisibility( node )
      }

      onClose: function() {
        if (pagesStackView.depth > 1)  {
          pagesStackView.pop( StackView.PopTransition )
        }
        else {
          root.close()
        }
      }

      onSearchboxClicked: function() {
        let item = pagesStackView.push( searchLayersPage, {}, StackView.PushTransition )
        item.forceActiveFocus()
      }
    }
  }

  Component {
    id: layerDetailsPage

    LayerDetail {
      onClose: function() {
        if (pagesStackView.depth > 1) {
          pagesStackView.pop( StackView.PopTransition )
        }
        else {
          root.close()
        }
      }

      onFeatureClicked: function( featurePair ) {
        root.selectFeature( featurePair )
      }

      onAddFeatureClicked: function( targetLayer ) {
        root.addFeature( targetLayer )
      }
    }
  }

  Component {
    id: searchLayersPage

    LayersListSearchPage {

      model: layerTreeFlatSortFilterModel

      onNodeClicked: function( node, nodeType, nodeName ) {
        if ( nodeType === "group" )
        {
          // convert to layersListSortFilterModel index
          const index = layerTreeProxyModel.node2index( node )
          const props = {
            parentNodeIndex: index,
            pageTitle: nodeName
          }

          let item = pagesStackView.push( layersListPage, props , StackView.PushTransition )
          item.forceActiveFocus()
        }
        else if ( nodeType === "layer" )
        {
          const props = {
            mapLayer: node
          }

          let item = pagesStackView.push( layerDetailsPage, props, StackView.PushTransition )
          item.forceActiveFocus()
        }
      }

      onSearchTextChanged: function( searchText ) {
        layerTreeFlatSortFilterModel.searchExpression = searchText
      }

      onNodeVisibilityClicked: function( node ) {
        __activeProject.switchLayerTreeNodeVisibility( node )
      }

      onClose: function() {
        if (pagesStackView.depth > 1)  {
          pagesStackView.pop( StackView.PopTransition )
        }
        else {
          root.close()
        }
      }
    }
  }

  Connections {
    target: __activeProject

    function onProjectWillBeReloaded() {
      layerTreeModel.reset()
      layerTreeFlatModel.reset()
    }

    function onProjectReloaded( qgsProject ) {
      layerTreeModel.qgsProject = __activeProject.qgsProject
      layerTreeFlatModel.qgsProject = __activeProject.qgsProject
    }
  }

  Component.onCompleted: {
    //
    // We need to initialize pixmap providers so that it knows where to look for icons
    //
    __layerTreeModelPixmapProvider.setModel( layerTreeModel )
    __layerTreeFlatModelPixmapProvider.setModel( layerTreeFlatModel )

    let item = pagesStackView.push( layersListPage, {}, StackView.Immediate )
    item.forceActiveFocus()
  }

  Component.onDestruction: {
    __layerTreeModelPixmapProvider.reset()
    __layerTreeFlatModelPixmapProvider.reset()
  }
}
