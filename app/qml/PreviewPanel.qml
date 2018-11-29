import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton

Item {
    id: previewPanel
    property real rowHeight: InputStyle.rowHeight
    property QgsQuick.AttributeFormModel model

    property alias titleBorder: titleBorder
    property string title: ""
    property variant previewFields: []

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
            anchors.topMargin: 0

            Item {
                id: header
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
                            anchors.rightMargin: 0
                            source: "edit.svg"
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

            Item {
                id: content
                width: parent.width
                anchors.top: header.bottom
                anchors.bottom: parent.bottom

                ListView {
                    model: previewPanel.model
                    anchors.fill: parent
                    anchors.topMargin: InputStyle.panelMargin
                    spacing: 2 * QgsQuick.Utils.dp

                    delegate: Item {
                        id: root
                        width: parent.width
                        height:previewFields.indexOf(Name) >= 0 ? previewPanel.rowHeight/2 : 0
                        visible: height

                        Text {
                            id: fieldName
                            text: Name
                            width: root.width/2
                            height: root.height
                            font.pixelSize: InputStyle.fontPixelSizeNormal
                            color: InputStyle.fontColorBright
                            elide: Text.ElideRight
                            anchors.rightMargin: InputStyle.panelMargin
                        }

                        Text {
                            id: text2
                            text: AttributeValue
                            anchors.left: fieldName.right
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            anchors.top: parent.top
                            height: root.height
                            font.pixelSize: InputStyle.fontPixelSizeNormal
                            color: InputStyle.fontColor
                            elide: Text.ElideRight
                        }
                    }
                }
            }
        }
    }
}
