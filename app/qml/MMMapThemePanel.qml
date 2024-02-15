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
import "./components"

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
    radius: 20 * __dp
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
      clip: true

      property int cellWidth: width
      property int cellHeight: InputStyle.rowHeight
      property int borderWidth: 1

      TextHyperlink {
        anchors.fill: parent
        visible: parent.count == 0
        text: qsTr("Project has no themes defined. See %1how to setup themes%2.")
        .arg("<a href='"+ __inputHelp.howToSetupThemesLink +"'>")
        .arg("</a>")
        color: __style.nightColor
        font: __style.t3
      }
    }
  }

  Component {
    id: delegateItem

    Rectangle {
      id: itemContainer

      property bool isSelected: __activeProject.mapTheme === model.display
      property bool isOneBeforeSelected: {
        if ( index + 1 >= mapThemesModel.rowCount() ) {
          return false
        }

        const modelindex = mapThemesModel.index( index + 1, 0 )
        const previousThemeName = mapThemesModel.data( modelindex, 0 )

        if ( previousThemeName === __activeProject.mapTheme ) {
          return true
        }

        return false
      }

      width: listView.cellWidth - 2 * root.padding
      height: 67 * __dp

      //property bool isSelected: __activeProject.mapTheme === model.display

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
            height: parent.height
            width: 20 * __dp
            color: __style.forestColor
            source: __style.comboBoxCheckIcon
            visible: itemContainer.isSelected
          }
        }

        MMLine {
          id: grid
          visible: index !== mapThemesModel.rowCount() - 1
        }

      }

      //MMLine {}

      MouseArea {
        anchors.fill: parent
        onClicked: {
          //root.selectedIndex = index
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
}
