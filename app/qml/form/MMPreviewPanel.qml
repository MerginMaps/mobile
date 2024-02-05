/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
//import Qt5Compat.GraphicalEffects

//import ".."  // import InputStyle singleton
import "../components" as Components
import lc 1.0

Item {
  id: root

  property bool layerIsReadOnly: false
  property AttributePreviewController controller

  signal contentClicked()
  signal editClicked()
  signal stakeoutClicked( var feature ) // TODO: remove the feature

  ColumnLayout {
    anchors {
      fill: parent
      margins: __style.pageMargins
    }

    spacing: 20 * __dp

    Item {
      id: photoContainer

      Layout.fillWidth: true
      Layout.preferredHeight: 160 * __dp

      visible: root.controller.type === AttributePreviewController.Photo

      Components.MMPhoto {
        width: parent.width
        height: parent.height

        photoUrl: root.controller.photo

        fillMode: Image.PreserveAspectCrop
      }
    }

    Text {
      Layout.fillWidth: true
      Layout.preferredHeight: paintedHeight

      text: root.controller.title

      font: __style.t1
      color: __style.forestColor

      horizontalAlignment: Text.AlignLeft
      verticalAlignment: Text.AlignVCenter

      maximumLineCount: 2
      elide: Text.ElideRight
      wrapMode: Text.WordWrap
    }

    // TODO: HTML type

    Item {
      id: buttonGroup

      Layout.fillWidth: true
      Layout.preferredHeight: 40 * __dp

    }

    Item {
      Layout.fillHeight: true
      Layout.fillWidth: true
    }
  }

  MouseArea {
    anchors.fill: parent
    onClicked: function( mouse ) {
      mouse.accepted = true
      root.contentClicked()
    }
  }


//    property real rowHeight: InputStyle.rowHeight



//    MouseArea {
//      anchors.fill: parent
//      onClicked: {
//        contentClicked()
//      }
//    }

//    layer.enabled: true
//    layer.effect: Components.Shadow {}

//    Rectangle {
//        anchors.fill: parent
//        color: InputStyle.clrPanelMain

//        Rectangle {
//            anchors.fill: parent
//            anchors.margins: InputStyle.panelMargin
//            anchors.topMargin: 0

//            Item {
//                id: header
//                width: parent.width
//                height: previewPanel.rowHeight

//                Row {
//                  id: title
//                  height: rowHeight
//                  width: parent.width

//                  Text {
//                    id: titleText

////                    height: rowHeight
//                    width: parent.width

//                    text: controller.title

//                    wrapMode: Text.WordWrap
//                    maximumLineCount: 2
//                    elide: Text.ElideRight

//                    font: __style.t1

//                    color: __style.forestColor

//                    horizontalAlignment: Text.AlignLeft
//                    verticalAlignment: Text.AlignVCenter
//                  }

////                  Button {
////                    id: stakeoutIconContainerSpace
////                    height: rowHeight
////                    width: rowHeight

////                    background: Item {
////                      visible: __inputUtils.isPointLayerFeature( controller.featureLayerPair )
////                      enabled: visible

////                      anchors.fill: parent

////                      Image {
////                        id: stakeoutIcon

////                        anchors.fill: parent
////                        anchors.margins: rowHeight/8
////                        anchors.rightMargin: 0
////                        source: InputStyle.stakeoutIcon
////                        sourceSize.width: width
////                        sourceSize.height: height
////                        fillMode: Image.PreserveAspectFit
////                      }

////                      ColorOverlay {
////                        anchors.fill: stakeoutIcon
////                        source: stakeoutIcon
////                        color: InputStyle.fontColor
////                      }
////                    }

////                    onClicked: previewPanel.stakeoutFeature( controller.featureLayerPair )
////                  }

////                  Button {
////                    id: iconContainer
////                    height: rowHeight
////                    width: rowHeight
////                    visible: !previewPanel.isReadOnly

////                    background: Item {
////                      anchors.fill: parent

////                      Image {
////                        id: icon
////                        anchors.fill: parent
////                        anchors.margins: rowHeight/4
////                        anchors.rightMargin: 0
////                        source: InputStyle.editIcon
////                        sourceSize.width: width
////                        sourceSize.height: height
////                        fillMode: Image.PreserveAspectFit
////                      }

////                      ColorOverlay {
////                        anchors.fill: icon
////                        source: icon
////                        color: InputStyle.fontColor
////                      }
////                    }

////                    onClicked: editClicked()
////                  }
//                }

////                Rectangle {
////                    id: titleBorder
////                    width: parent.width
////                    height: 1
////                    color: InputStyle.fontColor
////                    anchors.bottom: title.bottom
////                }
//            }

//            Item {
//                id: content
//                width: parent.width
//                anchors.top: header.bottom
//                anchors.bottom: parent.bottom

//                // we have three options what will be in the preview content: html content, image or field values

//                Text {
//                    visible: controller.type == AttributePreviewController.Empty
//                    text: qsTr("No map tip available.")
//                    anchors.fill: parent
//                    anchors.topMargin: InputStyle.panelMargin
//                }

//                Text {
//                    visible: controller.type == AttributePreviewController.HTML
//                    text: controller.html
//                    anchors.fill: parent
//                    anchors.topMargin: InputStyle.panelMargin
//                }

//                Image {
//                    visible: controller.type == AttributePreviewController.Photo
//                    source: controller.photo
//                    sourceSize: Qt.size(width, height)
//                    fillMode: Image.PreserveAspectFit
//                    autoTransform : true
//                    anchors.fill: parent
//                    anchors.topMargin: InputStyle.panelMargin
//                }

//                ListView {
//                    visible: controller.type == AttributePreviewController.Fields
//                    model: controller.fieldModel
//                    anchors.fill: parent
//                    anchors.topMargin: InputStyle.panelMargin
//                    spacing: 2 * __dp
//                    interactive: false

//                    delegate: Row {
//                        id: root
//                        spacing: InputStyle.panelMargin
//                        width: ListView.view.width

//                        Text {
//                            id: fieldName
//                            text: Name
//                            width: root.width / 2
//                            font.pixelSize: InputStyle.fontPixelSizeNormal
//                            color: InputStyle.fontColorBright
//                            elide: Text.ElideRight
//                        }

//                        Text {
//                            id: fieldValue
//                            text: Value ? Value : ""
//                            width: root.width / 2 - root.spacing
//                            font.pixelSize: InputStyle.fontPixelSizeNormal
//                            color: InputStyle.fontColor
//                            elide: Text.ElideRight

//                        }
//                    }
//                }
//            }
//        }
//    }
}
