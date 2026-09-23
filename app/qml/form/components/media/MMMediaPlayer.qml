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
import QtMultimedia

import "../../../components" as MMComponents

/*
 * Simple audio/video player used by the media form editor and the audio recorder.
 * Shows video output (only when showVideo is set) and play/pause button with seek bar.
 */

Rectangle {
  id: root

  property url source: ""
  property bool showVideo: false

  // space reserved on the right side of the controls, e.g. for buttons placed above the player
  property real controlsRightInset: 0

  readonly property alias playbackState: player.playbackState

  color: __style.polarColor
  radius: __style.radius20
  clip: true

  function stop() {
    player.stop()
  }

  MediaPlayer {
    id: player

    source: root.source
    audioOutput: AudioOutput {}
    videoOutput: root.showVideo ? videoOutput : null

    onErrorOccurred: function( error, errorString ) {
      __inputUtils.log( "Media player", "Could not play " + root.source + ": " + errorString )
    }
  }

  VideoOutput {
    id: videoOutput

    anchors {
      top: parent.top
      left: parent.left
      right: parent.right
      bottom: controls.top
    }

    visible: root.showVideo
    fillMode: VideoOutput.PreserveAspectFit

    Rectangle {
      anchors.fill: parent
      z: -1
      color: __style.nightColor
      radius: __style.radius20
    }

    MMComponents.MMIcon {
      anchors.centerIn: parent

      size: __style.icon32
      color: __style.polarColor
      source: __style.playIcon

      visible: player.playbackState !== MediaPlayer.PlayingState
    }

    MouseArea {
      anchors.fill: parent
      onClicked: internal.togglePlayback()
    }
  }

  Item {
    id: controls

    anchors {
      left: parent.left
      right: parent.right
      bottom: root.showVideo ? parent.bottom : undefined
      verticalCenter: root.showVideo ? undefined : parent.verticalCenter
      leftMargin: __style.margin10
      rightMargin: __style.margin10 + root.controlsRightInset
    }

    height: __style.row60

    MMComponents.MMRoundButton {
      id: playButton

      anchors {
        left: parent.left
        verticalCenter: parent.verticalCenter
      }

      bgndColor: __style.lightGreenColor
      iconColor: __style.forestColor
      iconSource: player.playbackState === MediaPlayer.PlayingState ? __style.pauseIcon : __style.playIcon

      onClicked: internal.togglePlayback()
    }

    Slider {
      id: seekBar

      anchors {
        left: playButton.right
        right: timeText.left
        leftMargin: __style.margin10
        rightMargin: __style.margin10
        verticalCenter: parent.verticalCenter
      }

      from: 0
      to: Math.max( player.duration, 1 )
      value: player.position
      enabled: player.seekable

      onMoved: player.setPosition( value )

      background: Rectangle {
        x: seekBar.leftPadding
        y: seekBar.topPadding + seekBar.availableHeight / 2 - height / 2
        width: seekBar.availableWidth
        height: __style.row4
        radius: __style.radius2

        color: __style.lightGreenColor

        Rectangle {
          height: parent.height
          width: seekBar.visualPosition * parent.width

          color: __style.forestColor
          radius: __style.radius2
        }
      }

      handle: Rectangle {
        x: seekBar.leftPadding + seekBar.visualPosition * ( seekBar.availableWidth - width )
        y: seekBar.topPadding + seekBar.availableHeight / 2 - height / 2
        width: 16 * __dp
        height: width
        radius: height / 2

        color: __style.forestColor
      }
    }

    MMComponents.MMText {
      id: timeText

      anchors {
        right: parent.right
        verticalCenter: parent.verticalCenter
      }

      font: __style.p6
      color: __style.nightColor
      text: internal.formatTime( player.position ) + " / " + internal.formatTime( player.duration )
    }
  }

  QtObject {
    id: internal

    function togglePlayback() {
      if ( player.playbackState === MediaPlayer.PlayingState ) {
        player.pause()
      }
      else {
        player.play()
      }
    }

    function formatTime( ms ) {
      const totalSeconds = Math.floor( Math.max( ms, 0 ) / 1000 )
      const minutes = Math.floor( totalSeconds / 60 )
      const seconds = totalSeconds % 60
      return minutes + ":" + ( seconds < 10 ? "0" : "" ) + seconds
    }
  }
}
