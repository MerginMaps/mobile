import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0

Item {
    id: previewPanel
    property real rowHeight: InputStyle.scale(InputStyle.buttonSize)

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
            anchors.margins: InputStyle.scale(InputStyle.panelMargin)

            Item {
                width: parent.width
                height: previewPanel.rowHeight

                Item {
                    id: title
                    width: parent.width
                    height: parent.height - titleBorder.height
                    Text {
                        height: parent.height
                        text: previewPanel.title
                        font.pixelSize: InputStyle.fontPixelSizeNormal
                        color: InputStyle.fontColor
                        font.bold: true
                        anchors.left: parent.left
                        anchors.right: titleImage.left
                        horizontalAlignment: Text.AlignLeft
                        verticalAlignment: Text.AlignVCenter
                    }

                    Image {
                        id: titleImage
                        height: parent.height
                        width: height
                        source: "ic_edit_48px.svg"
                        anchors.right: parent.right
                        anchors.margins: height/4

                    }

                    ColorOverlay {
                        anchors.fill: titleImage
                        source: titleImage
                        color: InputStyle.highlightColor
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
