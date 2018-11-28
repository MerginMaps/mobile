import QtQuick 2.0
import "."  // import InputStyle singleton

Rectangle {
    id: root
    height: InputStyle.rowHeight
    width: parent.width
    color: InputStyle.clrPanelMain

    property string text: ""
    property bool bold: false

    Text {
        text: root.text
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.leftMargin: InputStyle.panelMargin
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        color: InputStyle.fontColor
        font.bold: bold
        font.pixelSize: InputStyle.fontPixelSizeNormal
        elide: Text.ElideLeft
    }
}
