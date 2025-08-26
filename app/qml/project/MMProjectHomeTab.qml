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

  // TODO (in future): refactor to ColumnLayout (to avoid the dynamic reparenting)

  property string activeProjectId: ""
  property alias list: projectlist
  property alias noWorkspaceBannerVisible: noWorkspaceBanner.visible
  property alias deviceSharingPolicyViolationBannerVisible: deviceSharingPolicyViolationBanner.visible

  signal openProjectRequested( string projectFilePath )
  signal showLocalChangesRequested( string projectId )
  signal createWorkspaceRequested()

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

  MMInfoBox {
    id: deviceSharingPolicyViolationBanner

    visible: !noWorkspaceBanner.visible && __merginApi.workspaceInfo.serviceState === MMerginWorkspaceInfo.DeviceSharePolicyViolation
    width: root.width - 2 * root.hPadding
    anchors {
      top: searchBar.bottom
      left: parent.left
      right: parent.right
      topMargin: root.spacing
    }
    title: qsTr("Device sharing policy violation")
    description: qsTr("One or more devices from your workspace are used across multiple accounts. Act now to avoid losing access to your data.")
    imageSource: __style.warnLogoImage

    color: __style.nightColor
    textColor: __style.polarColor

    onClicked: Qt.openUrlExternally( __inputHelp.merginDashboardLink )
  }

  MMProjectList {
    id: projectlist

    projectModelType: MM.ProjectsModel.LocalProjectsModel
    activeProjectId: root.activeProjectId
    searchText: searchBar.searchText

    spacing: root.spacing

    anchors {
      left: parent.left
      right: parent.right
      top: {
        if( deviceSharingPolicyViolationBanner.visible )
          return deviceSharingPolicyViolationBanner.bottom
        if( noWorkspaceBanner.visible )
          return noWorkspaceBanner.bottom
        return searchBar.bottom
      }
      bottom: parent.bottom
      topMargin: __style.margin20
    }

    activeProjectAlwaysFirst: true
    projectsModel.activeProjectId: root.activeProjectId

    list.section {
      property: "ProjectIsActiveProject"
      criteria: ViewSection.FullString
      delegate: Column {
        width: ListView.view.width
        spacing: __style.margin6

        MMText {
          width: parent.width
          text: section === "true" ? qsTr("Currently open") : qsTr("Downloaded projects")
          font: __style.p6
          color: __style.nightColor
          wrapMode: Text.WordWrap
          verticalAlignment: Text.AlignVCenter
          elide: Text.ElideRight
        }

        MMLine {}

        Item { width: 1; height: __style.margin14 }

        }
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

  MMRemoveProjectDialog {
    id: removeProjectDialog

    onRemoveClicked: {
      if (relatedProjectId === "") {
        return
      }

      __inputUtils.log(
            "Delete project",
            "Project " + __localProjectsManager.projectName( relatedProjectId ) + " deleted by " +
            ( __merginApi.userInfo ? __merginApi.userInfo.username : "unknown" ) + " (" + __localProjectsManager.projectChanges( relatedProjectId ) + ")" )

      projectlist.projectsModel.removeLocalProject( relatedProjectId )

      removeProjectDialog.relatedProjectId = ""
    }
  }
}
