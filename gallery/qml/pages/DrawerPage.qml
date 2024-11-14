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

Page {
  id: pane

  ScrollView {
    anchors.fill: parent

    Column {
      width: parent.width
      spacing: 10
      padding: 10

      Button {
        text: "MMPositionTrackingDialog"
        onClicked: positionTrackingDialog.open()
      }

      Button {
        text: "MMStorageLimitDialog"
        onClicked: storageLimitDialog.open()
      }

      Button {
        text: "MMMonthlyContributorsLimitDialog"
        onClicked: monthlyContributorsLimitDialog.open()
      }

      Button {
        text: "MMCloseAccountDialog"
        onClicked: closeAccountDialog.open()
      }

      Button {
        text: "MMRemoveProjectDialog"
        onClicked: removeProjectDialog.open()
      }

      Button {
        text: "MMDownloadProjectDialog"
        onClicked: downloadProjectDialog.open()
      }

      Button {
        text: "MMStreamingModeDialog"
        onClicked: streamingModeDialog.open()
      }

      Button {
        text: "MMBluetoothConnectionDrawer"
        onClicked: {
          bluetoothConnectionDrawer.positionProvider.state = PositionProvider.Connecting
          bluetoothConnectionTimer.start()
          bluetoothConnectionDrawer.open()
        }
      }

      Button {
        text: "MMSyncFailedDialog"

        onClicked: {
          syncFailedDialog.open()
        }
      }

      Button {
        text: "MMNoPermissionsDialog"

        onClicked: {
          noPermissionsDialog.open()
        }
      }

      Button {
        text: "MMMissingAuthDialog"

        onClicked: {
          missingAuthDialog.open()
        }
      }

      Button {
        text: "MMSplittingFailedDialog"

        onClicked: {
          splittingFailedDialog.open()
        }
      }

      Button {
        text: "MMMigrateToMerginDialog"

        onClicked: {
          migrateToMerginDialog.open()
        }
      }

      Button {
        text: "MMProjectLimitDialog"

        onClicked: {
          projectLimitDialog.open()
        }
      }

      Button {
        text: "removePhotoDialog"

        onClicked: {
          removePhotoDialog.open()
        }
      }

      Button {
        text: "formSaveChangesDialog"

        onClicked: {
          formSaveChangesDialog.open()
        }
      }

      Button {
        text: "FormEditingFailedDialog"

        onClicked: {
          editingFailedDialog.open()
        }
      }

      Button {
        text: "FormDeleteFeatureDialog"

        onClicked: {
          deleteDialog.open()
        }
      }

      Button {
        text: "projErrorDialog"

        onClicked: {
          projErrorDialog.open()
        }
      }

      Button {
        text: "outOfDateServerDialog"

        onClicked: {
          outOfDateServerDialog.open()
        }
      }

      Button {
        text: "projectLoadErrorDialog"

        onClicked: {
          projectLoadErrorDialog.open()
        }
      }

      Button {
        text: "discardGeometryChangesDlg"

        onClicked: {
          discardGeometryChangesDlg.open()
        }
      }

      Button {
        text: "removeReceiverDialog"

        onClicked: {
          removeReceiverDialog.open( testModelItem.providerId )
        }
      }

      Button {
        text: "welcomeToNewDesignDialog"

        onClicked: {
          welcomeToNewDesignDialog.open()
        }
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

  MMStreamingModeDialog {
    id: streamingModeDialog

    streamingActive: false

    onStreamingBtnClicked: {
      streamingActive = !streamingActive
    }
  }

  MMCloseAccountDialog {
    id: closeAccountDialog

    username: "Superman"
  }

  MMRemoveProjectDialog {
    id: removeProjectDialog

    relatedProjectId: "Cool Project"

    onRemoveClicked: console.log("on remove clicked")
  }

  MMProjectLimitDialog {
    id: projectLimitDialog

    maxProjectNumber: 2
    apiSupportsSubscription: true

    onPrimaryButtonClicked: {
      console.log("Manage workspace clicked")
    }
  }

  MMDownloadProjectDialog {
    id: downloadProjectDialog

    relatedProjectId: "Best Project"

    onDownloadClicked: console.log("on download clicked")
  }

  MMPositionTrackingDialog {
    id: positionTrackingDialog
    distanceTraveled: "5m"
    trackingStartedAt: "10:00:01"
    onTrackingBtnClicked: trackingActive = !trackingActive
  }

  MMStorageLimitDialog {
    id: storageLimitDialog

    dataToSync: "643.8 MB"
    dataUsing: "9.23 MB / 10.0 MB"
    usedData: 0.923
    apiSupportsSubscription: true

    onPrimaryButtonClicked: {
      console.log("Manage workspace clicked")
    }
  }

  MMMonthlyContributorsLimitDialog {
    id: monthlyContributorsLimitDialog

    apiSupportsSubscription: true

    onPrimaryButtonClicked: {
      console.log("Manage workspace clicked")
    }
  }

  MMSyncFailedDialog {
    id: syncFailedDialog
  }

  MMNoPermissionsDialog {
    id: noPermissionsDialog
  }

  MMMissingAuthDialog {
    id: missingAuthDialog
  }

  MMSplittingFailedDialog {
    id: splittingFailedDialog
  }

  MMMigrateToMerginDialog {
    id: migrateToMerginDialog
  }

  MMRemovePhotoDialog {
    id: removePhotoDialog
  }

  MMFormSaveChangesDialog {
    id: formSaveChangesDialog
  }

  MMFormEditFailedDialog {
    id: editingFailedDialog
  }

  MMFormDeleteFeatureDialog {
    id: deleteDialog
  }

  MMProjErrorDialog {
    id: projErrorDialog

    helpLink: "https://merginmaps.com"
  }

  MMOutOfDateCustomServerDialog {
    id: outOfDateServerDialog

    helpLink: "https://merginmaps.com"
  }

  MMProjectLoadErrorDialog {
    id: projectLoadErrorDialog

    helpLink: "https://merginmaps.com"
  }

  MMDiscardGeometryChangesDialog {
    id: discardGeometryChangesDlg

    state: "record"
  }

  Item {
    id: testModelItem

    property string providerId: "123"
  }

  MMProviderRemoveReceiverDialog {
    id: removeReceiverDialog

    function open( providerId ) {
      this.providerId = providerId
      visible = true
    }
  }

  MMWelcomeToNewDesignDialog {
    id: welcomeToNewDesignDialog
  }
}
