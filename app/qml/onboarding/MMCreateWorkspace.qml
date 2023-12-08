

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

Page {
  id: root

  signal continueClicked

  ColumnLayout {
    MMOnboardingHeader {
      headerTitle: qsTr("Create Workspace")
      step: 1
      backVisible: false
    }

    Label {
      text: qsTr("Workspace is a place to store your projects. Colleagues can be invited to your workspace to collaborate on projects.")
    }

    Label {
      // TODO
      text: qsTr("A good candidate for a workspace name is the name of your team or organisation.")
    }

    ToolButton {
      onClicked: root.continueClicked()
      text: qsTr("Continue")
    }
  }
}
