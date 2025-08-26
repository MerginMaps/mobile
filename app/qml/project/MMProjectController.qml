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

import "../components"
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
  }

  function hidePanel() {
    if ( root.activeProjectId ) {
      root.visible = false
      stackView.clearStackAndClose()
      root.closed()
    }
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

  function showLogin()
  {
    accountController.start()
  }

  function openChangesPanel( projectId, closeOnBack )
  {
    stackView.push( statusPageComp, {
                     hasChanges: __merginProjectStatusModel.loadProjectInfo( projectId ),
                     closeOnBack: closeOnBack
                   }
    )
  }

  function showSelectWorkspacePage() {
    stackView.push( workspaceListComponent )
  }

  visible: false
  focus: true

  onFocusChanged: { // pass focus to stackview
    stackView.focus = true
  }

  StackView {
    id: stackView

    initialItem: workspaceProjectsPanelComp

    anchors {
      top: parent.top
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

  MMBusyIndicator {
    id: busyIndicator
    running: stackView.pending
    anchors.centerIn: parent
    z: parent.z + 1
  }

  Component {
    id: workspaceProjectsPanelComp

    MMPage {
      id: projectsPage

      state: root.visible ? "home" : ""

      states: [
        State { name: "home" },
        State { name: "workspace" },
        State { name: "explore" }
      ]

      pageBottomMargin: 0
      pageHeader.backVisible: root.activeProjectId

      onBackClicked: root.hidePanel()

      onStateChanged: {
        __merginApi.pingMergin()
        projectsPage.refreshProjectList()

        pageFooter.setActiveButton( projectsPage.state )

        // NOTE: we do not have any tooltip ATM - TODO: what to do?
        //
        // Show ws explanation tooltip if user is in workspace projects, on the ws server,
        // has more than one ws and has not seen it yet for too many times
        //
      }

      pageHeader.title: {
        if ( projectsPage.state === "home" ) {
          return qsTr("Home")
        }
        else if ( projectsPage.state === "workspace" ) {
          return __merginApi.userInfo.activeWorkspaceName ? __merginApi.userInfo.activeWorkspaceName : qsTr( "Projects" )
        }
        return qsTr("Explore")
      }

      pageHeader.rightItemContent: Item {

        width: 40 * __dp
        height: width

        anchors.verticalCenter: parent.verticalCenter

        Rectangle {
          width: parent.width
          height: parent.height
          radius: width / 2

          color: __style.fieldColor

          MMIcon {
            anchors.centerIn: parent

            visible: !accountNameAbbIcon.visible

            size: __style.icon24
            source: __style.personalIcon
          }
        }

        MMAvatarLettersItem {
          id: accountNameAbbIcon

          width: parent.width
          height: parent.height

          text.font: __style.t4
          hasNotification: __merginApi.userInfo.hasInvitations

          abbrv: __merginApi.userInfo.nameAbbr
          visible: abbrv
        }

        MouseArea {
          anchors {
            fill: parent
            margins: -__style.margin12
          }

          onClicked: {
            if ( __merginApi.userAuth.hasAuthData() && __merginApi.apiVersionStatus === MM.MerginApiStatus.OK ) {

              __merginApi.refreshUserData()

              if ( __merginApi.serverType === MM.MerginServerType.OLD ) {
                __notificationModel.addWarning( qsTr( "Unsupported server, please contact your server administrator." ) )
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

      pageContent: Item {
        id: pageContent

        width: parent.width
        height: parent.height

        StackLayout {
          id: projectListLayout

          width: parent.width
          height: parent.height

          currentIndex: pageFooter.index

          MMProjectHomeTab {
            id: homeTab

            activeProjectId: root.activeProjectId
            list.visible: !stackView.pending

            onOpenProjectRequested: function( projectFilePath ) {
              setupProjectOpen( projectFilePath )
            }
            onShowLocalChangesRequested: function( projectId ) {
              root.openChangesPanel( projectId, false )
            }
            list.onActiveProjectDeleted: setupProjectOpen( "" )

            noWorkspaceBannerVisible: {
              if ( !__merginApi.apiSupportsWorkspaces ) {
                return false;
              }
              if ( !__merginApi.userAuth.hasValidToken() ) {
                return false;
              }
              // do not show the banner in case of accepting invitation or creating a workspace
              if (accountController.inProgress) {
                return false;
              }
              return !__merginApi.userInfo.hasWorkspaces
            }

            onCreateWorkspaceRequested: {
              createWorkspaceController.createNewWorkspace()
            }
          }

          MMProjectServerTab {
            id: workspaceProjectsTab

            projectModelType: MM.ProjectsModel.WorkspaceProjectsModel
            activeProjectId: root.activeProjectId
            list.visible: !stackView.pending && __merginApi.serverType !== MM.MerginServerType.OLD

            onOpenProjectRequested: function( projectFilePath ) {
              setupProjectOpen( projectFilePath )
            }
            onShowLocalChangesRequested: function( projectId ) {
              root.openChangesPanel( projectId, false )
            }
            list.onActiveProjectDeleted: setupProjectOpen( "" )
          }

          MMProjectServerTab {
            id: publicProjectsTab

            projectModelType: MM.ProjectsModel.PublicProjectsModel
            activeProjectId: root.activeProjectId
            list.visible: !stackView.pending && __merginApi.serverType !== MM.MerginServerType.OLD

            onOpenProjectRequested: function( projectFilePath ) {
              setupProjectOpen( projectFilePath )
            }
            onShowLocalChangesRequested: function( projectId ) {
              root.openChangesPanel( projectId, false )
            }
            list.onActiveProjectDeleted: function() {
              setupProjectOpen( "" )
            }
          }
        }
      }

      footer: MMToolbar {
        id: pageFooter

        width: projectsPage.width
        property int buttonWidth: Math.floor((projectsPage.width - 2 * __style.pageMargins) / 3)

        Component.onCompleted: setActiveButton( projectsPage.state )

        function setActiveButton( state ) {
          switch( state ) {
            case "home": pageFooter.index = 0; break
            case "workspace": pageFooter.index = 1; break
            case "explore": pageFooter.index = 2; break
          }
        }

        model: ObjectModel {

          MMToolbarButton {
            id: localProjectsBtn
            text: qsTr("Home")
            iconSource: __style.homeIcon
            iconSourceSelected: __style.homeFilledIcon
            onClicked: projectsPage.state = "home"
          }

          MMToolbarButton {
            id: createdProjectsBtn

            text: qsTr("Projects")
            iconSource: __style.projectsIcon
            iconSourceSelected: __style.projectsFilledIcon
            onClicked: projectsPage.state = "workspace"
          }

          MMToolbarButton {
            id: publicProjectsBtn

            text: qsTr("Explore")
            iconSource: __style.globalIcon
            iconSourceSelected: __style.globalFilledIcon
            onClicked: projectsPage.state = "explore"
          }
        }
      }

      function refreshProjectList( keepSearchFilter = false ) {
        stackView.pending = true
        switch( projectsPage.state ) {
          case "home":
            homeTab.refreshProjectsList( keepSearchFilter )
            break
          case "workspace":
            workspaceProjectsTab.refreshProjectsList( keepSearchFilter )
            break
          case "explore":
            publicProjectsTab.refreshProjectsList( keepSearchFilter )
            break
        }
      }

      Connections {
        target: root

        function onVisibleChanged() {
          if ( root.visible ) { // projectsPanel opened
            projectsPage.state = "home"
          }
          else {
            projectsPage.state = ""
          }
        }

        function onResetView() {
          if ( projectsPage.state === "workspace" )
            projectsPage.state = "home"
        }

        function onRefreshProjects() {
          projectsPage.refreshProjectList()
        }
      }

      Connections {
        target: __projectWizard

        function onProjectCreationFailed( message ) {
          __notificationModel.addError( message )
          stackView.pending = false
        }

        function onProjectCreated( projectDir, projectName ) {
          if  (stackView.currentItem.objectName === "projectWizard") {
            __inputUtils.log(
                  "Create project",
                  "Local project " + projectName + " created at path: " + projectDir + " by "
                  + ( __merginApi.userInfo ? __merginApi.userInfo.username : "unknown" ) )
            stackView.popOnePageOrClose()
          }
        }
      }
    }
  }

  MMAccountController {
    id: accountController
    enabled: root.visible
    stackView: stackView
  }

  MMCreateWorkspaceController {
    id: createWorkspaceController
    enabled: root.visible
    stackView: stackView
  }

  MMAcceptInvitationController {
    id: acceptInvitationController
    // Do not show accept invitation notification when
    // - there is onboarding ongoing
    // - account page is already opened
    enabled: !accountController.inProgress &&
             !stackView.containsPage("accountPage") &&
             __merginApi.apiSupportsWorkspaces
  }

  Component {
    id: statusPageComp
    MMProjectStatusPage {
      id: statusPage

      // Close project controller when back is clicked
      // e.g. when project changes are requested from
      // map view
      property bool closeOnBack: false

      height: root.height
      width: root.width
      onBack: {
        if (closeOnBack) {
          root.hidePanel()
        } else {
          stackView.popOnePageOrClose()
        }
      }
    }
  }

  Component {
    id: workspaceAccountPageComp

    MMAccountPage {
      id: workspaceAccountPage

      objectName: "accountPage"

      abbrName: __merginApi.userInfo.nameAbbr
      fullName: __merginApi.userInfo.name
      userName: __merginApi.userInfo.username
      email: __merginApi.userInfo.email
      workspaceName: __merginApi.userInfo.activeWorkspaceName
      invitationsCount: __merginApi.userInfo.invitationsCount

      workspaceRole: __merginApi.workspaceInfo.role
      subscription: __merginApi.workspaceInfo.serviceState

      storage: "%1/%2".arg(__inputUtils.bytesToHumanSize(__merginApi.workspaceInfo.diskUsage)).arg(__inputUtils.bytesToHumanSize(__merginApi.workspaceInfo.storageLimit))
      storageFill: {
        if (__merginApi.workspaceInfo.storageLimit > 0 )
          return Number( ( __merginApi.workspaceInfo.diskUsage / __merginApi.workspaceInfo.storageLimit ).toFixed( 2 ) );
        else
          return 0.0
      }

      onBackClicked: stackView.popOnePageOrClose()
      onManageAccountClicked: Qt.openUrlExternally(__inputHelp.merginSubscriptionLink)

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

      onSelectWorkspaceClicked: root.showSelectWorkspacePage()
    }
  }

  MMCloseAccountDialog {
    id: closeAccountDialog
    username: __merginApi.userInfo.username

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

      activeWorkspaceId: __merginApi.userInfo.activeWorkspaceId

      invitationsModel: MM.InvitationsProxyModel {
        invitationsSourceModel: MM.InvitationsModel {
          merginApi: __merginApi
        }
      }

      workspacesModel: MM.WorkspacesProxyModel {
        id: wsProxyModel

        workspacesSourceModel: MM.WorkspacesModel {
          merginApi: __merginApi
        }
      }

      onWorkspaceClicked: ( workspaceId ) => { __merginApi.userInfo.setActiveWorkspace( workspaceId ); stackView.popOnePageOrClose() }

      onInvitationClicked: ( uuid, accepted ) => { __merginApi.processInvitation( uuid, accepted ) }

      onSearchTextChanged: ( searchText ) => { wsProxyModel.searchExpression = searchText }

      onBackClicked: {
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

    function onListProjectsFailed() {
      stackView.pending = false
    }

    function onApiVersionStatusChanged() {
      stackView.pending = false

      if (__merginApi.apiVersionStatus === MM.MerginApiStatus.OK) {
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

      if ( __merginApi.userAuth.hasValidToken() ) {

        if ( __merginApi.serverType === MM.MerginServerType.OLD || ( stackView.currentItem.objectName === "loginPage" ) ) {
          stackView.popPage( "loginPage" )
          accountController.end()
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
      if ( __merginApi.userAuth.hasValidToken() ) {
        root.refreshProjects()
      }
    }
  }
}
