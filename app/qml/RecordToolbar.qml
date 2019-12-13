import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
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
    signal layerLabelClicked

    property int rowHeight: InputStyle.rowHeightHeader
    property int extraPanelHeight: InputStyle.rowHeightHeader * 0.6
    property int itemSize: rowHeight * 0.8
    property color gpsIndicatorColor: InputStyle.softRed
    property bool pointLayerSelected: true
    property bool manualRecordig: false
    property bool extraPanelVisible: true

    property QgsQuick.VectorLayer activeVectorLayer: (__layersModel.activeIndex >= 0) ?
                                                         __layersModel.data(__layersModel.index(__layersModel.activeIndex), LayersModel.VectorLayer) :
                                                         null
    property string activeLayerName: __layersModel.data(__layersModel.index(__layersModel.activeIndex), LayersModel.Name)
    property string activeLayerIcon: __layersModel.data(__layersModel.index(__layersModel.activeIndex), LayersModel.IconSource)

    id: root
    onClose: visible = false

    Rectangle {
        anchors.fill: parent
        color: InputStyle.clrPanelBackground
        opacity: InputStyle.panelOpacity
    }

    Rectangle {
        id: extraPanel
        height: extraPanelHeight
        width: parent.width
        color: InputStyle.fontColorBright
        visible: extraPanelVisible

        MouseArea {
            anchors.fill: parent
            onClicked: {
                layerLabelClicked()
            }
        }

        Item {
            anchors.centerIn: parent
            height: extraPanelHeight

            Item {
                id: iconContainer
                height: extraPanelHeight
                width: extraPanelHeight
                anchors.right: label.left

                Image {
                    id: icon
                    anchors.fill: parent
                    anchors.margins: extraPanelHeight/4
                    sourceSize.width: width
                    sourceSize.height: height
                    source: root.activeLayerIcon
                    fillMode: Image.PreserveAspectFit
                }

                ColorOverlay {
                    anchors.fill: icon
                    source: icon
                    color: "white"
                }
            }

            Text {
                id: label
                height: extraPanel.height
                text: root.activeLayerName
                color: "white"
                font.bold: true
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignHCenter
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
                imageSource: InputStyle.plusIcon
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
