import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import lc 1.0
import "."  // import InputStyle singleton

Popup {

    property real rowHeight: InputStyle.rowHeight
    property string defaultLayer: __appSettings.defaultLayer
    property alias gpsAccuracyTolerance: gpsAccuracySpin.value

    signal defaultProjectClicked()
    signal defaultLayerClicked()

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
        titleText: qsTr("Settings")

        onBack: settingsPanel.close()
    }

    Rectangle {
        id: settingsList
        anchors.top: header.bottom
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.margins: 0
        color: InputStyle.panelBackgroundDark
        width: parent.width
        height: parent.height

        Column {
            id: settingListContent
            anchors.fill: parent
            spacing: 1

            // Header "Start app with"
            PanelItem {
                color: InputStyle.panelBackgroundLight
                text: qsTr("Start app with")
                bold: true
            }

            PanelItem {
                color: InputStyle.clrPanelMain
                text: qsTr("Default project") + ": " + (__appSettings.defaultProject ? __appSettings.defaultProjectName : "(none)")

                MouseArea {
                    anchors.fill: parent
                    onClicked: settingsPanel.defaultProjectClicked()
                }
            }

            PanelItem {
                color: InputStyle.clrPanelMain
                text: qsTr("Default survey layer") + ": " + (settingsPanel.defaultLayer ? settingsPanel.defaultLayer : "(none)")

                MouseArea {
                    anchors.fill: parent
                    onClicked: settingsPanel.defaultLayerClicked()
                }
            }

             // Header "GPS SETTINGS"
            PanelItem {
                color: InputStyle.panelBackgroundLight
                text: qsTr("GPS SETTINGS")
                bold: true
            }

            PanelItem {
                height: settingsPanel.rowHeight
                width: parent.width
                color: InputStyle.clrPanelMain
                text: qsTr("Follow GPS with map")

                Switch {
                    anchors.margins: 0
                    padding: 0
                    id: autoCenterMapCheckBox
                    height: InputStyle.fontPixelSizeNormal
                    width: height * 3
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: InputStyle.panelMargin

                    onCheckedChanged: __appSettings.autoCenterMapChecked = checked
                }

                Connections {
                    target: __appSettings
                    onAutoCenterMapCheckedChanged: autoCenterMapCheckBox.checked = __appSettings.autoCenterMapChecked
                }
            }

            PanelItem {
                id: panelItem1
                height: settingsPanel.rowHeight
                width: parent.width
                color: InputStyle.clrPanelMain
                text: qsTr("GPS accuracy")

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
                        color: InputStyle.panelBackgroundLight
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
                text: qsTr("Acceptable GPS accuracy")

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
