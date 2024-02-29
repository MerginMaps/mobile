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
    description: qsTr("Please visit the %1Mergin dashboard%2.")
    .arg("<a style=\"text-decoration: underline; color:" + __style.whiteColor + ";\" href='" + __inputHelp.merginDashboardLink + "'>")
    .arg("</a>")

    onClicked: Qt.openUrlExternally( __inputHelp.merginDashboardLink )
  }

  Column {
    id: currentProjectColumn

    width: parent.width
    visible: root.activeProjectId !== ""

    anchors {
      top: warningBubble.visible ? warningBubble.bottom : searchBar.bottom
      left: parent.left
      right: parent.right
      topMargin: 2 * root.spacing
    }

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

    Repeater {
      width: parent.width
      model: ProjectsProxyModel {
        id: viewModel

        projectSourceModel: ProjectsModel {
          id: controllerModel

          merginApi: __merginApi
          localProjectsManager: __localProjectsManager
          syncManager: __syncManager
          modelType: ProjectsModel.LocalProjectsModel
        }
      }
      delegate: MMProjectItem {
        id: projectDelegate

        width: currentProjectColumn.width

        projectDisplayName: root.projectModelType === ProjectsModel.CreatedProjectsModel ? model.ProjectName : model.ProjectFullName
        projectId: model.ProjectId
        projectDescription: model.ProjectDescription
        projectStatus: model.ProjectStatus ? model.ProjectStatus : ProjectStatus.NoVersion
        projectIsValid: model.ProjectIsValid
        projectIsPending: model.ProjectSyncPending ? model.ProjectSyncPending : false
        projectSyncProgress: model.ProjectSyncProgress ? model.ProjectSyncProgress : -1
        projectIsLocal: model.ProjectIsLocal
        projectIsMergin: model.ProjectIsMergin
        projectRemoteError: model.ProjectRemoteError ? model.ProjectRemoteError : ""

        visible: model.ProjectId === root.activeProjectId

        onOpenRequested: {
          if ( model.ProjectIsLocal )
            root.openProjectRequested( model.ProjectFilePath )
          else if ( !model.ProjectIsLocal && model.ProjectIsMergin && !model.ProjectSyncPending) {
            downloadProjectDialog.relatedProjectId = model.ProjectId
            downloadProjectDialog.open()
          }
        }
        onSyncRequested: controllerModel.syncProject( projectId )
        onMigrateRequested: controllerModel.migrateProject( projectId )
        onRemoveRequested: {
          removeDialog.relatedProjectId = projectId
          removeDialog.open()
        }
        onStopSyncRequested: controllerModel.stopProjectSync( projectId )
        onShowChangesRequested: root.showLocalChangesRequested( projectId )
      }
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

  MMProjectList {
    id: projectlist

    projectModelType: ProjectsModel.LocalProjectsModel
    activeProjectId: root.activeProjectId
    hideActiveProject: true
    searchText: searchBar.text
    spacing: root.spacing

    anchors {
      left: parent.left
      right: parent.right
      top: {
        if( currentProjectColumn.visible )
          return currentProjectColumn.bottom
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
