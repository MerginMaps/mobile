import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.7
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton

Item {
    signal addClicked
    signal removeClicked
    signal gpsSwitchClicked // TODO rename
    signal manualRecordingClicked
    signal stopRecordingClicked
    signal removePointClicked
    signal close

    // TODO current point index

    property int itemSize: mainPanel.height * 0.8
    property color gpsIndicatorColor: InputStyle.softRed
    property bool pointLayerSelected: true

    id: root
    onClose: visible = false

    onPointLayerSelectedChanged: {
        console.log("pointLayerSelectedpointLayerSelected", pointLayerSelected)
        visible = false
        visible = true
    }

    Rectangle {
        anchors.fill: parent
        color: InputStyle.clrPanelBackground
        opacity: InputStyle.panelOpacity
    }

    RowLayout {
        height: parent.height
        width: parent.width

        Item {
            height: parent.height
            Layout.fillWidth: true

            MainPanelButton {
                id: gpsSwitchBtn
                width: root.itemSize
                text: qsTr("GPS")
                imageSource: "ic_gps_fixed_48px.svg"
                onActivated: root.gpsSwitchClicked()
                onActivatedOnHold: root.manualRecordingClicked()

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
                id: removePointButton
                width: root.itemSize
                text: qsTr("Delete")
                imageSource: "trash.svg"

                onActivated: root.removePointClicked()
            }
        }

        Item {
            height: parent.height
            Layout.fillWidth: true

            MainPanelButton {
                id: addButton
                width: root.itemSize
                text: qsTr("Add")
                imageSource: "check.svg"

                onActivated: root.addClicked()
            }
        }

        Item {
            Layout.fillWidth: true
            height: parent.height
            visible: root.pointLayerSelected ? false : true

            MainPanelButton {
                id: finishButton
                width: root.itemSize
                text: qsTr("Done")
                imageSource: "check.svg"

                onActivated: root.stopRecordingClicked()
            }
        }

        Item {
            height: parent.height
            Layout.fillWidth: true

            MainPanelButton {
                id: cancelButton
                width: root.itemSize
                text: qsTr("Cancel")
                imageSource: "no.svg"

                onActivated: root.removeClicked()
            }
        }
    }

}
