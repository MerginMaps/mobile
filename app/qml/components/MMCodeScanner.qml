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

Drawer {
  id: root

  signal scanFinished( var data )

  width: ApplicationWindow.window.width
  height: ApplicationWindow.window.height + 40 * __dp
  edge: Qt.BottomEdge
  dim: true
  interactive: false
  dragMargin: 0
  closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

  CaptureSession {
    id: captureSession

    camera: Camera {
      id: camera
      active: true
      focusMode: Camera.FocusModeAutoNear
    }
    videoOutput: videoOutput
  }

  VideoOutput {
    id: videoOutput

    anchors.fill: parent
    fillMode: VideoOutput.PreserveAspectCrop
  }

  MM.QrCodeDecoder {
    id: qrcodeScanner

    videoSink: videoOutput.videoSink

    onCodeScanned: function( codeData ) {
      root.unload()
      root.scanFinished( codeData )
    }
  }

  function unload() {
    qrcodeScanner.videoSink = null
    camera.active = false
    captureSession.videoOutput = null
    captureSession.camera = null
  }

  Item {
    id: scannerText

    width: parent.width
    height: (parent.width < parent.height) ? parent.height / 2 - parent.width / 4 : parent.height / 4
    anchors.horizontalCenter: parent.horizontalCenter

    Column {
      id: textColumn

      width: parent.width - 40 * __dp
      anchors.centerIn: parent
      spacing: 10 * __dp

      Text {
        width: parent.width
        text: qsTr("Scan the QR code")
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font: __style.t1
        wrapMode: Text.WordWrap
        color: __style.whiteColor
      }

      Text {
        width: parent.width
        text: qsTr("Please make sure that the lense is clear.")
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font: __style.p5
        wrapMode: Text.WordWrap
        color: __style.whiteColor
      }
    }
  }

  Image {
    id: closeButton

    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 2 * __style.pageMargins
    source: __style.closeIcon

    MouseArea {
      anchors.fill: parent
      onClicked: {
        unload()
        close()
      }
    }
  }
}
