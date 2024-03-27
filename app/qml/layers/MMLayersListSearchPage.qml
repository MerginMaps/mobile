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

import "../components" as MMComponents
import "../inputs"

MMComponents.MMPage {
  id: root

  property var model: null
  property var parentNodeIndex: null
  property string pageTitle: qsTr("Search layers")

  signal close()
  signal nodeClicked( var node, string nodeType, string nodeName )
  signal nodeVisibilityClicked( var node )
  signal searchTextChanged( string searchText )

  onBackClicked: root.close()
  pageHeader.title: root.pageTitle

  pageContent: Item {
    width: parent.width
    height: parent.height

    MMSearchInput {
      id: searchbox

      anchors.top: parent.top
      anchors.topMargin: __style.spacing20
      width: parent.width

      onSearchTextChanged: function( searchText ) {
        root.searchTextChanged( searchText )
      }
    }

    MMLayersList {
      id: layers

      width: parent.width

      anchors {
        top: searchbox.bottom
        topMargin: __style.spacing20
        bottom: parent.bottom
      }

      clip: true

      model: root.model
      parentNodeIndex: root.parentNodeIndex

      showNodePath: true
      imageProviderPath: "image://LayerTreeFlatModelPixmapProvider/"

      onNodeClicked: function( node, nodeType, nodeName ) {
        root.nodeClicked( node, nodeType, nodeName )
      }

      onNodeVisibilityClicked: function( node ) {
        root.nodeVisibilityClicked( node )
      }
    }
  }

  // open keyboard automatically
  Component.onCompleted: searchbox.textFieldComponent.forceActiveFocus()
}
