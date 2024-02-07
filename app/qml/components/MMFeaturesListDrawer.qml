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
import QtQuick.Controls.Basic
import "../inputs"

Drawer {
  id: root

  property alias title: header.title
  property alias model: listView.model
  property bool withSearch: false

  padding: 20 * __dp

  signal featureClicked( var selectedFeatures )
  signal createLinkedFeature()

  width: ApplicationWindow.window.width
  height: ApplicationWindow.window.height
  edge: Qt.BottomEdge

  background: Rectangle {
    color: __style.lightGreenColor
  }

  Rectangle {
    color: __style.lightGreenColor
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    height: 20 * __dp
    anchors.topMargin: -height
  }

  MMHeader {
    id: header
    onBackClicked: root.close()
  }

  Rectangle {
    anchors {
      top: header.bottom
      left: parent.left
      right: parent.right
      bottom: parent.bottom
    }

    color: __style.lightGreenColor

    Column {
      id: mainColumn

      width: parent.width
      spacing: 20 * __dp
      leftPadding: root.padding
      rightPadding: root.padding
      bottomPadding: root.padding

      MMSearchInput {
        id: searchBar

        width: parent.width - 2 * root.padding
        placeholderText: qsTr("Search for features...")
        visible: root.withSearch

        onSearchTextChanged: function(text) {
          root.model.searchExpression = text
        }
      }

      Item {
        width: 1
        height: 10 * __dp
        visible: !searchBar.visible
      }

      ListView {
        id: listView

        bottomMargin: primaryButton.visible ? primaryButton.height + 20 * __dp : 0
        width: parent.width - 2 * root.padding
        height: ApplicationWindow.window ? ApplicationWindow.window.height - searchBar.height - 100 * __dp : 0
        clip: true

        delegate: Item {
          id: delegate

          width: listView.width
          height: 59 * __dp

          Rectangle {
            anchors.top: parent.bottom
            width: parent.width
            height: 1
            color: __style.greyColor
          }

          Column {
            y: 10 * __dp
            height: parent.height
            width: parent.width
            spacing: 6 * __dp

            Text {
              width: parent.width
              text: model.FeatureTitle
              color: __style.nightColor
              font: __style.t3
              elide: Text.ElideRight
            }

            Text {
              width: parent.width
              text: qsTr("Feature ID ") + model.FeatureId
              color: __style.nightColor
              font: __style.p6
            }
          }

          MouseArea {
            anchors.fill: parent
            onClicked: {
              root.featureClicked( model.FeaturePair )
            }
          }
        }
      }
    }

    MMButton {
      id: primaryButton

      width: parent.width - 2 * 20 * __dp
      anchors.horizontalCenter: parent.horizontalCenter
      anchors.bottom: parent.bottom
      anchors.bottomMargin: 20 * __dp

      text: qsTr("Add feature")

      onClicked: {
        root.createLinkedFeature()
      }
    }
  }
}
