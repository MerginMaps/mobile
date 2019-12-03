import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import lc 1.0
import QgsQuick 0.1 as QgsQuick

Item {
  id: statusPanel
  property real rowHeight: InputStyle.rowHeightHeader * 1.2

  function open(projectFullName) {
    if (__merginProjectStatusModel.loadProjectInfo(projectFullName)) {
      statusPanel.visible = true;
    } else __inputUtils.showNotification(qsTr("No Changes"))
  }

  function close() {
    statusPanel.visible = false
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
    titleText: qsTr("Status")
    z: contentLayout.z + 1

    onBack: {
      statusPanel.visible = false
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

      section.property: qsTr("section")
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
        height: statusPanel.rowHeight
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
              visible: extendedText.height

              Row {


              }

            }

//            Text {
//              id: secondaryText
//              height: textContainer.height - mainText.height
//              visible: secondaryText.height
//              text: subtext
//              anchors.right: parent.right
//              anchors.bottom: parent.bottom
//              anchors.left: parent.left
//              anchors.top: mainText.bottom
//              font.pixelSize: InputStyle.fontPixelSizeNormal
//              color: InputStyle.fontColor
//              horizontalAlignment: Text.AlignLeft
//              verticalAlignment: Text.AlignTop
//              elide: Text.ElideRight
//            }
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
              height: statusPanel.rowHeight/2
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
