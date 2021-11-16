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
import "components"

Drawer {
  id: codeReader
  palette.dark: InputStyle.fontColor // changes busy indicator color

  signal scanFinished(var value)

  onVisibleChanged: {
    zxingFilter.active = codeReader.visible;
    if (zxingFilter.active) {
      camera.cameraState = Camera.ActiveState;
    } else
      camera.cameraState = Camera.UnloadedState;
  }

  CodeFilter {
    id: zxingFilter
    onCapturedDataChanged: {
      codeReader.scanFinished(capturedData);
      camera.cameraState = Camera.UnloadedState;
      codeReaderTimer.start();
    }
  }
  Camera {
    id: camera
    cameraState: Camera.UnloadedState

    onDeviceIdChanged: {
      focus.focusMode = CameraFocus.FocusContinuous;
      focus.focusPointMode = CameraFocus.FocusPointAuto;
    }
    onError: __inputUtils.showNotificationRequested(errorString)
  }
  ColumnLayout {
    height: codeReader.height
    spacing: 0
    width: codeReader.width

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
      color: InputStyle.clrPanelBackground
      height: codeReader.height - header.height
      width: codeReader.width

      VideoOutput {
        id: videoOutput
        autoOrientation: true
        fillMode: VideoOutput.PreserveAspectCrop
        filters: [zxingFilter]
        flushMode: VideoOutput.LastFrame
        height: codeReader.height
        source: camera
        width: codeReader.width
      }
      CodeReaderOverlay {
        id: overlay
        height: codeReader.height - header.height
        rectSize: Math.min(codeReader.height, codeReader.width) * 0.8
        width: codeReader.width
      }
    }
  }
  BusyIndicator {
    id: codeReaderBusyIndicator
    anchors.centerIn: parent
    height: width
    running: codeReaderTimer.running
    visible: running
    width: codeReader.width / 8
    z: codeReader.z + 1
  }
  Timer {
    id: codeReaderTimer
    interval: 1000
    repeat: false
    triggeredOnStart: false

    onTriggered: codeReader.visible = false
  }
}
