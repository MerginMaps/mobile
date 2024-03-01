/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import lc 1.0

import "../components"
import "../misc"
import "../inputs"
import "."

Item {
  id: root

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

    onSearchTextChanged: function(text) {
    }

    anchors {
      top: parent.top
      left: parent.left
      right: parent.right
    }

    allowTimer: true
  }

  MMWarningBubble {
    id: warningBubble
    visible: __merginApi.subscriptionInfo ? __merginApi.subscriptionInfo.actionRequired : false
    width: root.width - 2 * root.hPadding
    anchors {
      top: searchBar.bottom
      left: parent.left
      right: parent.right
      topMargin: root.spacing
    }
    title: qsTr("Your attention is required.")
    description: qsTr("Click here to visit Mergin Maps dashboard")
    onClicked: Qt.openUrlExternally( __inputHelp.merginDashboardLink )
  }

  Component {
    id: activeProjectComponent

    Column {
      id: currentProjectColumn

      width: ListView.view.width
      visible: activeProjectItem.model !== undefined

      Text {
        width: parent.width
        height: 31 * __dp
        text: qsTr("Currently open")
        font: __style.p6
        color: __style.nightColor
        wrapMode: Text.WordWrap
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
      }

      MMLine {}

      Item { width: 1; height: 20 * __dp }

      MMProjectItem {
        id: activeProjectItem

        property var model: projectlist.projectsModel
        property var index: projectlist.projectsModel.projectModelIndexFromId(root.activeProjectId)

        width: parent.width

        highlight: true
        projectDisplayName: model.data(index, ProjectsModel.ProjectFullName)
        projectId: model.data(index, ProjectsModel.ProjectId)
        projectDescription: model.data(index, ProjectsModel.ProjectDescription)
        projectStatus: model.data(index, ProjectsModel.ProjectStatus) ? model.data(index, ProjectsModel.ProjectStatus) : ProjectStatus.NoVersion
        projectIsValid: model.data(index, ProjectsModel.ProjectIsValid)
        projectIsPending: model.data(index, ProjectsModel.ProjectSyncPending) ? model.data(index, ProjectsModel.ProjectSyncPending) : false
        projectSyncProgress: model.data(index, ProjectsModel.ProjectSyncProgress) ? model.data(index, ProjectsModel.ProjectSyncProgress) : -1
        projectIsLocal: model.data(index, ProjectsModel.ProjectIsLocal)
        projectIsMergin: model.data(index, ProjectsModel.ProjectIsMergin)
        projectRemoteError: model.data(index, ProjectsModel.ProjectRemoteError) ? model.data(index, ProjectsModel.ProjectRemoteError) : ""
        property string projectFilePath: model.data(index, ProjectsModel.ProjectFilePath)

        onOpenRequested: {
          if ( projectIsLocal )
            root.openProjectRequested( projectFilePath )
          else if ( !projectIsLocal && projectIsMergin && !projectSyncPending) {
            projectlist.downloadProjectDialog.relatedProjectId = projectId
            projectlist.downloadProjectDialog.open()
          }
        }
        onSyncRequested: projectlist.projectsModel.syncProject( projectId )
        onMigrateRequested: projectlist.projectsModel.migrateProject( projectId )
        onRemoveRequested: {
          removeDialog.relatedProjectId = projectId
          removeDialog.open()
        }
        onStopSyncRequested: projectlist.projectsModel.stopProjectSync( projectId )
        onShowChangesRequested: root.showLocalChangesRequested( projectId )
      }

      Item { width: 1; height: 40 * __dp }

      Text {
        width: parent.width
        height: 31 * __dp
        text: qsTr("Downloaded projects")
        font: __style.p6
        color: __style.nightColor
        wrapMode: Text.WordWrap
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
      }

      MMLine {}

      Item { width: 1; height: 20 * __dp }
    }
  }

  MMProjectList {
    id: projectlist

    projectModelType: ProjectsModel.LocalProjectsModel
    activeProjectId: root.activeProjectId
    hideActiveProject: true
    searchText: searchBar.text
    spacing: root.spacing

    listHeader: activeProjectComponent

    anchors {
      left: parent.left
      right: parent.right
      top: {
        if( warningBubble.visible )
          return warningBubble.bottom
        return searchBar.bottom
      }
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
