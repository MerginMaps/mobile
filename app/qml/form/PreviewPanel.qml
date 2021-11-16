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
import "../"  // import InputStyle singleton
import "../components" as Components
import lc 1.0

Item {
  id: previewPanel
  property AttributePreviewController controller
  property bool isReadOnly
  property real rowHeight: InputStyle.rowHeight

  layer.enabled: true

  signal contentClicked
  signal editClicked

  MouseArea {
    anchors.fill: parent

    onClicked: {
      contentClicked();
    }
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
        height: previewPanel.rowHeight
        width: parent.width

        Item {
          id: title
          height: parent.height - titleBorder.height
          width: parent.width

          Text {
            id: titleText
            color: InputStyle.fontColor
            elide: Qt.ElideRight
            font.bold: true
            font.pixelSize: InputStyle.fontPixelSizeTitle
            height: parent.height
            horizontalAlignment: Text.AlignLeft
            text: controller.title
            verticalAlignment: Text.AlignVCenter
            width: parent.width - rowHeight
          }
          Item {
            id: iconContainer
            anchors.left: titleText.right
            anchors.right: parent.right
            height: rowHeight
            visible: !previewPanel.isReadOnly
            width: height

            MouseArea {
              id: editArea
              anchors.fill: iconContainer

              onClicked: editClicked()
            }
            Image {
              id: icon
              anchors.fill: parent
              anchors.margins: rowHeight / 4
              anchors.rightMargin: 0
              fillMode: Image.PreserveAspectFit
              source: InputStyle.editIcon
              sourceSize.height: height
              sourceSize.width: width
            }
            ColorOverlay {
              anchors.fill: icon
              color: InputStyle.fontColor
              source: icon
            }
          }
        }
        Rectangle {
          id: titleBorder
          anchors.bottom: title.bottom
          color: InputStyle.fontColor
          height: 1
          width: parent.width
        }
      }
      Item {
        id: content
        anchors.bottom: parent.bottom
        anchors.top: header.bottom
        width: parent.width

        // we have three options what will be in the preview content: html content, image or field values
        Text {
          anchors.fill: parent
          anchors.topMargin: InputStyle.panelMargin
          text: qsTr("No map tip available.")
          visible: controller.type == AttributePreviewController.Empty
        }
        Text {
          anchors.fill: parent
          anchors.topMargin: InputStyle.panelMargin
          text: controller.html
          visible: controller.type == AttributePreviewController.HTML
        }
        Image {
          anchors.fill: parent
          anchors.topMargin: InputStyle.panelMargin
          fillMode: Image.PreserveAspectFit
          source: controller.photo
          sourceSize: Qt.size(width, height)
          visible: controller.type == AttributePreviewController.Photo
        }
        ListView {
          anchors.fill: parent
          anchors.topMargin: InputStyle.panelMargin
          interactive: false
          model: controller.fieldModel
          spacing: 2 * QgsQuick.Utils.dp
          visible: controller.type == AttributePreviewController.Fields

          delegate: Row {
            id: root
            spacing: InputStyle.panelMargin
            width: parent.width

            Text {
              id: fieldName
              color: InputStyle.fontColorBright
              elide: Text.ElideRight
              font.pixelSize: InputStyle.fontPixelSizeNormal
              text: Name
              width: root.width / 2
            }
            Text {
              id: fieldValue
              color: InputStyle.fontColor
              elide: Text.ElideRight
              font.pixelSize: InputStyle.fontPixelSizeNormal
              text: Value ? Value : ""
            }
          }
        }
      }
    }
  }

  layer.effect: Components.Shadow {
  }
}
