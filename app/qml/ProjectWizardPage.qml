import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.2
import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "."
import "./components"

// import InputStyle singleton
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
    height: projectWizardPanel.height - header.height
    width: projectWizardPanel.width
    y: header.height
    spacing: InputStyle.panelSpacing

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
      id: projectNameField
      width: parent.width
      height: projectWizardPanel.rowHeight
      font.pixelSize: InputStyle.fontPixelSizeNormal
      color: projectWizardPanel.fontColor
      placeholderText: qsTr("Project name")
      font.capitalization: Font.MixedCase
      inputMethodHints: Qt.ImhNoPredictiveText
      text: "TODO" // TODO
    }

    Repeater {
      model: __fieldsModel

      FieldRow {
        height: projectWizardPanel.rowHeight
        width: projectWizardPanel.width
        color: projectWizardPanel.fontColor
        widgetList: __fieldsModel.supportedTypes()
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

        onClicked: __fieldsModel.addField("", "text")

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

  // footer toolbar
  Rectangle {
    property int itemSize: toolbar.height * 0.8

    id: toolbar
    height: InputStyle.rowHeightHeader
    width: parent.width
    anchors.bottom: parent.bottom
    color: InputStyle.clrPanelBackground

    MouseArea {
      anchors.fill: parent
      onClicked: {} // dont do anything, just do not let click event propagate
    }

    Row {
      height: toolbar.height
      width: parent.width
      anchors.bottom: parent.bottom

      Item {
        width: parent.width/parent.children.length
        height: parent.height
        MainPanelButton {
          id: createProjectBtn
          width: toolbar.itemSize
          text: qsTr("Create project")
          faded: false // TOOD
          imageSource: InputStyle.plusIcon

          onActivated: {
            __projectWizard.createProject(projectNameField.text)
          }
        }
      }
    }
  }


}
