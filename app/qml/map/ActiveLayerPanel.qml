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
import ".."
import "../components" as Components

Drawer {
    property string activeLayerId: __activeLayer.layerId

    signal activeLayerChangeRequested( var layerId )

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

    Components.PanelHeader {
      id: header
      height: InputStyle.rowHeightHeader
      width: parent.width
      color: InputStyle.panelBackgroundLight
      rowHeight: InputStyle.rowHeightHeader
      titleText: qsTr("Choose Active Layer")
      onBack: layerPanel.close()
      withBackButton: true
      layer.enabled: true
      layer.effect: Components.Shadow {}
    }

    Components.LayerList {
        height: layerPanel.height - header.height
        width: parent.width
        y: header.height
        model: __recordingLayersModel
        noLayersText: qsTr("Could not find any editable layers in the project. See %1how to enable digitizing in your project%2.")
                      .arg("<a href='"+ __inputHelp.howToEnableDigitizingLink +"'>")
                      .arg("</a>")

        activeLayerId: layerPanel.activeLayerId

        cellWidth: width
        cellHeight: InputStyle.rowHeight
        borderWidth: 1

        onListItemClicked: {
          __loader.setActiveLayer( __recordingLayersModel.layerFromLayerId( layerId ) )
          layerPanel.visible = false
        }
    }
}
