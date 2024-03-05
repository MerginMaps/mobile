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

import mm 1.0 as MM

import "./components"
import "../components"

Page {
  id: root

  signal close
  signal manageGpsClicked
  signal aboutClicked()
  signal changelogClicked()
  signal helpClicked()
  signal privacyPolicyClicked()
  signal termsOfServiceClicked()
  signal diagnosticLogClicked()

  padding: 0

  background: Rectangle {
    color: __style.lightGreenColor
  }

  header: MMPageHeader {
    id: header
    titleFont: __style.t3

    onBackClicked: root.close()
    backVisible: true
  }

  ScrollView {
    id: scrollPage
    width: root.width - 2 * __style.pageMargins
    height: root.height - header.height
    contentWidth: availableWidth // to only scroll vertically
    anchors.horizontalCenter: parent.horizontalCenter
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
    ScrollBar.vertical.policy: ScrollBar.AlwaysOff

    background: Rectangle {
      anchors.fill: parent
      color: __style.lightGreenColor
    }

    Column {
      id: settingListContent
      anchors.fill: parent
      spacing: 20 * __dp

      Item { width: 1; height: 1 }

      Text {
        text: qsTr("GPS")
        wrapMode: Text.WordWrap
        width: parent.width
        font: __style.h3
        color: __style.forestColor
      }

      Item { width: 1; height: 1 }

      MMSettingSwitch {
        width: parent.width
        title: qsTr("Follow GPS with map")
        description: qsTr("Determines whether the map automatically centers to your GPS position")
        checked: __appSettings.autoCenterMapChecked
        onClicked: __appSettings.autoCenterMapChecked = !checked
      }

      MMLine {}

      Item { width: 1; height: 1 }

      MMSettingInput {
        width: parent.width
        title: qsTr("GPS accuracy treshold")
        description: qsTr("Determines when the accuracy indicator turns yellow")
        valueDescription: qsTr("GPS accuracy treshold, in meters")
        value: __appSettings.gpsAccuracyTolerance
        suffix: " m"

        onValueWasChanged: function( newValue ) {
          __appSettings.gpsAccuracyTolerance = newValue
        }
      }

      MMLine {}

      MMSettingItem {
        width: parent.width
        title: qsTr("Manage GPS receivers")
        value: "Internal"

        onClicked: root.manageGpsClicked()
      }

      MMLine {}

      MMSettingInput {
        width: parent.width
        title: qsTr("GPS antenna height")
        description: qsTr("Includes pole height and GPS receiver’s antenna height")
        valueDescription: qsTr("GPS antenna height, in meters")
        value: __appSettings.gpsAntennaHeight
        suffix: " m"

        onValueWasChanged: function( newValue ) {
          __appSettings.gpsAntennaHeight = newValue
        }
      }

      Item { width: 1; height: 1 }

      Text {
        text: qsTr("Streaming mode")
        wrapMode: Text.WordWrap
        width: parent.width
        font: __style.h3
        color: __style.forestColor
      }

      Item { width: 1; height: 1 }

      MMSettingDropdown {
        width: parent.width
        title: qsTr("Interval threshold type")
        description: qsTr("Choose a type of threshold for streaming mode")
        valueDescription: qsTr("Interval threshold type")
        value: __appSettings.intervalType === MM.StreamingIntervalType.Distance ? qsTr("Distance Traveled") : qsTr("Time elapsed")
        selected: [__appSettings.intervalType]
        model: ListModel {
          ListElement {
            value: MM.StreamingIntervalType.Time
            text: qsTr("Time elapsed")
          }
          ListElement {
            value: MM.StreamingIntervalType.Distance
            text: qsTr("Distance traveled")
          }
        }

        onValueWasChanged: function( newValue ) {
          //  comparing enum with QJSValue
          __appSettings.intervalType = (newValue == 1 ? StreamingIntervalType.Distance : StreamingIntervalType.Time)
        }
      }

      MMLine {}

      MMSettingInput {
        width: parent.width
        title: qsTr("Threshold interval")
        description: qsTr("Streaming mode will add a point to the object at each interval")
        valueDescription:  __appSettings.intervalType === MM.StreamingIntervalType.Distance ? qsTr("Threshold interval, in meters") : qsTr("Threshold interval, in seconds")
        value: __appSettings.lineRecordingInterval
        suffix: __appSettings.intervalType === MM.StreamingIntervalType.Distance ? " m" : " s"

        onValueWasChanged: function( newValue ) {
          __appSettings.lineRecordingInterval = newValue
        }
      }

      Item { width: 1; height: 1 }

      Text {
        text: qsTr("Recording")
        wrapMode: Text.WordWrap
        width: parent.width
        font: __style.h3
        color: __style.forestColor
      }

      Item { width: 1; height: 1 }

      MMSettingSwitch {
        width: parent.width
        title: qsTr("Reuse last entered value")
        description: qsTr("Each field offers an option to reuse its value on the next feature")
        checked: __appSettings.reuseLastEnteredValues

        onClicked: __appSettings.reuseLastEnteredValues = !checked
      }

      MMLine {}

      MMSettingSwitch {
        width: parent.width
        title: qsTr("Automatically sync changes")
        description: qsTr("Each time you save changes, the app will sync automatically")
        checked: __appSettings.autosyncAllowed

        onClicked: __appSettings.autosyncAllowed = !checked
      }

      Item { width: 1; height: 1 }

      Text {
        text: qsTr("General")
        wrapMode: Text.WordWrap
        width: parent.width
        font: __style.h3
        color: __style.forestColor
      }

      Item { width: 1; height: 1 }

      MMSettingItem {
        width: parent.width
        title: qsTr("About")
        value: ""

        onClicked: root.aboutClicked()
      }

      MMLine {}

      MMSettingItem {
        width: parent.width
        title: qsTr("Changelog")
        value: ""

        onClicked: root.changelogClicked()
      }

      MMLine {}

      MMSettingItem {
        width: parent.width
        title: qsTr("Help")
        value: ""

        onClicked: root.helpClicked()
      }

      MMLine {}

      MMSettingItem {
        width: parent.width
        title: qsTr("Privacy policy")
        value: ""

        onClicked: root.privacyPolicyClicked()
      }

      MMLine {}

      MMSettingItem {
        width: parent.width
        title: qsTr("Terms of service")
        value: ""

        onClicked: root.termsOfServiceClicked()
      }

      MMLine {}

      MMSettingItem {
        width: parent.width
        title: qsTr("Diagnostic log")
        value: ""

        onClicked: root.diagnosticLogClicked()
      }

      Item { width: 1; height: 10 }
    }
  }
}
