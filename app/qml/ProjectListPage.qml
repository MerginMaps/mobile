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
import "components"

Item {
  id: root
  property string activeProjectId: ""
  property alias list: projectlist
  property int projectModelType: ProjectsModel.EmptyProjectsModel

  signal openProjectRequested(string projectId, string projectFilePath)
  function refreshProjectsList() {
    searchBar.deactivate();
    projectlist.refreshProjectList(searchBar.text);
  }
  signal showLocalChangesRequested(string projectId)

  SearchBar {
    id: searchBar
    allowTimer: true

    anchors {
      bottom: projectlist.top
      left: parent.left
      right: parent.right
      top: parent.top
    }
  }
  ProjectList {
    id: projectlist
    activeProjectId: root.activeProjectId
    projectModelType: root.projectModelType
    searchText: searchBar.text

    onOpenProjectRequested: root.openProjectRequested(projectId, projectFilePath)
    onShowLocalChangesRequested: root.showLocalChangesRequested(projectId)

    anchors {
      bottom: parent.bottom
      left: parent.left
      right: parent.right
      top: searchBar.bottom
    }
  }
}
