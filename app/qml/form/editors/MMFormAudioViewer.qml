/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtMultimedia

import "../../components" as MMComponents
import "../../components/private" as MMPrivateComponents
import "../components/audio" as MMAudioComponents

/*
 * Audio viewer for feature form.
 * Shows the empty state, the recording in progress and a player for the assigned audio file.
 * It is not combined with MMFormAudioEditor so it can be used in the gallery app.
 *
 * Serves as a base class for MMFormAudioEditor.
 */

MMPrivateComponents.MMBaseInput {
  id: root

  property string audioUrl: ""
  property string fileName: ""
  property bool hasRecordCapability: true
  property bool playbackEnabled: true // false while any field of the form records, the player is unloaded then

  property bool isRecording: false
  property real recordingDuration: 0 // milliseconds

  property alias audioState: audioStateGroup.state

  signal recordClicked()
  signal chooseFromDeviceClicked()
  signal stopRecordingClicked()
  signal rerecordClicked()
  signal trashClicked()

  StateGroup {
    id: audioStateGroup

    states: [
      State {
        name: "valid"
      },
      State {
        name: "notSet"
      },
      State {
        name: "notAvailable"
      }
    ]

    state: "notSet"
  }

  onAudioUrlChanged: player.stop()
  onIsRecordingChanged: player.stop()
  onPlaybackEnabledChanged: if ( !playbackEnabled ) player.stop()

  inputContent: Rectangle {
    width: parent.width
    height: {
      if ( root.isRecording ) {
        return recordingView.implicitHeight
      }
      if ( audioStateGroup.state === "notSet" ) {
        return __style.row160
      }
      return playerView.implicitHeight
    }

    color: __style.polarColor
    radius: __style.radius20

    MMAudioComponents.MMAudioAttachment {
      width: parent.width
      height: parent.height

      visible: !root.isRecording && audioStateGroup.state === "notSet"
      enabled: root.editState === "enabled"

      hasRecordCapability: root.hasRecordCapability

      onRecordClicked: root.recordClicked()
      onChooseFromDeviceClicked: root.chooseFromDeviceClicked()
    }

    Column {
      id: recordingView

      width: parent.width
      padding: __style.margin20
      spacing: __style.spacing12

      visible: root.isRecording

      Row {
        anchors.horizontalCenter: parent.horizontalCenter
        spacing: __style.spacing10

        Rectangle {
          id: recordingDot

          anchors.verticalCenter: parent.verticalCenter

          width: __style.margin12
          height: width
          radius: width / 2

          color: __style.grapeColor

          SequentialAnimation on opacity {
            running: root.isRecording
            loops: Animation.Infinite

            NumberAnimation { to: 0.3; duration: 600 }
            NumberAnimation { to: 1; duration: 600 }
          }
        }

        MMComponents.MMText {
          font: __style.t3
          color: __style.forestColor
          text: internal.formatDuration( root.recordingDuration, true )
        }
      }

      // simple level animation, Qt Multimedia does not expose the input level while recording
      Row {
        anchors.horizontalCenter: parent.horizontalCenter
        height: __style.row40
        spacing: __style.margin4

        Repeater {
          id: levelBars

          model: 28

          Rectangle {
            anchors.verticalCenter: parent.verticalCenter

            width: __style.margin4
            height: __style.margin6
            radius: width / 2

            color: index % 2 ? __style.grassColor : __style.forestColor

            Behavior on height { NumberAnimation { duration: 120 } }
          }
        }

        Timer {
          interval: 150
          repeat: true
          running: root.isRecording

          onTriggered: {
            for ( let i = 0; i < levelBars.count; i++ ) {
              levelBars.itemAt( i ).height = __style.margin6 + Math.random() * ( __style.row36 - __style.margin6 )
            }
          }
        }
      }

      MMComponents.MMButton {
        anchors.horizontalCenter: parent.horizontalCenter

        text: qsTr( "Stop recording" )
        iconSourceLeft: __style.stopIcon

        fontColor: __style.grapeColor
        iconColor: __style.grapeColor
        bgndColor: __style.negativeColor
        fontColorHover: __style.negativeColor
        iconColorHover: __style.negativeColor
        bgndColorHover: __style.grapeColor

        onClicked: root.stopRecordingClicked()
      }
    }

    Column {
      id: playerView

      width: parent.width
      padding: __style.margin20
      spacing: __style.spacing12

      visible: !root.isRecording && audioStateGroup.state !== "notSet"

      Row {
        width: parent.width - 2 * parent.padding
        spacing: __style.spacing12

        MMComponents.MMRoundButton {
          id: playButton

          anchors.verticalCenter: parent.verticalCenter

          enabled: audioStateGroup.state === "valid" && root.playbackEnabled

          bgndColor: __style.grassColor
          iconColor: __style.forestColor
          iconSource: player.playbackState === MediaPlayer.PlayingState ? __style.pauseIcon : __style.playIcon

          onClicked: {
            if ( player.playbackState === MediaPlayer.PlayingState ) {
              player.pause()
            }
            else {
              player.play()
            }
          }
        }

        Item {
          anchors.verticalCenter: parent.verticalCenter

          width: parent.width - playButton.width - parent.spacing
          height: __style.row40

          Rectangle {
            id: track

            anchors.verticalCenter: parent.verticalCenter

            width: parent.width
            height: __style.margin4
            radius: height / 2

            color: __style.mediumGreenColor

            Rectangle {
              width: player.duration > 0 ? track.width * player.position / player.duration : 0
              height: parent.height
              radius: parent.radius

              color: __style.forestColor
            }
          }

          MouseArea {
            anchors.fill: parent
            enabled: player.seekable && root.playbackEnabled

            onClicked: function( mouse ) {
              player.position = player.duration * Math.max( 0, Math.min( 1, mouse.x / width ) )
            }
          }
        }
      }

      Row {
        width: parent.width - 2 * parent.padding
        spacing: __style.spacing12

        MMComponents.MMText {
          id: timeText

          font: __style.p6
          color: __style.forestColor
          // empty while no file is loaded, e.g. during a recording
          text: player.duration > 0 ? internal.formatDuration( player.position, false ) + " / " + internal.formatDuration( player.duration, false ) : ""
        }

        MMComponents.MMText {
          width: parent.width - timeText.width - parent.spacing

          font: __style.p6
          color: __style.nightColor
          text: root.fileName

          horizontalAlignment: Text.AlignRight
          elide: Text.ElideMiddle
        }
      }

      // same look as the feature form toolbar: icon over a label, equal columns
      Row {
        id: actionsRow

        width: parent.width - 2 * parent.padding
        height: __style.row60

        visible: root.editState === "enabled"

        property int buttonCount: ( rerecordButton.visible ? 1 : 0 ) + 1

        MMPrivateComponents.MMToolbarShortButton {
          id: rerecordButton

          width: actionsRow.width / actionsRow.buttonCount
          height: actionsRow.height

          visible: root.hasRecordCapability

          text: qsTr( "Re-record" )
          iconSource: __style.reRecordIcon
          iconSourceSelected: __style.reRecordIcon
          iconColor: __style.forestColor
          iconColorDisabled: __style.mediumGreenColor

          onClicked: root.rerecordClicked()
        }

        MMPrivateComponents.MMToolbarShortButton {
          width: actionsRow.width / actionsRow.buttonCount
          height: actionsRow.height

          text: qsTr( "Delete" )
          iconSource: __style.deleteIcon
          iconSourceSelected: __style.deleteIcon
          iconColor: __style.grapeColor
          iconColorDisabled: __style.mediumGreenColor

          onClicked: root.trashClicked()
        }
      }
    }
  }

  MediaPlayer {
    id: player

    // No file is loaded while a field records. On iOS loading a file switches the shared audio session
    // to playback and unloading the last one deactivates it, which must not happen under a running recording,
    // e.g. when this editor is created or destroyed while the user scrolls through the form
    source: audioStateGroup.state === "valid" && root.playbackEnabled ? root.audioUrl : ""
    audioOutput: AudioOutput {}

    onErrorOccurred: function( error, errorString ) {
      __inputUtils.log( "Audio playback", "Could not play " + root.audioUrl + ": " + errorString )
    }
  }

  QtObject {
    id: internal

    // m:ss, or mm:ss for the recording timer
    function formatDuration( milliseconds, padMinutes ) {
      const totalSeconds = Math.max( 0, Math.floor( milliseconds / 1000 ) )
      const minutes = Math.floor( totalSeconds / 60 )
      const seconds = String( totalSeconds % 60 ).padStart( 2, "0" )

      if ( padMinutes ) {
        return String( minutes ).padStart( 2, "0" ) + ":" + seconds
      }
      return minutes + ":" + seconds
    }
  }
}
