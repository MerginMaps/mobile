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

import lc 1.0
import "."
import "../components"

Drawer {
  id: root

  visible: false
  modal: true
  interactive: false
  dragMargin: 0 // prevents opening the drawer by dragging.

  padding: 20

  width: ApplicationWindow.window.width
  height: ApplicationWindow.window.height / 2
  edge: Qt.BottomEdge

  Item {
    focus: true
    Keys.onReleased: function( event ) {
      if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
        root.close()
      }
    }
  }

  Rectangle {
    color: roundedRect.color
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    height: 2 * radius
    anchors.topMargin: -radius
    radius: __style.pageMargins
  }

  Rectangle {
    id: roundedRect

    anchors.fill: parent
    color: __style.whiteColor

    MMHeader {
      id: header

      rightMarginShift: 0
      backVisible: false

      title: qsTr("Map Themes")
      titleFont: __style.t2

      MMRoundButton {
        id: backBtn

        anchors.right: parent.right
        anchors.rightMargin: __style.pageMargins
        anchors.verticalCenter: parent.verticalCenter

        iconSource: __style.closeIcon
        iconColor: __style.forestColor

        bgndColor: __style.lightGreenColor
        bgndHoverColor: __style.mediumGreenColor

        onClicked: root.visible = false
      }
    }

    ListView {
      id: listView

      height: root.height - header.height - 40 * __dp
      width: parent.width
      y: header.height + 40 * __dp
      implicitWidth: parent.width
      implicitHeight: contentHeight

      model: MapThemesModel {
        id: mapThemesModel
        qgsProject: __activeProject.qgsProject
      }

      delegate: delegateItem

      // No themes message + link
      Rectangle {
        id: noMapThemesRectangle
        anchors.fill: parent
        color: __style.whiteColor
        visible: parent.count === 0

        Column {
          width: parent.width
          spacing: __style.pageMargins
          leftPadding: __style.pageMargins
          rightPadding: __style.pageMargins
          bottomPadding: __style.pageMargins
          anchors.horizontalCenter: parent.verticalCenter

          Image {
            source: __style.noMapThemesImage
            anchors.horizontalCenter: parent.horizontalCenter
          }

          Text {
            text: qsTr("There are currently no map themes")
            elide: Text.ElideMiddle
            anchors.horizontalCenter: parent.horizontalCenter
            font: __style.t1
            width: parent.width - 2*__style.pageMargins
            color: __style.forestColor
            horizontalAlignment: Text.AlignHCenter
          }

          Text {
            elide: Text.ElideMiddle
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width
            color: __style.nightColor
            textFormat: Text.RichText
            wrapMode: Text.WordWrap
            font: __style.t3
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            linkColor: __style.forestColor
            text: qsTr("Learn more about <a href='%1' style='color: %2;'>how to setup themes</a>.")
            .arg(__inputHelp.howToSetupThemesLink)
            .arg(__style.forestColor)

            onLinkActivated: function(link) {
              Qt.openUrlExternally(link)
            }
          }
        }
      }
    }
  }

  Component {
    id: delegateItem

    Rectangle {
      id: itemContainer

      property bool isSelected: __activeProject.mapTheme === model.display

      width: listView.width - 2 * root.padding
      height: internal.comboBoxItemHeight

      x: root.padding

      Column {
        height: parent.height
        width: parent.width

        Row {
          id: itemRow

          height: parent.height - grid.height
          width: parent.width
          spacing: 10 * __dp

          Text {
            width: parent.width - icon.width - parent.spacing
            height: parent.height
            verticalAlignment: Text.AlignVCenter
            text: model.display
            color: __style.nightColor
            font: __style.t3
            elide: Text.ElideRight
          }

          MMIcon {
            id: icon
            source: __style.doneCircleIcon
            visible: itemContainer.isSelected
            anchors.verticalCenter: parent.verticalCenter
          }
        }

        MMLine {
          id: grid
          visible: index !== mapThemesModel.rowCount() - 1
        }
      }

      MouseArea {
        anchors.fill: parent
        onClicked: {
          __activeProject.mapTheme = model.display
          root.close()
        }
      }
    }
  }

  Connections {
    target: __activeProject

    function onProjectWillBeReloaded() {
      mapThemesModel.reset()
    }

    function onProjectReloaded( qgsProject ) {
      mapThemesModel.qgsProject = __activeProject.qgsProject
    }
  }

  QtObject {
    id: internal

    property real comboBoxItemHeight: 67 * __dp
  }
}
