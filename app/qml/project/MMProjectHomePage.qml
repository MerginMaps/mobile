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
    onClicked: Qt.openUrlExternally( __inputHelp.merginDashboardLink )
  }

  MMWarningBubble {
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
    onClicked: root.createWorkspaceRequested()
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
        projectDisplayName: model.data(index, MM.ProjectsModel.ProjectFullName)
        projectId: model.data(index, MM.ProjectsModel.ProjectId)
        projectDescription: model.data(index, MM.ProjectsModel.ProjectDescription)
        projectStatus: model.data(index, MM.ProjectsModel.ProjectStatus) ? model.data(index, MM.ProjectsModel.ProjectStatus) : MM.ProjectStatus.NoVersion
        projectIsValid: model.data(index, MM.ProjectsModel.ProjectIsValid)
        projectIsPending: model.data(index, MM.ProjectsModel.ProjectSyncPending) ? model.data(index, MM.ProjectsModel.ProjectSyncPending) : false
        projectSyncProgress: model.data(index, MM.ProjectsModel.ProjectSyncProgress) ? model.data(index, MM.ProjectsModel.ProjectSyncProgress) : -1
        projectIsLocal: model.data(index, MM.ProjectsModel.ProjectIsLocal)
        projectIsMergin: model.data(index, MM.ProjectsModel.ProjectIsMergin)
        projectRemoteError: model.data(index, MM.ProjectsModel.ProjectRemoteError) ? model.data(index, MM.ProjectsModel.ProjectRemoteError) : ""
        property string projectFilePath: model.data(index, MM.ProjectsModel.ProjectFilePath)

        onOpenRequested: {
          if ( projectIsLocal )
            root.openProjectRequested( projectFilePath )
          else if ( !projectIsLocal && projectIsMergin && !projectSyncPending) {
            projectlist.downloadProjectDialog.relatedProjectId = projectId
            projectlist.downloadProjectDialog.open()
          }
        }
        onSyncRequested: projectlist.MM.ProjectsModel.syncProject( projectId )
        onMigrateRequested: projectlist.MM.ProjectsModel.migrateProject( projectId )
        onRemoveRequested: {
          removeDialog.relatedProjectId = projectId
          removeDialog.open()
        }
        onStopSyncRequested: projectlist.MM.ProjectsModel.stopProjectSync( projectId )
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
    hideActiveProject: true
    searchText: searchBar.text
    spacing: root.spacing

    listHeader: activeProjectComponent

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
  }
}
