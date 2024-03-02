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
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import QtQuick.Dialogs

import mm 1.0 as MM

import "../misc"
import "../components"
import "../onboarding"
import "../inputs"
import "../account"
import "../dialogs"

Item {
  id: root

  //! Used to determine if any project is loaded and to highlight it if so
  property string activeProjectId: ""

  signal openProjectRequested( string projectPath )
  signal refreshProjects()
  signal resetView() // resets view to state as when panel is opened
  signal closed()

  function openPanel() {
    root.visible = true
    stackView.visible = true
    getServiceInfo() // ensure attention banner status is refreshed
  }

  function hidePanel() {
    root.visible = false
    stackView.clearStackAndClose()
    root.closed()
  }

  function setupProjectOpen( projectPath ) {
    if ( projectPath === __activeProject.localProject.qgisProjectFilePath )
    {
      // just hide the panel - project already loaded
      hidePanel()
    }
    else
    {
      openProjectRequested( projectPath )
    }
  }

  function manageSubscriptionPlans() {
    Qt.openUrlExternally(__inputHelp.merginDashboardLink);
  }

  function getServiceInfo() {
    if (__merginApi.userAuth.hasAuthData() && __merginApi.apiVersionStatus === MM.MerginApiStatus.OK && __merginApi.apiSupportsSubscriptions) {
        __merginApi.getServiceInfo()
    }
  }

  function showLogin()
  {
    onboardingController.start()
  }

  function openChangesPanel( projectId )
  {
    stackView.push( statusPanelComp, {hasChanges: __merginProjectStatusModel.loadProjectInfo( projectId )} )
  }

  function showChanges( projectId ) {
    root.openChangesPanel( projectId )
  }

  visible: false
  focus: true

  onFocusChanged: { // pass focus to stackview
    stackView.focus = true
  }

  NoWorkspaceBanner {
    id: noWorkspaceBanner
    visible: {
      if ( !__merginApi.apiSupportsWorkspaces ) {
        return false;
      }
      if ( !__merginApi.userAuth.hasAuthData() ) {
        return false;
      }
      // do not show the banner in case of accepting invitation or creating a workspace
      if (onboardingController.inProgress) {
        return false;
      }
      return !__merginApi.userInfo.hasWorkspaces
    }
    z: parent.z + 1
    anchors {
      top: parent.top
      left: parent.left
      right: parent.right
    }

    onCreateWorkspaceRequested: {
      createWorkspaceController.createNewWorkspace()
    }
  }

  StackView {
    id: stackView

    initialItem: workspaceProjectsPanelComp

    anchors {
      top: noWorkspaceBanner.visible ? noWorkspaceBanner.bottom : parent.top
      left: parent.left
      right: parent.right
      bottom: parent.bottom
    }

    focus: true
    visible: false
    z: root.z + 1
    property bool pending: false

    function clearStackAndClose() {
      if ( stackView.depth > 1 )
        stackView.pop( null ) // pops everything besides an initialItem

      stackView.visible = false
    }

    function popOnePageOrClose() {
      if ( stackView.depth > 1 )
      {
        stackView.pop()
      }
    }

    function popPage( pageName, operation = StackView.PopTransition ) {
      for ( let i = 0; i < stackView.depth; i++ ) {
        let item = stackView.get( i )

        if ( item && item.objectName && item.objectName === pageName ) {
          stackView.pop( item, operation )
          stackView.pop()
        }
      }
    }

    function containsPage( pageName ) {
      for ( let i = 0; i < stackView.depth; i++ ) {
        let item = stackView.get( i );

        if ( item && item.objectName && item.objectName === pageName ) {
          return true;
        }
      }
      return false;
    }

    Keys.onReleased: function( event ) {
      if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
        event.accepted = true;

        if (stackView.depth > 1) {
          stackView.currentItem.back()
        }
        else if (root.activeProjectId) {
          root.hidePanel()
        }
      }
    }

    onVisibleChanged: {
      if ( stackView.visible )
        stackView.forceActiveFocus()
    }
  }

  BusyIndicator {
    id: busyIndicator
    width: parent.width/8
    height: width
    running: stackView.pending
    visible: running
    anchors.centerIn: parent
    z: parent.z + 1
  }

  Component {
    id: workspaceProjectsPanelComp

    Page {
      id: projectsPage

      function refreshProjectList( keepSearchFilter = false ) {
        stackView.pending = true
        switch( pageContent.state ) {
          case "local":
            localProjectsPage.refreshProjectsList( keepSearchFilter )
            break
          case "created":
            workspaceProjectsPage.refreshProjectsList( keepSearchFilter )
            break
          case "public":
            publicProjectsPage.refreshProjectsList( keepSearchFilter )
            break
        }
      }

      header: MMPageHeader {
        id: headerRow

        title: {
          if ( pageContent.state === "local" ) {
            return qsTr("Home")
          }
          else if ( pageContent.state === "created" ) {
            return qsTr("Projects")
          }
          return qsTr("Explore")
        }

        rightMarginShift: personIconRect.width + __style.pageMargins

        onBackClicked: root.hidePanel()

        Rectangle {
          visible: pageContent.state === "local"
          id: personIconRect

          anchors {
            right: parent.right
            rightMargin: __style.pageMargins
            verticalCenter: parent.verticalCenter
          }

          width: 40 * __dp
          height: width
          radius: width / 2
          color: __style.fieldColor

          MMIcon {
            id: genericAccountIcon
            visible: !accountNameAbbIcon.visible
            anchors.centerIn: parent
            source: __style.personalIcon
            size: __style.icon24
          }

          Text {
            id: accountNameAbbIcon
            visible: text
            text: __merginApi.userInfo.nameAbbr
            anchors.centerIn: parent
            color: __style.forestColor
            font: __style.t2
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
          }

          MouseArea {
            anchors.fill: parent
            onClicked: {
              if ( __merginApi.userAuth.hasAuthData() && __merginApi.apiVersionStatus === MerginApiStatus.OK ) {

                __merginApi.refreshUserData()

                if ( __merginApi.serverType === MerginServerType.OLD ) {
                  __inputUtils.showNotification( qsTr( "Unsupported server, please contact your server administrator." ) )
                }
                else {
                  stackView.push( workspaceAccountPageComp )
                }
              }
              else {
                root.showLogin()
              }
            }
          }
        }
      }

      background: Rectangle {
        anchors.fill: parent
        color: __style.lightGreenColor
      }

      Item {
        id: pageContent

        anchors.fill: parent

        states: [
          State {
            name: "local"
          },
          State {
            name: "created"
          },
          State {
            name: "public"
          }
        ]

        state: root.visible ? "local" : ""

        onStateChanged: {
          __merginApi.pingMergin()
          projectsPage.refreshProjectList()

          pageFooter.setActiveButton( pageContent.state )

          //
          // Show ws explanation tooltip if user is in workspace projects, on the ws server,
          // has more than one ws and has not seen it yet for too many times
          //
          if ( state === "created" ) {
            if (__merginApi.apiSupportsWorkspaces &&
                __merginApi.userInfo.hasMoreThanOneWorkspace &&
                !__appSettings.ignoreWsTooltip ) {

              __appSettings.wsTooltipShown()
              headerRow.openTooltip()
            }
          }
        }

        Connections {
          target: root
          function onVisibleChanged() {
            if ( root.visible ) { // projectsPanel opened
              pageContent.state = "local"
            }
            else {
              pageContent.state = ""
            }
          }

          function onResetView() {
            if ( pageContent.state === "created" )
              pageContent.state = "local"
          }

          function onRefreshProjects() {
            projectsPage.refreshProjectList()
          }
        }

        StackLayout {
          id: projectListLayout

          width: parent.width - 2*__style.pageMargins
          height: parent.height
          anchors.horizontalCenter: parent.horizontalCenter

          currentIndex: pageFooter.index

          MMProjectHomePage {
            id: localProjectsPage

            activeProjectId: root.activeProjectId
            list.visible: !stackView.pending

            onOpenProjectRequested: function( projectFilePath ) {
              setupProjectOpen( projectFilePath )
            }
            onShowLocalChangesRequested: function( projectId ) {
              showChanges( projectId )
            }
            list.onActiveProjectDeleted: setupProjectOpen( "" )
          }

          MMProjectListPage {
            id: workspaceProjectsPage

            projectModelType: MM.ProjectsModel.WorkspaceProjectsModel
            activeProjectId: root.activeProjectId
            list.visible: !stackView.pending && __merginApi.serverType !== MM.MerginServerType.OLD

            onOpenProjectRequested: function( projectFilePath ) {
              setupProjectOpen( projectFilePath )
            }
            onShowLocalChangesRequested: function( projectId ) {
              showChanges( projectId )
            }
            list.onActiveProjectDeleted: setupProjectOpen( "" )
          }

          MMProjectListPage {
            id: publicProjectsPage

            projectModelType: MM.ProjectsModel.PublicProjectsModel
            activeProjectId: root.activeProjectId
            list.visible: !stackView.pending && __merginApi.serverType !== MM.MerginServerType.OLD

            onOpenProjectRequested: function( projectFilePath ) {
              setupProjectOpen( projectFilePath )
            }
            onShowLocalChangesRequested: function( projectId ) {
              showChanges( projectId )
            }
            list.onActiveProjectDeleted: function() {
              setupProjectOpen( "" )
            }
          }
        }
      }

      footer: MMSelectableToolbar {
        id: pageFooter

        width: projectsPage.width
        property int buttonWidth: Math.floor((projectsPage.width - 2 * __style.pageMargins) / 3)

        Component.onCompleted: setActiveButton( pageContent.state )

        function setActiveButton( state ) {
          switch( state ) {
            case "local": pageFooter.index = 0; break
            case "created": pageFooter.index = 1; break
            case "public": pageFooter.index = 2; break
          }
        }

        model: ObjectModel {

          MMSelectableToolbarButton {
            id: localProjectsBtn
            width: pageFooter.buttonWidth
            text: qsTr("Home")
            iconSource: __style.homeIcon
            selectedIconSource: __style.homeFilledIcon
            checked: pageFooter.index === 0
            onClicked: pageContent.state = "local"
          }

          MMSelectableToolbarButton {
            id: createdProjectsBtn
            width: pageFooter.buttonWidth
            text: qsTr("Projects")
            iconSource: __style.projectsIcon
            selectedIconSource: __style.projectsFilledIcon
            checked: pageFooter.index === 1
            onClicked: pageContent.state = "created"
          }

          MMSelectableToolbarButton {
            id: publicProjectsBtn
            width: pageFooter.buttonWidth
            text: qsTr("Explore")
            iconSource: __style.globalIcon
            selectedIconSource: __style.globalFilledIcon
            checked: pageFooter.index === 2
            onClicked: pageContent.state = "public"
          }
        }
      }

      // Other components

      Connections {
        target: __projectWizard
        function onProjectCreationFailed(message) {
          __inputUtils.showNotification(message)
          stackView.pending = false
        }
        function onProjectCreated( projectDir, projectName ) {
          if  (stackView.currentItem.objectName === "projectWizard") {
            __inputUtils.log(
                  "Create project",
                  "Local project " + projectName + " created at path: " + projectDir + " by "
                  + ( __merginApi.userAuth ? __merginApi.userAuth.username : "unknown" ) )
            stackView.popOnePageOrClose()
          }
        }
      }
    }
  }

  MMOnboardingController {
    id: onboardingController
    enabled: root.visible
    stackView: stackView
  }

  MMCreateWorkspaceController {
    // TODO move to main.qml?
    id: createWorkspaceController
    enabled: root.visible
    stackView: stackView
  }

  MMAcceptInvitationController {
    // TODO move to main.qml?
    id: acceptInvitationController
    // TODO enabled add controller.showInvitationsList
    enabled: root.visible && __merginApi.apiSupportsWorkspaces
    stackView: stackView
  }

  Component {
    id: statusPanelComp
    MMProjectStatusPanel {
      id: statusPanel
      height: root.height
      width: root.width
      onBack: stackView.popOnePageOrClose()
    }
  }

  Component {
    id: workspaceAccountPageComp

    MMAcountPage {
      id: workspaceAccountPage

      abbrName: __merginApi.userInfo.nameAbbr
      fullName: __merginApi.userInfo.name
      userName: __merginApi.userAuth.username
      email: __merginApi.userInfo.email
      workspaceName: __merginApi.userInfo.activeWorkspaceName
      invitationsCount: __merginApi.userInfo.invitationsCount

      workspaceRole: __merginApi.workspaceInfo.role

      subscription: __merginApi.subscriptionInfo.planAlias
      storage: "%1/%2".arg(__inputUtils.bytesToHumanSize(__merginApi.workspaceInfo.diskUsage)).arg(__inputUtils.bytesToHumanSize(__merginApi.workspaceInfo.storageLimit))
      storageFill: {
        if (__merginApi.workspaceInfo.storageLimit > 0 )
          return __merginApi.workspaceInfo.diskUsage / __merginApi.workspaceInfo.storageLimit
        else
          return 0.0
      }

      onBackClicked: stackView.popOnePageOrClose()

      onManageAccountClicked: manageSubscriptionPlans()

      onSignOutClicked: {
        __merginApi.signOut()
        stackView.pop( null )
        root.resetView()
      }

      onCloseAccountClicked: {
        stackView.popOnePageOrClose()
        closeAccountDialog.open()
        root.resetView()
      }

      onSelectWorkspaceClicked: {
        stackView.push( workspaceListComponent )
      }
    }
  }

  MMCloseAccountDialog {
    id: closeAccountDialog
    username: __merginApi.userAuth.username

    onCloseAccountClicked: {
      __merginApi.deleteAccount()
    }
  }

  Component {
    id: projectWizardComp

    MMProjectWizardPage {
      id: projectWizardPanel
      objectName: "projectWizard"
      height: root.height
      width: root.width
      onBackClicked: {
        stackView.popOnePageOrClose()
      }
    }
  }

  Component {
    id: workspaceListComponent

    MMSwitchWorkspacePage {
      id: switchWorkspacePanel

      height: root.height
      width: root.width

      onBack: {
        stackView.popOnePageOrClose()
      }

      onCreateWorkspaceRequested: {
        createWorkspaceController.createNewWorkspace()
      }
    }
  }

  Connections {
    target: __merginApi
    enabled: root.visible

    function onListProjectsFinished( merginProjects, projectCount, page, requestId ) {
      stackView.pending = false
    }

    function onListProjectsByNameFinished( merginProjects, requestId ) {
      stackView.pending = false
    }

    function onApiVersionStatusChanged() {
      stackView.pending = false

      if (__merginApi.apiVersionStatus === MerginApiStatus.OK) {
        if (__merginApi.userAuth.hasAuthData()) {
          root.refreshProjects()
        }
      }
    }

    function onAuthRequested() {
      stackView.pending = false

      root.showLogin()
    }

    function onAuthChanged() {
      stackView.pending = false

      if ( __merginApi.userAuth.hasAuthData() ) {

        if ( __merginApi.serverType === MerginServerType.OLD || ( stackView.currentItem.objectName === "loginPage" ) ) {
          stackView.popPage( "loginPage" )
        }

        root.refreshProjects()
        root.forceActiveFocus()
      }
      else {
        // logged out
      }
    }

    function onAuthFailed() {
      stackView.pending = false
    }

    function onActiveWorkspaceChanged() {
      root.refreshProjects()
    }
  }
}
