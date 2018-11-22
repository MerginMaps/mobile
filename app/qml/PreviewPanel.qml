import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import "."  // import InputStyle singleton

Item {
    id: previewPanel
    property real rowHeight: InputStyle.rowHeight

    property alias titleBorder: titleBorder
    property string title: ""

    signal contentClicked()

    MouseArea {
        anchors.fill: parent
        onClicked: contentClicked()
    }

    Rectangle {
        anchors.fill: parent
        color: InputStyle.clrPanelMain

        Rectangle {
            anchors.fill: parent
            anchors.margins: InputStyle.panelMargin

            Item {
                width: parent.width
                height: previewPanel.rowHeight

                Item {
                    id: title
                    width: parent.width
                    height: parent.height - titleBorder.height
                    Text {
                        id: titleText
                        height: parent.height
                        width: parent.width - rowHeight
                        text: previewPanel.title
                        font.pixelSize: InputStyle.fontPixelSizeTitle
                        color: InputStyle.fontColor
                        font.bold: true
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                    }

                    Item {
                        id: iconContainer
                        height: rowHeight
                        width: height
                        anchors.left: titleText.right
                        anchors.right: parent.right

                        Image {
                            id: icon
                            anchors.fill: parent
                            anchors.margins: rowHeight/4
                            source: "ic_edit_48px.svg"
                            sourceSize.width: width
                            sourceSize.height: height
                            fillMode: Image.PreserveAspectFit
                        }

                        ColorOverlay {
                            anchors.fill: icon
                            source: icon
                            color: InputStyle.fontColor
                        }
                    }
                }

                Rectangle {
                    id: titleBorder
                    width: parent.width
                    height: 1
                    color: InputStyle.fontColor
                    anchors.bottom: title.bottom
                }

            }

        }

    }

}
