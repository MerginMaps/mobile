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
import "../../app/qml/components"
import "../../app/qml/project"

MMProjectListPage {
  id: pane

  headerTitle: "Projects"
  withSearch: true
  showCreateProjectButton: true

  activeProjectId: "1"

  controllerModel: QtObject {
    signal syncProject( var projectId )
    signal migrateProject( var projectId )
    signal stopProjectSync( var projectId )

    onSyncProject: function (projectId) { console.log("onSyncProject " + projectId) }
    onMigrateProject: function (projectId) { console.log("onMigrateProject " + projectId) }
    onStopProjectSync: function (projectId) { console.log("onStopProjectSync " + projectId) }
  }

  model: ListModel {

    Component.onCompleted: {
      append( {ProjectId: "1",
               ProjectStatus: ProjectStatus.NoVersion,
               ProjectName: "ProjectName 1",
               ProjectFullName: "ProjectFullName 1",
               ProjectDescription: "Highlighted Mergin Local Valid Sync",
               ProjectIsValid: true,
               ProjectIsLocal: true,
               ProjectIsMergin: true,
               ProjectSyncPending: true,
               ProjectSyncProgress: 0.6
             } )
      append( {ProjectId: "2",
               ProjectStatus: ProjectStatus.NoVersion,
               ProjectName: "ProjectName 2",
               ProjectFullName: "ProjectFullName 2",
               ProjectDescription: "Mergin Local Valid Sync",
               ProjectIsValid: true,
               ProjectIsLocal: true,
               ProjectIsMergin: true,
               ProjectSyncPending: true,
               ProjectSyncProgress: 0.6,
               ProjectRemoteError: "ProjectRemoteError"
             } )
      append( {ProjectId: "3",
               ProjectStatus: ProjectStatus.NeedsSync,
               ProjectName: "ProjectName 3",
               ProjectFullName: "ProjectFullName 3",
               ProjectDescription: "Mergin Local Valid NeedsSync",
               ProjectIsValid: true,
               ProjectIsLocal: true,
               ProjectIsMergin: true,
               ProjectSyncPending: false
             } )
      append( {ProjectId: "4",
               ProjectStatus: ProjectStatus.NoVersion,
               ProjectName: "ProjectName 4",
               ProjectFullName: "ProjectFullName 4",
               ProjectDescription: "Local Valid",
               ProjectIsValid: true,
               ProjectIsLocal: true,
               ProjectIsMergin: false,
               ProjectSyncPending: false
             } )
      append( {ProjectId: "5",
               ProjectStatus: ProjectStatus.NoVersion,
               ProjectName: "ProjectName 5",
               ProjectFullName: "ProjectFullName 5",
               ProjectDescription: "Valid",
               ProjectIsValid: true,
               ProjectIsLocal: false,
               ProjectIsMergin: false,
               ProjectSyncPending: false
             } )
      append( {ProjectId: "6",
               ProjectStatus: ProjectStatus.NoVersion,
               ProjectName: "ProjectName 6",
               ProjectFullName: "ProjectFullName 6",
               ProjectDescription: "Invalid project",
               ProjectIsValid: false,
               ProjectIsLocal: false,
               ProjectIsMergin: false,
               ProjectSyncPending: false
             } )
      append( {ProjectId: "7",
               ProjectStatus: ProjectStatus.NoVersion,
               ProjectName: "ProjectName 7",
               ProjectFullName: "ProjectFullName 7",
               ProjectDescription: "Mergin Valid",
               ProjectIsValid: true,
               ProjectIsLocal: false,
               ProjectIsMergin: true,
               ProjectSyncPending: false
             } )
    }
  }

  onBackClicked: console.log("Back clicked")
  onCreateProjectClicked: console.log("Create project clicked")
  onPersonIconClicked: console.log("Person icon clicked")
  onShowLocalChangesRequested: function (projectId) { console.log("Show Local Changes " + projectId) }
}
