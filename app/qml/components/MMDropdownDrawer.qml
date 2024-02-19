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

  property alias title: title.text
  property alias model: listView.model

  property bool withSearchbar: true

  property bool multiSelect: false
  property var selectedFeatures: [] // in/out property, contains list of selected feature ids
  property int minFeaturesCountToFullScreenMode: 6

  required property string valueRole
  required property string textRole

  padding: 20 * __dp

  signal selectionFinished( var selectedFeatures )

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

        MMRoundButton {
          id: closeButton

          iconSource: __style.xIcon
          onClicked: root.close()
        }
      }

      MMSearchInput {
        id: searchBar

        width: parent.width - 2 * root.padding
        placeholderText: qsTr("Search")
        bgColor: __style.lightGreenColor
        visible: root.withSearchbar

        onSearchTextChanged: function(text) {

          // Listview height is derived from the number of items in the model.
          // We intenionally break the binding here in order to stop evaluating height -
          // the drawer would jump places otherwise
          listView.height = listView.height

          root.model.searchExpression = text
        }
      }

      ListView {
        id: listView

        bottomMargin: primaryButton.visible ? primaryButton.height + 20 * __dp : 0
        width: parent.width - 2 * root.padding
        height: {
          if ( root.model.count >= root.minFeaturesCountToFullScreenMode ) {
            if ( ApplicationWindow.window )
              return ApplicationWindow.window.height - searchBar.height - 100 * __dp
            else return 0
          }
          if ( root.model )
            return root.model.count * internal.comboBoxItemHeight
          return 0
        }
        clip: true

        delegate: Item {
          id: delegate

          property bool checked: root.selectedFeatures.includes( root.valueRole === "value" ? model.value : model.FeatureId )

          width: listView.width
          height: internal.comboBoxItemHeight

          Rectangle {
            anchors.top: parent.top
            width: parent.width
            height: 1
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
              text: root.textRole === "text" ? model.text : model.FeatureTitle
              color: __style.nightColor
              font: __style.t3
              elide: Text.ElideRight
            }

            MMIcon {
              anchors.verticalCenter: parent.verticalCenter
              id: icon
              size: __style.icon24
              color: __style.forestColor
              source: __style.doneCircleIcon
              visible: delegate.checked
            }
          }

          MouseArea {
            anchors.fill: parent
            onClicked: {
              if ( root.multiSelect ) {
                delegate.checked = !delegate.checked

                // add or remove the item from the selected features list
                addOrRemoveFeature( root.valueRole === "value" ? model.value : model.FeatureId )
              }
              else {
                root.selectionFinished( [root.valueRole === "value" ? model.value : model.FeatureId] )
                root.close()
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
        root.selectionFinished( root.selectedFeatures )
        close()
      }
    }
  }

  function addOrRemoveFeature( fid )
  {
    if ( root.selectedFeatures.indexOf( fid ) === -1 )
    {
      root.selectedFeatures.push( fid )
    }
    else
    {
      root.selectedFeatures = root.selectedFeatures.filter( function (_id) { return _id !== fid } )
    }
  }

  QtObject {
    id: internal

    property real comboBoxItemHeight: 67 * __dp
  }
}
