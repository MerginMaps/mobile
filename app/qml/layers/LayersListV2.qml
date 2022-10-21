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
import QtQml.Models 2.14
import QtQuick.Layouts 1.14

import "../components" as MMComponents
import ".."

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
        height: InputStyle.rowHeightMedium * ( root.showNodePath ? 1.5 : 1 )

        Rectangle {
          anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
          }

          height: InputStyle.borderSize
          color: InputStyle.panelBackgroundDark
        }

        MouseArea {
          anchors {
            fill: layerRow // make the click area bigger
            leftMargin: -InputStyle.buttonClickArea
            rightMargin: -InputStyle.buttonClickArea
            bottomMargin: -InputStyle.buttonClickArea
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
            Layout.preferredWidth: InputStyle.iconSizeMedium
            Layout.preferredHeight: InputStyle.iconSizeMedium

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

              color: InputStyle.fontColor

              font.bold: true
              font.pixelSize: InputStyle.fontPixelSizeBig

              elide: Text.ElideMiddle
            }

            Text {
              text: model.nodePath
              Layout.fillWidth: true
              Layout.fillHeight: true

              // show it only if there is some path
              visible: root.showNodePath && model.nodePath

              color: InputStyle.secondaryFontColor
              font.pixelSize: InputStyle.fontPixelSizeNormal

              elide: Text.ElideMiddle
            }
          }

          Image {
            Layout.rightMargin: InputStyle.listMargins
            Layout.preferredWidth: InputStyle.iconSizeMedium
            Layout.preferredHeight: InputStyle.iconSizeMedium

            source: {
              if ( model.nodeIsVisible === "yes" )
              {
                return InputStyle.eyeIconV2
              }
              else if ( model.nodeIsVisible === "no" )
              {
                return InputStyle.eyeSlashIconV2
              }
              return "" // not a spatial layer
            }

            MouseArea {
              anchors {
                fill: parent // make the click area bigger
                leftMargin: -InputStyle.buttonClickArea
                topMargin: -InputStyle.buttonClickArea
                rightMargin: -InputStyle.buttonClickArea
                bottomMargin: -InputStyle.buttonClickArea
              }

              onClicked: root.nodeVisibilityClicked( model.node )
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
