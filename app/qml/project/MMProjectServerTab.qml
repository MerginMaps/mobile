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

  property int spacing: __style.margin12

  MMSearchInput {
    id: searchBar

    width: parent.width - 2 * root.padding
    placeholderText: qsTr("Search for projects...")

    anchors {
      top: parent.top
      left: parent.left
      right: parent.right
    }

    delayedSearch: true
    emitInterval: 400
  }


  MMProjectList {
    id: projectlist

    projectModelType: root.projectModelType
    activeProjectId: root.activeProjectId
    searchText: searchBar.searchText
    spacing: root.spacing

    anchors {
      left: parent.left
      right: parent.right
      top: searchBar.bottom
      bottom: parent.bottom
      topMargin: __style.margin20
    }

    onOpenProjectRequested: function( projectFilePath ) {

      //
      // There was an issue when closing the projects controller while having the searchbar textField focused
      // (not neccessarily with keyboard opened). This is a kind-of workaround and hotfix.
      // It might be related to https://bugreports.qt.io/browse/QTBUG-123876 as it started
      // to occur more frequently when we upgraded to Qt 6.6.3
      //
      // See https://github.com/MerginMaps/mobile/issues/3027
      //
      if ( searchBar.textField.activeFocus ) {
        searchBar.textField.focus = false
      }

      root.openProjectRequested( projectFilePath )
    }
    onShowLocalChangesRequested: function( projectId ) {
      root.showLocalChangesRequested( projectId )
    }
  }
}
