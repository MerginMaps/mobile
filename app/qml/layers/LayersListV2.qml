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
  property var parentModelIndex: null

  signal layerClicked( var layerindex, bool isGroup )

  QtObject {
    id: internal

    property int indexColumn: root.parentModelIndex ? __inputUtils.colFromIndex( root.parentModelIndex ) : -1
  }

  ListView {
    id: layerslist

    anchors.fill: parent

    model: DelegateModel {
      id: delegatemodel

      model: root.model

      delegate: Item {
        id: layerrow

        width: ListView.view.width
        height: InputStyle.rowHeightMedium

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

          onClicked: root.layerClicked( root.model.index( index, internal.indexColumn ), model.whatsThis )
        }

        RowLayout {
          id: layerRow

          anchors {
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
          }

//          Image {
//            source: "image://LayerTreePixmapProvider/" + index + "/" + internal.indexColumn
//          }

          Rectangle {
            // temp placeholder in place of icon

            Layout.preferredWidth: 20 * __dp
            Layout.preferredHeight: 20 * __dp

            radius: index % 3 === 0 ? InputStyle.circleRadius : 0

            color: ["blue", "red", "orange"][index % 3]
          }

          Text {
            text: model.display

            Layout.fillWidth: true

            color: InputStyle.fontColor

            font.bold: true
            font.pixelSize: InputStyle.fontPixelSizeBig

            elide: Text.ElideMiddle
          }

          Image {
            Layout.rightMargin: InputStyle.listMargins
            Layout.preferredWidth: InputStyle.iconSizeMedium
            Layout.preferredHeight: InputStyle.iconSizeMedium

            source: index % 2 === 0 ? InputStyle.eyeIconV2 : InputStyle.eyeSlashIconV2

            MouseArea {
              anchors {
                fill: parent // make the click area bigger
                leftMargin: -InputStyle.buttonClickArea
                topMargin: -InputStyle.buttonClickArea
                rightMargin: -InputStyle.buttonClickArea
                bottomMargin: -InputStyle.buttonClickArea
              }

              onClicked: console.log( "Change visibility of", index )
            }
          }
        }
      }
    }
  }

  Component.onCompleted: {
    if ( root.parentModelIndex )
    {
      delegatemodel.rootIndex = root.parentModelIndex
    }
  }
}
