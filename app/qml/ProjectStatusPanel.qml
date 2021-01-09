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

Item {
  id: statusPanel
  property real rowHeight: InputStyle.rowHeight * 1.2
  signal back()

  function open(projectFullName) {
    if (__merginProjectStatusModel.loadProjectInfo(projectFullName)) {
      statusPanel.visible = true;
    } else __inputUtils.showNotification(qsTr("No Changes"))
  }

  // background
  Rectangle {
    width: parent.width
    height: parent.height
    color: InputStyle.clrPanelMain
  }

  PanelHeader {
    id: header
    height: InputStyle.rowHeightHeader
    width: parent.width
    color: InputStyle.clrPanelMain
    rowHeight: InputStyle.rowHeightHeader
    titleText: qsTr("Project Status")
    z: contentLayout.z + 1

    onBack: {
      statusPanel.back()
    }
    withBackButton: true

  }

  // Content
  ColumnLayout {
    id: contentLayout
    height: statusPanel.height-header.height
    width: statusPanel.width
    y: header.height
    spacing: 0

    ListView {
      id: statusList
      model: __merginProjectStatusModel
      Layout.fillWidth: true
      Layout.fillHeight: true

      section.property: "section"
      section.criteria: ViewSection.FullString
      section.delegate: Item {
        height: statusPanel.rowHeight/2
        width: parent.width

        Rectangle {
          anchors.fill: parent
          color: InputStyle.panelBackgroundLight

          Text {
            color: InputStyle.panelBackgroundDarker
            anchors.fill: parent
            text: section
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: InputStyle.fontPixelSizeNormal
            font.weight: Font.Bold
          }
        }
      }


      delegate: Item {
        id: delegateItem
        height: fileStatus === MerginProjectStatusModel.Changelog ? statusPanel.rowHeight * 1.2 : statusPanel.rowHeight
        width: parent.width

        RowLayout {
          id: row
          anchors.fill: parent
          anchors.rightMargin: InputStyle.panelMargin/2
          anchors.leftMargin: InputStyle.panelMargin/2
          spacing: 0

          Item {
            id: textContainer
            height: statusPanel.rowHeight
            Layout.fillWidth: true


            Text {
              id: mainText
              text:itemText
              height: fileStatus === MerginProjectStatusModel.Changelog ? textContainer.height/2 : textContainer.height
              width: textContainer.width
              font.pixelSize: InputStyle.fontPixelSizeNormal
              color: InputStyle.fontColor
              horizontalAlignment: Text.AlignLeft
              verticalAlignment: fileStatus === MerginProjectStatusModel.Changelog ? Text.AlignBottom : Text.AlignVCenter
              elide: Text.ElideRight
              font.bold: true
            }

            Item {
              id: extendedText
              height: textContainer.height - mainText.height
              width: parent.width
              visible: extendedText.height
              y: mainText.height
              x: 0
              property real tileWidth: extendedText.width/3


                ExtendedMenuItem {
                  id: insertsCount
                  height: parent.height
                  width: inserts ? extendedText.tileWidth : 0
                  visible: width
                  contentText: inserts
                  rowHeight: parent.height
                  panelMargin: 0
                  imageColor: InputStyle.fontColor
                  imageSource: InputStyle.plusIcon
                  anchors.fill: undefined
                  showBorder:false
                  x: 0
                }
                ExtendedMenuItem {
                  id: updatesCount
                  height: parent.height
                  width: updates ? extendedText.tileWidth : 0
                  visible: width
                  contentText: updates
                  rowHeight: parent.height
                  panelMargin: 0
                  imageColor: InputStyle.highlightColor
                  imageSource: InputStyle.editIcon
                  anchors.fill: undefined
                  showBorder:false
                  x: insertsCount.width
                }
                ExtendedMenuItem {
                  id: removalsCount
                  height: parent.height
                  width: deletes ? extendedText.tileWidth : 0
                  visible: width
                  contentText: deletes
                  rowHeight: parent.height
                  panelMargin: 0
                  imageColor: "red"
                  imageSource: InputStyle.removeIcon
                  anchors.fill: undefined
                  showBorder:false
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
              source: {
                if (fileStatus === MerginProjectStatusModel.Added ) return InputStyle.plusIcon
                else if (fileStatus === MerginProjectStatusModel.Deleted) return InputStyle.removeIcon
                else if (fileStatus === MerginProjectStatusModel.Updated) return InputStyle.editIcon
                else if (fileStatus === MerginProjectStatusModel.Changelog) return InputStyle.tableIcon

                return ""
              }
              height: statusPanel.rowHeight/3
              width: height
              sourceSize.width: width
              sourceSize.height: height
              fillMode: Image.PreserveAspectFit
            }

            ColorOverlay {
              anchors.fill: statusIcon
              source: statusIcon
              color: {
                if (fileStatus === MerginProjectStatusModel.Added) return InputStyle.fontColor
                else if (fileStatus === MerginProjectStatusModel.Deleted) return "red"
                else if (fileStatus === MerginProjectStatusModel.Updated) return InputStyle.highlightColor
                else return InputStyle.fontColor
              }
            }
          }
        }
        Rectangle {
          id: borderLine
          color: InputStyle.panelBackground2
          width: delegateItem.width
          height: 1 * QgsQuick.Utils.dp
          anchors.bottom: parent.bottom
        }
      }
    }
  }
}


