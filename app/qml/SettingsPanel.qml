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
    property alias gpsAccuracyTolerance: gpsAccuracySpin.value


    id: settingsPanel
    visible: false
    padding: 0

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
                text: "Follow GPS with map"

                Switch {
                    anchors.margins: 0
                    padding: 0
                    id: autoCenterMapCheckBox
                    height: InputStyle.fontPixelSizeNormal
                    width: height * 3
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: InputStyle.panelMargin
                }
            }

            PanelItem {
                id: panelItem1
                height: settingsPanel.rowHeight
                width: parent.width
                color: InputStyle.clrPanelMain
                text: "GPS accuracy"

                Row {
                    id: widget
                    property real indicatorSize: height/3
                    width: indicatorSize * 4
                    anchors.top: parent.top
                    anchors.topMargin: 0
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 0
                    anchors.right: parent.right
                    anchors.rightMargin: InputStyle.panelMargin

                    RoundIndicator {
                        width: widget.indicatorSize
                        height: width
                        anchors.margins: height/3
                        color: InputStyle.clrPanelBackground2
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    RoundIndicator {
                        width: widget.indicatorSize
                        height: width
                        color: InputStyle.softRed
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    RoundIndicator {
                        width: widget.indicatorSize
                        height: width
                        color: InputStyle.softOrange
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    RoundIndicator {
                        width: widget.indicatorSize
                        height: width
                        color: InputStyle.softGreen
                        anchors.verticalCenter: parent.verticalCenter
                    }

                }
            }

            PanelItem {
                id: panelItem
                height: settingsPanel.rowHeight
                width: parent.width
                text: "Acceptable GPS accuracy"

                SpinBox {
                    id: gpsAccuracySpin
                    height: settingsPanel.rowHeight/3
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: InputStyle.panelMargin
                }
            }

        }

    }
}
