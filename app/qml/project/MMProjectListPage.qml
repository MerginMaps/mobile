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
import "../components"
import "../inputs"

Page {
  id: root

  property string headerTitle: qsTr("Projects")
  property bool withSearch: false
  property alias model: listView.model
  property bool showCreateProjectButton: false

  property string activeProjectId: ""
  property int projectModelType: ProjectsModel.EmptyProjectsModel

  signal backClicked
  signal createProjectClicked
  signal personIconClicked

  Rectangle {
    anchors.fill: parent
    color: __style.lightGreenColor
  }

  header: MMHeader {
    id: header

    title: root.headerTitle
    color: __style.lightGreenColor
    personIconVisible: true
    rightMarginShift: 40 * __dp

    onBackClicked: root.backClicked()
    onPersonIconClicked: root.personIconClicked()
  }

  Column {
    width: parent.width - 40 * __dp
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.top: parent.top
    anchors.topMargin: 10 * __dp
    spacing: 20 * __dp

    MMSearchEditor {
      id: searchBar

      width: parent.width - 2 * root.padding
      placeholderText: qsTr("Search for projects...")
      visible: root.withSearch

      onSearchTextChanged: function(text) {
        //root.model.searchExpression = text
      }
    }

    ListView {
      id: listView

      width: parent.width - 2 * root.padding
      height: ApplicationWindow.window ? ApplicationWindow.window.height - searchBar.height - 130 * __dp : 0
      clip: true
      spacing: 10 * __dp

      delegate: MMProjectItem {
        width: listView.width

        highlight: model.ProjectId === root.activeProjectId
        projectId: model.ProjectId
        projectStatus: model.ProjectStatus ? model.ProjectStatus : ProjectStatus.NoVersion
        projectDisplayName: root.projectModelType === ProjectsModel.CreatedProjectsModel ? model.ProjectName : model.ProjectFullName
        projectDescription: model.ProjectDescription
        projectIsValid: model.ProjectIsValid
        projectIsLocal: model.ProjectIsLocal
        projectIsMergin: model.ProjectIsMergin
        projectIsPending: model.ProjectSyncPending ? model.ProjectSyncPending : false
        projectSyncProgress: model.ProjectSyncProgress ? model.ProjectSyncProgress : -1
        projectRemoteError: model.ProjectRemoteError ? model.ProjectRemoteError : ""

        onOpenRequested: console.log("onOpenRequested")
        onStopSyncRequested: projectIsPending = false
        onShowChangesRequested: console.log("onShowChangesRequested")
        onSyncRequested: projectIsPending = true
        onRemoveRequested: console.log("onRemoveRequested")
        onMigrateRequested: console.log("onMigrateRequested")
      }

      footer: Item { width: 1; height: createProjectButton.visible ? 80 * __dp : 0 }
    }
  }

  MMButton {
    id: createProjectButton

    width: root.width - 2 * 20 * __dp
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.bottom: parent.bottom
    anchors.bottomMargin: 20 * __dp

    visible: root.showCreateProjectButton
    text: qsTr("Create project")

    onClicked: root.createProjectClicked()
  }
}

