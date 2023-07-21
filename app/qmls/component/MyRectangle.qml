import QtQuick
import QtQuick.Controls
import "../Style.js" as Style

Rectangle {
  anchors.centerIn: parent
  color: "white"
  width: 200
  height: 200
  radius: 20
  Text {
    anchors.centerIn: parent
    text: Style.dynamicText()
    color: Style.textColor
  }
}

