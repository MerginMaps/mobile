import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.1
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton

Item {
    signal addClicked
    signal removeClicked
    signal gpsSwitchClicked
    signal close
    property int itemSize: mainPanel.height * 0.8
    property bool manualRecording: true

    id: root

    onClose: visible = false

    Rectangle {
        anchors.fill: parent
        color: InputStyle.clrPanelBackground
        opacity: InputStyle.panelOpacity
    }

    Row {
        height: parent.height
        width: parent.width
        anchors.fill: parent

        Item {
            width: parent.width/parent.children.length
            height: parent.height

            MainPanelButton {
                id: gpsSwitchBtn
                width: mainPanel.itemSize
                text: qsTr("Gps")
                imageSource: root.manualRecording ? "ic_gps_not_fixed_48px.svg" : "ic_gps_fixed_48px.svg"
                onActivated: root.gpsSwitchClicked()
            }
        }

        Item {
            width: parent.width/parent.children.length
            height: parent.height

            MainPanelButton {
                id: addButton
                width: mainPanel.itemSize
                text: qsTr("Add")
                imageSource: "check.svg"

                onActivated: {
                    console.log("TOOLBAR ADD")
                    root.addClicked()
                }
            }
        }

        Item {
            width: parent.width/parent.children.length
            height: parent.height

            MainPanelButton {
                id: cancelButton
                width: mainPanel.itemSize
                text: qsTr("Cancel")
                imageSource: "no.svg"

                onActivated: root.removeClicked()
            }
        }
    }

}
