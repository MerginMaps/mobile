/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.0
import lc 1.0

import "./components"

Item {
  id: root

  property int projectModelType: ProjectsModel.EmptyProjectsModel
  property string activeProjectId: ""

  property alias list: projectlist

  signal openProjectRequested( string projectId, string projectFilePath )
  signal showLocalChangesRequested( string projectId )

  function refreshProjectsList( keepSearchFilter = false ) {
    if ( !keepSearchFilter )
      searchBar.deactivate()

    projectlist.refreshProjectList( searchBar.text )
  }

  SearchBar {
    id: searchBar

    anchors {
      top: parent.top
      left: parent.left
      right: parent.right
      bottom: projectlist.top
    }

    allowTimer: true

    onSearchTextChanged: projectlist.searchTextChanged( text )
  }

  ProjectList {
    id: projectlist

    projectModelType: root.projectModelType
    activeProjectId: root.activeProjectId

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
