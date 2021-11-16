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
import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "../"
import "../components" as Components

Drawer {
  id: layerPanel
  property string activeLayerId: __activeLayer.layerId

  dragMargin: 0 // prevents opening the drawer by dragging.
  interactive: false
  modal: true
  visible: false

  signal activeLayerChangeRequested(var layerId)
  function openPanel() {
    layerPanel.visible = true;
  }

  Item {
    focus: true

    Keys.onReleased: {
      if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
        layerPanel.close();
      }
    }
  }
  Components.PanelHeader {
    id: header
    color: InputStyle.panelBackgroundLight
    height: InputStyle.rowHeightHeader
    layer.enabled: true
    rowHeight: InputStyle.rowHeightHeader
    titleText: qsTr("Choose Active Layer")
    width: parent.width
    withBackButton: true

    onBack: layerPanel.close()

    layer.effect: Components.Shadow {
    }
  }
  Components.LayerList {
    activeLayerId: layerPanel.activeLayerId
    borderWidth: 1
    cellHeight: InputStyle.rowHeight
    cellWidth: width
    height: layerPanel.height - header.height
    model: __recordingLayersModel
    noLayersText: qsTr("Could not find any editable layers in the project. See %1how to enable digitizing in your project%2.").arg("<a href='" + __inputHelp.howToEnableDigitizingLink + "'>").arg("</a>")
    width: parent.width
    y: header.height

    onListItemClicked: {
      __loader.setActiveLayer(__recordingLayersModel.layerFromLayerId(layerId));
      layerPanel.visible = false;
    }
  }

  background: Rectangle {
    color: InputStyle.clrPanelMain
  }
}
