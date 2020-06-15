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

    property string title: qsTr("Survey Layer")

    function openPanel() {
        layerPanel.visible = true
    }

    id: layerPanel
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
          layerPanel.close()
        }
      }
    }

    PanelHeader {
      id: header
      height: InputStyle.rowHeightHeader
      width: parent.width
      color: InputStyle.panelBackgroundLight
      rowHeight: InputStyle.rowHeightHeader
      titleText: qsTr("Select Active Layer")
      backTextVisible: false
      onBack: layerPanel.close()
      withBackButton: true
      layer.enabled: true
      layer.effect: Shadow {}
    }

    ListView {
        id: listView
        height: layerPanel.height - header.height
        width: parent.width
        y: header.height
        implicitWidth: parent.width
        implicitHeight: contentHeight
        model: __layersModel
        clip: true
        delegate: delegateItem

        property int cellWidth: width
        property int cellHeight: InputStyle.rowHeight
        property int borderWidth: 1

        Label {
            anchors.fill: parent
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            visible: parent.count == 0
            text: qsTr("No editable layers in the project!")
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
            height: isVector && !isReadOnly && hasGeometry ? listView.cellHeight : 0
            visible: height ? true : false
            anchors.leftMargin: InputStyle.panelMargin
            anchors.rightMargin: InputStyle.panelMargin
            color: item.highlight ? secondaryColor : primaryColor

            MouseArea {
              anchors.fill: parent
              onClicked: {
                __loader.setActiveLayer(index)
                layerPanel.visible = false
              }
            }

            ExtendedMenuItem {
                id: item
                anchors.rightMargin: InputStyle.panelMargin
                anchors.leftMargin: InputStyle.panelMargin
                contentText: name ? name : ""
                imageSource: iconSource ? iconSource : ""
                overlayImage: false
                highlight: __layersModel.activeIndex === index
                showBorder: !__appSettings.defaultLayer || __layersModel.activeIndex - 1 !== index
            }
        }

    }

}
