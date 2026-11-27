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
import "../../app/qml/form/editors"
import "../../app/qml/dialogs"

Page {

  Rectangle {
    anchors.fill: parent
    color: __style.lightGreenColor
  }

  ScrollView {
    anchors.fill: parent

    contentWidth: availableWidth

    Column {
      width: parent.width
      spacing: 20
      topPadding: 20
      rightPadding: 20
      leftPadding: 20

      MMFormAudioViewer {
        width: parent.width - parent.leftPadding - parent.rightPadding

        title: "Empty"

        onRecordClicked: console.log( "onRecordClicked" )
        onChooseFromDeviceClicked: console.log( "onChooseFromDeviceClicked" )
      }

      MMFormAudioViewer {
        width: parent.width - parent.leftPadding - parent.rightPadding

        title: "Empty, no microphone"
        hasRecordCapability: false
      }

      MMFormAudioViewer {
        id: recordingViewer

        width: parent.width - parent.leftPadding - parent.rightPadding

        title: "Recording"
        isRecording: true

        onStopRecordingClicked: isRecording = false

        Timer {
          interval: 1000
          repeat: true
          running: recordingViewer.isRecording
          onTriggered: recordingViewer.recordingDuration += 1000
        }
      }

      MMFormAudioViewer {
        width: parent.width - parent.leftPadding - parent.rightPadding

        title: "Recorded"
        audioState: "valid"
        fileName: "audio/audio_20261110_101544.m4a"

        onRerecordClicked: replaceDialog.open()
        onTrashClicked: removeDialog.open()
      }

      MMFormAudioViewer {
        width: parent.width - parent.leftPadding - parent.rightPadding

        title: "Missing file"
        audioState: "notAvailable"
        warningMsg: "Audio file is missing."
        fileName: "audio/missing.m4a"
      }

      MMFormAudioViewer {
        width: parent.width - parent.leftPadding - parent.rightPadding

        title: "Read-only"
        audioState: "valid"
        readOnly: true
        fileName: "audio/audio_20261110_101544.m4a"
      }
    }
  }

  MMRemoveAudioDialog {
    id: removeDialog

    onDeleteAudio: close()
    onUnlinkAudio: close()
  }

  MMReplaceAudioDialog {
    id: replaceDialog
  }
}
