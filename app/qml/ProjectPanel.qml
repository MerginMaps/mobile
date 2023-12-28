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

import lc 1.0
import "."  // import InputStyle singleton
import "./misc"
import "./components/"
import "./onboarding/"

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
    if (__merginApi.userAuth.hasAuthData() && __merginApi.apiVersionStatus === MerginApiStatus.OK && __merginApi.apiSupportsSubscriptions) {
        __merginApi.getServiceInfo()
    }
  }

  function showLogin()
  {
    onboardingController.start()
  }

  function openChangesPanel()
  {
    stackView.push( statusPanelComp )
  }

  function showChanges( projectId ) {
    if ( __merginProjectStatusModel.loadProjectInfo( projectId ) )
    {
      root.openChangesPanel()
    }
    else __inputUtils.showNotification( qsTr( "No Changes" ) )
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

      header: PanelHeaderV2 {
        id: headerRow

        width: projectsPage.width

        headerTitle: {
          if ( pageContent.state === "local" ) {
            return qsTr("Downloaded projects")
          }
          else if ( pageContent.state === "created" ) {
            return __merginApi.userInfo.hasWorkspaces ? __merginApi.userInfo.activeWorkspaceName : qsTr("Projects")
          }
          return qsTr("Public projects")
        }

        tooltipText: qsTr("Your other projects are accessible%1by switching your workspace here").arg("\n")

        haveBackButton: root.activeProjectId
        haveAccountButton: true

        onBackClicked: root.hidePanel()
        onAccountClicked: {
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

      background: Rectangle {
        anchors.fill: parent
        color: InputStyle.clrPanelMain
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

          anchors {
              left: parent.left
              right: parent.right
              top: parent.top
              bottom: parent.bottom
          }
          currentIndex: pageFooter.currentIndex

          ProjectListPage {
            id: localProjectsPage

            projectModelType: ProjectsModel.LocalProjectsModel
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

          ProjectListPage {
            id: workspaceProjectsPage

            projectModelType: ProjectsModel.WorkspaceProjectsModel
            activeProjectId: root.activeProjectId
            list.visible: !stackView.pending && __merginApi.serverType !== MerginServerType.OLD

            onOpenProjectRequested: function( projectFilePath ) {
              setupProjectOpen( projectFilePath )
            }
            onShowLocalChangesRequested: function( projectId ) {
              showChanges( projectId )
            }
            list.onActiveProjectDeleted: setupProjectOpen( "" )
          }

          ProjectListPage {
            id: publicProjectsPage

            projectModelType: ProjectsModel.PublicProjectsModel
            activeProjectId: root.activeProjectId
            list.visible: !stackView.pending && __merginApi.serverType !== MerginServerType.OLD

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

      footer: TabBar {
        id: pageFooter

        property int itemSize: pageFooter.height * 0.8

        function setActiveButton( state ) {
          switch( state ) {
            case "local": pageFooter.setCurrentIndex( 0 ); break
            case "created": pageFooter.setCurrentIndex( 1 ); break
            case "public": pageFooter.setCurrentIndex( 2 ); break
          }
        }

        spacing: 0
        contentHeight: InputStyle.rowHeightHeader

        TabButton {
          id: localProjectsBtn

          background: Rectangle {
            anchors.fill: parent
            color: InputStyle.fontColor
          }

          MainPanelButton {
            id: localProjectsInnerBtn

            text: qsTr("Home")
            imageSource: InputStyle.homeIcon
            width: pageFooter.itemSize

            handleClicks: false
            faded: pageFooter.currentIndex !== localProjectsBtn.TabBar.index
          }

          onClicked: pageContent.state = "local"
        }

        TabButton {
          id: createdProjectsBtn

          background: Rectangle {
            anchors.fill: parent
            color: InputStyle.fontColor
          }

          MainPanelButton {
            id: createdProjectsInnerBtn

            text: qsTr("Projects")
            imageSource: InputStyle.mapSearchIcon
            width: pageFooter.itemSize

            handleClicks: false
            faded: pageFooter.currentIndex !== createdProjectsBtn.TabBar.index
          }

          onClicked: pageContent.state = "created"
        }

        TabButton {
          id: publicProjectsBtn

          background: Rectangle {
            anchors.fill: parent
            color: InputStyle.fontColor
          }

          MainPanelButton {
            id: publicProjectsInnerBtn

            text: qsTr("Explore")
            imageSource: InputStyle.exploreIcon
            width: pageFooter.itemSize

            handleClicks: false
            faded: pageFooter.currentIndex !== publicProjectsBtn.TabBar.index
          }

          onClicked: pageContent.state = "public"
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
    ProjectStatusPanel {
      id: statusPanel
      height: root.height
      width: root.width
      visible: false
      onBack: stackView.popOnePageOrClose()
    }
  }

  Component {
    id: workspaceAccountPageComp

    WorkspaceAccountPage {
      id: workspaceAccountPage

      onBack: stackView.popOnePageOrClose()

      onManagePlansClicked: manageSubscriptionPlans()

      onSignOutClicked: {
        __merginApi.signOut()
        stackView.pop( null )
        root.resetView()
      }

      onAccountDeleted: {
        stackView.popOnePageOrClose()
        root.resetView()
      }

      onSwitchWorkspace: {
        stackView.push( workspaceListComponent )
      }
    }
  }

  Component {
    id: projectWizardComp

    ProjectWizardPage {
      id: projectWizardPanel
      objectName: "projectWizard"
      height: root.height
      width: root.width
      onBack: {
        stackView.popOnePageOrClose()
      }
    }
  }

  Component {
    id: workspaceListComponent

    SwitchWorkspacePage {
      id: switchWorkspacePanel

      height: root.height
      width: root.width

      onBack: {
        stackView.popOnePageOrClose()
      }

      onCreateWorkspaceRequested: {
        onboardingController.createNewWorkspace()
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
        // log out - reenable openInvitationsListener
        acceptInvitationController.showInvitationList = true
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
