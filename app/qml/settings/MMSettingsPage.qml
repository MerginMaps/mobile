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
import MMInput

import "./components" as MMSettingsComponents
import "../components"
import "../dialogs"

MMPage {
  id: root

  signal helpClicked()
  signal aboutClicked()
  signal manageGpsClicked()
  signal changelogClicked()
  signal privacyPolicyClicked()
  signal diagnosticLogClicked()
  signal termsOfServiceClicked()

  ListModel {
    id: intervalTypeModel

    Component.onCompleted: {
      intervalTypeModel.append({ value: MM.StreamingIntervalType.Time, text: qsTr("Time elapsed") });
      intervalTypeModel.append({ value: MM.StreamingIntervalType.Distance, text: qsTr("Distance traveled") });
    }
  }

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

      MMSettingsComponents.MMSettingsInput {
        width: parent.width
        title: qsTr("GPS accuracy threshold")
        description: qsTr("Determines when the accuracy indicator turns yellow")
        valueDescription: qsTr("GPS accuracy threshold, in meters")
        value: AppSettings.gpsAccuracyTolerance
        suffix: " m"

        onValueWasChanged: function( newValue ) {
          AppSettings.gpsAccuracyTolerance = newValue
        }
      }

      MMLine {}

      MMSettingsComponents.MMSettingsItem {
        width: parent.width
        title: qsTr("Manage GPS receivers")
        value: PositionKit.positionProvider.name()

        onClicked: root.manageGpsClicked()
      }

      MMLine {}

      MMSettingsComponents.MMSettingsInput {
        width: parent.width
        title: qsTr("GPS antenna height")
        description: qsTr("Includes pole height and GPS receiver’s antenna height")
        valueDescription: qsTr("GPS antenna height, in meters")
        value: AppSettings.gpsAntennaHeight
        suffix: " m"

        onValueWasChanged: function( newValue ) {
          AppSettings.gpsAntennaHeight = newValue
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

      MMSettingsComponents.MMSettingsDropdown {
        width: parent.width

        title: qsTr("Interval threshold type")
        description: qsTr("Choose a type of threshold for streaming mode")

        value: AppSettings.intervalType === MM.StreamingIntervalType.Distance ? qsTr("Distance Traveled") : qsTr("Time elapsed")
        currentIndex: AppSettings.intervalType

        // To dynamically assign values like "MM.StreamingIntervalType.Distance," derived from a C++ enum or even from if-else blocks,
        // to a ListElement field, you need to build the model using a function. This function should append the required data to the model
        // and then return the fully assembled model, ready for use.
        model: intervalTypeModel

        onCurrentIndexChanged: AppSettings.intervalType = currentIndex
      }

      MMLine {}

      MMSettingsComponents.MMSettingsInput {
        width: parent.width
        title: qsTr("Threshold interval")
        description: qsTr("Streaming mode will add a point to the object at each interval")
        valueDescription:  AppSettings.intervalType === MM.StreamingIntervalType.Distance ? qsTr("Threshold interval, in meters") : qsTr("Threshold interval, in seconds")
        value: AppSettings.lineRecordingInterval
        suffix: AppSettings.intervalType === MM.StreamingIntervalType.Distance ? " m" : " s"

        onValueWasChanged: function( newValue ) {
          AppSettings.lineRecordingInterval = newValue
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

      MMSettingsComponents.MMSettingsSwitch {
        width: parent.width
        title: qsTr("Reuse last entered value")
        description: qsTr("Each field offers an option to reuse its value on the next feature")
        checked: AppSettings.reuseLastEnteredValues

        onClicked: AppSettings.reuseLastEnteredValues = !checked
      }

      MMLine {}

      MMSettingsComponents.MMSettingsSwitch {
        width: parent.width
        title: qsTr("Automatically sync changes")
        description: qsTr("Your project is synced automatically to keep you up to date")
        checked: AppSettings.autosyncAllowed

        onClicked: AppSettings.autosyncAllowed = !checked
      }

      MMLine {}

      MMSettingsComponents.MMSettingsSwitch {
        width: parent.width
        title: qsTr("Auto-lock position")
        description: qsTr("Each time you start recording, the app centers to GPS")
        checked: AppSettings.autolockPosition

        onClicked: AppSettings.autolockPosition = !checked
      }

      MMLine {}

      MMSettingsComponents.MMSettingsDropdown {
        id: hapticDropdown
        width: parent.width

        title: qsTr("Touch Feedback")
        description: qsTr("The app can vibrate and play a sound when adding a feature")

        currentIndex: AppSettings.hapticsType

        model: HapticsModel {}

        onCurrentIndexChanged: {
          AppSettings.hapticsType = currentIndex
          hapticDropdown.value = hapticDropdown.model.data( hapticDropdown.model.index( currentIndex, 0 ), HapticsModel.TextRole )
        }
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

      MMSettingsComponents.MMSettingsItem {
        width: parent.width
        title: qsTr("About")
        value: ""

        onClicked: root.aboutClicked()
      }

      MMLine {}

      MMSettingsComponents.MMSettingsItem {
        width: parent.width
        title: qsTr("Changelog")
        value: ""

        onClicked: root.changelogClicked()
      }

      MMLine {}

      MMSettingsComponents.MMSettingsItem {
        width: parent.width
        title: qsTr("Help")
        value: ""

        onClicked: root.helpClicked()
      }

      MMLine {}

      MMSettingsComponents.MMSettingsItem {
        width: parent.width
        title: qsTr("Privacy policy")
        value: ""

        onClicked: root.privacyPolicyClicked()
      }

      MMLine {}

      MMSettingsComponents.MMSettingsItem {
        width: parent.width
        title: qsTr("Terms of service")
        value: ""

        onClicked: root.termsOfServiceClicked()
      }

      MMLine {}

      MMSettingsComponents.MMSettingsItem {
        width: parent.width
        title: qsTr("Diagnostic log")
        value: ""

        onClicked: root.diagnosticLogClicked()
      }

      MMListFooterSpacer{}
    }
  }
}
