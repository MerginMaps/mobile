/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.3
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0

import ".."  // import InputStyle singleton
import "../components" as Components
import lc 1.0

Item {
    id: previewPanel
    property real rowHeight: InputStyle.rowHeight
    property AttributePreviewController controller

    property bool isReadOnly

    signal contentClicked()
    signal editClicked()
    signal stakeoutFeature( var feature )

    MouseArea {
      anchors.fill: parent
      onClicked: {
        contentClicked()
      }
    }

    layer.enabled: true
    layer.effect: Components.Shadow {}

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

                Row {
                  id: title
                  height: rowHeight
                  width: parent.width

                  Text {
                      id: titleText
                      height: rowHeight
                      width: parent.width - 2 * rowHeight
                      text: controller.title
                      font.pixelSize: InputStyle.fontPixelSizeBig
                      color: InputStyle.fontColor
                      font.bold: true
                      horizontalAlignment: Text.AlignLeft
                      verticalAlignment: Text.AlignVCenter
                      elide: Qt.ElideRight
                  }

                  Item {
                      id: stakeoutIconContainerSpace
                      height: rowHeight
                      width: rowHeight

                      Item {
                        visible: __inputUtils.isPointLayerFeature( controller.featureLayerPair )
                        enabled: visible

                        anchors.fill: parent

                        MouseArea {
                            anchors.fill: parent
                            onClicked: previewPanel.stakeoutFeature( controller.featureLayerPair )
                        }

                        Image {
                            id: stakeoutIcon

                            anchors.fill: parent
                            anchors.margins: rowHeight/8
                            anchors.rightMargin: 0
                            source: InputStyle.stakeoutIcon
                            sourceSize.width: width
                            sourceSize.height: height
                            fillMode: Image.PreserveAspectFit
                        }

                        ColorOverlay {
                            anchors.fill: stakeoutIcon
                            source: stakeoutIcon
                            color: InputStyle.fontColor
                        }
                      }
                  }

                  Item {
                      id: iconContainer
                      height: rowHeight
                      width: rowHeight
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
                    visible: controller.type == AttributePreviewController.Empty
                    text: qsTr("No map tip available.")
                    anchors.fill: parent
                    anchors.topMargin: InputStyle.panelMargin
                }

                Text {
                    visible: controller.type == AttributePreviewController.HTML
                    text: controller.html
                    anchors.fill: parent
                    anchors.topMargin: InputStyle.panelMargin
                }

                Image {
                    visible: controller.type == AttributePreviewController.Photo
                    source: controller.photo
                    sourceSize: Qt.size(width, height)
                    fillMode: Image.PreserveAspectFit
                    anchors.fill: parent
                    anchors.topMargin: InputStyle.panelMargin
                }

                ListView {
                    visible: controller.type == AttributePreviewController.Fields
                    model: controller.fieldModel
                    anchors.fill: parent
                    anchors.topMargin: InputStyle.panelMargin
                    spacing: 2 * __dp
                    interactive: false

                    delegate: Row {
                        id: root
                        spacing: InputStyle.panelMargin
                        width: parent.width

                        Text {
                            id: fieldName
                            text: Name
                            width: root.width/2
                            font.pixelSize: InputStyle.fontPixelSizeNormal
                            color: InputStyle.fontColorBright
                            elide: Text.ElideRight
                        }

                        Text {
                            id: fieldValue
                            text: Value ? Value : ""
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
