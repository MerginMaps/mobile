/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import lc 1.0

import "."
import "./components"
import "./misc"

Page {
  id: root

  signal back
  signal createWorkspaceRequested

  header: PanelHeaderV2 {
    width: root.width
    headerTitle: qsTr("Create a workspace")
    onBackClicked: root.back()
  }

  ColumnLayout {
    anchors {
      fill: parent
      leftMargin: InputStyle.outerFieldMargin
      rightMargin: InputStyle.outerFieldMargin
    }

    Image {
      Layout.alignment: Qt.AlignHCenter

      width: parent.width / 2
      source: InputStyle.mmLogoHorizontal
      sourceSize.width: width
    }

    Rectangle {
      Layout.fillWidth: true
      Layout.preferredHeight: InputStyle.xSmallGap
      color: "transparent"
    }

    Label {
      Layout.fillWidth: true
      Layout.preferredHeight: InputStyle.rowHeightSmall

      text: qsTr("Hello") + " " + __merginApi.userAuth.username + "!"

      font.pixelSize: InputStyle.fontPixelSizeNormal
      font.bold: true
      color: InputStyle.fontColor

      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
    }

    Label {
      Layout.fillWidth: true
      Layout.preferredHeight: InputStyle.rowHeightSmall

      text: qsTr("You are about to create a new workspace for your projects and colleagues. Please specify its name")

      font.pixelSize: InputStyle.fontPixelSizeNormal
      font.bold: true
      color: InputStyle.fontColor

      wrapMode: Text.WordWrap
      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
    }

    Rectangle {
      Layout.fillWidth: true
      Layout.preferredHeight: InputStyle.xSmallGap
      color: "transparent"
    }

    InputTextField {
      id: workspaceNameField

      Layout.fillWidth: true
      Layout.preferredHeight: InputStyle.fieldHeight
      Layout.alignment: Qt.AlignHCenter
    }

    Rectangle {
      Layout.fillWidth: true
      Layout.preferredHeight: InputStyle.xSmallGap
      color: "transparent"
    }

    DelegateButton {
      id: createWorkspaceButton

      Layout.fillWidth: true
      Layout.preferredHeight: InputStyle.fieldHeight

      text: qsTr("Create")

      onClicked: {
        if (workspaceNameField.displayText === "") {
          __inputUtils.showNotification(qsTr("Empty workspace name"))
        } else {
          __merginApi.createWorkspace(workspaceNameField.displayText)
        }
      }
    }

    Rectangle {
      Layout.fillWidth: true
      Layout.preferredHeight: InputStyle.xSmallGap
      color: "transparent"
    }
  }
}
