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
import QtQml.Models
import QtQuick.Layouts

import "../components" as MMComponents
import "../inputs"

Item {
  id: root

  property var model: null
  property var parentNodeIndex: null

  property bool showNodePath: false // show path to node groupA/groupB under the item
  property string imageProviderPath: ""

  signal nodeClicked( var node, string nodeType, string nodeName )
  signal nodeVisibilityClicked( var node )

  MMComponents.MMMessage {
    anchors.fill: parent

    image: __style.positiveMMSymbolImage
    title: qsTr("No layers found")
    visible: layerslist.model.count === 0
  }

  ListView {
    id: layerslist

    anchors.fill: parent

    model: DelegateModel {
      id: delegatemodel

      model: root.model

      delegate: MMComponents.MMListDelegate {
        width: ListView.view.width

        property bool secondaryTextVisible: root.showNodePath && model.nodePath

        text: model.display
        secondaryText: secondaryTextVisible ? model.nodePath: ""

        leftContent: Image {
          width: __style.icon24
          height: width
          sourceSize: Qt.size( width, height)
          cache: false // important! Otherwise pixmap providers would not be called for the same id again

          source: root.imageProviderPath + model.serializedNode
        }

        rightContent: MMComponents.MMSwitch {
          checked: model.nodeIsVisible === "yes"
          onReleased: {
            root.nodeVisibilityClicked( model.node )
          }
        }

        onClicked: root.nodeClicked( model.node, model.nodeType, model.display )
      }
    }
  }

  Component.onCompleted: {
    if ( root.parentNodeIndex )
    {
      delegatemodel.rootIndex = root.parentNodeIndex
    }
  }
}
