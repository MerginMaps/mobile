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

import "./components" as MMComponents

Page {
  id: root

  signal finished

  header: PanelHeaderV2 {
    width: root.width
    headerTitle: qsTr("Create a workspace")
    haveBackButton: true
    onBackClicked: root.finished()
  }

  StackView {
    id: stackView

    anchors.fill: parent
  }

  MMComponents.LoadingSpinner {
    id: spinner

    running: true
    anchors.centerIn: parent
  }

  Component {
    id: createWorkspaceComponent

    CreateWorkspacePage {
      id: createWorkspacePanel

      haveBackButton: false

      onBack: {
        stackView.pop( StackView.Immediate )
      }
    }
  }

  Connections {
    target: __merginApi

    function onHasWorkspacesChanged() {
      spinner.running = false

      if ( !__merginApi.userInfo.hasWorkspaces ) {
        stackView.push(createWorkspaceComponent)
      } else {
        root.finished()
      }
    }

    function onWorkspaceCreated() {
      root.finished()
    }
  }
}
