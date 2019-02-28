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
    property color gpsIndicatorColor: InputStyle.softRed

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
                width: root.itemSize
                text: qsTr("GPS")
                imageSource: "ic_gps_fixed_48px.svg"
                onActivated: root.gpsSwitchClicked()

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
            width: parent.width/parent.children.length
            height: parent.height

            MainPanelButton {
                id: addButton
                width: root.itemSize
                text: qsTr("Add")
                imageSource: "check.svg"

                onActivated: root.addClicked()
            }
        }

        Item {
            width: parent.width/parent.children.length
            height: parent.height

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
