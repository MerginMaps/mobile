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
  id: control

  property alias title: title.text
  property alias model: listView.model
  property alias dropDownTitle: title.text
  property bool multiSelect: false

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
      leftPadding: control.padding
      rightPadding: control.padding
      bottomPadding: control.padding

      Row {
        width: parent.width - 2 * control.padding
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
            onClicked: control.visible = false
          }
        }
      }

      MMSearchEditor {
        id: searchBar

        width: parent.width - 2 * control.padding
        placeholderText: qsTr("Text value")
        bgColor: __style.lightGreenColor
        visible: control.model.count > 4

        onSearchTextChanged: function(text) {
          control.model.searchExpression = text
        }
      }

      ListView {
        id: listView

        bottomMargin: primaryButton.visible ? primaryButton.height + 20 * __dp : 0
        width: parent.width - 2 * control.padding
        height: {
          if(control.model.count > 4) {
            if(ApplicationWindow.window)
              return ApplicationWindow.window.height - searchBar.height - 100 * __dp
            else return 0
          }
          if(control.model)
            return control.model.count * __style.menuDrawerHeight //+ primaryButton.height
          return 0
        }
        clip: true
        currentIndex: -1

        delegate: Item {
          id: delegate

          property bool checked: root.multiSelect ? false : listView.currentIndex === model.index

          width: listView.width
          height: __style.menuDrawerHeight

          Rectangle {
            anchors.top: parent.top
            width: parent.width
            height: 1 * __dp
            color: __style.grayColor
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
                control.featureClicked(model.FeatureId)
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
      visible: control.multiSelect

      onClicked: {
        let selectedFeatures = []
        for(let i=0; i<listView.model.count; i++) {
          if(listView.itemAtIndex(i).checked)
            selectedFeatures.push(listView.model.get(i).FeatureId)
        }
        control.featureClicked(selectedFeatures)
        close()
      }
    }
  }
}
