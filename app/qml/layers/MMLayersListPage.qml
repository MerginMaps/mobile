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

import "../components" as MMComponents
import "../inputs"

MMComponents.MMPage {
  id: root

  property var model: null
  property var parentNodeIndex: null
  property string pageTitle: qsTr("Layers")

  signal nodeClicked( var node, string nodeType, string nodeName )
  signal nodeVisibilityClicked( var node )
  signal searchBarClicked()
  signal resumeDraftClicked()

  pageHeader.title: root.pageTitle

  pageBottomMarginPolicy: MMComponents.MMPage.PaintBehindSystemBar

  pageContent: Item {
    width: parent.width
    height: parent.height

    MMSearchInput {
      id: searchBar

      anchors.top: parent.top
      anchors.topMargin: __style.spacing20
      width: parent.width

      delayedSearch: true

      placeholderText: qsTr( "Search" ) + "..."

      MouseArea {
        anchors.fill: parent
        onClicked: root.searchBarClicked()
      }
    }

    ColumnLayout {
      id: contentColumn

      anchors.top: searchBar.bottom
      anchors.topMargin: __style.spacing20
      anchors.left: parent.left
      anchors.right: parent.right
      anchors.bottom: parent.bottom

      spacing: __style.spacing10

      MMComponents.MMListBanner {
        id: draftBanner

        Layout.fillWidth: true

        visible: __activeProject.featureDraftController.hasDraft

        variant: MMComponents.MMListBanner.Warning
        text: qsTr( "%1 has unsaved changes" ).arg( __activeProject.featureDraftController.draftLayerName )
        actionText: qsTr( "Resume" )

        onActionClicked: () => root.resumeDraftClicked()
      }

      MMLayersList {
        id: layers

        Layout.fillWidth: true
        Layout.fillHeight: true

        clip: true

        basemodel: root.model
        parentNodeIndex: root.parentNodeIndex

        imageProviderPath: "image://LayerTreeModelPixmapProvider/"

        footer: MMComponents.MMListFooterSpacer {}

        onNodeClicked: function( node, nodeType, nodeName ) {
          root.nodeClicked( node, nodeType, nodeName )
        }

        onNodeVisibilityClicked: function( node ) {
          root.nodeVisibilityClicked( node )
        }
      }
    }
  }
}
