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
  property string pageTitle: qsTr("Layers")

  signal close()
  signal nodeClicked( var node, string nodeType, string nodeName )
  signal nodeVisibilityClicked( var node )
  signal searchboxClicked()

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

      placeholderText: qsTr( "Search" ) + "..."

      MouseArea {
        anchors.fill: parent
        onClicked: root.searchboxClicked()
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

      imageProviderPath: "image://LayerTreeModelPixmapProvider/"

      onNodeClicked: function( node, nodeType, nodeName ) {
        root.nodeClicked( node, nodeType, nodeName )
      }

      onNodeVisibilityClicked: function( node ) {
        root.nodeVisibilityClicked( node )
      }
    }
  }
}
