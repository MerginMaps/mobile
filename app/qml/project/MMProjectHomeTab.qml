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

import "./components"
import "../components"
import "../inputs"
import "../dialogs"

Item {
  id: root

  property string activeProjectId: ""
  property alias list: projectlist
  property alias noWorkspaceBannerVisible: noWorkspaceBanner.visible

  signal openProjectRequested( string projectFilePath )
  signal showLocalChangesRequested( string projectId )
  signal createWorkspaceRequested()

  function refreshProjectsList() {
    searchBar.deactivate()
    projectlist.refreshProjectList( searchBar.text )
  }

  property int spacing: 10 * __dp

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
  }

  MMInfoBox {
    id: attentionRequiredBanner

    visible: !noWorkspaceBanner.visible && (__merginApi.subscriptionInfo ? __merginApi.subscriptionInfo.actionRequired : false)
    width: root.width - 2 * root.hPadding
    anchors {
      top: searchBar.bottom
      left: parent.left
      right: parent.right
      topMargin: root.spacing
    }
    title: qsTr("Your attention is required")
    description: qsTr("Click here to visit Mergin Maps dashboard")
    imageSource: __style.warnLogoImage

    color: __style.nightColor
    textColor: __style.polarColor

    onClicked: Qt.openUrlExternally( __inputHelp.merginDashboardLink )
  }

  MMInfoBox {
    id: noWorkspaceBanner

    visible: false // Set from parent
    width: root.width - 2 * root.hPadding
    anchors {
      top: searchBar.bottom
      left: parent.left
      right: parent.right
      topMargin: root.spacing
    }
    title: qsTr("No workspace detected")
    description: qsTr("Create your workspace by clicking here")
    imageSource: __style.noWorkspaceImage

    color: __style.nightColor
    textColor: __style.polarColor

    onClicked: root.createWorkspaceRequested()
  }

  Component {
    id: activeProjectComponent

    Column {
      id: currentProjectColumn

      width: ListView.view.width

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

      MMProjectDelegate {
        id: activeProjectItem

        property var model: projectlist.projectsModel
        property var index: projectlist.projectsModel.projectModelIndexFromId(root.activeProjectId)

        property string projectRemoteError: model.data(index, MM.ProjectsModel.ProjectRemoteError) ? model.data(index, MM.ProjectsModel.ProjectRemoteError) : ""

        width: parent.width

        projectIsOpened: true
        projectDisplayName: model.data(index, MM.ProjectsModel.ProjectFullName) ? model.data(index, MM.ProjectsModel.ProjectFullName) : ""
        projectId: model.data(index, MM.ProjectsModel.ProjectId) ? model.data(index, MM.ProjectsModel.ProjectId) : ""
        projectDescription: model.data(index, MM.ProjectsModel.ProjectDescription) ? model.data(index, MM.ProjectsModel.ProjectDescription) : ""
        projectIsInSync: model.data(index, MM.ProjectsModel.ProjectSyncPending) ? model.data(index, MM.ProjectsModel.ProjectSyncPending) : false
        projectSyncProgress: model.data(index, MM.ProjectsModel.ProjectSyncProgress) ? model.data(index, MM.ProjectsModel.ProjectSyncProgress) : -1
        property string projectFilePath: model.data(index, MM.ProjectsModel.ProjectFilePath) ? model.data(index, MM.ProjectsModel.ProjectFilePath) : ""

        state: {
          let status = model.data(index, MM.ProjectsModel.ProjectStatus) ? model.data(index, MM.ProjectsModel.ProjectStatus) : MM.ProjectStatus.NoVersion

          if ( status === MM.ProjectStatus.NeedsSync ) {
            return "NeedsSync"
          }
          else if ( status === MM.ProjectStatus.UpToDate )
          {
            return "UpToDate"
          }
          else if ( status === MM.ProjectStatus.NoVersion )
          {
            return "NeedsSync"
          }

          return "UpToDate" // fallback, should never happen
        }

        projectActionButtons: {
          let status = model.data(index, MM.ProjectsModel.ProjectStatus) ? model.data(index, MM.ProjectsModel.ProjectStatus) : MM.ProjectStatus.NoVersion

          if ( status === MM.ProjectStatus.NeedsSync ) {
            return ["sync", "changes", "remove"]
          }
          else if ( status === MM.ProjectStatus.NoVersion ) {
            return ["upload", "remove"]
          }
          return ["changes", "remove"] // UpToDate
        }

        onOpenRequested: root.openProjectRequested( projectFilePath )

        onSyncRequested: {
          if ( projectRemoteError ) {
            __notificationModel.addError( qsTr( "Could not synchronize project, please make sure you are logged in and have sufficient rights." ) )
          }
          else if ( !model.data(index, MM.ProjectsModel.ProjectIsMergin) ) {
            projectlist.projectsModel.migrateProject( projectId )
          }
          else {
            projectlist.projectsModel.syncProject( projectId )
          }
        }

        onMigrateRequested: projectlist.projectsModel.migrateProject( projectId )
        onRemoveRequested: {
          removeProjectDialog.relatedProjectId = projectId
          removeProjectDialog.open()
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

    projectModelType: MM.ProjectsModel.LocalProjectsModel
    activeProjectId: root.activeProjectId
    hideActiveProject: true // TODO: do not hide when searching!
    searchText: searchBar.searchText
    spacing: root.spacing

    anchors {
      left: parent.left
      right: parent.right
      top: {
        if( attentionRequiredBanner.visible )
          return attentionRequiredBanner.bottom
        if( noWorkspaceBanner.visible )
          return noWorkspaceBanner.bottom
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

    function updateListHeader() {
      // ugly ugly ugly #2
      projectlist.listHeader = null

      if ( root.activeProjectId ) {
        projectlist.listHeader = activeProjectComponent
      }
    }

    onActiveProjectIdChanged: projectlist.updateListHeader()

    Connections {
      target: projectlist.projectsModel

      function onModelReset() { projectlist.updateListHeader() }
    }
  }

  MMRemoveProjectDialog {
    id: removeProjectDialog

    onRemoveClicked: {
      if (relatedProjectId === "") {
        return
      }

      if ( root.activeProjectId === relatedProjectId ) {
        projectlist.activeProjectDeleted() // ugly, ugly, ugly
        projectlist.listHeader = null
      }

      __inputUtils.log(
            "Delete project",
            "Project " + __localProjectsManager.projectName( relatedProjectId ) + " deleted by " +
            ( __merginApi.userAuth ? __merginApi.userAuth.username : "unknown" ) + " (" + __localProjectsManager.projectChanges( relatedProjectId ) + ")" )

      projectlist.projectsModel.removeLocalProject( relatedProjectId )

      removeProjectDialog.relatedProjectId = ""
    }
  }
}
