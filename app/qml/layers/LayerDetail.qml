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
import QtQuick.Layouts 1.14

import lc 1.0
import "../components" as MMComponents
import ".."

Page {
  id: root

  signal close()

  property var layerTreeNode: null

  Keys.onReleased: {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true
      root.close()
    }
  }

  header: MMComponents.PanelHeaderV2 {
    width: parent.width
    headerTitle: internal.mapLayer ? internal.mapLayer.name : ""
    onBackClicked: root.close()
    color: InputStyle.panelBackgroundLight
  }


  StackLayout {
    id: stackLayoutContent

    anchors.fill: parent

    Rectangle {
      Layout.fillWidth: true
      Layout.fillHeight: true
      color: InputStyle.panelBackgroundLight
    }

    Loader {
      active: internal.isVectorLayer

      sourceComponent: Item {
        id: featuresListContent

        Rectangle {
          anchors.fill: parent
          color: "yellow"

          Text {
            anchors.centerIn: parent
            text: "FEATURES"
          }
        }
      }
    }

    Item {
      id: layerDetailContent

      ScrollView {
        anchors.fill: parent

        spacing: InputStyle.panelSpacing

        contentWidth: availableWidth // to only scroll vertically

        background: Rectangle {
          anchors.fill: parent
          color: "blue"
        }

        Column {
          anchors.fill: parent

          Rectangle {
            width: parent.width
            height: InputStyle.rowHeight
            color: "blue"
          }
        }
      }
    }
  }

  footer: Rectangle {

    height: InputStyle.toolbarHeight
    color: InputStyle.clrPanelBackground

    RowLayout {
      anchors.fill: parent

      Item {
        id: featuresButtonParent

        Layout.fillWidth: true
        Layout.fillHeight: true

        visible: internal.isVectorLayer

        MMComponents.ToolbarButton {

          text: qsTr( "Features" )
          imageSource: InputStyle.tableV2Icon

          anchors.centerIn: parent

          onClicked: {
            if ( internal.isVectorLayer ) {
              stackLayoutContent.currentIndex = 0
            }
          }
        }
      }

      Item {
        id: layerInfoButtonParent

        Layout.fillWidth: true
        Layout.fillHeight: true

        MMComponents.ToolbarButton {

          text: qsTr( "Layer info" )
          imageSource: InputStyle.fileInfoIcon

          anchors.centerIn: parent

          onClicked: {
            if ( internal.isVectorLayer ) {
              stackLayoutContent.currentIndex = 1
            }
          }
        }
      }
    }
  }

  QtObject {
    id: internal

    property var mapLayer: layerTreeNode ? __inputUtils.node2Layer( root.layerTreeNode ) : null
    property bool isVectorLayer: __inputUtils.isVectorLayer( mapLayer )
  }
}
