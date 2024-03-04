/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import mm 1.0 as MM

import "../components"
import "../inputs"

Item {
  id: root

  property int projectModelType: MM.ProjectsModel.EmptyProjectsModel
  property string activeProjectId: ""
  property alias list: projectlist

  signal openProjectRequested( string projectFilePath )
  signal showLocalChangesRequested( string projectId )

  function refreshProjectsList() {
    searchBar.deactivate()
    projectlist.refreshProjectList( searchBar.text )
  }

  property int spacing: 10 * __dp

  MMSearchInput {
    id: searchBar

    width: parent.width - 2 * root.padding
    placeholderText: qsTr("Search for projects...")
    // visible: root.withSearch

    onSearchTextChanged: function(text) {
      //root.model.searchExpression = text
    }

    anchors {
      top: parent.top
      left: parent.left
      right: parent.right
    }

    allowTimer: true
  }


  MMProjectList {
    id: projectlist

    projectModelType: root.projectModelType
    activeProjectId: root.activeProjectId
    searchText: searchBar.text
    spacing: root.spacing

    anchors {
      left: parent.left
      right: parent.right
      top: searchBar.bottom
      bottom: parent.bottom
      topMargin: root.spacing
    }

    onOpenProjectRequested: function( projectFilePath ) {
      root.openProjectRequested( projectFilePath )
    }
    onShowLocalChangesRequested: function( projectId ) {
      root.showLocalChangesRequested( projectId )
    }
  }
}
