/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import lc 1.0
import QgsQuick 0.1 as QgsQuick
import "components"

Item {
  id: statusPanel
  property real rowHeight: InputStyle.rowHeight * 1.2

  signal back
  function open(projectFullName) {
    if (__merginProjectStatusModel.loadProjectInfo(projectFullName)) {
      statusPanel.visible = true;
    } else
      __inputUtils.showNotification(qsTr("No Changes"));
  }

  // background
  Rectangle {
    color: InputStyle.clrPanelMain
    height: parent.height
    width: parent.width
  }
  PanelHeader {
    id: header
    color: InputStyle.clrPanelMain
    height: InputStyle.rowHeightHeader
    rowHeight: InputStyle.rowHeightHeader
    titleText: qsTr("Project Status")
    width: parent.width
    withBackButton: true
    z: contentLayout.z + 1

    onBack: {
      statusPanel.back();
    }
  }

  // Content
  ColumnLayout {
    id: contentLayout
    height: statusPanel.height - header.height
    spacing: 0
    width: statusPanel.width
    y: header.height

    ListView {
      id: statusList
      Layout.fillHeight: true
      Layout.fillWidth: true
      model: __merginProjectStatusModel
      section.criteria: ViewSection.FullString
      section.property: "section"

      delegate: Item {
        id: delegateItem
        height: fileStatus === MerginProjectStatusModel.Changelog ? statusPanel.rowHeight * 1.2 : statusPanel.rowHeight
        width: parent.width

        RowLayout {
          id: row
          anchors.fill: parent
          anchors.leftMargin: InputStyle.panelMargin / 2
          anchors.rightMargin: InputStyle.panelMargin / 2
          spacing: 0

          Item {
            id: textContainer
            Layout.fillWidth: true
            height: statusPanel.rowHeight

            Text {
              id: mainText
              color: InputStyle.fontColor
              elide: Text.ElideRight
              font.bold: true
              font.pixelSize: InputStyle.fontPixelSizeNormal
              height: fileStatus === MerginProjectStatusModel.Changelog ? textContainer.height / 2 : textContainer.height
              horizontalAlignment: Text.AlignLeft
              text: itemText
              verticalAlignment: fileStatus === MerginProjectStatusModel.Changelog ? Text.AlignBottom : Text.AlignVCenter
              width: textContainer.width
            }
            Item {
              id: extendedText
              property real tileWidth: extendedText.width / 3

              height: textContainer.height - mainText.height
              visible: extendedText.height
              width: parent.width
              x: 0
              y: mainText.height

              ExtendedMenuItem {
                id: insertsCount
                anchors.fill: undefined
                contentText: inserts
                height: parent.height
                imageColor: InputStyle.fontColor
                imageSource: InputStyle.plusIcon
                panelMargin: 0
                rowHeight: parent.height
                showBorder: false
                visible: width
                width: inserts ? extendedText.tileWidth : 0
                x: 0
              }
              ExtendedMenuItem {
                id: updatesCount
                anchors.fill: undefined
                contentText: updates
                height: parent.height
                imageColor: InputStyle.highlightColor
                imageSource: InputStyle.editIcon
                panelMargin: 0
                rowHeight: parent.height
                showBorder: false
                visible: width
                width: updates ? extendedText.tileWidth : 0
                x: insertsCount.width
              }
              ExtendedMenuItem {
                id: removalsCount
                anchors.fill: undefined
                contentText: deletes
                height: parent.height
                imageColor: "red"
                imageSource: InputStyle.removeIcon
                panelMargin: 0
                rowHeight: parent.height
                showBorder: false
                visible: width
                width: deletes ? extendedText.tileWidth : 0
                x: insertsCount.width + updatesCount.width
              }
            }
          }
          Item {
            id: statusContainer
            height: statusPanel.rowHeight
            width: statusPanel.rowHeight
            y: 0

            Image {
              id: statusIcon
              anchors.centerIn: parent
              fillMode: Image.PreserveAspectFit
              height: statusPanel.rowHeight / 3
              source: {
                if (fileStatus === MerginProjectStatusModel.Added)
                  return InputStyle.plusIcon;
                else if (fileStatus === MerginProjectStatusModel.Deleted)
                  return InputStyle.removeIcon;
                else if (fileStatus === MerginProjectStatusModel.Updated)
                  return InputStyle.editIcon;
                else if (fileStatus === MerginProjectStatusModel.Changelog)
                  return InputStyle.tableIcon;
                return "";
              }
              sourceSize.height: height
              sourceSize.width: width
              width: height
            }
            ColorOverlay {
              anchors.fill: statusIcon
              color: {
                if (fileStatus === MerginProjectStatusModel.Added)
                  return InputStyle.fontColor;
                else if (fileStatus === MerginProjectStatusModel.Deleted)
                  return "red";
                else if (fileStatus === MerginProjectStatusModel.Updated)
                  return InputStyle.highlightColor;
                else
                  return InputStyle.fontColor;
              }
              source: statusIcon
            }
          }
        }
        Rectangle {
          id: borderLine
          anchors.bottom: parent.bottom
          color: InputStyle.panelBackground2
          height: 1 * QgsQuick.Utils.dp
          width: delegateItem.width
        }
      }
      section.delegate: Item {
        height: statusPanel.rowHeight / 2
        width: parent.width

        Rectangle {
          anchors.fill: parent
          color: InputStyle.panelBackgroundLight

          Text {
            anchors.fill: parent
            color: InputStyle.panelBackgroundDarker
            font.pixelSize: InputStyle.fontPixelSizeNormal
            font.weight: Font.Bold
            horizontalAlignment: Text.AlignHCenter
            text: section
            verticalAlignment: Text.AlignVCenter
          }
        }
      }
    }
  }
}
