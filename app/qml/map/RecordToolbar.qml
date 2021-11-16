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
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.14
import QtGraphicalEffects 1.14
import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "../"
import "../components"

Item {
  id: root
  property string activeLayerIcon: __loader.loadIconFromLayer(activeVectorLayer)
  property string activeLayerName: activeVectorLayer ? activeVectorLayer.name : ""
  property QgsQuick.VectorLayer activeVectorLayer: __activeLayer.vectorLayer
  property int extraPanelHeight: InputStyle.rowHeightHeader * 0.6
  property bool extraPanelVisible: true
  property color gpsIndicatorColor: InputStyle.softRed
  property int itemSize: rowHeight * 0.8
  property bool manualRecording: false
  property bool pointLayerSelected: true
  property int rowHeight: InputStyle.rowHeightHeader

  focus: true

  signal addClicked
  signal cancelClicked
  signal close
  signal gpsSwitchClicked
  signal layerLabelClicked
  signal manualRecordingClicked
  signal removePointClicked
  signal stopRecordingClicked

  Keys.onReleased: {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true;
      cancelButton.activated();
    }
  }
  onClose: visible = false

  Rectangle {
    anchors.fill: parent
    color: InputStyle.clrPanelBackground
    opacity: InputStyle.panelOpacity

    MouseArea {
      anchors.fill: parent

      onClicked: {
      } // dont do anything, just do not let click event propagate
    }
  }
  SimpleTextWithIcon {
    id: extraPanel
    color: InputStyle.fontColorBright
    fontColor: "white"
    fontPixelSize: InputStyle.fontPixelSizeSmall
    height: extraPanelHeight
    source: root.activeLayerIcon
    text: root.activeLayerName
    visible: extraPanelVisible
    width: parent.width

    onClicked: layerLabelClicked()
  }
  RowLayout {
    anchors.bottom: parent.bottom
    height: root.rowHeight
    width: parent.width

    Item {
      Layout.fillWidth: true
      height: parent.height

      MainPanelButton {
        id: gpsSwitchBtn
        imageSource: InputStyle.gpsFixedIcon
        text: qsTr("GPS")
        width: root.itemSize

        onActivated: root.gpsSwitchClicked()
        onActivatedOnHold: root.manualRecordingClicked()

        RoundIndicator {
          anchors.right: parent.right
          anchors.top: parent.top
          color: gpsIndicatorColor
          height: width
          width: parent.height / 4
        }
      }
    }
    Item {
      Layout.fillWidth: true
      height: parent.height
      visible: root.pointLayerSelected ? false : true

      MainPanelButton {
        id: removePointButton
        enabled: manualRecording
        imageSource: InputStyle.undoIcon
        text: qsTr("Undo")
        width: root.itemSize

        onActivated: root.removePointClicked()
      }
    }
    Item {
      Layout.fillWidth: true
      height: parent.height

      MainPanelButton {
        id: addButton
        enabled: manualRecording
        imageSource: InputStyle.plusIcon
        text: qsTr("Add Point")
        width: root.itemSize

        onActivated: root.addClicked()
      }
    }
    Item {
      Layout.fillWidth: true
      height: parent.height
      visible: root.pointLayerSelected ? false : true

      MainPanelButton {
        id: finishButton
        imageSource: InputStyle.checkIcon
        text: qsTr("Done")
        width: root.itemSize

        onActivated: root.stopRecordingClicked()
      }
    }
    Item {
      Layout.fillWidth: true
      height: parent.height

      MainPanelButton {
        id: cancelButton
        imageSource: InputStyle.noIcon
        text: qsTr("Cancel")
        width: root.itemSize

        onActivated: root.cancelClicked()
      }
    }
  }
}
