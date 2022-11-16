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
import lc 1.0

import "./components"

Drawer {
  id: codeReader
  palette.dark: InputStyle.fontColor // changes busy indicator color

  signal scanFinished(var value)

  onVisibleChanged: {
    qrcodeScanner.setProcessing( codeReader.visible )
  }

  CodeScanner {
    id: qrcodeScanner

    videoSink: videoOutput.videoSink

    captureRect: Qt.rect(root.width / 4, root.height / 4, root.width / 2, root.height / 2)

    onCapturedStringChanged: function( captured ) {
      codeReader.scanFinished( captured )
      codeReaderTimer.start()
    }
  }

  onHeightChanged: {
    qrcodeScanner.captureRect = Qt.rect(width / 4, height / 4, width / 2, height / 2)
  }

  onWidthChanged: {
    qrcodeScanner.captureRect = Qt.rect(width / 4, height / 4, width / 2, height / 2)
  }

  ColumnLayout {
    width: codeReader.width
    height: codeReader.height
    spacing: 0

    PanelHeader {
      id: header
      Layout.fillWidth: true
      height: InputStyle.rowHeightHeader
      rowHeight: InputStyle.rowHeightHeader
      titleText: qsTr("Scan code")
      withBackButton: true
      onBack: codeReader.visible = false
    }

    Rectangle {
      id: videoContainer
      width: codeReader.width
      height: codeReader.height - header.height
      color: InputStyle.clrPanelBackground

      VideoOutput {
        id: videoOutput

        width: codeReader.width
        height: codeReader.height
        focus: visible
        fillMode: VideoOutput.PreserveAspectCrop
      }

      CodeScannerOverlay {
        id: scannerOverlay

        rectSize: Math.min(codeReader.height, codeReader.width) * 0.8
        width: codeReader.width
        height: codeReader.height - header.height
      }
    }
  }

  BusyIndicator {
    id: codeReaderBusyIndicator
    width: codeReader.width / 8
    height: width
    running: codeReaderTimer.running
    visible: running
    anchors.centerIn: parent
    z: codeReader.z + 1
  }

  Timer {
    id: codeReaderTimer
    interval: 1000
    triggeredOnStart: false
    repeat: false
    onTriggered: codeReader.visible = false
  }
}
