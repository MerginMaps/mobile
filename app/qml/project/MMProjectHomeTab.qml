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

    onSearchTextChanged: function(text) {
    }

    anchors {
      top: parent.top
      left: parent.left
      right: parent.right
    }

    allowTimer: true
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

  MMProjectList {
    id: projectlist

    projectModelType: MM.ProjectsModel.LocalProjectsModel
    activeProjectId: root.activeProjectId
    searchText: searchBar.text
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

    activeProjectAlwaysFirst: true
    projectsModel.activeProjectId: root.activeProjectId

    list.section {
      property: "ProjectIsActiveProject"
      criteria: ViewSection.FullString
      delegate: Column {
        width: ListView.view.width

        Text {
          width: parent.width
          height: 31 * __dp
          text: section === "true" ? qsTr("Currently open") : qsTr("Downloaded projects")
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

    onOpenProjectRequested: function( projectFilePath ) {
      root.openProjectRequested( projectFilePath )
    }
    onShowLocalChangesRequested: function( projectId ) {
      root.showLocalChangesRequested( projectId )
    }
  }

  MMRemoveProjectDialog {
    id: removeProjectDialog

    onRemoveClicked: {
      if (relatedProjectId === "") {
        return
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
