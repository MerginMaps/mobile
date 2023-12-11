

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import "../components"

Page {
  id: root

  signal continueClicked

  Column {
    MMHeader {
      headerTitle: qsTr("Create Workspace")
      step: 1
      backVisible: false
    }

    Label {
      text: qsTr("Workspace is a place to store your projects. Colleagues can be invited to your workspace to collaborate on projects.")
      font: __style.p5
      color: __style.forestColor
    }

    MMInput {
      title: qsTr("Workspace name")
      placeholderText: qsTr("e.g. my-company")
    }

    MMTextBubble {
      title: qsTr("Workspace is a place to store your projects. Colleagues can be invited to your workspace to collaborate on projects.")
      description: qsTr("A good candidate for a workspace name is the name of your team or organisation.")
    }

    MMButton {
      onClicked: root.continueClicked()
      text: qsTr("Create workspace")
    }
  }
}
