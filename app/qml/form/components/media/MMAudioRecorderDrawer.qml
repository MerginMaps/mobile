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
import QtCore
import QtMultimedia

import "../../../components" as MMComponents

/*
 * Drawer for recording audio attachments in the feature form.
 * The recording is written to targetDir, when user confirms the recording,
 * recordingFinished( path ) is emitted, otherwise the recorded file is removed.
 */

MMComponents.MMDrawer {
  id: root

  property string targetDir

  signal recordingFinished( string path )

  dim: true
  interactive: false
  closePolicy: Popup.NoAutoClose

  drawerHeader.title: qsTr( "Record audio" )

  onClosed: {
    // user closed the drawer without saving the recording
    if ( !internal.saved ) {
      internal.discarded = true
      if ( internal.isRecording ) {
        // the file is removed once the recorder stops, see onRecorderStateChanged
        recorder.stop()
      }
      else {
        internal.removeRecording()
      }
    }
  }

  Component.onCompleted: {
    if ( microphonePermission.status === Qt.Undetermined ) {
      microphonePermission.request()
    }
  }

  MicrophonePermission {
    id: microphonePermission

    onStatusChanged: {
      if ( status === Qt.Denied ) {
        __notificationModel.addError( qsTr( "Microphone permission is denied, please allow it in settings" ) )
      }
    }
  }

  CaptureSession {
    id: captureSession

    audioInput: AudioInput {}
    recorder: MediaRecorder {
      id: recorder

      mediaFormat {
        fileFormat: MediaFormat.Mpeg4Audio
        audioCodec: MediaFormat.AudioCodec.AAC
      }
      quality: MediaRecorder.HighQuality

      onRecorderStateChanged: {
        if ( recorderState === MediaRecorder.StoppedState && internal.recordingStarted ) {
          // the backend may adjust the file extension, so take the file name from the actual location
          const fileName = __inputUtils.getFileName( actualLocation.toString() )
          internal.recordedPath = __inputUtils.getAbsolutePath( fileName, root.targetDir )

          // recording finished only after the drawer was closed
          if ( internal.discarded ) {
            internal.removeRecording()
          }
        }
      }

      onErrorOccurred: function( error, errorString ) {
        __inputUtils.log( "Audio recorder", "Recording failed: " + errorString )
        __notificationModel.addError( qsTr( "Audio recording failed: %1" ).arg( errorString ) )
      }
    }
  }

  drawerContent: Column {
    width: parent.width
    spacing: __style.margin20

    MMComponents.MMText {
      width: parent.width

      font: __style.t1
      color: __style.nightColor
      horizontalAlignment: Text.AlignHCenter

      text: internal.formatTime( recorder.duration )
      visible: !internal.hasRecording
    }

    MMComponents.MMRoundButton {
      anchors.horizontalCenter: parent.horizontalCenter

      implicitWidth: __style.row80
      implicitHeight: __style.row80

      visible: !internal.hasRecording
      enabled: microphonePermission.status === Qt.Granted

      bgndColor: internal.isRecording ? __style.negativeColor : __style.lightGreenColor
      bgndHoverColor: internal.isRecording ? __style.negativeColor : __style.mediumGreenColor
      iconColor: internal.isRecording ? __style.grapeColor : __style.forestColor
      iconSource: internal.isRecording ? __style.stopIcon : __style.microphoneIcon

      onClicked: {
        if ( internal.isRecording ) {
          recorder.stop()
        }
        else {
          internal.startRecording()
        }
      }
    }

    MMComponents.MMText {
      width: parent.width

      font: __style.p5
      color: __style.nightColor
      horizontalAlignment: Text.AlignHCenter
      wrapMode: Text.WordWrap

      text: internal.isRecording ? qsTr( "Tap to stop recording" ) : qsTr( "Tap to start recording" )
      visible: !internal.hasRecording
    }

    MMMediaPlayer {
      id: preview

      width: parent.width
      height: __style.row60

      source: internal.hasRecording ? __inputUtils.localFileToUrl( internal.recordedPath ) : ""
      visible: internal.hasRecording
    }

    MMComponents.MMButton {
      width: parent.width

      text: qsTr( "Save recording" )
      visible: internal.hasRecording

      onClicked: {
        preview.stop()
        internal.saved = true
        root.recordingFinished( internal.recordedPath )
        root.close()
      }
    }

    MMComponents.MMButton {
      width: parent.width

      type: MMComponents.MMButton.Types.Tertiary
      text: qsTr( "Record again" )
      visible: internal.hasRecording

      onClicked: {
        preview.stop()
        internal.removeRecording()
      }
    }
  }

  QtObject {
    id: internal

    property bool recordingStarted: false
    property bool saved: false
    property bool discarded: false
    property string recordedPath: ""

    readonly property bool isRecording: recorder.recorderState === MediaRecorder.RecordingState
    readonly property bool hasRecording: recordedPath !== "" && !isRecording

    function startRecording() {
      if ( !__inputUtils.createDirectory( root.targetDir ) ) {
        __inputUtils.log( "Audio recorder", "Could not create directory " + root.targetDir )
        __notificationModel.addError( qsTr( "Could not create directory %1." ).arg( root.targetDir ) )
        return
      }

      const fileName = "AUDIO_" + Qt.formatDateTime( new Date(), "yyyyMMdd_hhmmss" ) + ".m4a"
      recorder.outputLocation = __inputUtils.localFileToUrl( __inputUtils.getAbsolutePath( fileName, root.targetDir ) )
      recordingStarted = true
      recorder.record()
    }

    function removeRecording() {
      if ( recordedPath !== "" ) {
        __inputUtils.removeFile( recordedPath )
      }
      recordedPath = ""
      recordingStarted = false
    }

    function formatTime( ms ) {
      const totalSeconds = Math.floor( Math.max( ms, 0 ) / 1000 )
      const minutes = Math.floor( totalSeconds / 60 )
      const seconds = totalSeconds % 60
      return minutes + ":" + ( seconds < 10 ? "0" : "" ) + seconds
    }
  }
}
