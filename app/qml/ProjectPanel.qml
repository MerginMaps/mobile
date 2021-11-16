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
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.2
import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "."  // import InputStyle singleton
import "components"

Item {
  id: root
  property string activeProjectId: ""
  property string activeProjectPath: ""

  focus: true
  visible: false

  signal closed
  function hidePanel() {
    root.visible = false;
    stackView.clearStackAndClose();
    root.closed();
  }
  function manageSubscriptionPlans() {
    if (__purchasing.hasInAppPurchases && (__purchasing.hasManageSubscriptionCapability || !__merginApi.subscriptionInfo.ownsActiveSubscription)) {
      stackView.push(subscribePanelComp);
    } else {
      Qt.openUrlExternally(__purchasing.subscriptionManageUrl);
    }
  }
  function openPanel() {
    root.visible = true;
    stackView.visible = true;
  }
  signal openProjectRequested(string projectId, string projectPath)
  signal resetView // resets view to state as when panel is opened

  onFocusChanged: {
    // pass focus to stackview
    stackView.focus = true;
  }

  StackView {
    id: stackView
    property bool pending: false

    anchors.fill: parent
    focus: true
    initialItem: projectsPanelComp
    visible: false
    z: root.z + 1

    function clearStackAndClose() {
      if (stackView.depth > 1)
        stackView.pop(null); // pops everything besides an initialItem
      stackView.visible = false;
    }
    function popOnePageOrClose() {
      if (stackView.depth > 1) {
        stackView.pop();
      }
    }

    Keys.onReleased: {
      if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
        event.accepted = true;
        if (stackView.depth > 1) {
          stackView.currentItem.back();
        } else if (root.activeProjectPath) {
          root.hidePanel();
        }
      }
    }
    onVisibleChanged: {
      if (stackView.visible)
        stackView.forceActiveFocus();
    }
  }
  BusyIndicator {
    id: busyIndicator
    anchors.centerIn: parent
    height: width
    running: stackView.pending
    visible: running
    width: parent.width / 8
    z: parent.z + 1
  }
  Component {
    id: projectsPanelComp
    Page {
      id: projectsPage
      function refreshProjectList(keepSearchFilter = false) {
        stackView.pending = true;
        switch (pageContent.state) {
        case "local":
          localProjectsPage.refreshProjectsList(keepSearchFilter);
          break;
        case "created":
          createdProjectsPage.refreshProjectsList(keepSearchFilter);
          break;
        case "shared":
          sharedProjectsPage.refreshProjectsList(keepSearchFilter);
          break;
        case "public":
          publicProjectsPage.refreshProjectsList(keepSearchFilter);
          break;
        }
      }
      function setupProjectOpen(projectId, projectPath) {
        activeProjectId = projectId;
        activeProjectPath = projectPath;
        openProjectRequested(projectId, projectPath);
        if (projectId && projectPath)
          // this is not project reset
          hidePanel();
      }
      function showChanges(projectId) {
        if (__merginProjectStatusModel.loadProjectInfo(projectId)) {
          stackView.push(statusPanelComp);
        } else
          __inputUtils.showNotification(qsTr("No Changes"));
      }

      Item {
        id: pageContent
        anchors.fill: parent

        onStateChanged: {
          __merginApi.pingMergin();
          refreshProjectList();
          pageFooter.setActiveButton(pageContent.state);
        }

        Connections {
          target: root

          onResetView: {
            if (pageContent.state === "created" || pageContent.state === "shared")
              pageContent.state = "local";
          }
          onVisibleChanged: {
            if (root.visible) {
              // projectsPanel opened
              pageContent.state = "local";
            } else {
              pageContent.state = "";
            }
          }
        }
        StackLayout {
          id: projectListLayout
          anchors.fill: parent
          currentIndex: pageFooter.currentIndex

          ProjectListPage {
            id: localProjectsPage
            activeProjectId: root.activeProjectId
            list.visible: !stackView.pending
            projectModelType: ProjectsModel.LocalProjectsModel

            list.onActiveProjectDeleted: setupProjectOpen("", "")
            onOpenProjectRequested: setupProjectOpen(projectId, projectFilePath)
            onShowLocalChangesRequested: showChanges(projectId)
          }
          ProjectListPage {
            id: createdProjectsPage
            activeProjectId: root.activeProjectId
            list.visible: !stackView.pending
            projectModelType: ProjectsModel.CreatedProjectsModel

            list.onActiveProjectDeleted: setupProjectOpen("", "")
            onOpenProjectRequested: setupProjectOpen(projectId, projectFilePath)
            onShowLocalChangesRequested: showChanges(projectId)
          }
          ProjectListPage {
            id: sharedProjectsPage
            activeProjectId: root.activeProjectId
            list.visible: !stackView.pending
            projectModelType: ProjectsModel.SharedProjectsModel

            list.onActiveProjectDeleted: setupProjectOpen("", "")
            onOpenProjectRequested: setupProjectOpen(projectId, projectFilePath)
            onShowLocalChangesRequested: showChanges(projectId)
          }
          ProjectListPage {
            id: publicProjectsPage
            activeProjectId: root.activeProjectId
            list.visible: !stackView.pending
            projectModelType: ProjectsModel.PublicProjectsModel

            list.onActiveProjectDeleted: setupProjectOpen("", "")
            onOpenProjectRequested: setupProjectOpen(projectId, projectFilePath)
            onShowLocalChangesRequested: showChanges(projectId)
          }
        }

        states: [
          State {
            name: "local"
          },
          State {
            name: "created"
          },
          State {
            name: "shared"
          },
          State {
            name: "public"
          }
        ]
      }

      // Other components
      Connections {
        target: __projectWizard

        onProjectCreated: {
          if (stackView.currentItem.objectName === "projectWizard") {
            stackView.popOnePageOrClose();
          }
        }
      }
      Connections {
        target: __merginApi

        onApiVersionStatusChanged: {
          stackView.pending = false;
          if (__merginApi.apiVersionStatus === MerginApiStatus.OK && stackView.currentItem.objectName === "authPanel") {
            if (__merginApi.userAuth.hasAuthData()) {
              refreshProjectList();
            } else if (pageContent.state !== 'local') {
              if (stackView.currentItem.objectName !== "authPanel") {
                stackView.push(authPanelComp, {
                    "state": "login"
                  });
              }
            }
          }
        }
        onAuthChanged: {
          stackView.pending = false;
          if (__merginApi.userAuth.hasAuthData()) {
            stackView.popOnePageOrClose();
            projectsPage.refreshProjectList();
            root.forceActiveFocus();
          }
        }
        onAuthFailed: stackView.pending = false
        onAuthRequested: {
          stackView.pending = false;
          stackView.push(authPanelComp, {
              "state": "login"
            });
        }
        onListProjectsByNameFinished: stackView.pending = false
        onListProjectsFinished: stackView.pending = false
        onRegistrationFailed: stackView.pending = false
        onRegistrationSucceeded: stackView.pending = false
      }

      background: Rectangle {
        anchors.fill: parent
        color: InputStyle.clrPanelMain
      }
      footer: TabBar {
        id: pageFooter
        property int itemSize: pageFooter.height * 0.8

        contentHeight: InputStyle.rowHeightHeader
        spacing: 0

        function setActiveButton(state) {
          switch (state) {
          case "local":
            pageFooter.setCurrentIndex(0);
            break;
          case "created":
            pageFooter.setCurrentIndex(1);
            break;
          case "shared":
            pageFooter.setCurrentIndex(2);
            break;
          case "public":
            pageFooter.setCurrentIndex(3);
            break;
          }
        }

        TabButton {
          id: localProjectsBtn
          onClicked: pageContent.state = "local"

          MainPanelButton {
            id: localProjectsInnerBtn
            faded: pageFooter.currentIndex !== localProjectsBtn.TabBar.index
            handleClicks: false
            imageSource: InputStyle.homeIcon
            text: qsTr("Home")
            width: pageFooter.itemSize
          }

          background: Rectangle {
            anchors.fill: parent
            color: InputStyle.fontColor
          }
        }
        TabButton {
          id: createdProjectsBtn
          onClicked: pageContent.state = "created"

          MainPanelButton {
            id: createdProjectsInnerBtn
            faded: pageFooter.currentIndex !== createdProjectsBtn.TabBar.index
            handleClicks: false
            imageSource: InputStyle.accountIcon
            text: qsTr("My projects")
            width: pageFooter.itemSize
          }

          background: Rectangle {
            anchors.fill: parent
            color: InputStyle.fontColor
          }
        }
        TabButton {
          id: sharedProjectsBtn
          onClicked: pageContent.state = "shared"

          MainPanelButton {
            id: sharedProjectsInnerBtn
            faded: pageFooter.currentIndex !== sharedProjectsBtn.TabBar.index
            handleClicks: false
            imageSource: InputStyle.accountMultiIcon
            text: parent.width > sharedProjectsInnerBtn.width * 2 ? qsTr("Shared with me") : qsTr("Shared")
            width: pageFooter.itemSize
          }

          background: Rectangle {
            anchors.fill: parent
            color: InputStyle.fontColor
          }
        }
        TabButton {
          id: publicProjectsBtn
          onClicked: pageContent.state = "public"

          MainPanelButton {
            id: publicProjectsInnerBtn
            faded: pageFooter.currentIndex !== publicProjectsBtn.TabBar.index
            handleClicks: false
            imageSource: InputStyle.exploreIcon
            text: qsTr("Explore")
            width: pageFooter.itemSize
          }

          background: Rectangle {
            anchors.fill: parent
            color: InputStyle.fontColor
          }
        }
      }
      header: PanelHeader {
        id: pageHeader
        color: InputStyle.clrPanelMain
        height: InputStyle.rowHeightHeader
        rowHeight: InputStyle.rowHeightHeader
        titleText: qsTr("Projects")
        withBackButton: root.activeProjectPath

        onBack: {
          if (root.activeProjectId) {
            root.hidePanel();
          }
        }

        Item {
          id: avatar
          anchors.right: parent.right
          anchors.rightMargin: InputStyle.panelMargin
          height: InputStyle.rowHeightHeader
          width: InputStyle.rowHeightHeader * 0.8

          Rectangle {
            id: avatarImage
            anchors.centerIn: parent
            antialiasing: true
            color: InputStyle.fontColor
            height: avatar.width
            radius: width * 0.5
            width: avatar.width

            MouseArea {
              anchors.fill: parent

              onClicked: {
                if (__merginApi.userAuth.hasAuthData() && __merginApi.apiVersionStatus === MerginApiStatus.OK) {
                  __merginApi.getUserInfo();
                  if (__merginApi.apiSupportsSubscriptions)
                    __merginApi.getSubscriptionInfo();
                  stackView.push(accountPanelComp);
                } else
                  stackView.push(authPanelComp, {
                      "state": "login"
                    });
              }
            }
            Image {
              id: userIcon
              anchors.centerIn: avatarImage
              fillMode: Image.PreserveAspectFit
              height: avatarImage.height * 0.8
              source: InputStyle.accountIcon
              sourceSize.height: height
              sourceSize.width: width
              width: height
            }
            ColorOverlay {
              anchors.fill: userIcon
              color: "#FFFFFF"
              source: userIcon
            }
          }
        }
      }
    }
  }
  Component {
    id: authPanelComp
    AuthPanel {
      id: authPanel
      height: root.height
      objectName: "authPanel"
      pending: stackView.pending
      toolbarHeight: InputStyle.rowHeightHeader
      visible: false
      width: root.width

      onBack: {
        stackView.popOnePageOrClose();
        if (!__merginApi.userAuth.hasAuthData()) {
          root.resetView();
        }
      }
    }
  }
  Component {
    id: statusPanelComp
    ProjectStatusPanel {
      id: statusPanel
      height: root.height
      visible: false
      width: root.width

      onBack: stackView.popOnePageOrClose()
    }
  }
  Component {
    id: accountPanelComp
    AccountPage {
      id: accountPanel
      height: root.height
      visible: true
      width: root.width

      onBack: {
        stackView.popOnePageOrClose();
      }
      onManagePlansClicked: manageSubscriptionPlans()
      onRestorePurchasesClicked: {
        __purchasing.restore();
      }
      onSignOutClicked: {
        if (__merginApi.userAuth.hasAuthData()) {
          __merginApi.clearAuth();
        }
        stackView.popOnePageOrClose();
        root.resetView();
      }
    }
  }
  Component {
    id: subscribePanelComp
    SubscribePage {
      id: subscribePanel
      height: root.height
      width: root.width

      onBackClicked: {
        stackView.popOnePageOrClose();
      }
      onSubscribeClicked: {
        stackView.popOnePageOrClose();
      }
    }
  }
  Component {
    id: projectWizardComp
    ProjectWizardPage {
      id: projectWizardPanel
      height: root.height
      objectName: "projectWizard"
      width: root.width

      onBack: {
        stackView.popOnePageOrClose();
      }
    }
  }
}
