/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.7
import QtQuick.Controls 2.2

import lc 1.0
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton

Drawer {

    id: mapThemePanel
    visible: false
    modal: true
    interactive: false
    dragMargin: 0 // prevents opening the drawer by dragging.

    background: Rectangle {
        color: InputStyle.clrPanelMain
    }
    PanelHeader {
          id: header
          height: InputStyle.rowHeightHeader
          width: parent.width
          color: InputStyle.panelBackgroundLight
          rowHeight: InputStyle.rowHeightHeader
          titleText: qsTr("Map Themes")
          backTextVisible: false
          onBack: mapThemePanel.close()
          withBackButton: true
          layer.enabled: true
          layer.effect: Shadow {}
    }

    ListView {
        id: listView
        height: mapThemePanel.height - header.height
        width: parent.width
        y: header.height
        implicitWidth: parent.width
        implicitHeight: contentHeight
        model: __mapThemesModel
        delegate: delegateItem
        clip: true

        property int cellWidth: width
        property int cellHeight: InputStyle.rowHeight
        property int borderWidth: 1

        Label {
            anchors.fill: parent
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            visible: parent.count == 0
            text: qsTr("No themes in the project!")
            color: InputStyle.fontColor
            font.pixelSize: InputStyle.fontPixelSizeNormal
            font.bold: true
        }

    }

    Component {
        id: delegateItem
        Rectangle {
            id: itemContainer
            property color primaryColor: InputStyle.clrPanelMain
            property color secondaryColor: InputStyle.fontColorBright
            width: listView.cellWidth
            height: listView.cellHeight
            anchors.leftMargin: InputStyle.panelMargin
            anchors.rightMargin: InputStyle.panelMargin
            color: item.highlight ? secondaryColor : primaryColor

            MouseArea {
                anchors.fill: parent
                onClicked: {
                  __loader.setActiveMapTheme(index)
                  mapThemePanel.close()
                }
            }

            ExtendedMenuItem {
                id: item
                panelMargin: InputStyle.panelMargin
                contentText: name
                imageSource: "map_styles.svg"
                anchors.rightMargin: panelMargin
                anchors.leftMargin: panelMargin
                highlight: __mapThemesModel.activeThemeIndex === index
                showBorder: __mapThemesModel.activeThemeIndex - 1 !== index
            }
        }

    }

}
