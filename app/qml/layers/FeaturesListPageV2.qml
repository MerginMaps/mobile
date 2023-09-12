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

import lc 1.0
import ".."
import "../components" as MMComponents

Page {
  id: root

  property var selectedLayer: null

  signal close()
  signal featureClicked( var featurePair )
  signal addFeatureClicked( var toLayer )

  header: MMComponents.PanelHeaderV2 {
    width: parent.width
    headerTitle: root.selectedLayer ? root.selectedLayer.name + " (" + featuresModel.layerFeaturesCount + ")": ""
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

    onSearchTextChanged: searchDelay.restart()
  }

  ListView {
    id: listView

    model: FeaturesModel {
      id: featuresModel

      layer: root.selectedLayer
    }

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

    delegate: Item {
      height: InputStyle.rowHeight
      width: ListView.view.width

      ColumnLayout {
        id: delegateContent

        anchors {
          left: parent.left
          leftMargin: InputStyle.listMarginsSmall
          right: parent.right
          rightMargin: InputStyle.listMarginsSmall
          top: parent.top
        }

        height: parent.height * 0.9

        spacing: 0

        Text {
          Layout.fillWidth: true

          color: InputStyle.fontColor
          font.bold: true
          text: model.display.replace(/\n/g, ' ')

          font.pixelSize: InputStyle.fontPixelSizeNormal

          elide: Text.ElideMiddle
        }

        Text {
          Layout.fillWidth: true

          color: InputStyle.secondaryFontColor
          text: model.Description + ( model.SearchResult ? ", " + model.SearchResult.replace(/\n/g, ' ') : "" )

          font.pixelSize: InputStyle.fontPixelSizeSmall

          elide: Text.ElideMiddle
        }
      }

      Rectangle {
        anchors {
          bottom: parent.bottom
          left: parent.left
          right: parent.right
        }

        height: InputStyle.borderSize
        color: InputStyle.panelBackgroundLight
      }

      MouseArea {
        anchors.fill: parent
        onClicked: root.featureClicked( model.FeaturePair )
      }
    }
  }

  Timer {
    id: searchDelay
    interval: 500
    running: false
    repeat: false
    onTriggered: featuresModel.searchExpression = searchbox.searchText()
  }

  BusyIndicator {
    id: busyIndicator
    width: parent.width/8
    height: width
    running: featuresModel.fetchingResults
    visible: running
    anchors.centerIn: parent
  }

  Component {
    id: addFeatureHeaderComponent

    Item {
      height: InputStyle.rowHeightMedium
      width: ListView.view.width

      RowLayout {
        id: content

        anchors.fill: parent

        Image {
          Layout.preferredHeight: InputStyle.iconSizeMedium
          Layout.preferredWidth: InputStyle.iconSizeMedium

          source: InputStyle.plusIconV2
        }

        Text {
          Layout.fillWidth: true

          color: InputStyle.actionColor
          font.bold: true
          text: qsTr( "Add feature" )
          elide: Text.ElideRight
          font.pixelSize: InputStyle.fontPixelSizeNormal
        }
      }

      Rectangle {
        anchors {
          bottom: parent.bottom
          left: parent.left
          right: parent.right
        }

        height: InputStyle.borderSize
        color: InputStyle.panelBackgroundLight
      }

      MouseArea {
        anchors.fill: parent
        onClicked: root.addFeatureClicked( root.selectedLayer )
      }
    }
  }

  Component.onCompleted: {
    featuresModel.reloadFeatures()

    if ( __inputUtils.isNoGeometryLayer( root.selectedLayer ) ) {
      listView.header = addFeatureHeaderComponent
    }
  }
}
