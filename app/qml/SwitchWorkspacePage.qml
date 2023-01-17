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
import lc 1.0

import "."
import "./components"
import "./misc"

Page {
  id: root

  signal back
  signal createWorkspace()

  header: PanelHeader {
    id: header
    height: InputStyle.rowHeightHeader
    width: parent.width
    color: InputStyle.clrPanelMain
    rowHeight: InputStyle.rowHeightHeader
    titleText: qsTr("Select a workspace")
    withBackButton: true

    onBack: {
      root.back()
    }
  }

  SearchBar {
    id: searchBar

    anchors {
      top: parent.top
      left: parent.left
      right: parent.right
    }

    allowTimer: true
  }

  WorkspaceList {
    id: workspacelist

    onWorkspaceChangeRequested: function (workspaceId) {
      __merginApi.userInfo.setActiveWorkspace( workspaceId )
      root.back()
    }

    onCreateWorkspaceRequested: root.createWorkspace()

    searchText: searchBar.text

    anchors {
      left: parent.left
      right: parent.right
      top: searchBar.bottom
      bottom: parent.bottom
    }
  }
}