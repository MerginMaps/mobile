import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.2
import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "." // import InputStyle singleton
import "components"

Item {
  id: projectWizardPanel
  property var bgColor: InputStyle.clrPanelMain
  property var fontColor: InputStyle.fontColor

  //! Inits widgetsModel data just after its created, but before Component.complete is emitted (for both model or components where its used)
  property bool isWidgetModelReady: {
    var types = fieldsModel.supportedTypes();
    for (var prop in types) {
      projectWizardPanel.widgetsModel.append({
          "display": types[prop],
          "widget": prop
        });
    }
    true;
  }
  property real panelMargin: 10 * QgsQuick.Utils.dp
  property real rowHeight: InputStyle.fieldHeight
  property ListModel widgetsModel: ListModel {
  }

  signal back

  FieldsModel {
    id: fieldsModel
    Component.onCompleted: fieldsModel.initModel()
    onNotify: __inputUtils.showNotification(message)
  }

  // background
  Rectangle {
    color: projectWizardPanel.bgColor
    height: parent.height
    width: parent.width
  }
  PanelHeader {
    id: header
    color: InputStyle.clrPanelMain
    height: InputStyle.rowHeightHeader
    rowHeight: InputStyle.rowHeightHeader
    titleText: qsTr("Create Project")
    width: projectWizardPanel.width

    onBack: {
      projectWizardPanel.back();
    }
  }
  Item {
    height: projectWizardPanel.height - header.height - toolbar.height
    width: projectWizardPanel.width
    y: header.height

    ColumnLayout {
      id: contentLayout
      anchors.fill: parent
      anchors.leftMargin: InputStyle.outerFieldMargin
      anchors.rightMargin: InputStyle.outerFieldMargin
      spacing: 0

      Label {
        Layout.preferredHeight: projectWizardPanel.rowHeight
        color: InputStyle.fontColor
        font.pixelSize: InputStyle.fontPixelSizeNormal
        height: projectWizardPanel.rowheight
        horizontalAlignment: Text.AlignLeft
        leftPadding: InputStyle.innerFieldMargin
        text: qsTr("Project name")
        verticalAlignment: Text.AlignVCenter
        width: parent.width
      }
      InputTextField {
        id: projectNameField
        height: projectWizardPanel.rowHeight
      }
      Label {
        id: attributesLabel
        Layout.preferredHeight: projectWizardPanel.rowHeight
        color: InputStyle.fontColor
        font.pixelSize: InputStyle.fontPixelSizeNormal
        height: projectWizardPanel.rowheight
        horizontalAlignment: Text.AlignLeft
        leftPadding: InputStyle.innerFieldMargin
        text: qsTr("Fields")
        verticalAlignment: Text.AlignVCenter
        width: parent.width
      }
      ListView {
        id: fieldList
        Layout.fillHeight: true
        Layout.fillWidth: true
        clip: true
        model: fieldsModel
        spacing: projectWizardPanel.rowHeight * 0.1 // same as delegateButton "margin"
        width: parent.width

        delegate: FieldRow {
          color: projectWizardPanel.fontColor
          height: rowHeight
          rowHeight: projectWizardPanel.rowHeight
          widgetList: projectWizardPanel.widgetsModel
          width: contentLayout.width

          onRemoveClicked: fieldsModel.removeField(index)
        }
        footer: Item {
          height: projectWizardPanel.rowHeight + 2 * (projectWizardPanel.rowHeight * 0.1)
          width: parent.width

          DelegateButton {
            anchors.centerIn: parent
            height: projectWizardPanel.rowHeight
            iconSource: InputStyle.plusIcon
            text: qsTr("Add field")
            width: parent.width

            onClicked: {
              fieldsModel.addField("", "TextEdit");
              if (fieldList.visible) {
                fieldList.positionViewAtEnd();
              }
            }
          }
        }
      }
    }
  }

  // footer toolbar
  Rectangle {
    id: toolbar
    property int itemSize: toolbar.height * 0.8

    anchors.bottom: parent.bottom
    color: InputStyle.clrPanelBackground
    height: InputStyle.rowHeightHeader
    width: parent.width

    MouseArea {
      anchors.fill: parent

      onClicked: {
      } // dont do anything, just do not let click event propagate
    }
    Row {
      anchors.bottom: parent.bottom
      height: toolbar.height
      width: parent.width

      Item {
        height: parent.height
        width: parent.width / parent.children.length

        MainPanelButton {
          id: createProjectBtn
          faded: !projectNameField.displayText
          imageSource: InputStyle.checkIcon
          text: qsTr("Create project")
          width: toolbar.itemSize

          onActivated: {
            if (faded) {
              __inputUtils.showNotification(qsTr("Empty project name"));
            } else {
              __projectWizard.createProject(projectNameField.displayText, fieldsModel);
            }
          }
        }
      }
    }
  }
}
