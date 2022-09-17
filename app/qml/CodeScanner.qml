import QtQuick
import QtQuick.Controls
import QtMultimedia

CodeScanner {
  id: codeScanner

  videoSink: videoOutput.videoSink

  captureRect: Qt.rect(root.width / 4, root.height / 4, root.width / 2, root.height / 2)

  function onCapturedStringChanged(capturedString) {
    scanResultText.text = capturedString
    barcodeScanner.setProcessing(false)
  }
}

VideoOutput {
  id: videoOutput

  anchors.fill: parent

  width: root.width

  focus: visible
  fillMode: VideoOutput.PreserveAspectCrop
}

