import QtQuick 2.7
import QtGraphicalEffects 1.0
Item {
    id: root
    property string text: ""
    property string imageSource: ""
    property real rowHeight: InputStyle.rowHeight
    property real panelMargin: 0
    property bool overlayImage: true
    anchors.fill: parent

    Item {
        id: row
        anchors.fill: parent
        width: parent.width
        height: parent.height
        anchors.rightMargin: panelMargin
        anchors.leftMargin: panelMargin

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
                color: InputStyle.fontColor
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
                text: root.text
                height: parent.height
                width: parent.width

                font.pixelSize: InputStyle.fontPixelSizeSmall
                font.weight: Font.Bold
                color: InputStyle.fontColor
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
            }
        }
    }

    Rectangle {
        id: borderLine
        color: InputStyle.fontColor
        width: row.width
        height: 1
        anchors.bottom: parent.bottom
    }
}
