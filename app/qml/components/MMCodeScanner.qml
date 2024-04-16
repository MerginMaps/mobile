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

import mm 1.0 as MM

Popup {
  id: root

  parent: Overlay.overlay
  visible: true
  height: ApplicationWindow.window?.height ?? 0
  width: ApplicationWindow.window?.width ?? 0
  closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

  signal scanFinished( var data )

  function unload() {
    qrcodeScanner.videoSink = null
    camera.active = false
    captureSession.videoOutput = null
    captureSession.camera = null
  }

  CaptureSession {
    id: captureSession

    camera: Camera {
      id: camera
      active: true
      focusMode: Camera.FocusModeAutoNear
    }
    videoOutput: videoOutput
  }

  MM.QrCodeDecoder {
    id: qrcodeScanner

    videoSink: videoOutput.videoSink

    onCodeScanned: function( codeData ) {
      root.unload()
      root.scanFinished( codeData )
    }
  }

  contentItem: Item {
    anchors.fill: parent

    MMBusyIndicator {
      anchors.centerIn: parent
      visible: true
    }

    VideoOutput {
      id: videoOutput

      anchors.fill: parent
      fillMode: VideoOutput.PreserveAspectCrop
    }

    Item {
      x: __style.safeAreaLeft
      y: __style.safeAreaTop
      width: parent.width - __style.safeAreaLeft - __style.safeAreaRight
      height: parent.height - __style.safeAreaBottom - __style.safeAreaTop

      MMInfoBox {
        width: Math.min(parent.width - 2 * __style.spacing20, 353 * __dp)
        height: __style.row92

        anchors {
          horizontalCenter: parent.horizontalCenter
          top: parent.top
          topMargin: __style.spacing20
        }

        title: qsTr( "Scan the QR code" )
        description: qsTr( "Please make sure that the lense is clear." )
        imageSource: __style.blueInfoImage
      }

      MMRoundButton {
        id: closeButton

        anchors {
          horizontalCenter: parent.horizontalCenter
          bottom: parent.bottom
          bottomMargin: __style.spacing20
        }

        bgndColor: __style.lightGreenColor
        iconSource: __style.closeIcon
        onClicked: {
          root.unload()
          close()
        }
      }
    }
  }
}
