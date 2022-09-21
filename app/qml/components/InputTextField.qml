import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "./.." // import InputStyle singleton

TextField {

  id: textField
  height: InputStyle.rowHeight
  Layout.fillWidth: true
  Layout.preferredHeight: height
  leftPadding: InputStyle.innerFieldMargin
  rightPadding: InputStyle.innerFieldMargin
  color: InputStyle.fontColor
  font.capitalization: Font.MixedCase
  font.pixelSize: InputStyle.fontPixelSizeNormal
  inputMethodHints: Qt.ImhNoPredictiveText

  background: Rectangle {
    anchors.fill: parent
    border.color: textField.activeFocus ? InputStyle.fontColor : InputStyle.panelBackgroundLight
    border.width: textField.activeFocus ? 2 : 1
    color: InputStyle.panelBackgroundLight
    radius: InputStyle.cornerRadius
  }
}
