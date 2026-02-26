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
import MMInput

import "../components"
import "../inputs"

Item {
  id: root

  signal close()

  signal addFeature( var targetLayer )
  signal selectFeature( var featurePair )

  MM.LayerTreeSortFilterModel {
    id: layerTreeProxyModel

    layerTreeModel: MM.LayerTreeModel {
      id: layerTreeModel
      qgsProject: __activeProject.qgsProject
    }
  }

  MM.LayerTreeFlatSortFilterModel {
    id: layerTreeFlatSortFilterModel

    layerTreeFlatModel: MM.LayerTreeFlatModel {
      id: layerTreeFlatModel
      qgsProject: __activeProject.qgsProject
    }
  }

  MMStackView {
    id: pagesStackView

    anchors.fill: parent
  }

  Keys.onReleased: function( event ) {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true

      if ( pagesStackView.depth === 1 ) {
        root.close()
      }
      else {
        pagesStackView.pop( StackView.PopTransition )
      }
    }
  }

  Component {
    id: layersListPage

    MMLayersListPage {
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

      onBackClicked: function() {
        if (pagesStackView.depth > 1)  {
          pagesStackView.pop( StackView.PopTransition )
        }
        else {
          root.close()
        }
      }

      onSearchBarClicked: function() {
        let item = pagesStackView.push( searchLayersPage, {}, StackView.Immediate )
        item.forceActiveFocus()
      }
    }
  }

  Component {
    id: layerDetailsPage

    MMLayerDetailPage {
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

    MMLayersListSearchPage {

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
            layerTreeNode: node
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

      onBackClicked: function() {
        if (pagesStackView.depth > 1)  {
          pagesStackView.pop( StackView.Immediate )
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
