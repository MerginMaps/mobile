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
import "."
import "../components"
import "../misc"

Item {
  id: root

  visible: false

  signal close

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
        color: __style.lightGreenColor
      }

      header: MMHeader {
        id: header
        titleFont: __style.t3

        onBackClicked: { root.visible = false; root.close() }
        backVisible: true
      }

      ScrollView {
        id: scrollPage
        width: settingsPanel.width - 2 * __style.pageMargins
        height: settingsPanel.height - header.height
        contentWidth: availableWidth // to only scroll vertically
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: 0 * __dp

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

          MMSettingItem {
            width: parent.width
            title: qsTr("GPS accuracy treshold")
            description: qsTr("Determines when the accuracy indicator turns yellow")
            valueDescription: qsTr("GPS accuracy treshold, in meters")
            value: __appSettings.gpsAccuracyTolerance
            suffix: " m"
            editable: true

            onValueWasChanged: function( newValue ) {
              __appSettings.gpsAccuracyTolerance = newValue
            }
          }

          MMLine {}

          MMSettingItem {
            width: parent.width
            title: qsTr("Manage GPS receivers")
            value: "Internal"

            onClicked: console.log("TODO: follow new design ...") // stackview.push( positionProviderComponent )
          }

          MMLine {}

          MMSettingItem {
            width: parent.width
            title: qsTr("GPS antenna height")
            description: qsTr("Includes pole height and GPS receiver’s antenna height")
            valueDescription: qsTr("GPS antenna height, in meters")
            value: __appSettings.gpsAntennaHeight
            suffix: " m"
            editable: true

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

          MMSettingItem {
            width: parent.width
            title: qsTr("Interval threshold type")
            description: qsTr("Choose a type of threshold for streaming mode")
            valueDescription: qsTr("Interval threshold type")
            value: __appSettings.intervalType === StreamingIntervalType.Distance ? qsTr("Distance Traveled") : qsTr("Time elapsed")
            selected: [__appSettings.intervalType]
            model: ListModel {
              ListElement {
                value: StreamingIntervalType.Time
                text: qsTr("Time elapsed")
              }
              ListElement {
                value: StreamingIntervalType.Distance
                text: qsTr("Distance traveled")
              }
            }

            onValueWasChanged: function( newValue ) {
              __appSettings.intervalType = (newValue == 1 ? StreamingIntervalType.Distance : StreamingIntervalType.Time)
            }
          }

          MMLine {}

          MMSettingItem {
            width: parent.width
            title: qsTr("Threshold interval")
            description: qsTr("Streaming mode will add a point to the object at each interval")
            valueDescription:  __appSettings.intervalType === StreamingIntervalType.Distance ? qsTr("Threshold interval, in meters") : qsTr("Threshold interval, in seconds")
            value: __appSettings.lineRecordingInterval
            suffix: __appSettings.intervalType === StreamingIntervalType.Distance ? " m" : " s"
            editable: true

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

        }

      }
    }
  }

  Component {
    id: aboutPanelComponent
    MMAboutPanel {
      onClose: stackview.pop(null)
      onVisitWebsiteClicked: Qt.openUrlExternally( __inputHelp.inputWebLink )
      Component.onCompleted: forceActiveFocus()
    }
  }
}
