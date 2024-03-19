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

import mm 1.0 as MM

import "./components" as  MMSettingsComponents
import "../components"

MMPage {
  id: root

  signal helpClicked()
  signal aboutClicked()
  signal manageGpsClicked()
  signal changelogClicked()
  signal privacyPolicyClicked()
  signal diagnosticLogClicked()
  signal termsOfServiceClicked()

  pageBottomMarginPolicy: MMPage.BottomMarginPolicy.PaintBehindSystemBar

  pageContent: MMScrollView {

    width: parent.width
    height: parent.height

    Column {
      width: parent.width
      height: childrenRect.height

      spacing: __style.spacing20

      Text {
        text: qsTr("GPS")
        wrapMode: Text.WordWrap
        width: parent.width
        font: __style.h3
        color: __style.forestColor
      }

      Item { width: 1; height: 1 }

      MMSettingsComponents.MMSettingInput {
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

      MMSettingsComponents.MMSettingItem {
        width: parent.width
        title: qsTr("Manage GPS receivers")
        value: "Internal"

        onClicked: root.manageGpsClicked()
      }

      MMLine {}

      MMSettingsComponents.MMSettingInput {
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

      MMSettingsComponents.MMSettingDropdown {
        width: parent.width

        title: qsTr("Interval threshold type")
        description: qsTr("Choose a type of threshold for streaming mode")
        valueDescription: qsTr("Interval threshold type")

        value: __appSettings.intervalType === MM.StreamingIntervalType.Distance ? qsTr("Distance Traveled") : qsTr("Time elapsed")
        selected: [__appSettings.intervalType]

        model: ListModel {
          ListElement {
            value: 0//MM.StreamingIntervalType.Time
            text: qsTr("Time elapsed")
          }
          ListElement {
            value: 1 //MM.StreamingIntervalType.Distance
            text: qsTr("Distance traveled")
          }
        }

        onValueWasChanged: function( newValue ) {
          console.log("new valiue", newValue[0])
          console.log(" M.StreamingIntervalType.Time, ", MM.StreamingIntervalType.Time)
          console.log("  MM.StreamingIntervalType.Distance, ",  MM.StreamingIntervalType.Distance)
          __appSettings.intervalType = newValue[0] //(newValue === MM.StreamingIntervalType.Distance ? MM.StreamingIntervalType.Distance : MM.StreamingIntervalType.Time)
          //__appSettings.intervalType = MM.StreamingIntervalType.Distance
          console.log(" app settings: ", __appSettings.intervalType)
        }
      }

      MMLine {}

      MMSettingsComponents.MMSettingInput {
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

      MMSettingsComponents.MMSettingSwitch {
        width: parent.width
        title: qsTr("Reuse last entered value")
        description: qsTr("Each field offers an option to reuse its value on the next feature")
        checked: __appSettings.reuseLastEnteredValues

        onClicked: __appSettings.reuseLastEnteredValues = !checked
      }

      MMLine {}

      MMSettingsComponents.MMSettingSwitch {
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

      MMSettingsComponents.MMSettingItem {
        width: parent.width
        title: qsTr("About")
        value: ""

        onClicked: root.aboutClicked()
      }

      MMLine {}

      MMSettingsComponents.MMSettingItem {
        width: parent.width
        title: qsTr("Changelog")
        value: ""

        onClicked: root.changelogClicked()
      }

      MMLine {}

      MMSettingsComponents.MMSettingItem {
        width: parent.width
        title: qsTr("Help")
        value: ""

        onClicked: root.helpClicked()
      }

      MMLine {}

      MMSettingsComponents.MMSettingItem {
        width: parent.width
        title: qsTr("Privacy policy")
        value: ""

        onClicked: root.privacyPolicyClicked()
      }

      MMLine {}

      MMSettingsComponents.MMSettingItem {
        width: parent.width
        title: qsTr("Terms of service")
        value: ""

        onClicked: root.termsOfServiceClicked()
      }

      MMLine {}

      MMSettingsComponents.MMSettingItem {
        width: parent.width
        title: qsTr("Diagnostic log")
        value: ""

        onClicked: root.diagnosticLogClicked()
      }

      MMListFooterSpacer{}
    }
  }
}
