/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.12
import lc 1.0

import "./components"
import "./misc"

Item {
  id: root

  property int projectModelType: ProjectsModel.EmptyProjectsModel
  property string activeProjectId: ""
  property alias list: projectlist

  signal openProjectRequested( string projectId, string projectFilePath )
  signal showLocalChangesRequested( string projectId )

  function refreshProjectsList() {
    searchBar.deactivate()
    projectlist.refreshProjectList( searchBar.text )
  }

  AttentionBanner {
    id: attentionBanner
    visible: __merginApi.subscriptionInfo ? __merginApi.subscriptionInfo.actionRequired : false
    anchors {
      top: parent.top
      left: parent.left
      right: parent.right
    }
  }

  SearchBar {
    id: searchBar

    anchors {
      top: attentionBanner.visible ? attentionBanner.bottom : parent.top
      left: parent.left
      right: parent.right
    }

    allowTimer: true
  }

  ProjectList {
    id: projectlist

    projectModelType: root.projectModelType
    activeProjectId: root.activeProjectId
    searchText: searchBar.text

    anchors {
      left: parent.left
      right: parent.right
      top: searchBar.bottom
      bottom: parent.bottom
    }

    onOpenProjectRequested: root.openProjectRequested( projectId, projectFilePath )
    onShowLocalChangesRequested: root.showLocalChangesRequested( projectId )
  }
}
