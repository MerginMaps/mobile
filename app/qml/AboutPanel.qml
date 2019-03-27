import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import QtGraphicalEffects 1.0
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton

Item {
    property string inputLink: "https://inputapp.io/"

    property real fieldHeight: InputStyle.rowHeight
    property real panelMargin: fieldHeight/4
    property color fontColor: "white"

    function close() {
        visible = false
    }

    id: root

    Pane {
        id: pane

        width: parent.width
        height: parent.height
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter


        background: Rectangle {
            color: InputStyle.fontColor
        }

        Item {
            id: content
            anchors.fill: parent
            anchors.bottomMargin: Qt.inputMethod.keyboardRectangle.height ? Qt.inputMethod.keyboardRectangle.height: 0


            Column {
                id: columnLayout
                anchors.verticalCenter: parent.verticalCenter
                width: parent.width

                Image {
                    id: inputLogo
                    source: "input.svg"
                    width: content.width/2
                    sourceSize.width: width
                    anchors.horizontalCenter: parent.horizontalCenter
                }

                Text {
                    text: "v" + __version
                    font.pixelSize: inputLinkBtn.height/2
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: fontColor
                }

                Button {
                    id: inputLinkBtn
                    width: content.width - 2* root.panelMargin
                    height: fieldHeight * 0.7
                    text: root.inputLink
                    font.pixelSize: inputLinkBtn.height/2
                    anchors.horizontalCenter: parent.horizontalCenter
                    onClicked:Qt.openUrlExternally(root.inputLink);
                    background: Rectangle {
                        color: InputStyle.fontColor
                    }

                    contentItem: Text {
                        text: inputLinkBtn.text
                        font: inputLinkBtn.font
                        color: InputStyle.highlightColor
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                        elide: Text.ElideRight
                    }
                }
            }

            Text {
                id: developedText
                text: "Developed by"
                font.pixelSize: inputLinkBtn.height/2
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: lutraLogo.top
                color: fontColor
            }

            Image {
                id: lutraLogo
                source: "lutra_logo.svg"
                width: inputLogo.width/2
                sourceSize.width: width
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: InputStyle.panelMargin * 2
            }

            ColorOverlay {
                anchors.fill: lutraLogo
                source: lutraLogo
                color: fontColor
            }
        }
    }
}
