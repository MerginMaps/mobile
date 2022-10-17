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

    sourceModel: LayerTreeModel {
      id: layerTreeModel
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

      onLayerClicked: function ( layerindex, isGroup ) {
        if ( isGroup )
        {
          let item = pagesStackView.push( layersListPage, { parentModelIndex: layerindex }, StackView.PushTransition )
          item.forceActiveFocus()
        }
        else
        {
          let item = pagesStackView.push( layerDetailsPage, {}, StackView.PushTransition )
          item.forceActiveFocus()
        }
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
    }

    function onProjectReloaded( qgsProject ) {
      layerTreeModel.qgsProject = __activeProject.qgsProject
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
