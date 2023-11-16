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
import QtMultimedia
import lc 1.0 as InputClass
import "./components"

Page {
  id: root

  signal backButtonClicked()
  signal scanFinished( var data )

  function unload() {
    // unlink camera and qr decoder
    // otherwise on iOS there are some
    // crashes time to time on back button click
    qrcodeScanner.videoSink = null
    camera.active = false
    captureSession.videoOutput = null
    captureSession.camera = null
  }

  header: PanelHeader {
    id: scannerPageHeader

    width: parent.width
    height: InputStyle.rowHeightHeader

    color: InputStyle.clrPanelMain
    rowHeight: InputStyle.rowHeightHeader

    titleText: qsTr( "Scan code" )

    onBack: {
      root.unload()
      root.backButtonClicked()
    }

    withBackButton: true
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


  InputClass.QrCodeDecoder {
    id: qrcodeScanner

    videoSink: videoOutput.videoSink

    onCodeScanned: function( codeData ) {
      root.unload()
      root.scanFinished( codeData )
    }
  }

  Rectangle {
    id: videoContainer

    width: root.width
    height: root.height - header.height

    VideoOutput {
      id: videoOutput

      anchors.fill: parent
      fillMode: VideoOutput.PreserveAspectCrop
    }

    CodeScannerOverlay {
      id: scannerOverlay

      rectSize: Math.min(root.height, root.width) * 0.8
      width: root.width
      height: root.height - header.height
    }
  }
}
