import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.2
import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "."
import "./components" // import InputStyle singleton
Item {
  id: projectWizardPanel

  signal backClicked

  property real rowHeight: InputStyle.rowHeight
  property var fontColor: InputStyle.fontColor
  property var bgColor: InputStyle.clrPanelMain
  property real panelMargin: 10 * QgsQuick.Utils.dp

  property ListModel items: ListModel {}
  property var supportedTypes: {
    var types = __fieldsModel.supportedTypes()
    for (var prop in types) {
      projectWizardPanel.items.append({ "display": types[prop], "widget": prop })
    }

    projectWizardPanel.items
  }

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

  Item {
    height: projectWizardPanel.height - header.height - toolbar.height
    width: projectWizardPanel.width
    y: header.height

    ColumnLayout {
      id: contentLayout
      spacing: InputStyle.panelSpacing
      anchors.fill: parent
      anchors.margins: projectWizardPanel.panelMargin

      Label {
        height: projectWizardPanel.rowheight
        width: parent.width
        Layout.preferredHeight: projectWizardPanel.rowHeight
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
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
        Layout.fillWidth: true
        Layout.preferredHeight: projectWizardPanel.rowHeight

        background: Rectangle {
          anchors.fill: parent
          border.color: projectNameField.activeFocus ? InputStyle.fontColor : InputStyle.panelBackgroundLight
          border.width: projectNameField.activeFocus ? 2 : 1
          color: InputStyle.clrPanelMain
          radius: InputStyle.cornerRadius
        }
      }

      Label {
        id: attributesLabel
        height: projectWizardPanel.rowheight
        width: parent.width
        text: qsTr("Attributes")
        color: InputStyle.fontColor
        font.pixelSize: InputStyle.fontPixelSizeNormal
        Layout.preferredHeight: projectWizardPanel.rowHeight
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
      }

      ListView {
        id: fieldList
        model: __fieldsModel
        width: parent.width
        Layout.fillWidth: true
        Layout.fillHeight: true
        clip: true

        onCountChanged: {
          if (fieldList.visible) {
            fieldList.positionViewAtEnd()
          }
        }

        delegate: FieldRow {
          height: projectWizardPanel.rowHeight
          width: contentLayout.width
          color: projectWizardPanel.fontColor
          widgetList: projectWizardPanel.supportedTypes

          onRemoveClicked: __fieldsModel.removeField(index)
        }
      }

      Row {
        id: listButtonContainer
        width: parent.width
        Layout.preferredHeight: projectWizardPanel.rowHeight * 0.8
        Layout.fillWidth: true
        Button {
          id: delegateButton
          text: qsTr("Add field")
          height: parent.height
          width: height * 2
          anchors.verticalCenter: parent.verticalCenter
          font.pixelSize: InputStyle.fontPixelSizeTitle

          background: Rectangle {
            color: InputStyle.highlightColor
            radius: InputStyle.cornerRadius
          }

          onClicked: __fieldsModel.addField("", "TextEdit")

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
        width: parent.width / parent.children.length
        height: parent.height
        MainPanelButton {
          id: createProjectBtn
          width: toolbar.itemSize
          text: qsTr("Create project")
          faded: !projectNameField.text
          imageSource: InputStyle.checkIcon

          onActivated: {
            if (faded) {
              __inputUtils.showNotification(qsTr("Empty project name"))
            } else {
              __projectWizard.createProject(projectNameField.text)
            }
          }
        }
      }
    }
  }
}
