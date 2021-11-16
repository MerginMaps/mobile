/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.7
import QtQuick.Controls 2.7
import QtQuick.Layouts 1.3
import QtMultimedia 5.13
import lc 1.0

import "./components"

Drawer {
  id: codeReader
  palette.dark: InputStyle.fontColor // changes busy indicator color

  signal scanFinished(var value)

  onVisibleChanged: {
    zxingFilter.active = codeReader.visible
    if (zxingFilter.active) {
      camera.cameraState = Camera.ActiveState
    } else
      camera.cameraState = Camera.UnloadedState
  }

  CodeFilter {
    id: zxingFilter

    onCapturedDataChanged: {
      codeReader.scanFinished(capturedData)
      camera.cameraState = Camera.UnloadedState
      codeReaderTimer.start()
    }
  }

  Camera {
    id: camera
    onDeviceIdChanged: {
      focus.focusMode = CameraFocus.FocusContinuous
      focus.focusPointMode = CameraFocus.FocusPointAuto
    }
    cameraState: Camera.UnloadedState
    onError: __inputUtils.showNotificationRequested(errorString)
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
        filters: [zxingFilter]
        source: camera
        autoOrientation: true
        fillMode: VideoOutput.PreserveAspectCrop
        flushMode: VideoOutput.LastFrame
      }

      CodeReaderOverlay {
        id: overlay
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
