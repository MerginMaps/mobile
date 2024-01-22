/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import lc 1.0
import "." // import InputStyle singleton
import "./components"
import "./misc"

Item {
  id: root

  visible: false
  property real rowHeight: InputStyle.rowHeight
  property string defaultLayer: __appSettings.defaultLayer
  property color gpsIndicatorColor: InputStyle.softRed

  function open( subsetting="" )
  {
    // opens settings panel and if subsetting is provided, opens directly that
    root.visible = true

    if ( subsetting === "gps" )
    {
      stackview.push( positionProviderComponent )
    }
  }

  Keys.onReleased: function( event ) {
    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true

      if (stackview.depth > 1) {
        // hide about or log panel
        stackview.pop(null)
      } else
        root.visible = false
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

      background: Rectangle {
        anchors.fill: parent
        color: InputStyle.clrPanelMain
      }

      header: PanelHeader {
        id: header
        height: root.rowHeight
        width: parent.width
        color: InputStyle.clrPanelMain
        rowHeight: InputStyle.rowHeightHeader
        titleText: qsTr("Settings")

        onBack: root.visible = false
      }

      ScrollView {
        id: scrollPage
        width: settingsPanel.width
        height: settingsPanel.height - header.height
        contentWidth: availableWidth // to only scroll vertically
        spacing: InputStyle.panelSpacing

        background: Rectangle {
          anchors.fill: parent
          color: InputStyle.panelBackgroundDark
        }

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
            height: root.rowHeight
            width: parent.width
            color: InputStyle.clrPanelMain
            text: qsTr("Follow GPS with map")

            SettingsSwitch {
              id: autoCenterMapSwitch

              checked: __appSettings.autoCenterMapChecked
              onCheckedChanged: __appSettings.autoCenterMapChecked = checked
            }

            MouseArea {
              anchors.fill: parent

              onClicked: {
                autoCenterMapSwitch.toggle()
              }
            }
          }

          PanelItem {
            height: root.rowHeight
            width: parent.width
            color: InputStyle.clrPanelMain
            text: qsTr("GPS accuracy")

            Row {
              id: widget
              property real indicatorSize: {
                var size = height / 3
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
                anchors.margins: height / 3
                color: InputStyle.panelBackgroundLight
                anchors.verticalCenter: parent.verticalCenter
                visible: false // disabled due no manual GPS on/off support
              }

              RoundIndicator {
                width: widget.indicatorSize
                height: width
                color: InputStyle.softRed
                isActive: color === root.gpsIndicatorColor
                anchors.verticalCenter: parent.verticalCenter
              }

              RoundIndicator {
                width: widget.indicatorSize
                height: width
                color: InputStyle.softOrange
                isActive: color === root.gpsIndicatorColor
                anchors.verticalCenter: parent.verticalCenter
              }

              RoundIndicator {
                width: widget.indicatorSize
                height: width
                color: InputStyle.softGreen
                isActive: color === root.gpsIndicatorColor
                anchors.verticalCenter: parent.verticalCenter
              }
            }
          }

          PanelItem {
            height: root.rowHeight
            width: parent.width
            text: qsTr("Accuracy threshold")

            MouseArea {
              anchors.fill: parent
              onClicked: inputField.getFocus()
            }

            NumberInputField {
              id: inputField

              number: __appSettings.gpsAccuracyTolerance
              onValueChanged: function(value) {
                __appSettings.gpsAccuracyTolerance = value
              }

              suffix: " m"

              height: parent.height
              width: height * 3

              anchors.right: parent.right
              anchors.rightMargin: InputStyle.panelMargin
            }
          }

          PanelItem {
            height: root.rowHeight
            width: parent.width
            color: InputStyle.clrPanelMain
            text: qsTr("Select GPS receiver")

            MouseArea {
              anchors.fill: parent
              onClicked: stackview.push( positionProviderComponent )
            }
          }

          SettingsNumberItem {
            width: parent.width

            title: qsTr("GPS antenna height")
            description: qsTr("Includes pole height and GPS receiver's antenna height")
            suffix: " m"
            value: __appSettings.gpsAntennaHeight

            onSettingChanged: function( value ) {
               __appSettings.gpsAntennaHeight = value
            }
          }

          // Header "Streaming mode"
          SettingsHeaderItem {
            color: InputStyle.panelBackgroundLight
            text: qsTr("Streaming mode")
          }

          SettingsComboBoxItem {
            width: parent.width

            title: qsTr("Interval type")
            description: qsTr("Choose action when to add a new point")
            value: __appSettings.intervalType

            onSettingChanged: function( value ) {
               __appSettings.intervalType = value
            }
          }

          SettingsNumberItem {
            width: parent.width

            title: qsTr("Line rec. interval")
            description: __appSettings.intervalType === StreamingIntervalType.Distance ? qsTr("in meters") : qsTr("in seconds")
            suffix: __appSettings.intervalType === StreamingIntervalType.Distance ? " m" : " s"
            value: __appSettings.lineRecordingInterval

            onSettingChanged: function( value ) {
               __appSettings.lineRecordingInterval = value
            }
          }

          // Header "Recording"
          PanelItem {
            color: InputStyle.panelBackgroundLight
            text: qsTr("Recording")
            bold: true
          }

          PanelItem {
            height: root.rowHeight
            width: parent.width
            color: InputStyle.clrPanelMain
            text: qsTr("Reuse last value option")

            SettingsSwitch {
              id: rememberValuesSwitch

              checked: __appSettings.reuseLastEnteredValues
              onCheckedChanged: __appSettings.reuseLastEnteredValues = checked
            }

            MouseArea {
              anchors.fill: parent

              onClicked: {
                rememberValuesSwitch.toggle()
              }
            }
          }

          PanelItem {
            height: root.rowHeight
            width: parent.width
            color: InputStyle.clrPanelMain
            text: qsTr( "Automatically sync changes" )

            SettingsSwitch {
              id: autosyncSwitch

              checked: __appSettings.autosyncAllowed
              onCheckedChanged: __appSettings.autosyncAllowed = checked
            }

            MouseArea {
              anchors.fill: parent

              onClicked: {
                autosyncSwitch.toggle()
              }
            }
          }

          // Delimeter
          PanelItem {
            color: InputStyle.panelBackgroundLight
            text: ""
            height: root.rowHeight / 3
          }

          // App info
          PanelItem {
            text: qsTr("About")
            MouseArea {
              anchors.fill: parent
              onClicked: stackview.push(aboutPanelComponent)
            }
          }

          // Changelog
          PanelItem {
            text: qsTr("Changelog")
            MouseArea {
              anchors.fill: parent
              onClicked: stackview.push(changelogPanelComponent)
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
            text: qsTr("Terms of service")
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
              onClicked: stackview.push(logPanelComponent, { "text": __inputHelp.fullLog( true ) } )
            }
          }
        }
      }
    }
  }

  Component {
    id: aboutPanelComponent
    AboutPanel {
      onClose: stackview.pop(null)
      Component.onCompleted: forceActiveFocus()
    }
  }

  Component {
    id: changelogPanelComponent
    ChangelogPanel {
      onClose: stackview.pop(null)
      Component.onCompleted: forceActiveFocus()
    }
  }

  Component {
    id: logPanelComponent
    LogPanel {
      onClose: stackview.pop(null)
      Component.onCompleted: forceActiveFocus()
    }
  }

  Component {
    id: positionProviderComponent
    PositionProviderPage {
      onClose: stackview.pop(null)
      stackView: stackview
      Component.onCompleted: forceActiveFocus()
    }
  }
}
