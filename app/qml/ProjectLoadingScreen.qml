import QtQuick 2.9
import QtQuick.Controls 2.2

Item {

  Rectangle {
    anchors.fill: parent
    color: InputStyle.fontColor
  }

  Image {
    id: logo
    anchors.centerIn: parent
    source: "input.svg"
    width: parent.width/2
    sourceSize.height: 0
    fillMode: Image.PreserveAspectFit
    sourceSize.width: width
  }

  Text {
    text: "Project loading ..."
    anchors.verticalCenterOffset: parent.height/6
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.verticalCenter: parent.verticalCenter
    font.pixelSize: InputStyle.fontPixelSizeNormal
    color: "white"
  }

}
