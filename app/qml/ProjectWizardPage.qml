import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.2
import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "."  // import InputStyle singleton

Item {
  id: projectWizardPanel

  signal backClicked

  property real rowHeight: InputStyle.rowHeight
  property var fontColor: InputStyle.fontColor
  property var bgColor: InputStyle.clrPanelMain


  // background
  Rectangle {
    width: parent.width
    height: parent.height
    color: projectWizardPanel.bgColor
  }

  PanelHeader {
    id: header
    height: InputStyle.rowHeightHeader
    width: projectWizardPanel.width
    color: InputStyle.clrPanelMain
    rowHeight: InputStyle.rowHeightHeader
    titleText: qsTr("Create Project")

    onBack: {
        projectWizardPanel.backClicked()
    }
  }

  Column {
    id: contentLayout
    height: projectWizardPanel.height-header.height
    width: projectWizardPanel.width
    y: header.height
    spacing: 0

    Label {
      height: projectWizardPanel.rowheight
      width: parent.width
//      horizontalAlignment: Qt.AlignHCenter
//      verticalAlignment: Qt.AlignLeft
      text: qsTr("Project name")
      color: InputStyle.fontColor
      font.pixelSize: InputStyle.fontPixelSizeNormal
    }

    TextField {
      id: registerName
      width: parent.width
      height: projectWizardPanel.rowHeight
      font.pixelSize: InputStyle.fontPixelSizeNormal
      color: projectWizardPanel.fontColor
      placeholderText: qsTr("")
      font.capitalization: Font.MixedCase
      inputMethodHints: Qt.ImhNoPredictiveText
    }

    Repeater {
      model: __fieldsModel

        Row {
          id: fieldItem
          //text: Name
          //color: "red"
          height: projectWizardPanel.rowHeight
          width: projectWizardPanel.width

          TextField {
            id: textField
            height: projectWizardPanel.rowHeight
            topPadding: 10 * QgsQuick.Utils.dp
            bottomPadding: 10 * QgsQuick.Utils.dp
            anchors.left: parent.left
            anchors.right: parent.right
            font.pixelSize: InputStyle.fontPixelSizeNormal
            color: projectWizardPanel.fontColor
            placeholderText: "Field name"

            background: Rectangle {
                anchors.fill: parent
                border.color: textField.activeFocus ? InputStyle.fontColor: InputStyle.panelBackgroundLight
                border.width: textField.activeFocus ? 2 : 1
                color: InputStyle.clrPanelMain
                radius: InputStyle.cornerRadius
            }
        }
      }
    }

   Row {
     width: parent.width
     height: projectWizardPanel.rowHeight
     Button {
         id: delegateButton
         text: qsTr("Add field")
         height: parent.height
         width: height * 2
         //anchors.horizontalCenter: parent.horizontalCenter
         anchors.verticalCenter: parent.verticalCenter
         font.pixelSize: InputStyle.fontPixelSizeTitle

         background: Rectangle {
           color: InputStyle.highlightColor
           radius: InputStyle.cornerRadius
         }

         onClicked: __fieldsModel.addField("new", "")

         contentItem: Text {
           text: delegateButton.text
           font: delegateButton.font
           color: "white"
           horizontalAlignment: Text.AlignHCenter
           verticalAlignment: Text.AlignVCenter
           elide: Text.ElideRight
         }
       }
   }

   }



}
