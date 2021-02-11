import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtMultimedia 5.7
import lc 1.0

Page {
  id: codeReader

  signal scanFinished(var value)

  onVisibleChanged: {
    if (codeReader.visible) {
      zxingFilter.active = true
    }
  }

  CodeFilter {
    id: zxingFilter

    onCapturedDataChanged: {
      active = false
      codeReader.visible = false
      codeReader.scanFinished(capturedData)
    }
  }

  Camera {
    id: camera
    onDeviceIdChanged: {
      focus.focusMode = CameraFocus.FocusContinuous
      focus.focusPointMode = CameraFocus.FocusPointAuto
    }

    onError: console.log("camera error:" + errorString)
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
        fillMode: VideoOutput.PreserveAspectFit
      }

      CodeReaderOverlay {
        id: overlay
        rectSize: Math.min(codeReader.height, codeReader.width) * 0.8
        width: codeReader.width
        height: codeReader.height
      }
    }
  }


}
