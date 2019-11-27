import QtQuick 2.0
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import lc 1.0

Item {
  id: statusPanel
  property real rowHeight: InputStyle.rowHeightHeader * 1.2

  function open(projectFullName) {
    if (__merginApi.infoProject(projectFullName)) {
      statusPanel.visible = true;
    } else __inputUtils.notify(qsTr("No Changes"))
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

      section.property: "section"
      section.criteria: ViewSection.FullString
      section.delegate: Item {
        height: statusPanel.rowHeight/2
        width: parent.width

        Rectangle {
          anchors.fill: parent
          color:  InputStyle.fontColor

          Text {
            color: "white"
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
        height: statusPanel.rowHeight
        width: parent.width

        ExtendedMenuItem {
          id: fileSummaryItem
          visible: section !== "Changelog"
          height: statusPanel.rowHeight
          width: parent.width
          contentText: "Inserted 1, Updated 1, Deleted 0"
          imageSource: "project.svg"
          overlayImage: true
        }

        ExtendedMenuItem {
            id: item
            visible: !fileSummaryItem.visible
            height: statusPanel.rowHeight
            width: parent.width
            contentText: filename
            imageSource: "file.svg"
            overlayImage: true

            Item {
              id: iconContainer
              height: rowHeight
              width: rowHeight
              anchors.right: parent.right

              Image {
                id: icon
                anchors.fill: parent
                anchors.margins: statusPanel.rowHeight/4
                source:  {
                  if (fileStatus === "Added") return InputStyle.plusIcon
                  else if (fileStatus === "Removed") return InputStyle.removeIcon
                  else if (fileStatus === "Updated") return InputStyle.editIcon

                  return ""
                }
                sourceSize.width: width
                sourceSize.height: height
                fillMode: Image.PreserveAspectFit
              }

              ColorOverlay {
                anchors.fill: icon
                source: icon
                color: {
                  if (fileStatus === "Added") return InputStyle.fontColor
                  else if (fileStatus === "Removed") return "red"
                  else if (fileStatus === "Updated") return InputStyle.highlightColor
                  else return InputStyle.fontColor
                }
              }
            }
        }
      }
    }
  }
}
