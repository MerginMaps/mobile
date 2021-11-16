import QtQuick 2.7
import QtQuick.Controls 2.7
import QtQuick.Layouts 1.3
import "../" // import InputStyle singleton

TextField {
  id: textField
  Layout.fillWidth: true
  Layout.preferredHeight: height
  color: InputStyle.fontColor
  font.capitalization: Font.MixedCase
  font.pixelSize: InputStyle.fontPixelSizeNormal
  height: InputStyle.rowHeight
  inputMethodHints: Qt.ImhNoPredictiveText
  leftPadding: InputStyle.innerFieldMargin
  rightPadding: InputStyle.innerFieldMargin

  background: Rectangle {
    anchors.fill: parent
    border.color: textField.activeFocus ? InputStyle.fontColor : InputStyle.panelBackgroundLight
    border.width: textField.activeFocus ? 2 : 1
    color: InputStyle.panelBackgroundLight
    radius: InputStyle.cornerRadius
  }
}
