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
import QtQuick.Dialogs
import QtQuick.Layouts
import lc 1.0

import "../components"
import "../dialogs"
import "."

Item {
  id: root

  property int projectModelType: ProjectsModel.EmptyProjectsModel
  property string activeProjectId: ""
  property string searchText: ""
  property int spacing: 0

  signal openProjectRequested( string projectFilePath )
  signal showLocalChangesRequested( string projectId )
  signal activeProjectDeleted()

  onSearchTextChanged: {
    if ( projectModelType !== ProjectsModel.LocalProjectsModel ) {
      controllerModel.listProjects( root.searchText )
    }
    else viewModel.searchExpression = root.searchText
  }

  function refreshProjectList() {
    controllerModel.listProjects( searchText )
  }

  ListView {
    id: listview

    Component.onCompleted: {
      // set proper footer (add project / fetch more)
      if ( root.projectModelType === ProjectsModel.LocalProjectsModel ) {
        addProjectButton.addToPanel = true
      }
      else
      {
        listview.footer = loadingSpinnerComponent
      }
    }

    onAtYEndChanged: {
      if ( atYEnd ) { // user reached end of the list
        if ( controllerModel.hasMoreProjects && !controllerModel.isLoading ) {
          controllerModel.fetchAnotherPage( viewModel.searchExpression )
        }
      }
    }

    anchors.fill: parent
    clip: true
    spacing: root.spacing

    maximumFlickVelocity: __androidUtils.isAndroid ? __style.scrollVelocityAndroid : maximumFlickVelocity

    // Proxy model with source projects model
    model: ProjectsProxyModel {
      id: viewModel

      projectSourceModel: ProjectsModel {
        id: controllerModel

        merginApi: __merginApi
        localProjectsManager: __localProjectsManager
        syncManager: __syncManager
        modelType: root.projectModelType
      }
    }

    // Project delegate
    delegate: MMProjectItem {
      id: projectDelegate

      width: ListView.view.width

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

      highlight: model.ProjectId === root.activeProjectId

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

  Component {
    id: loadingSpinnerComponent

    LoadingSpinner {
      x: parent.width / 2 - width / 2
      running: controllerModel.isLoading
    }
  }

  MMButton {
    id: addProjectButton
    property bool addToPanel: false
    width: parent.width - 2 * __style.pageMargins
    anchors.bottom: parent.bottom
    anchors.bottomMargin: __style.pageMargins
    anchors.horizontalCenter: parent.horizontalCenter
    visible: addToPanel && (listview.count > 0)
    text: qsTr("Create project")
    onClicked: stackView.push(projectWizardComp)
  }


  Item {
    id: noLocalProjectsMessageContainer

    visible: listview.count === 0 && // this check is getting longer and longer, would be good to replace with states
             projectModelType === ProjectsModel.LocalProjectsModel &&
             root.searchText === "" &&
             !controllerModel.isLoading

    anchors.fill: parent

    ColumnLayout {
      id: colayout

      anchors.fill: parent
      spacing: 0

      MMMessage {
        id: noLocalProjectsText

        Layout.fillHeight: true
        Layout.fillWidth: true

        image: __style.positiveMMSymbolImage
        title: qsTr( "No downloaded projects found")
        description: "<style>a:link { color: " + __style.forestColor + "; }</style>" +
                     qsTr( "Learn %1how to create projects%2 and %3download them%2 onto your device. You can also create new project by clicking button below." )
                    .arg("<a href='"+ __inputHelp.howToCreateNewProjectLink +"'>")
                    .arg("</a>")
                    .arg("<a href='"+ __inputHelp.howToDownloadProjectLink +"'>")
      }

      MMButton {
        id: createdProjectsWhenNone
        Layout.fillWidth: true
        text: qsTr("Create project")

        onClicked: stackView.push(projectWizardComp)
      }
    }
  }

  MMMessage {
    id: noMerginProjectsTexts

    anchors.centerIn: parent
    visible: reloadList.visible || !controllerModel.isLoading && ( projectModelType !== ProjectsModel.LocalProjectsModel && listview.count === 0 )
    title: reloadList.visible ? qsTr("Unable to get the list of projects.") : qsTr("No projects found!")
    image: __style.noWifiImage
  }

  Item {
    id: reloadList

    width: parent.width
    height: __style.row63
    visible: false
    y: root.height/3 * 2

    Connections {
      target: __merginApi

      function onListProjectsFailed() {
        reloadList.visible = root.projectModelType !== ProjectsModel.LocalProjectsModel // show reload list to all models except local
      }

      function onListProjectsFinished( merginProjects, projectCount, page, requestId ) {
        if ( projectCount > -1 )
          reloadList.visible = false
      }
    }

    MMButton {
      id: reloadBtn

      width: reloadList.width - 2* __style.pageMargins
      height: reloadList.height
      text: qsTr("Retry")

      anchors.horizontalCenter: parent.horizontalCenter
      onClicked: {
        // filters suppose to not change
        controllerModel.listProjects( root.searchText )
      }
    }
  }

  MMRemoveProjectDialog {
    id: removeDialog

    onRemoveClicked: {
      if (relatedProjectId === "") {
        return
      }

      if ( root.activeProjectId === relatedProjectId )
        root.activeProjectDeleted()

      __inputUtils.log(
            "Delete project",
            "Project " + __localProjectsManager.projectName( relatedProjectId ) + " deleted by " +
            ( __merginApi.userAuth ? __merginApi.userAuth.username : "unknown" ) + " (" + __localProjectsManager.projectChanges( relatedProjectId ) + ")" )

      controllerModel.removeLocalProject( relatedProjectId )

      removeDialog.relatedProjectId = ""
    }
  }

  MMDownloadProjectDialog {
    id: downloadProjectDialog

    onDownloadClicked: {
      controllerModel.syncProject( relatedProjectId )
      downloadProjectDialog.relatedProjectId = ""
    }
  }
}
