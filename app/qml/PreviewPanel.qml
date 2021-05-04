/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.0
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton

Item {
    id: previewPanel
    property real rowHeight: InputStyle.rowHeight
    property QgsQuick.AttributeController attributeController
    property string title: ""
    property string mapTipType: ""
    property string mapTipImage: ""
    property string mapTipHtml: ""

    property bool isReadOnly

    signal contentClicked()
    signal editClicked()

    MouseArea {
        anchors.fill: parent
        onClicked: {
            contentClicked()
        }
    }

    layer.enabled: true
    layer.effect: Shadow {}

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
                        elide: Qt.ElideRight
                    }

                    Item {
                        id: iconContainer
                        height: rowHeight
                        width: height
                        anchors.left: titleText.right
                        anchors.right: parent.right
                        visible: !previewPanel.isReadOnly

                        MouseArea {
                            id: editArea
                            anchors.fill: iconContainer
                            onClicked: editClicked()
                        }

                        Image {
                            id: icon
                            anchors.fill: parent
                            anchors.margins: rowHeight/4
                            anchors.rightMargin: 0
                            source: InputStyle.editIcon
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

                // we have three options what will be in the preview content: html content, image or field values

                Text {
                    visible: mapTipType == 'html'
                    text: mapTipHtml
                    anchors.fill: parent
                    anchors.topMargin: InputStyle.panelMargin
                }

                Image {
                    visible: mapTipType == 'image'
                    source: mapTipImage
                    sourceSize: Qt.size(width, height)
                    fillMode: Image.PreserveAspectFit
                    anchors.fill: parent
                    anchors.topMargin: InputStyle.panelMargin
                }

                ListView {
                    visible: mapTipType == 'fields'
                    model: controller.attributeFormPreviewModel
                    anchors.fill: parent
                    anchors.topMargin: InputStyle.panelMargin
                    spacing: 2 * QgsQuick.Utils.dp
                    interactive: false

                    delegate: Item {
                        id: root
                        width: parent.width
                        height: attributeController.attributeFormPreviewFields.indexOf(Name) >= 0 ? previewPanel.rowHeight/2 : 0
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
                            text: AttributeValue ? AttributeValue : ""
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
