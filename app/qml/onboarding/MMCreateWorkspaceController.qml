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

/**
 * Directly create workspace but without onboarding questions
 * e.g. from account page or projects panel
 */
Item {
  id: controller

  required property var stackView
  required property bool enabled

  function createNewWorkspace() {
    stackView.push(createWorkspaceComponent)
  }

  Connections {
    target: __merginApi
    enabled: controller.enabled && stackView.currentItem.objectName === "createWorkspaceDirectPanel"

    function onWorkspaceCreated(workspace) {
      stackView.pop()
    }
  }

  Component {
    id: createWorkspaceComponent

    MMCreateWorkspace {
      id: createWorkspacePanel

      objectName: "createWorkspaceDirectPanel"
      showProgress: false
      backVisible: true

      onCreateWorkspaceClicked: function (workspaceName) {
        __merginApi.createWorkspace(workspaceName)
      }

      onBackClicked: stackView.pop()
    }
  }
}
