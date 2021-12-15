import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.2

import lc 1.0
import "." // import InputStyle singleton
import "./components"
Item {
  id: projectWizardPanel

  signal back

  property real rowHeight: InputStyle.fieldHeight
  property var fontColor: InputStyle.fontColor
  property var bgColor: InputStyle.clrPanelMain
  property real panelMargin: 10 * __dp

  property ListModel widgetsModel: ListModel {}

  //! Inits widgetsModel data just after its created, but before Component.complete is emitted (for both model or components where its used)
  property bool isWidgetModelReady: {
    var types = fieldsModel.supportedTypes()
    for (var prop in types) {
      projectWizardPanel.widgetsModel.append({ "display": types[prop], "widget": prop })
    }

    true
  }

  FieldsModel {
    id: fieldsModel
    onNotify: __inputUtils.showNotification(message)
    Component.onCompleted: fieldsModel.initModel()
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
      projectWizardPanel.back()
    }
  }

  Item {
    height: projectWizardPanel.height - header.height - toolbar.height
    width: projectWizardPanel.width
    y: header.height

    ColumnLayout {
      id: contentLayout
      spacing: 0
      anchors.fill: parent
      anchors.leftMargin: InputStyle.outerFieldMargin
      anchors.rightMargin: InputStyle.outerFieldMargin

      Label {
        height: projectWizardPanel.rowheight
        width: parent.width
        Layout.preferredHeight: projectWizardPanel.rowHeight
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        text: qsTr("Project name")
        color: InputStyle.fontColor
        font.pixelSize: InputStyle.fontPixelSizeNormal
        leftPadding: InputStyle.innerFieldMargin
      }

      InputTextField {
        id: projectNameField
        height: projectWizardPanel.rowHeight
      }

      Label {
        id: attributesLabel
        height: projectWizardPanel.rowheight
        width: parent.width
        text: qsTr("Fields")
        color: InputStyle.fontColor
        font.pixelSize: InputStyle.fontPixelSizeNormal
        Layout.preferredHeight: projectWizardPanel.rowHeight
        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
        leftPadding: InputStyle.innerFieldMargin
      }

      ListView {
        id: fieldList
        model: fieldsModel
        width: parent.width
        Layout.fillWidth: true
        Layout.fillHeight: true
        clip: true
        spacing: projectWizardPanel.rowHeight * 0.1 // same as delegateButton "margin"

        delegate: FieldRow {
          rowHeight: projectWizardPanel.rowHeight
          height: rowHeight
          width: contentLayout.width
          color: projectWizardPanel.fontColor
          widgetList: projectWizardPanel.widgetsModel

          onRemoveClicked: fieldsModel.removeField(index)
        }

        footer: Item {
            width: parent.width
            height: projectWizardPanel.rowHeight + 2* (projectWizardPanel.rowHeight * 0.1)
          DelegateButton {
              height: projectWizardPanel.rowHeight
              width: parent.width
              text: qsTr("Add field")
              anchors.centerIn: parent
              iconSource: InputStyle.plusIcon
              onClicked: {
                fieldsModel.addField("", "TextEdit")
                if (fieldList.visible) {
                  fieldList.positionViewAtEnd()
                }
              }
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
          faded: !projectNameField.displayText
          imageSource: InputStyle.checkIcon

          onActivated: {
            if (faded) {
              __inputUtils.showNotification(qsTr("Empty project name"))
            } else {
              __projectWizard.createProject(projectNameField.displayText, fieldsModel )
            }
          }
        }
      }
    }
  }
}
