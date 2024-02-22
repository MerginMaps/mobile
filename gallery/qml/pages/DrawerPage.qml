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
import QtQuick.Controls.Basic

import "../../app/qml/components"
import "../../app/qml"
import "../../app/qml/project"
import "../../app/qml/dialogs"
import "../../app/qml/gps"

import lc 1.0

Page {
  id: pane

  Column {
    width: parent.width
    spacing: 10
    padding: 10

    Label {
      text: "Drawers"
    }

    MMButton {
      text: "Position Tracking"
      onClicked: drawerPositionTracking.open()
    }

    MMButton {
      text: "Upload"
      onClicked: drawer1.open()
    }
    MMButton {
      text: "MMStorageLimitDialog"
      onClicked: storageLimitDialog.open()
    }
    MMButton {
      text: "Synchronization Failed"
      onClicked: drawer3.open()
    }

    MMButton {
      text: "MMBluetoothConnectionDrawer"
      onClicked: {
        bluetoothConnectionDrawer.positionProvider.state = PositionProvider.Connecting
        bluetoothConnectionTimer.start()
        bluetoothConnectionDrawer.open()
      }
    }
  }

  MMBluetoothConnectionDrawer {
    id: bluetoothConnectionDrawer

    howToConnectGPSLink: "www.merginmaps.com"
    positionProvider: QtObject {
      function name() { return "Cool Phone" }
      property var state
    }

    Timer {
      id: bluetoothConnectionTimer
      interval: 2000
      repeat: true
      running: false
      onTriggered: {
        if ( bluetoothConnectionDrawer.positionProvider.state === PositionProvider.Connecting )
          bluetoothConnectionDrawer.positionProvider.state = PositionProvider.WaitingToReconnect
        else if ( bluetoothConnectionDrawer.positionProvider.state === PositionProvider.WaitingToReconnect )
          bluetoothConnectionDrawer.positionProvider.state = PositionProvider.NoConnection
        else if ( bluetoothConnectionDrawer.positionProvider.state === PositionProvider.NoConnection )
        {
          bluetoothConnectionDrawer.positionProvider.state = PositionProvider.Connected
        } else {
          bluetoothConnectionTimer.stop()
        }
      }
    }
  }

  MMPositionTrackingDrawer {
    id: drawerPositionTracking
    distanceTraveled: "5m"
    trackingStartedAt: "10:00:01"
    onTrackingBtnClicked: trackingActive = !trackingActive
  }

  MMDrawerDialog {
    id: drawer1

    picture: __style.uploadImage
    bigTitle: "Upload project to Margin?"
    description: "This project is currently not uploaded on Mergin. Upload it to Mergin in order to activate synchronization and collaboration."
    primaryButton: "Yes, Upload Project"
    secondaryButton: "No Cancel"

    onPrimaryButtonClicked: close()
    onSecondaryButtonClicked: close()
  }

  MMStorageLimitDialog {
    id: storageLimitDialog

    dataToSync: "643.8 MB"
    dataUsing: "9.23 MB / 10.0 MB"
    plan: "Individual"
    usedData: 0.923
    apiSupportsSubscription: true

    onPrimaryButtonClicked: {
      console.log("Manage workspace clicked")
      close()
    }
  }

  MMDrawerDialog {
    id: drawer3

    picture: __style.uploadImage
    bigTitle: "Failed to synchronize your changes"
    description: "Your changes could not be sent to server, make sure you are connected to internet and have write access to this project."
    primaryButton: "Ok, I understand"
    boundedDescription: "Failed to push changes. Ask the project workspace owner to log in to their Mergin Maps dashboard for more information."

    onPrimaryButtonClicked: close()
    onSecondaryButtonClicked: close()
  }
}
