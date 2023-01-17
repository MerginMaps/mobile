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

  Item {
    id: content

    anchors {
      top: searchBar.bottom
      left: parent.left
      right: parent.right
    }

    Column {
      id: columnLayout
      anchors.fill: parent

      Label {
        id: invitationsLabel
        height: InputStyle.fieldHeight
        width: parent.width
        wrapMode: Text.WordWrap
        text: qsTr("Invitations")
        color: InputStyle.fontColor
        font.pixelSize: InputStyle.fontPixelSizeNormal
        leftPadding: InputStyle.innerFieldMargin
        visible: __merginApi.userInfo.hasInvitations
        //horizontalAlignment: Text.AlignLeft
        //verticalAlignment: Text.AlignVCenter
        //anchors.horizontalCenter: parent.horizontalCenter
      }

      InvitationList {
        id: invitationList
        width: parent.width
        visible: __merginApi.userInfo.hasInvitations
        //anchors.horizontalCenter: parent.horizontalCenter
      }

      Label {
        id: workspacesLabel
        height: InputStyle.fieldHeight
        width: parent.width
        wrapMode: Text.WordWrap
        text: qsTr("Your workspaces")
        color: InputStyle.fontColor
        font.pixelSize: InputStyle.fontPixelSizeNormal
        leftPadding: InputStyle.innerFieldMargin
        //horizontalAlignment: Text.AlignLeft
        //verticalAlignment: Text.AlignVCenter
        //anchors.horizontalCenter: parent.horizontalCenter
      }

      WorkspaceList {
        id: workspacelist

        onWorkspaceChangeRequested: function (workspaceId) {
          __merginApi.userInfo.setActiveWorkspace( workspaceId )
          root.back()
        }

        onCreateWorkspaceRequested: root.createWorkspace()

        searchText: searchBar.text
        anchors.horizontalCenter: parent.horizontalCenter
/*
        anchors {
          left: parent.left
          right: parent.right
          top: workspacesLabel.bottom
          bottom: parent.bottom
        }
*/
      }


    }
  }

/*
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
*/
}
