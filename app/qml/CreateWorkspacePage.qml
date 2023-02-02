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

  property bool haveBackButton: true

  function closePage() {
    if ( root.haveBackButton ) {
      root.back()
    }
  }

  header: PanelHeaderV2 {
    width: root.width
    headerTitle: qsTr("Create a workspace")
    haveBackButton: root.haveBackButton
    onBackClicked: root.closePage()
  }

  ColumnLayout {
    anchors {
      fill: parent
      leftMargin: InputStyle.outerFieldMargin
      rightMargin: InputStyle.outerFieldMargin
    }

    Rectangle {
      Layout.fillWidth: true
      Layout.preferredHeight: InputStyle.xSmallGap
      Layout.alignment: Qt.AlignCenter
      color: "transparent"
    }

    Label {
      Layout.fillWidth: true
      Layout.preferredHeight: InputStyle.rowHeightSmall
      Layout.alignment: Qt.AlignCenter

      text: qsTr("A Workspace is a place to store your projects. Colleagues " +
                 "can be invited to your workspace to collaborate on projects. " +
                 "A good candidate for a workspace name is the name of your " +
                 "team or organisation.")

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
      Layout.alignment: Qt.AlignCenter
      color: "transparent"
    }

    InputTextField {
      id: workspaceNameField

      Layout.fillWidth: true
      Layout.preferredHeight: InputStyle.fieldHeight
      Layout.alignment: Qt.AlignCenter

      placeholderText: qsTr("My workspace name…")
    }

    Rectangle {
      Layout.fillWidth: true
      Layout.preferredHeight: InputStyle.xSmallGap
      Layout.alignment: Qt.AlignCenter
      color: "transparent"
    }

    DelegateButton {
      id: createWorkspaceButton

      Layout.fillWidth: true
      btnWidth: width
      Layout.preferredHeight: InputStyle.fieldHeight
      Layout.alignment: Qt.AlignCenter

      text: qsTr("Create workspace")

      onClicked: {
        if (workspaceNameField.displayText === "") {
          __inputUtils.showNotification(qsTr("Empty workspace name"))
        }
        else {
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
