import QtQuick 2.4
import QtQuick.Window 2.2

import QtQuick.Controls 1.3
import QtGraphicalEffects 1.0

Rectangle {
  id: mainRect
  color: "white"
  width: 200
  height: 100

  property string textLine1
  property string textLine2
  
  Label {
    id: firstLine
    text: textLine1
    anchors.left: mainRect.left
    anchors.top: mainRect.top
    anchors.margins: 15
    font.pointSize: 20
  }
  
  Label {
    id: secondLine
    text: textLine2
    anchors.left: mainRect.left
    anchors.top: firstLine.bottom
    anchors.leftMargin: 15
    anchors.topMargin: 5
    color: "gray"
  }
}
