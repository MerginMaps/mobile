/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtGraphicalEffects 1.0
import lc 1.0
import "." // import InputStyle singleton
import "components"

Item {
  id: root
  property string defaultLayer: __appSettings.defaultLayer
  property color gpsIndicatorColor: InputStyle.softRed
  property real rowHeight: InputStyle.rowHeight

  visible: false

  Keys.onReleased: {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true;
      if (stackview.depth > 1) {
        // hide about or log panel
        stackview.pop(null);
      } else
        root.visible = false;
    }
  }

  StackView {
    id: stackview
    anchors.fill: parent
    initialItem: settingsContentComponent
  }
  Component {
    id: settingsContentComponent
    Page {
      id: settingsPanel
      padding: 0

      ScrollView {
        id: scrollPage
        contentWidth: availableWidth // to only scroll vertically
        height: settingsPanel.height - header.height
        spacing: InputStyle.panelSpacing
        width: settingsPanel.width

        Column {
          id: settingListContent
          anchors.fill: parent
          spacing: 1

          // Header "GPS"
          PanelItem {
            bold: true
            color: InputStyle.panelBackgroundLight
            text: qsTr("GPS")
          }
          PanelItem {
            color: InputStyle.clrPanelMain
            height: root.rowHeight
            text: qsTr("Follow GPS with map")
            width: parent.width

            SettingsSwitch {
              id: autoCenterMapSwitch
              checked: __appSettings.autoCenterMapChecked

              onCheckedChanged: __appSettings.autoCenterMapChecked = checked
            }
            MouseArea {
              anchors.fill: parent

              onClicked: autoCenterMapSwitch.toggle()
            }
          }
          PanelItem {
            color: InputStyle.clrPanelMain
            height: root.rowHeight
            text: qsTr("GPS accuracy")
            width: parent.width

            Row {
              id: widget
              property real indicatorSize: {
                var size = height / 3;
                size % 2 === 0 ? size : size + 1;
              }

              anchors.bottom: parent.bottom
              anchors.bottomMargin: 0
              anchors.right: parent.right
              anchors.rightMargin: InputStyle.panelMargin
              anchors.top: parent.top
              anchors.topMargin: 0
              spacing: InputStyle.panelSpacing
              width: indicatorSize * 4

              RoundIndicator {
                anchors.margins: height / 3
                anchors.verticalCenter: parent.verticalCenter
                color: InputStyle.panelBackgroundLight
                height: width
                visible: false // disabled due no manual GPS on/off support
                width: widget.indicatorSize
              }
              RoundIndicator {
                anchors.verticalCenter: parent.verticalCenter
                color: InputStyle.softRed
                height: width
                isActive: color === root.gpsIndicatorColor
                width: widget.indicatorSize
              }
              RoundIndicator {
                anchors.verticalCenter: parent.verticalCenter
                color: InputStyle.softOrange
                height: width
                isActive: color === root.gpsIndicatorColor
                width: widget.indicatorSize
              }
              RoundIndicator {
                anchors.verticalCenter: parent.verticalCenter
                color: InputStyle.softGreen
                height: width
                isActive: color === root.gpsIndicatorColor
                width: widget.indicatorSize
              }
            }
          }
          PanelItem {
            height: root.rowHeight
            text: qsTr("Accuracy threshold")
            width: parent.width

            NumberSpin {
              anchors.right: parent.right
              anchors.rightMargin: InputStyle.panelMargin
              anchors.verticalCenter: parent.verticalCenter
              height: InputStyle.fontPixelSizeNormal
              rowHeight: parent.height
              suffix: " m"
              value: __appSettings.gpsAccuracyTolerance
              width: height * 6

              onValueChanged: __appSettings.gpsAccuracyTolerance = value
            }
          }
          PanelItem {
            height: settingsPanel.rowHeight
            text: qsTr("Show accuracy warning")
            width: parent.width

            SettingsSwitch {
              id: accuracyWarningSwitch
              checked: __appSettings.gpsAccuracyWarning

              onCheckedChanged: __appSettings.gpsAccuracyWarning = checked
            }
            MouseArea {
              anchors.fill: parent

              onClicked: accuracyWarningSwitch.toggle()
            }
          }

          // Header "Recording"
          PanelItem {
            bold: true
            color: InputStyle.panelBackgroundLight
            text: qsTr("Recording")
          }
          PanelItem {
            height: root.rowHeight
            text: qsTr("Line rec. interval")
            width: parent.width

            NumberSpin {
              id: spinRecordingInterval
              anchors.right: parent.right
              anchors.rightMargin: InputStyle.panelMargin
              anchors.verticalCenter: parent.verticalCenter
              height: InputStyle.fontPixelSizeNormal
              maxValue: 30
              minValue: 1
              rowHeight: parent.height
              suffix: " s"
              value: __appSettings.lineRecordingInterval
              width: height * 6

              onValueChanged: __appSettings.lineRecordingInterval = spinRecordingInterval.value
            }
          }
          PanelItem {
            color: InputStyle.clrPanelMain
            height: root.rowHeight
            text: qsTr("Reuse last value option")
            width: parent.width

            SettingsSwitch {
              id: rememberValuesSwitch
              checked: __appSettings.reuseLastEnteredValues

              onCheckedChanged: __appSettings.reuseLastEnteredValues = checked
            }
            MouseArea {
              anchors.fill: parent

              onClicked: rememberValuesSwitch.toggle()
            }
          }

          // Delimeter
          PanelItem {
            color: InputStyle.panelBackgroundLight
            height: root.rowHeight / 3
            text: ""
          }

          // App info
          PanelItem {
            text: qsTr("About")

            MouseArea {
              anchors.fill: parent

              onClicked: stackview.push(aboutPanelComponent)
            }
          }

          // Help
          PanelItem {
            text: qsTr("Help")

            MouseArea {
              anchors.fill: parent

              onClicked: Qt.openUrlExternally(__inputHelp.helpRootLink)
            }
          }

          // Privacy Policy
          PanelItem {
            text: qsTr("Privacy policy")

            MouseArea {
              anchors.fill: parent

              onClicked: Qt.openUrlExternally(__inputHelp.privacyPolicyLink)
            }
          }

          // Terms of Service
          PanelItem {
            text: qsTr("Mergin terms of service")

            MouseArea {
              anchors.fill: parent

              onClicked: Qt.openUrlExternally(__inputHelp.merginTermsLink)
            }
          }

          // Debug/Logging
          PanelItem {
            text: qsTr("Diagnostic log")

            MouseArea {
              anchors.fill: parent

              onClicked: stackview.push(logPanelComponent, {
                  "text": __inputHelp.fullLog(true, 200000)
                })
            }
          }
        }

        background: Rectangle {
          anchors.fill: parent
          color: InputStyle.panelBackgroundDark
        }
      }

      background: Rectangle {
        anchors.fill: parent
        color: InputStyle.clrPanelMain
      }
      header: PanelHeader {
        id: header
        color: InputStyle.clrPanelMain
        height: root.rowHeight
        rowHeight: InputStyle.rowHeightHeader
        titleText: qsTr("Settings")
        width: parent.width

        onBack: root.visible = false
      }
    }
  }
  Component {
    id: aboutPanelComponent
    AboutPanel {
      Component.onCompleted: forceActiveFocus()
      onClose: stackview.pop(null)
    }
  }
  Component {
    id: logPanelComponent
    LogPanel {
      Component.onCompleted: forceActiveFocus()
      onClose: stackview.pop(null)
    }
  }
}
