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
            id: item
            height: statusPanel.rowHeight
            width: parent.width
            contentText: text
            panelMargin: 0
            rowHeight: statusPanel.rowHeight
            imageSource: {
              if (fileStatus === "Added") return InputStyle.plusIcon
              else if (fileStatus === "Deleted") return InputStyle.removeIcon
              else if (fileStatus === "Updated") return InputStyle.editIcon
              else if (fileStatus === "Changelog") return InputStyle.infoIcon

              return ""
            }
            imageColor: {
              if (fileStatus === "Added") return InputStyle.fontColor
              else if (fileStatus === "Deleted") return "red"
              else if (fileStatus === "Updated") return InputStyle.highlightColor
              else return InputStyle.fontColor
            }
            overlayImage: true
        }
      }
    }
  }
}
