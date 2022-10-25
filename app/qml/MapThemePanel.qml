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

import lc 1.0
import "."  // import InputStyle singleton
import "./components"

Drawer {
    id: root

    visible: false
    modal: true
    interactive: false
    dragMargin: 0 // prevents opening the drawer by dragging.

    background: Rectangle {
        color: InputStyle.clrPanelMain
    }

    Item {
      focus: true
      Keys.onReleased: {
        if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
          root.close()
        }
      }
    }

    PanelHeader {
          id: header
          height: InputStyle.rowHeightHeader
          width: parent.width
          color: InputStyle.panelBackgroundLight
          rowHeight: InputStyle.rowHeightHeader
          titleText: qsTr("Map Themes")
          onBack: root.close()
          withBackButton: true
          layer.enabled: true
          layer.effect: Shadow {}
    }

    ListView {
        id: listView

        height: root.height - header.height
        width: parent.width
        y: header.height
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

            width: listView.cellWidth
            height: listView.cellHeight
            anchors.leftMargin: InputStyle.panelMargin
            anchors.rightMargin: InputStyle.panelMargin
            color: item.highlight ? InputStyle.panelItemHighlight : InputStyle.clrPanelMain

            MouseArea {
                anchors.fill: parent
                onClicked: {
                  __activeProject.mapTheme = model.display
                  root.close()
                }
            }

            ExtendedMenuItem {
                id: item
                panelMargin: InputStyle.panelMargin
                contentText: model.display
                imageSource: InputStyle.mapThemesIcon
                anchors.rightMargin: panelMargin
                anchors.leftMargin: panelMargin
                highlight: itemContainer.isSelected
                // Do not show border line for selected item and one before selected
                showBorder: !itemContainer.isSelected && !itemContainer.isOneBeforeSelected
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
