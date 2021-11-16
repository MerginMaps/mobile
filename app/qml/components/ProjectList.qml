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
  property string activeProjectId: ""
  property int projectModelType: ProjectsModel.EmptyProjectsModel
  property string searchText: ""

  signal activeProjectDeleted
  signal openProjectRequested(string projectId, string projectFilePath)
  function refreshProjectList() {
    controllerModel.listProjects(searchText);
  }
  signal showLocalChangesRequested(string projectId)

  onSearchTextChanged: {
    if (projectModelType === ProjectsModel.PublicProjectsModel) {
      controllerModel.listProjects(searchText);
    } else
      viewModel.searchExpression = searchText;
  }

  ListView {
    id: listview
    anchors.fill: parent
    clip: true
    maximumFlickVelocity: __androidUtils.isAndroid ? InputStyle.scrollVelocityAndroid : maximumFlickVelocity

    Component.onCompleted: {
      // set proper footer (add project / fetch more)
      if (root.projectModelType === ProjectsModel.LocalProjectsModel)
        listview.footer = addProjectButtonComponent;
      else
        listview.footer = loadingSpinnerComponent;
    }
    onAtYEndChanged: {
      if (atYEnd) {
        // user reached end of the list
        if (controllerModel.hasMoreProjects && !controllerModel.isLoading) {
          controllerModel.fetchAnotherPage(viewModel.searchExpression);
        }
      }
    }

    // Project delegate
    delegate: ProjectDelegateItem {
      id: projectDelegate
      height: InputStyle.projectItemHeight
      highlight: model.ProjectId === root.activeProjectId
      projectDescription: model.ProjectDescription
      projectDisplayName: root.projectModelType === ProjectsModel.CreatedProjectsModel ? model.ProjectName : model.ProjectFullName
      projectId: model.ProjectId
      projectIsLocal: model.ProjectIsLocal
      projectIsMergin: model.ProjectIsMergin
      projectIsPending: model.ProjectPending ? model.ProjectPending : false
      projectIsValid: model.ProjectIsValid
      projectRemoteError: model.ProjectRemoteError ? model.ProjectRemoteError : ""
      projectStatus: model.ProjectSyncStatus ? model.ProjectSyncStatus : ProjectStatus.NoVersion
      projectSyncProgress: model.ProjectSyncProgress ? model.ProjectSyncProgress : -1
      viewContentY: ListView.view.contentY
      viewHeight: ListView.view.height
      width: parent.width

      onMigrateRequested: controllerModel.migrateProject(projectId)
      onOpenRequested: {
        if (model.ProjectIsLocal)
          root.openProjectRequested(projectId, model.ProjectFilePath);
        else if (!model.ProjectIsLocal && model.ProjectIsMergin && !model.ProjectPending) {
          downloadProjectDialog.relatedProjectId = model.ProjectId;
          downloadProjectDialog.open();
        }
      }
      onRemoveRequested: {
        removeDialog.relatedProjectId = projectId;
        removeDialog.open();
      }
      onShowChangesRequested: root.showLocalChangesRequested(projectId)
      onStopSyncRequested: controllerModel.stopProjectSync(projectId)
      onSyncRequested: controllerModel.syncProject(projectId)
    }

    // Proxy model with source projects model
    model: ProjectsProxyModel {
      id: viewModel
      projectSourceModel: ProjectsModel {
        id: controllerModel
        localProjectsManager: __localProjectsManager
        merginApi: __merginApi
        modelType: root.projectModelType
      }
    }
  }
  Component {
    id: loadingSpinnerComponent
    LoadingSpinner {
      running: controllerModel.isLoading
      x: parent.width / 2 - width / 2
    }
  }
  Component {
    id: addProjectButtonComponent
    DelegateButton {
      height: InputStyle.rowHeight
      text: qsTr("Create project")
      visible: listview.count > 0
      width: parent.width

      onClicked: stackView.push(projectWizardComp)
    }
  }
  Item {
    id: noLocalProjectsMessageContainer
    anchors.fill: parent
    visible: listview.count === 0 && // this check is getting longer and longer, would be good to replace with states
    projectModelType === ProjectsModel.LocalProjectsModel && root.searchText === "" && !controllerModel.isLoading

    ColumnLayout {
      id: colayout
      anchors.fill: parent
      spacing: 0

      RichTextBlock {
        id: noLocalProjectsText
        Layout.fillHeight: true
        Layout.fillWidth: true
        text: "<style>a:link { color: " + InputStyle.fontColor + "; }</style>" + qsTr("No downloaded projects found.%1Learn %2how to create projects%3 and %4download them%3 onto your device.").arg("<br/>").arg("<a href='" + __inputHelp.howToCreateNewProjectLink + "'>").arg("</a>").arg("<a href='" + __inputHelp.howToDownloadProjectLink + "'>")

        onLinkActivated: Qt.openUrlExternally(link)
      }
      RichTextBlock {
        id: createProjectText
        Layout.fillHeight: true
        Layout.fillWidth: true
        text: qsTr("You can also create new project by clicking button below.")
      }
      DelegateButton {
        id: createdProjectsWhenNone
        Layout.fillWidth: true
        Layout.preferredHeight: InputStyle.rowHeight
        text: qsTr("Create project")

        onClicked: stackView.push(projectWizardComp)
      }
    }
  }
  Label {
    id: noMerginProjectsTexts
    anchors.fill: parent
    color: InputStyle.fontColor
    font.bold: true
    font.pixelSize: InputStyle.fontPixelSizeNormal
    horizontalAlignment: Qt.AlignHCenter
    text: reloadList.visible ? qsTr("Unable to get the list of projects.") : qsTr("No projects found!")
    verticalAlignment: Qt.AlignVCenter
    visible: reloadList.visible || !controllerModel.isLoading && (projectModelType !== ProjectsModel.LocalProjectsModel && listview.count === 0)
  }
  Item {
    id: reloadList
    height: InputStyle.rowHeightHeader
    visible: false
    width: parent.width
    y: root.height / 3 * 2

    Connections {
      target: __merginApi

      onListProjectsFailed: {
        reloadList.visible = root.projectModelType !== ProjectsModel.LocalProjectsModel; // show reload list to all models except local
      }
      onListProjectsFinished: {
        if (projectCount > -1)
          reloadList.visible = false;
      }
    }
    Button {
      id: reloadBtn
      anchors.horizontalCenter: parent.horizontalCenter
      font.pixelSize: reloadBtn.height / 2
      height: reloadList.height
      text: qsTr("Retry")
      width: reloadList.width - 2 * InputStyle.panelMargin

      onClicked: {
        // filters suppose to not change
        controllerModel.listProjects(root.searchText);
      }

      background: Rectangle {
        color: InputStyle.highlightColor
        radius: InputStyle.cornerRadius
      }
      contentItem: Text {
        color: InputStyle.clrPanelMain
        elide: Text.ElideRight
        font: reloadBtn.font
        horizontalAlignment: Text.AlignHCenter
        text: reloadBtn.text
        verticalAlignment: Text.AlignVCenter
      }
    }
  }
  MessageDialog {
    id: removeDialog
    property string relatedProjectId

    icon: StandardIcon.Warning
    standardButtons: StandardButton.Ok | StandardButton.Cancel
    text: qsTr("Any unsynchronized changes will be lost.")
    title: qsTr("Remove project")

    //! Using onButtonClicked instead of onAccepted,onRejected which have been called twice
    onButtonClicked: {
      if (clickedButton === StandardButton.Ok) {
        if (relatedProjectId === "")
          return;
        if (root.activeProjectId === relatedProjectId)
          root.activeProjectDeleted();
        controllerModel.removeLocalProject(relatedProjectId);
        removeDialog.relatedProjectId = "";
        visible = false;
      } else if (clickedButton === StandardButton.Cancel) {
        removeDialog.relatedProjectId = "";
        visible = false;
      }
    }
  }
  MessageDialog {
    id: downloadProjectDialog
    property string relatedProjectId

    icon: StandardIcon.Question
    standardButtons: StandardButton.Yes | StandardButton.No
    text: qsTr("Would you like to download the project\n %1 ?").arg(relatedProjectId)
    title: qsTr("Download project")

    onYes: controllerModel.syncProject(relatedProjectId)
  }
}
