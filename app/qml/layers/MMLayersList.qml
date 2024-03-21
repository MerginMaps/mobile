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

import "../components"
import "../inputs"

Item {
  id: root

  property var model: null
  property var parentNodeIndex: null

  property bool showNodePath: false // show path to node groupA/groupB under the item
  property string imageProviderPath: ""

  signal nodeClicked( var node, string nodeType, string nodeName )
  signal nodeVisibilityClicked( var node )

  ListView {
    id: layerslist

    anchors.fill: parent

    model: DelegateModel {
      id: delegatemodel

      model: root.model

      delegate: Item {
        id: layerrow

        width: ListView.view.width
        height: __style.row49 * ( root.showNodePath ? 1.5 : 1 )

        MMLine {
          anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
          }
        }

        MouseArea {
          anchors {
            fill: layerRow // make the click area bigger
            leftMargin: -__style.margin12
            rightMargin: -__style.margin12
            bottomMargin: -__style.margin12
          }

          onClicked: root.nodeClicked( model.node, model.nodeType, model.display )
        }

        RowLayout {
          id: layerRow

          anchors {
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
          }

          Item {
            Layout.preferredWidth: __style.icon24
            Layout.preferredHeight: __style.icon24

            Image {
              anchors.fill: parent

              sourceSize: Qt.size( width, height)
              cache: false // important! Otherwise pixmap providers would not be called for the same id again

              source: root.imageProviderPath + model.serializedNode
            }
          }

          ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            Text {
              text: model.display

              Layout.fillWidth: true
              Layout.fillHeight: true
              verticalAlignment: Text.AlignVCenter

              color: __style.nightColor
              font: __style.t3
              elide: Text.ElideMiddle
            }

            Text {
              text: model.nodePath
              Layout.fillWidth: true
              Layout.fillHeight: true
              verticalAlignment: Text.AlignVCenter

              // show it only if there is some path
              visible: root.showNodePath && model.nodePath

              color: __style.nightColor
              font: __style.t3

              elide: Text.ElideMiddle
            }
          }

          MMSwitch { // TODO: might need adjustments
            id: visibleSwitch

            checked: model.nodeIsVisible === "yes"
            Layout.rightMargin: __style.pageMargins
            onReleased: function() {
              root.nodeVisibilityClicked( model.node )
            }
          }
        }
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
