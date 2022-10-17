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

Page {
  id: root

  property var model: null
  property var parentModelIndex: null

  signal close()
  signal layerClicked( var layerindex, bool isGroup )

  Keys.onReleased: {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true
      root.close()
    }
  }

  header: MMComponents.PanelHeaderV2 {
    width: parent.width
    headerTitle: qsTr("Search layers")
    onBackClicked: root.close()
  }

  MMComponents.SearchBoxV2 {
    id: searchbox

    anchors {
      left: parent.left
      leftMargin: InputStyle.panelMargin
      right: parent.right
      rightMargin: InputStyle.panelMargin
      top: parent.top
      topMargin: InputStyle.panelMarginV2
    }

//    onSearchTextChanged: listModel.listModelFilter()
  }

  LayersListV2 {
    id: layers

    anchors {
      top: searchbox.bottom
      topMargin: InputStyle.panelMarginV2
      left: parent.left
      leftMargin: InputStyle.panelMargin
      right: parent.right
      rightMargin: InputStyle.panelMargin
      bottom: parent.bottom
    }

    clip: true

    model: root.model
    parentModelIndex: root.parentModelIndex

    onLayerClicked: function( layerindex, isGroup ) {
      root.layerClicked( layerindex, isGroup )
    }
  }

  Component.onCompleted: {
    searchbox.setActive()
  }
}
