/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Dialogs 1.2
import QtQuick.Layouts 1.12
import lc 1.0
import "../"
import "."

Item {
  id: root

  property int projectModelType: ProjectsModel.EmptyProjectsModel
  property string activeProjectId: ""
  property string searchText: ""

  signal openProjectRequested( string projectId, string projectFilePath )
  signal showLocalChangesRequested( string projectId )
  signal activeProjectDeleted()

  onSearchTextChanged: {
    if ( projectModelType === ProjectsModel.PublicProjectsModel ) {
      controllerModel.listProjects( searchText )
    }
    else viewModel.searchExpression = searchText
  }

  function refreshProjectList() {
    controllerModel.listProjects( searchText )
  }

  ListView {
    id: listview

    Component.onCompleted: {
      // set proper footer (add project / fetch more)
      if ( root.projectModelType === ProjectsModel.LocalProjectsModel )
        listview.footer = addProjectButtonComponent
      else
        listview.footer = loadingSpinnerComponent
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
    maximumFlickVelocity: __androidUtils.isAndroid ? InputStyle.scrollVelocityAndroid : maximumFlickVelocity

    // Proxy model with source projects model
    model: ProjectsProxyModel {
      id: viewModel

      projectSourceModel: ProjectsModel {
        id: controllerModel

        merginApi: __merginApi
        localProjectsManager: __localProjectsManager
        modelType: root.projectModelType
      }
    }

    // Project delegate
    delegate: ProjectDelegateItem {
      id: projectDelegate

      width: parent.width
      height: InputStyle.projectItemHeight

      projectDisplayName: root.projectModelType === ProjectsModel.CreatedProjectsModel ? model.ProjectName : model.ProjectFullName
      projectId: model.ProjectId
      projectDescription: model.ProjectDescription
      projectStatus: model.ProjectSyncStatus ? model.ProjectSyncStatus : ProjectStatus.NoVersion
      projectIsValid: model.ProjectIsValid
      projectIsPending: model.ProjectPending ? model.ProjectPending : false
      projectSyncProgress: model.ProjectSyncProgress ? model.ProjectSyncProgress : -1
      projectIsLocal: model.ProjectIsLocal
      projectIsMergin: model.ProjectIsMergin
      projectRemoteError: model.ProjectRemoteError ? model.ProjectRemoteError : ""

      highlight: model.ProjectId === root.activeProjectId

      viewContentY: ListView.view.contentY
      viewHeight: ListView.view.height

      onOpenRequested: {
        if ( model.ProjectIsLocal )
          root.openProjectRequested( projectId, model.ProjectFilePath )
        else if ( !model.ProjectIsLocal && model.ProjectIsMergin && !model.ProjectPending) {
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

  Component {
    id: addProjectButtonComponent

    DelegateButton {
      width: parent.width
      height: InputStyle.rowHeight
      text: qsTr("Create project")
      visible: listview.count > 0

      onClicked: stackView.push(projectWizardComp)
    }
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

      RichTextBlock {
        id: noLocalProjectsText

        Layout.fillHeight: true
        Layout.fillWidth: true

        text: "<style>a:link { color: " + InputStyle.fontColor + "; }</style>" +
              qsTr( "No downloaded projects found.%1Learn %2how to create projects%3 and %4download them%3 onto your device." )
        .arg("<br/>")
        .arg("<a href='"+ __inputHelp.howToCreateNewProjectLink +"'>")
        .arg("</a>")
        .arg("<a href='"+ __inputHelp.howToDownloadProjectLink +"'>")

        onLinkActivated: Qt.openUrlExternally(link)
      }


      RichTextBlock {
        id: createProjectText

        Layout.fillHeight: true
        Layout.fillWidth: true

        text: qsTr( "You can also create new project by clicking button below." )
      }

      DelegateButton {
        id: createdProjectsWhenNone

        Layout.preferredHeight: InputStyle.rowHeight
        Layout.fillWidth: true

        text: qsTr( "Create project" )
        onClicked: stackView.push(projectWizardComp)
      }
    }
  }

  Label {
    id: noMerginProjectsTexts

    anchors.fill: parent
    horizontalAlignment: Qt.AlignHCenter
    verticalAlignment: Qt.AlignVCenter
    visible: reloadList.visible || !controllerModel.isLoading && ( projectModelType !== ProjectsModel.LocalProjectsModel && listview.count === 0 )
    text: reloadList.visible ? qsTr("Unable to get the list of projects.") : qsTr("No projects found!")
    color: InputStyle.fontColor
    font.pixelSize: InputStyle.fontPixelSizeNormal
    font.bold: true
  }

  Item {
    id: reloadList

    width: parent.width
    height: InputStyle.rowHeightHeader
    visible: false
    y: root.height/3 * 2

    Connections {
      target: __merginApi

      onListProjectsFailed: {
        reloadList.visible = root.projectModelType !== ProjectsModel.LocalProjectsModel // show reload list to all models except local
      }

      onListProjectsFinished: {
        if ( projectCount > -1 )
          reloadList.visible = false
      }
    }

    Button {
      id: reloadBtn
      width: reloadList.width - 2* InputStyle.panelMargin
      height: reloadList.height
      text: qsTr("Retry")
      font.pixelSize: InputStyle.fontPixelSizeNormal
      anchors.horizontalCenter: parent.horizontalCenter
      onClicked: {
        // filters suppose to not change
        controllerModel.listProjects( root.searchText )
      }
      background: Rectangle {
        color: InputStyle.highlightColor
        radius: InputStyle.cornerRadius
      }

      contentItem: Text {
        text: reloadBtn.text
        font: reloadBtn.font
        color: InputStyle.clrPanelMain
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        elide: Text.ElideRight
      }
    }
  }

  MessageDialog {
    id: removeDialog

    property string relatedProjectId

    title: qsTr( "Remove project" )
    text: qsTr( "Any unsynchronized changes will be lost." )
    icon: StandardIcon.Warning
    standardButtons: StandardButton.Ok | StandardButton.Cancel

    //! Using onButtonClicked instead of onAccepted,onRejected which have been called twice
    onButtonClicked: {
      if (clickedButton === StandardButton.Ok) {
        if (relatedProjectId === "")
          return

        if ( root.activeProjectId === relatedProjectId )
          root.activeProjectDeleted()

        __inputUtils.log(
              "Delete project",
              "Project " + __localProjectsManager.projectName( relatedProjectId ) + " deleted by " +
              ( __merginApi.userAuth ? __merginApi.userAuth.username : "unknown" ) + " (" + __localProjectsManager.projectChanges( relatedProjectId ) + ")" )

        controllerModel.removeLocalProject( relatedProjectId )

        removeDialog.relatedProjectId = ""
        visible = false
      }
      else if (clickedButton === StandardButton.Cancel) {
        removeDialog.relatedProjectId = ""
        visible = false
      }
    }
  }

  MessageDialog {
    id: downloadProjectDialog

    property string relatedProjectId

    title: qsTr( "Download project" )
    text: qsTr( "Would you like to download the project\n %1 ?" ).arg( relatedProjectId )
    icon: StandardIcon.Question
    standardButtons: StandardButton.Yes | StandardButton.No
    onYes: controllerModel.syncProject( relatedProjectId )
  }
}
