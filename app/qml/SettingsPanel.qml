import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
//import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "."  // import InputStyle singleton

Popup {

    property alias autoCenterMapChecked: autoCenterMapCheckBox.checked
    property real rowHeight: 90 //InputStyle.rowHeight
    property string defaultProject: "<none>"


    id: settingsPanel
    visible: false
    padding: 0
    height: 1136
    width: 640

    background: Rectangle {
        anchors.fill: parent
        color: InputStyle.clrPanelMain
    }

    PanelHeader {
        id: header
        height: settingsPanel.rowHeight
        width: parent.width
        color: InputStyle.clrPanelMain
        rowHeight: InputStyle.rowHeightHeader
        titleText: "Settings"

        onBack: settingsPanel.close()
    }

    Rectangle {
        id: settingsList
        anchors.top: header.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 0
        color: "grey"
        width: parent.width
        height: parent.height

        Column {
            id: settingListContent
            //width: parent.width
            anchors.fill: parent
            spacing: 1

            PanelItem {
                color: InputStyle.clrPanelBackground2
                text: "Start app with"
                bold: true
            }

            PanelItem {
                color: InputStyle.clrPanelMain
                text: settingsPanel.defaultProject
            }

            PanelItem {
                color: InputStyle.clrPanelBackground2
                text: "GPS SETTINGS"
                bold: true
            }

            PanelItem {
                height: settingsPanel.rowHeight
                width: parent.width
                color: InputStyle.clrPanelMain
                text: "Follow gps with map"

                Switch {
                    id: autoCenterMapCheckBox
                    //height: settingsPanel.rowHeight //InputStyle.fontPixelSizeNormal
                    //width: height * 2
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: InputStyle.panelMargin
                }
            }

            PanelItem {
                height: settingsPanel.rowHeight
                width: parent.width
                color: InputStyle.clrPanelMain
                text: "GPS accuracy"

                Item {
                    id: widget
                    property real indicatorSize: height/3
                    height: settingsPanel.rowHeight
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: InputStyle.panelMargin

                    RoundIndicator {
                        width: widget.indicatorSize
                        height: width
                        anchors.margins: height/3
                        //anchors.left: parent.left
                        color: InputStyle.clrPanelBackground2
                    }

                    RoundIndicator {
                        width: widget.indicatorSize
                        height: width
                        color: InputStyle.softRed
                    }

                    RoundIndicator {
                        width: widget.indicatorSize
                        height: width
                        color: InputStyle.softOrange
                    }

                    RoundIndicator {
                        width: widget.indicatorSize
                        height: width
                        color: InputStyle.softGreen
                    }

                }
            }

            PanelItem {
                id: panelItem
                height: settingsPanel.rowHeight
                width: parent.width
                text: "Acceptable gps acuuracy"

                SpinBox {
                    height: settingsPanel.rowHeight //InputStyle.fontPixelSizeNormal
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: InputStyle.panelMargin
                }
            }

        }

//        PanelItem {
//            y: panelItem.height + settingsPanel.rowHeight
//            x: 0

//            width: parent.width
//            height: parent.height - y
//            color: InputStyle.clrPanelMain
//        }

    }
}
