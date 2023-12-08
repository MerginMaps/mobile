

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

  signal backClicked
  signal continueClicked
  signal createWorkspaceClicked

  ColumnLayout {
    MMOnboardingHeader {
      headerTitle: qsTr("Accept Invitation")

      onBackClicked: root.backClicked()
    }

    ToolButton {
      onClicked: root.continueClicked()
      text: qsTr("Continue")
    }

    ToolButton {
      onClicked: root.createWorkspaceClicked()
      text: qsTr("Create Workspace")
    }
  }
}
