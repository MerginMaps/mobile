import QtQuick
import QtQuick.Controls
import ".." // import InputStyle singleton

Rectangle {
  id: root

  property alias text: errorText.text

  anchors.horizontalCenter: parent.horizontalCenter
  color: "transparent"
  border.color: InputStyle.errorTextBorderColor
  height: errorText.height
  visible: errorText.text.length > 0

  Text {
    id: errorText
    width: parent.width
    padding: InputStyle.errorTextPadding
    wrapMode: Text.WordWrap
    color: InputStyle.errorTextColor
    font.pixelSize: InputStyle.fontPixelSizeNormal
  }
}
