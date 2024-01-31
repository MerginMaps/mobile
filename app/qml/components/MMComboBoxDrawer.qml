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


// TODO: rename to MMDropdownDrawer
Drawer {
  id: root

  property alias title: title.text
  property alias model: listView.model
  property bool multiSelect: false
  property int minFeaturesCountToFullScreenMode: 4
  property var preselectedFeatures: []

  padding: 20 * __dp

  signal featureClicked( var selectedFeatures )

  width: ApplicationWindow.window.width
  height: (mainColumn.height > ApplicationWindow.window.height ? ApplicationWindow.window.height : mainColumn.height) - 20 * __dp
  edge: Qt.BottomEdge

  Rectangle {
    color: roundedRect.color
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    height: 2 * radius
    anchors.topMargin: -radius
    radius: 20 * __dp
  }

  Rectangle {
    id: roundedRect

    anchors.fill: parent
    color: __style.whiteColor

    Column {
      id: mainColumn

      width: parent.width
      spacing: 20 * __dp
      leftPadding: root.padding
      rightPadding: root.padding
      bottomPadding: root.padding

      Row {
        width: parent.width - 2 * root.padding
        anchors.horizontalCenter: parent.horizontalCenter

        Item { width: closeButton.width; height: 1 }

        Text {
          id: title

          anchors.verticalCenter: parent.verticalCenter
          font: __style.t2
          width: parent.width - closeButton.width * 2
          color: __style.forestColor
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          elide: Text.ElideRight
        }

        Image {
          id: closeButton

          source: __style.closeButtonIcon

          MouseArea {
            anchors.fill: parent
            onClicked: root.visible = false
          }
        }
      }

      MMSearchInput {
        id: searchBar

        width: parent.width - 2 * root.padding
        placeholderText: qsTr("Text value")
        bgColor: __style.lightGreenColor
        visible: root.model.count >= root.minFeaturesCountToFullScreenMode

        onSearchTextChanged: function(text) {
          root.model.searchExpression = text
        }
      }

      ListView {
        id: listView

        bottomMargin: primaryButton.visible ? primaryButton.height + 20 * __dp : 0
        width: parent.width - 2 * root.padding
        height: {
          if(root.model.count >= root.minFeaturesCountToFullScreenMode) {
            if(ApplicationWindow.window)
              return ApplicationWindow.window.height - searchBar.height - 100 * __dp
            else return 0
          }
          if(root.model)
            return root.model.count * internal.comboBoxItemHeight
          return 0
        }
        clip: true
        currentIndex: -1

        delegate: Item {
          id: delegate

          property bool checked: root.multiSelect ? root.preselectedFeatures.includes(model.FeatureId) : listView.currentIndex === model.index

          width: listView.width
          height: internal.comboBoxItemHeight

          Rectangle {
            anchors.top: parent.top
            width: parent.width
            height: 1 * __dp
            color: __style.greyColor
            visible: model.index
          }

          Row {
            height: parent.height
            width: parent.width
            spacing: 10 * __dp

            Text {
              width: parent.width - icon.width - parent.spacing
              height: parent.height
              verticalAlignment: Text.AlignVCenter
              text: model.FeatureTitle
              color: __style.nightColor
              font: __style.t3
              elide: Text.ElideRight
            }

            MMIcon {
              id: icon
              height: parent.height
              width: 20 * __dp
              color: __style.forestColor
              source: __style.comboBoxCheckIcon
              visible: delegate.checked
            }
          }

          MouseArea {
            anchors.fill: parent
            onClicked: {
              listView.currentIndex = model.index
              if(root.multiSelect) {
                delegate.checked = !delegate.checked
                delegate.forceActiveFocus()
              }
              else {
                root.featureClicked(model.FeatureId)
                close()
              }
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

      text: qsTr("Confirm selection")
      visible: root.multiSelect

      onClicked: {
        let selectedFeatures = []
        for(let i=0; i<listView.model.count; i++) {
          if(listView.itemAtIndex(i).checked)
            selectedFeatures.push(listView.model.get(i).FeatureId)
        }
        root.featureClicked(selectedFeatures)
        close()
      }
    }
  }

  QtObject {
    id: internal

    property real comboBoxItemHeight: 67 * __dp
  }
}
