import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2


Item {
    id: previewPanel
    property real rowHeight: InputStyle.buttonSize

    property alias titleBorder: titleBorder
    property string title: ""

    signal contentClicked()

    MouseArea {
        anchors.fill: parent
        onClicked: contentClicked()
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // TODO empty photo container
        Rectangle {
            id: photoContainer
            Layout.fillWidth: true
            color: "transparent"
            height: 0
        }

        Rectangle {
            height: parent.height - photoContainer.height
            Layout.fillWidth: true

            Rectangle {
                anchors.fill: parent
                anchors.margins: 20

                height: parent.height/2
                Layout.fillWidth: true


                Item {
                    Layout.fillWidth: true
                    width: parent.width
                    height: previewPanel.rowHeight

                    Column {
                        Layout.fillWidth: true
                        width: parent.width
                        anchors.fill: parent
                        id: titleContainer

                        Item {
                            id: title
                            width: parent.width
                            height: parent.height - titleBorder.height
                            y:0
                            Text {
                                height: parent.height
                                text: previewPanel.title
                                font.pointSize: InputStyle.fontPointSizeBig
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
                                source: "home.svg"
                                anchors.right: parent.right
                                anchors.margins: height/4

                            }

                        }

                        Rectangle {
                            id: titleBorder
                            width: parent.width
                            height: 1
                            color: InputStyle.fontColor
                            anchors.bottom: titleContainer.bottom
                        }

                    }


                } //end title container

            }

        }


    }

}
