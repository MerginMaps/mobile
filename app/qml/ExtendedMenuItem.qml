import QtQuick 2.7
import QtGraphicalEffects 1.0
import QtQuick.Controls 2.2
import "."  // import InputStyle singleton

Item {
    id: root
    property string contentText: ""
    property string imageSource: ""
    property real rowHeight: InputStyle.rowHeight
    property real panelMargin: InputStyle.panelMargin
    property bool overlayImage: true
    property color fontColor: InputStyle.fontColor

    anchors.fill: parent

    Item {
        id: row
        anchors.fill: parent
        width: parent.width
        height: parent.height

        Item {
            id: iconContainer
            height: rowHeight
            width: rowHeight

            Image {
                id: icon
                anchors.fill: parent
                anchors.margins: rowHeight/4
                source: imageSource
                sourceSize.width: width
                sourceSize.height: height
                fillMode: Image.PreserveAspectFit
            }

            ColorOverlay {
                anchors.fill: icon
                source: icon
                color: root.fontColor
                visible: overlayImage
            }
        }

        Item {
            id: textContainer
            y: 0
            x: iconContainer.width + panelMargin
            width: parent.width - rowHeight
            height: rowHeight

            Text {
                id: mainText
                text: root.contentText
                height: parent.height
                width: parent.width

                font.pixelSize: InputStyle.fontPixelSizeNormal
                font.weight: Font.Bold
                color: root.fontColor
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

    Rectangle {
        id: borderLine
        color: root.fontColor
        width: row.width
        height: 1
        anchors.bottom: parent.bottom
    }
}
