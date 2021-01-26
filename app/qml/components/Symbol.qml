import QtQuick 2.7
import QtGraphicalEffects 1.0
import "./.." // import InputStyle singleton

Rectangle {

  property real iconSize: InputStyle.rowHeight
  property string source: ""
  property color bgColor: "transparent"
  property color fontColor: InputStyle.fontColor

  id: iconContainer
  height: iconContainer.iconSize
  width: iconContainer.iconSize
  color: iconContainer.bgColor

  Image {
    id: icon
    anchors.centerIn: parent
    source: iconContainer.source
    width: iconContainer.iconSize
    height: iconContainer.iconSize
    sourceSize.width: width
    sourceSize.height: height
    fillMode: Image.PreserveAspectFit
  }

  ColorOverlay {
    anchors.fill: icon
    source: icon
    color: iconContainer.fontColor
  }
}
