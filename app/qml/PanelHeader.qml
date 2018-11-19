import QtQuick 2.7
import QtGraphicalEffects 1.0
Rectangle {
    id: header
    color: InputStyle.clrPanelMain

    property real rowHeight
    property string titleText: ""

    signal back()

    Item {
        id: backButton
        height: header.rowHeight
        width: height * 2
        anchors.left: header.left
        z: title.z + 1

        Image {
            id: image
            height: header.rowHeight
            width: height
            source: "Ic_chevron_left_48px.svg"
            fillMode: Image.PreserveAspectFit
        }

        ColorOverlay {
            anchors.fill: image
            source: image
            color: InputStyle.fontColor
        }

        Text {
            id: backButtonText
            text: "Back"
            color: InputStyle.fontColor
            font.pixelSize: InputStyle.fontPixelSizeTitle
            font.bold: true
            height: header.rowHeight
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft
            anchors.left: image.right
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.top: parent.top
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
