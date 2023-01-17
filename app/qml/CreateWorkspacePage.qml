import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import QtQuick.Dialogs

import lc 1.0
import "." // import InputStyle singleton
import "./components"

Item {
  id: createWorkspacePanel

  signal back

  // background
  Rectangle {
    width: parent.width
    height: parent.height
    color: InputStyle.clrPanelMain
  }

  PanelHeader {
    id: header

    height: InputStyle.rowHeightHeader
    width: createWorkspacePanel.width
    color: InputStyle.clrPanelMain
    rowHeight: InputStyle.rowHeightHeader
    titleText: qsTr("Create workspace")

    onBack: {
      createWorkspacePanel.back()
    }
  }

  Item {
    id: content
    anchors.fill: parent
    anchors.bottomMargin: Qt.inputMethod.keyboardRectangle.height ? Qt.inputMethod.keyboardRectangle.height : 0

    Column {
      id: columnLayout
      anchors.verticalCenter: parent.verticalCenter
      width: parent.width

      Image {
        id: mmLogo
        source: InputStyle.mmLogoHorizontal
        width: content.width / 2
        sourceSize.width: width
        anchors.horizontalCenter: parent.horizontalCenter
      }

      Label {
        id: helloLabel
        height: InputStyle.fieldHeight
        width: content.width * 0.75
        text: qsTr("Hello") + " " + __merginApi.userAuth.username + "!"
        color: InputStyle.fontColor
        font.bold: true
        font.pixelSize: InputStyle.fontPixelSizeNormal
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        leftPadding: InputStyle.innerFieldMargin
        anchors.horizontalCenter: parent.horizontalCenter
      }

      Label {
        id: infoLabel
        height: InputStyle.fieldHeight
        width: content.width * 0.75
        wrapMode: Text.WordWrap
        text: qsTr("You are about to create a new workspace for your projects and colleagues. Please specify its name")
        color: InputStyle.fontColor
        font.pixelSize: InputStyle.fontPixelSizeNormal
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        leftPadding: InputStyle.innerFieldMargin
        anchors.horizontalCenter: parent.horizontalCenter
      }

      InputTextField {
        id: workspaceNameField
        height: InputStyle.fieldHeight
        width: content.width * 0.75
        anchors.horizontalCenter: parent.horizontalCenter
      }

      DelegateButton {
        id: createWorkspaceButton

        width: content.width * 0.75
        height: InputStyle.fieldHeight
        anchors.horizontalCenter: parent.horizontalCenter

        text: qsTr("Create")
        onClicked: {
          if (workspaceNameField.displayText === "") {
            __inputUtils.showNotification(qsTr("Empty workspace name"))
          } else {
            __merginApi.createWorkspace(workspaceNameField.displayText)
          }
        }
      }
    }
  }
}
