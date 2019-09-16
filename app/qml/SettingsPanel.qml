import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import lc 1.0
import "."  // import InputStyle singleton

Popup {

    property real rowHeight: InputStyle.rowHeight
    property string defaultLayer: __appSettings.defaultLayer
    property color gpsIndicatorColor: InputStyle.softRed

    id: settingsPanel
    visible: false
    padding: 0

    background: Rectangle {
        anchors.fill: parent
        color: InputStyle.clrPanelMain
    }

    onAboutToHide: {
        if (aboutPanel.visible) {
            aboutPanel.visible = false
        }
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

             // Header "GPS"
            PanelItem {
                color: InputStyle.panelBackgroundLight
                text: qsTr("GPS")
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
                    width: height * 2
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.right: parent.right
                    anchors.rightMargin: InputStyle.panelMargin
                    checked: __appSettings.autoCenterMapChecked
                    onCheckedChanged: __appSettings.autoCenterMapChecked = checked

                    property color highlighColor: InputStyle.softGreen
                    property color disabledColor: InputStyle.panelBackgroundDark

                    indicator: Rectangle {
                        implicitWidth: parent.width
                        implicitHeight: parent.height
                        x: autoCenterMapCheckBox.leftPadding
                        y: parent.height / 2 - height / 2
                        radius: parent.height/2
                        color: autoCenterMapCheckBox.checked ? InputStyle.softGreen : "#ffffff"
                        border.color: autoCenterMapCheckBox.checked ? InputStyle.softGreen : autoCenterMapCheckBox.disabledColor

                        Rectangle {
                            x: autoCenterMapCheckBox.checked ? parent.width - width : 0
                            width: parent.height
                            height: parent.height
                            radius: parent.height/2
                            color: "#ffffff"
                            border.color: autoCenterMapCheckBox.checked ? InputStyle.softGreen : autoCenterMapCheckBox.disabledColor
                        }
                    }
                }
            }

            PanelItem {
                height: settingsPanel.rowHeight
                width: parent.width
                color: InputStyle.clrPanelMain
                text: qsTr("GPS accuracy")

                Row {
                    id: widget
                    property real indicatorSize: {
                        var size = height/3
                        size % 2 === 0 ? size : size + 1
                    }
                    width: indicatorSize * 4
                    anchors.top: parent.top
                    anchors.topMargin: 0
                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: 0
                    anchors.right: parent.right
                    anchors.rightMargin: InputStyle.panelMargin
                    spacing: InputStyle.panelSpacing

                    RoundIndicator {
                        width: widget.indicatorSize
                        height: width
                        anchors.margins: height/3
                        color: InputStyle.panelBackgroundLight
                        anchors.verticalCenter: parent.verticalCenter
                        visible: false // disabled due no manual GPS on/off support
                    }

                    RoundIndicator {
                        width: widget.indicatorSize
                        height: width
                        color: InputStyle.softRed
                        isActive: color === gpsIndicatorColor
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    RoundIndicator {
                        width: widget.indicatorSize
                        height: width
                        color: InputStyle.softOrange
                        isActive: color === gpsIndicatorColor
                        anchors.verticalCenter: parent.verticalCenter
                    }

                    RoundIndicator {
                        width: widget.indicatorSize
                        height: width
                        color: InputStyle.softGreen
                        isActive: color === gpsIndicatorColor
                        anchors.verticalCenter: parent.verticalCenter
                    }
                }
            }

            PanelItem {
                height: settingsPanel.rowHeight
                width: parent.width
                text: qsTr("Accuracy threshold")

                NumberSpin {
                    value: __appSettings.gpsAccuracyTolerance
                    suffix: " m"
                    onValueChanged: __appSettings.gpsAccuracyTolerance = value
                    height: InputStyle.fontPixelSizeNormal
                    anchors.verticalCenter: parent.verticalCenter
                    width: height * 6
                    anchors.right: parent.right
                    anchors.rightMargin: InputStyle.panelMargin
                }
            }

            PanelItem {
                height: settingsPanel.rowHeight
                width: parent.width
                text: qsTr("Line rec. interval")

                NumberSpin {
                    id: spinRecordingInterval
                    value: __appSettings.lineRecordingInterval
                    minValue: 1
                    maxValue: 30
                    suffix: " s"
                    onValueChanged: __appSettings.lineRecordingInterval = spinRecordingInterval.value
                    height: InputStyle.fontPixelSizeNormal
                    anchors.verticalCenter: parent.verticalCenter
                    width: height * 6
                    anchors.right: parent.right
                    anchors.rightMargin: InputStyle.panelMargin
                }
            }

            // Delimeter
            PanelItem {
                color: InputStyle.panelBackgroundLight
                text: ""
                height: settingsPanel.rowHeight/3
            }

            // App info
           PanelItem {
               text: qsTr("About")
               MouseArea {
                   anchors.fill: parent
                   onClicked: aboutPanel.visible = true
               }
           }
        }

    }

    AboutPanel {
        id: aboutPanel
        anchors.fill: parent
        visible: false
    }
}
