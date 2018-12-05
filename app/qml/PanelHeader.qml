import QtQuick 2.7
import QtGraphicalEffects 1.0
import "."  // import InputStyle singleton

Rectangle {
    id: header
    color: InputStyle.clrPanelMain

    property real rowHeight
    property string titleText: ""
    property bool withBackButton: true

    signal back()

    Item {
        id: backButton
        height: header.rowHeight
        width: height * 2
        anchors.left: header.left
        anchors.leftMargin: InputStyle.panelMargin
        z: title.z + 1
        visible: withBackButton

        Image {
            id: image
            height: InputStyle.fontPixelSizeTitle
            width: height
            source: "back.svg"
            sourceSize.width: width
            sourceSize.height: height
            fillMode: Image.PreserveAspectFit
            anchors.bottomMargin: (header.rowHeight - height)/2
            anchors.topMargin: (header.rowHeight - height)/2
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.top: parent.top

        }

        ColorOverlay {
            anchors.fill: image
            source: image
            color: InputStyle.fontColorBright
        }

        Text {
            id: backButtonText
            text: "Back"
            color: InputStyle.fontColorBright
            font.pixelSize: InputStyle.fontPixelSizeTitle
            height: header.rowHeight
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            anchors.left: image.right
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.top: parent.top
            anchors.leftMargin: header.rowHeight/4
        }

        MouseArea {
            anchors.fill: parent
            onClicked: back()
        }
    }

    Text {
        id: title
        anchors.fill: parent
        text: header.titleText
        color: InputStyle.fontColor
        font.pixelSize: InputStyle.fontPixelSizeTitle
        font.bold: true
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

}
