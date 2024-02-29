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
import "../inputs"
import "../components"

Page {
  id: root

  property var selectedLayer: null

  signal close()
  signal featureClicked( var featurePair )
  signal addFeatureClicked( var toLayer )

  header: MMPageHeader {
    width: parent.width
    color: __style.lightGreenColor
    title: root.selectedLayer ? root.selectedLayer.name + " (" + featuresModel.layerFeaturesCount + ")": ""
    onBackClicked: root.close()
  }

  Rectangle {
    anchors.fill: parent
    color: __style.lightGreenColor
  }

  MMSearchInput {
    id: searchbox
    anchors {
      left: parent.left
      leftMargin: __style.pageMargins
      right: parent.right
      rightMargin: __style.pageMargins
      top: parent.top
      topMargin: __style.margin20
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
      topMargin: __style.margin20
      left: parent.left
      leftMargin: __style.pageMargins
      right: parent.right
      rightMargin: __style.pageMargins
      bottom: parent.bottom
    }

    clip: true

    delegate: Item {
      height: __style.row63
      width: ListView.view.width

      ColumnLayout {
        id: delegateContent

        anchors {
          left: parent.left
          leftMargin: __style.margin4
          right: parent.right
          rightMargin: __style.margin4
          top: parent.top
        }

        height: parent.height * 0.9

        spacing: 0

        Text {
          Layout.fillWidth: true

          color: __style.nightColor
          font: __style.t3
          text: model.display?.toString()?.replace(/\n/g, ' ') ?? ''

          elide: Text.ElideMiddle
        }

        Text {
          Layout.fillWidth: true

          text: model.Description + ( model.SearchResult ? ", " + model.SearchResult.replace(/\n/g, ' ') : "" )
          color: __style.nightColor
          font: __style.p6

          elide: Text.ElideMiddle
        }

        MMLine {
          Layout.fillWidth: true
        }
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
    onTriggered: featuresModel.searchExpression = searchbox.text
  }

  BusyIndicator {
    id: busyIndicator
    width: parent.width/8
    height: width
    running: featuresModel.fetchingResults
    visible: running
    anchors.centerIn: parent
  }

  MMButton {
    width: root.width - 2 * __style.pageMargins
    visible: __inputUtils.isNoGeometryLayer( root.selectedLayer )
    anchors {
      left: parent.left
      leftMargin: __style.pageMargins
      right: parent.right
      rightMargin: __style.pageMargins
      bottom: parent.bottom
      bottomMargin: __style.margin20
    }

    text: qsTr("Add feature")
    onClicked: root.addFeatureClicked( root.selectedLayer )
  }

  Component.onCompleted: {
    featuresModel.reloadFeatures()
  }
}
