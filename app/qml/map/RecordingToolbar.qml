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
import QtQuick.Dialogs
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects

import lc 1.0
import ".."
import "../components"

Item {
    id: root

    signal gpsSwitchClicked
    signal gpsSwithHeld
    signal removeClicked
    signal addClicked
    signal releaseClicked
    signal doneClicked
    signal cancelClicked
    signal undoClicked

    property color gpsIndicatorColor: InputStyle.softRed
    property bool pointLayerSelected: true
    property bool manualRecording: false
    property var recordingMapTool

    property int itemSize: rowHeight * 0.8
    property int rowHeight: InputStyle.rowHeightHeader

    focus: true

    Keys.onReleased: function( event ) {
      if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
        event.accepted = true;
        cancelButton.activated()
      }
    }

    Rectangle {
        anchors.fill: parent
        color: InputStyle.clrPanelBackground
        opacity: InputStyle.panelOpacity

        MouseArea {
          anchors.fill: parent
          onClicked: {} // dont do anything, just do not let click event propagate
        }
    }

    RowLayout {
        height: root.rowHeight
        width: parent.width
        anchors.bottom: parent.bottom

        Item {
            height: parent.height
            Layout.fillWidth: true

            MainPanelButton {
                id: gpsSwitchBtn
                width: root.itemSize
                text: qsTr("GPS")
                imageSource: __appSettings.autoCenterMapChecked ? InputStyle.gpsFixedIcon : InputStyle.gpsNotFixedIcon
                onActivated: root.gpsSwitchClicked()
                onActivatedOnHold: root.gpsSwithHeld()

                RoundIndicator {
                    width: parent.height/4
                    height: width
                    anchors.right: parent.right
                    anchors.top: parent.top
                    color: gpsIndicatorColor
                }
            }
        }

        Item {
            Layout.fillWidth: true
            height: parent.height
            visible: root.pointLayerSelected ? false : true

            MainPanelButton {
                id: undoButton
                width: root.itemSize
                text: qsTr("Undo")
                imageSource: InputStyle.undoIcon

                enabled: root.recordingMapTool.canUndo

                onActivated: root.undoClicked()
            }
        }

        Item {
            Layout.fillWidth: true
            height: parent.height
            visible: root.pointLayerSelected ? false : true

            MainPanelButton {
                id: removeButton
                width: root.itemSize
                text: qsTr("Remove")
                imageSource: InputStyle.minusIcon

                enabled: {
                  if ( !manualRecording ) return false;
                  if ( root.recordingMapTool.state === RecordingMapTool.View ) return false;
                  if ( __inputUtils.isEmptyGeometry( root.recordingMapTool.recordedGeometry ) ) return false;

                  return true;
                }

                onActivated: root.removeClicked()
            }
        }

        Item {
            height: parent.height
            Layout.fillWidth: true
            visible: root.recordingMapTool.state === RecordingMapTool.View || root.recordingMapTool.state === RecordingMapTool.Record

            MainPanelButton {
                id: addButton
                width: root.itemSize
                text: qsTr("Add")
                imageSource: InputStyle.plusIcon
                enabled: manualRecording && root.recordingMapTool.state !== RecordingMapTool.View

                onActivated: root.addClicked()
            }
        }

        Item {
            height: parent.height
            Layout.fillWidth: true
            visible: root.recordingMapTool.state === RecordingMapTool.Grab

            MainPanelButton {
                id: releaseButton
                width: root.itemSize
                text: qsTr("Release")
                imageSource: InputStyle.plusIcon

                onActivated: root.releaseClicked()
            }
        }

        Item {
            Layout.fillWidth: true
            height: parent.height

            MainPanelButton {
                id: finishButton
                width: root.itemSize
                text: qsTr("Done")
                imageSource: InputStyle.checkIcon

                onActivated: root.doneClicked()
            }
        }
    }
}
