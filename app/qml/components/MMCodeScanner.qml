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
import lc 1.0 as InputClass

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

  InputClass.QrCodeDecoder {
    id: qrcodeScanner

    videoSink: videoOutput.videoSink

    onCodeScanned: function( codeData ) {
      root.unload()
      root.scanFinished( codeData )
    }
  }

  Canvas {
    id: canvas

    anchors.fill: parent
    opacity: 0.8

    onPaint: {
      var ctx = getContext("2d");
      let w = parent.width
      let h = parent.height
      ctx.fillStyle = __style.nightColor
      ctx.fillRect(0, 0, parent.width, parent.height);
      ctx.fill();
      if(parent.width < parent.height)
        ctx.clearRect(w / 4, h / 2 - w / 4, w / 2, w / 2)
      else
        ctx.clearRect(w / 2 - h / 4, h / 4, h / 2, h / 2)
    }
  }

  Item {
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
    anchors.bottomMargin: 20 * __dp
    source: __style.closeButtonIcon

    MouseArea {
      anchors.fill: parent
      onClicked: {
        unload()
        close()
      }
    }
  }

  function unload() {
    qrcodeScanner.videoSink = null
    camera.active = false
    captureSession.videoOutput = null
    captureSession.camera = null
  }
}
