import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick 2.0
import "."  // import InputStyle singleton

TabButton {
    id: button

    //property string contentText: ""
    property real horizontalAlignment: Text.AlignHCenter

    height: projectMenuButtons.height
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 0
    anchors.top: parent.top
    anchors.topMargin: 0
    // overwritting TabButton default background
    background: Item {
        anchors.fill: parent
    }

    contentItem: Text {
        anchors.fill: parent
        anchors.margins: InputStyle.panelMargin
        text: button.text
        horizontalAlignment: button.horizontalAlignment
        verticalAlignment: Text.AlignVCenter
        color: InputStyle.fontColor
        opacity: button.checked ? 1 : 0.7
        font.underline: button.checked
        font.bold: true
        font.pixelSize: InputStyle.fontPixelSizeNormal
    }

}
