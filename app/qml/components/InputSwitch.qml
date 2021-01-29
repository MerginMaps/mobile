import QtQuick 2.0
import QtQuick.Controls 2.2

import "../" // import InputStyle singleton

Switch {
  id: root

  checked: false

  anchors.margins: 0
  padding: 0
  height: InputStyle.fontPixelSizeNormal
  width: height * 2
  anchors.verticalCenter: parent.verticalCenter
  anchors.right: parent.right
  anchors.rightMargin: InputStyle.panelMargin

  property color highlighColor: InputStyle.softGreen
  property color disabledColor: InputStyle.panelBackgroundDark

  indicator: Rectangle {
    implicitWidth: parent.width
    implicitHeight: parent.height
    x: root.leftPadding
    y: parent.height / 2 - height / 2
    radius: parent.height/2
    color: root.checked ? InputStyle.softGreen : "#ffffff"
    border.color: root.checked ? InputStyle.softGreen : root.disabledColor

    Rectangle {
      x: root.checked ? parent.width - width : 0
      width: parent.height
      height: parent.height
      radius: parent.height/2
      color: "#ffffff"
      border.color: root.checked ? InputStyle.softGreen : root.disabledColor
    }
  }
}
