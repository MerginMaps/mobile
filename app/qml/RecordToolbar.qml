import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.3
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton
import lc 1.0

Item {
    signal addClicked
    signal cancelClicked
    signal gpsSwitchClicked
    signal manualRecordingClicked
    signal stopRecordingClicked
    signal removePointClicked
    signal close

    property int rowHeight: mainPanel.height
    property int itemSize: rowHeight * 0.8
    property color gpsIndicatorColor: InputStyle.softRed
    property bool pointLayerSelected: true
    property bool manualRecordig: false
    property bool recording: false
    property QgsQuick.VectorLayer activeVectorLayer: __layersModel.data(__layersModel.index(comboBox.currentIndex), LayersModel.VectorLayer)

    id: root
    onClose: visible = false

    Rectangle {
        anchors.fill: parent
        color: InputStyle.clrPanelBackground
        opacity: InputStyle.panelOpacity
    }

    Rectangle {
        id: extraPanel
        height: root.rowHeight * 0.8
        width: parent.width
        color: InputStyle.panelBackgroundLight

        RowLayout {
            height: extraPanel.height
            width: parent.width

            Label {
                id: label
                height: extraPanel.height * 0.8
                text: qsTr("Survey Layer")
            }

            ComboBox {
                id: comboBox
                currentIndex: 1
                height: extraPanel.height * 0.8
                model: __layersModel
                textRole: "name"
                enabled: !root.recording
            }

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
                text: qsTr("Undo")
                imageSource: "undo.svg"
                enabled: manualRecordig

                onActivated: root.removePointClicked()
            }
        }

        Item {
            height: parent.height
            Layout.fillWidth: true

            MainPanelButton {
                id: addButton
                width: root.itemSize
                text: qsTr("Add Point")
                imageSource: "plus.svg"
                enabled: manualRecordig

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

                onActivated: root.cancelClicked()
            }
        }
    }

}
