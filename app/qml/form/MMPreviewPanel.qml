/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import "../components" as Components
import "./components" as FormComponents
import lc 1.0

Item {
  id: root

  property bool layerIsReadOnly: false
  property AttributePreviewController controller

  signal contentClicked()
  signal editClicked()
  signal stakeoutClicked( var feature )

  MouseArea {
    anchors.fill: parent
    onClicked: function( mouse ) {
      mouse.accepted = true
      root.contentClicked()
    }
  }

  // TODO: this needs to be revisited, the layout does not work very well

  Item {
    x: parent.width / 2 - width / 2
    width: parent.width > __style.maxPageWidth ? __style.maxPageWidth : parent.width
    height: parent.height

    ColumnLayout {
      id: layout

      anchors {
        fill: parent
        leftMargin: __style.pageMargins
        topMargin: __style.pageMargins
        rightMargin: __style.pageMargins
        bottomMargin: __style.pageMargins
      }

      spacing: 20 * __dp

      Item {
        id: photoContainer

        Layout.fillWidth: true
        Layout.preferredHeight: 160 * __dp

        visible: root.controller.type === AttributePreviewController.Photo

        Components.MMPhoto {
          width: parent.width
          height: parent.height

          photoUrl: root.controller.photo

          fillMode: Image.PreserveAspectCrop
        }
      }

      Text {
        Layout.fillWidth: true
        Layout.preferredHeight: paintedHeight

        text: root.controller.title

        font: __style.t1
        color: __style.forestColor

        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter

        maximumLineCount: 2
        elide: Text.ElideRight
        wrapMode: Text.WordWrap
      }

      Text {
        Layout.fillWidth: true
        Layout.fillHeight: true

        text: controller.html
        visible: root.controller.type === AttributePreviewController.HTML

        clip: true
      }

      Text {
        Layout.fillWidth: true
        Layout.fillHeight: true

        text: qsTr("No map tip available.")
        visible: root.controller.type === AttributePreviewController.Empty

        font: __style.p6
        color: __style.nightColor
      }

      Item {
        id:  fieldsContainer

        Layout.fillWidth: true
        Layout.fillHeight: true

        visible: root.controller.type === AttributePreviewController.Fields

        ListView {
          anchors.fill: parent

          spacing: 20 * __dp
          interactive: false

          model: root.controller.fieldModel

          delegate: Item {
            width: ListView.view.width
            height: childrenRect.height

            Column {
              width: parent.width
              spacing: 0

              Text {
                width: parent.width

                text: model.Name
                font: __style.p6
                color: __style.nightColor

                elide: Text.ElideRight
                wrapMode: Text.NoWrap
              }

              Text {
                width: parent.width

                text: model.Value
                font: __style.p5
                color: __style.nightColor

                elide: Text.ElideRight
                wrapMode: Text.NoWrap
              }
            }
          }
        }
      }

      Item {
        // Vertical spacer to keep action buttons on the bottom with photo type
        Layout.fillWidth: true
        Layout.fillHeight: true

        visible: root.controller.type === AttributePreviewController.Photo
      }

      ScrollView {

        Layout.fillWidth: true
        Layout.preferredHeight: 40 * __dp

        ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
        ScrollBar.vertical.policy: ScrollBar.AlwaysOff

        Row {
          height: parent.height
          spacing: 12 * __dp

          FormComponents.MMPreviewPanelActionButton {
            height: parent.height

            visible: !root.layerIsReadOnly

            buttonText: qsTr( "Edit" )
            iconSource: __style.editIcon

            onClicked: root.editClicked()
          }

          FormComponents.MMPreviewPanelActionButton {
            height: parent.height

            visible: __inputUtils.isPointLayerFeature( controller.featureLayerPair )

            buttonText: qsTr( "Stake out" )
            iconSource: __style.positionTrackingIcon // TODO: change to stakeout icon

            onClicked: root.stakeoutClicked( controller.featureLayerPair )
          }
        }
      }
    }
  }
}
