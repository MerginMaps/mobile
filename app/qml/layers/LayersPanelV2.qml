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

  LayerTreeSortFilterModel {
    id: layerTreeProxyModel

    layerTreeModel: LayerTreeModel {
      id: layerModel
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

      onNodeClicked: function ( nodeIndex, nodeType ) {
        if ( nodeType === "group" )
        {
          const groupName = layerTreeProxyModel.data(nodeIndex, 0) // group name (0 = display role)
          const props = {
            parentNodeIndex: nodeIndex,
            pageTitle: groupName
          }

          let item = pagesStackView.push( layersListPage, props , StackView.PushTransition )
          item.forceActiveFocus()
        }
        else if ( nodeType === "layer" )
        {
          const props = {
            layerTreeNode: layerTreeProxyModel.getNode( nodeIndex )
          }

          let item = pagesStackView.push( layerDetailsPage, props, StackView.PushTransition )
          item.forceActiveFocus()
        }
      }

      onNodeVisibilityClicked: function( nodeIndex ) {
        let node = layerTreeProxyModel.getNode( nodeIndex )
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
        if (pagesStackView.depth > 1)  {
          pagesStackView.pop( StackView.PopTransition )
        }
        else {
          root.close()
        }
      }
    }
  }

  Component {
    id: searchLayersPage

    LayersListSearchPage {
      model: layerTreeProxyModel

      onNodeClicked: function( nodeIndex, nodeType ) {
        if ( nodeType === "group" )
        {
          const groupName = layerTreeProxyModel.data(nodeIndex, 0) // group name (0 = display role)
          const props = {
            parentNodeIndex: nodeIndex,
            pageTitle: groupName
          }

          let item = pagesStackView.push( layersListPage, props , StackView.PushTransition )
          item.forceActiveFocus()
        }
        else if ( nodeType === "layer" )
        {
          const props = {
            mapLayer: layerTreeProxyModel.getNode( nodeIndex )
          }

          let item = pagesStackView.push( layerDetailsPage, props, StackView.PushTransition )
          item.forceActiveFocus()
        }
      }

      onSearchTextChanged: function( searchText ) {
        layerTreeProxyModel.searchExpression = searchText
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
      layerModel.reset()
    }

    function onProjectReloaded( qgsProject ) {
      layerModel.qgsProject = __activeProject.qgsProject
    }
  }

  Component.onCompleted: {

    //
    // We need to initialize pixmap provider so that it knows where to look for icons (model)
    //
    __layerTreePixmapProvider.setModel( layerTreeProxyModel )

    let item = pagesStackView.push( layersListPage, {}, StackView.Immediate )
    item.forceActiveFocus()
  }
}
