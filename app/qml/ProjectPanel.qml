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
import "./components/"

Item {
  id: root

  property string activeProjectId: ""
  property string activeProjectPath: ""

  signal openProjectRequested( string projectId, string projectPath )
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

  function manageSubscriptionPlans() {
    if (__purchasing.hasInAppPurchases && (__purchasing.hasManageSubscriptionCapability || !__merginApi.subscriptionInfo.ownsActiveSubscription )) {
      stackView.push( subscribePanelComp)
    } else {
      Qt.openUrlExternally(__purchasing.subscriptionManageUrl);
    }
  }

  function getServiceInfo() {
    if (__merginApi.userAuth.hasAuthData() && __merginApi.apiVersionStatus === MerginApiStatus.OK && __merginApi.apiSupportsSubscriptions) {
        __merginApi.getSubscriptionInfo()
    }
  }

  function openAuthPanel( authstate = "login" )
  {
    stackView.push( authPanelComp, { state: authstate } )
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

  StackView {
    id: stackView

    initialItem: __merginApi.serverType === MerginServerType.OLD ? projectsPanelComp : workspaceProjectsPanelComp
    anchors.fill: parent
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

    function clearStackOnServerChange() {
        stackView.clear()
        if ( __merginApi.serverType === MerginServerType.OLD ) {
            stackView.push( projectsPanelComp )
        } else {
            stackView.push( workspaceProjectsPanelComp )
        }
    }

    Keys.onReleased: function( event ) {
      if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
        event.accepted = true;

        if (stackView.depth > 1) {
          stackView.currentItem.back()
        }
        else if (root.activeProjectPath) {
          root.hidePanel()
        }
      }
    }

    onVisibleChanged: {
      if ( stackView.visible )
        stackView.forceActiveFocus()
    }

    Connections {
      target: __merginApi

      function onServerTypeChanged( serverType ) {
          stackView.clearStackOnServerChange()
      }
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
    id: projectsPanelComp

    Page {
      id: projectsPage

      function setupProjectOpen( projectId, projectPath ) {
        activeProjectId = projectId
        activeProjectPath = projectPath
        openProjectRequested( projectId, projectPath )

        if ( projectId && projectPath ) // this is not project reset
          hidePanel()
      }

      function refreshProjectList( keepSearchFilter = false ) {
        stackView.pending = true
        switch( pageContent.state ) {
          case "local":
            localProjectsPage.refreshProjectsList( keepSearchFilter )
            break
          case "created":
            createdProjectsPage.refreshProjectsList( keepSearchFilter )
            break
          case "shared":
            sharedProjectsPage.refreshProjectsList( keepSearchFilter )
            break
          case "public":
            publicProjectsPage.refreshProjectsList( keepSearchFilter )
            break
        }
      }

      header: PanelHeader {
        id: pageHeader

        titleText: qsTr("Projects")
        color: InputStyle.clrPanelMain
        height: InputStyle.rowHeightHeader
        rowHeight: InputStyle.rowHeightHeader

        onBack: {
          if ( root.activeProjectId ) {
            root.hidePanel()
          }
        }
        withBackButton: root.activeProjectPath

        Item {
          id: avatar

          width: InputStyle.rowHeightHeader * 0.8
          height: InputStyle.rowHeightHeader
          anchors.right: parent.right
          anchors.rightMargin: InputStyle.panelMargin

          Rectangle {
            id: avatarImage

            anchors.centerIn: parent
            width: avatar.width
            height: avatar.width
            color: InputStyle.fontColor
            radius: width*0.5
            antialiasing: true

            MouseArea {
              anchors.fill: parent
              onClicked: {
                if (__merginApi.userAuth.hasAuthData() && __merginApi.apiVersionStatus === MerginApiStatus.OK) {
                  __merginApi.getUserInfo()
                  if (__merginApi.apiSupportsSubscriptions)
                    __merginApi.getSubscriptionInfo()
                  stackView.push( accountPanelComp )
                }
                else
                  root.openAuthPanel()
              }
            }

            Image {
              id: userIcon

              anchors.centerIn: avatarImage
              source: InputStyle.accountIcon
              height: avatarImage.height * 0.8
              width: height
              sourceSize.width: width
              sourceSize.height: height
              fillMode: Image.PreserveAspectFit
            }

            ColorOverlay {
              anchors.fill: userIcon
              source: userIcon
              color: "#FFFFFF"
            }
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
            name: "shared"
          },
          State {
            name: "public"
          }
        ]

        onStateChanged: {
          __merginApi.pingMergin()
          refreshProjectList()
          pageFooter.setActiveButton( pageContent.state )
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
            if ( pageContent.state === "created" || pageContent.state === "shared" )
              pageContent.state = "local"
          }
        }

        StackLayout {
          id: projectListLayout

          anchors.fill: parent
          currentIndex: pageFooter.currentIndex

          ProjectListPage {
            id: localProjectsPage

            projectModelType: ProjectsModel.LocalProjectsModel
            activeProjectId: root.activeProjectId
            list.visible: !stackView.pending

            onOpenProjectRequested: function( projectId, projectFilePath ) {
              setupProjectOpen( projectId, projectFilePath )
            }
            onShowLocalChangesRequested: function( projectId ) {
              showChanges( projectId )
            }
            list.onActiveProjectDeleted: setupProjectOpen( "", "" )
          }

          ProjectListPage {
            id: createdProjectsPage

            projectModelType: ProjectsModel.CreatedProjectsModel
            activeProjectId: root.activeProjectId
            list.visible: !stackView.pending

            onOpenProjectRequested: function( projectId, projectFilePath ) {
              setupProjectOpen( projectId, projectFilePath )
            }
            onShowLocalChangesRequested: function( projectId ) {
              showChanges( projectId )
            }
            list.onActiveProjectDeleted: setupProjectOpen( "", "" )
          }

          ProjectListPage {
            id: sharedProjectsPage

            projectModelType: ProjectsModel.SharedProjectsModel
            activeProjectId: root.activeProjectId
            list.visible: !stackView.pending

            onOpenProjectRequested: function( projectId, projectFilePath ) {
              setupProjectOpen( projectId, projectFilePath )
            }
            onShowLocalChangesRequested: function( projectId ) {
              showChanges( projectId )
            }
            list.onActiveProjectDeleted: setupProjectOpen( "", "" )
          }

          ProjectListPage {
            id: publicProjectsPage

            projectModelType: ProjectsModel.PublicProjectsModel
            activeProjectId: root.activeProjectId
            list.visible: !stackView.pending

            onOpenProjectRequested: function( projectId, projectFilePath ) {
              setupProjectOpen( projectId, projectFilePath )
            }
            onShowLocalChangesRequested: function( projectId ) {
              showChanges( projectId )
            }
            list.onActiveProjectDeleted: function() {
              setupProjectOpen( "", "" )
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
            case "shared": pageFooter.setCurrentIndex( 2 ); break
            case "public": pageFooter.setCurrentIndex( 3 ); break
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

            text: qsTr("My projects")
            imageSource: InputStyle.accountIcon
            width: pageFooter.itemSize

            handleClicks: false
            faded: pageFooter.currentIndex !== createdProjectsBtn.TabBar.index
          }

          onClicked: pageContent.state = "created"
        }

        TabButton {
          id: sharedProjectsBtn

          background: Rectangle {
            anchors.fill: parent
            color: InputStyle.fontColor
          }

          MainPanelButton {
            id: sharedProjectsInnerBtn

            imageSource: InputStyle.accountMultiIcon
            width: pageFooter.itemSize
            text: parent.width > sharedProjectsInnerBtn.width * 2 ? qsTr("Shared with me") : qsTr("Shared")

            handleClicks: false
            faded: pageFooter.currentIndex !== sharedProjectsBtn.TabBar.index
          }

          onClicked: pageContent.state = "shared"
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
          if (__merginApi.apiVersionStatus === MerginApiStatus.OK && stackView.currentItem.objectName === "authPanel") {
            if (__merginApi.userAuth.hasAuthData()) {
              refreshProjectList()
            } else if (pageContent.state !== 'local') {
              if (stackView.currentItem.objectName !== "authPanel") {
                root.openAuthPanel()
              }
            }
          }
        }
        function onAuthRequested() {
          stackView.pending = false
          root.openAuthPanel()
        }
        function onAuthChanged() {
          stackView.pending = false
          if ( __merginApi.userAuth.hasAuthData() ) {
            stackView.popOnePageOrClose()
            projectsPage.refreshProjectList()
            root.forceActiveFocus()
          }
        }
        function onAuthFailed() {
          stackView.pending = false
        }
        function onRegistrationFailed() {
          stackView.pending = false
        }
        function onRegistrationSucceeded() {
          stackView.pending = false
        }
      }
    }
  }

  Component {
    id: workspaceProjectsPanelComp

    Page {
      id: projectsPage

      function setupProjectOpen( projectId, projectPath ) {
        activeProjectId = projectId
        activeProjectPath = projectPath
        openProjectRequested( projectId, projectPath )

        if ( projectId && projectPath ) // this is not project reset
          hidePanel()
      }

      function refreshProjectList( keepSearchFilter = false ) {
        stackView.pending = true
        switch( pageContent.state ) {
          case "local":
            localProjectsPage.refreshProjectsList( keepSearchFilter )
            break
          case "created":
            createdProjectsPage.refreshProjectsList( keepSearchFilter )
            break
          case "public":
            publicProjectsPage.refreshProjectsList( keepSearchFilter )
            break
        }
      }

      header: PanelHeader {
        id: pageHeader

        titleText: qsTr("Projects")
        color: InputStyle.clrPanelMain
        height: InputStyle.rowHeightHeader
        rowHeight: InputStyle.rowHeightHeader

        onBack: {
          if ( root.activeProjectId ) {
            root.hidePanel()
          }
        }
        withBackButton: root.activeProjectPath

        Item {
          id: avatar

          width: InputStyle.rowHeightHeader * 0.8
          height: InputStyle.rowHeightHeader
          anchors.right: parent.right
          anchors.rightMargin: InputStyle.panelMargin

          Rectangle {
            id: avatarImage

            anchors.centerIn: parent
            width: avatar.width
            height: avatar.width
            color: InputStyle.fontColor
            radius: width*0.5
            antialiasing: true

            MouseArea {
              anchors.fill: parent
              onClicked: {
                if (__merginApi.userAuth.hasAuthData() && __merginApi.apiVersionStatus === MerginApiStatus.OK) {
                  __merginApi.getUserInfo()
                  if (__merginApi.apiSupportsSubscriptions)
                    __merginApi.getSubscriptionInfo()
                  stackView.push( accountPanelComp )
                }
                else
                  root.openAuthPanel()
              }
            }

            Image {
              id: userIcon

              anchors.centerIn: avatarImage
              source: InputStyle.accountIcon
              height: avatarImage.height * 0.8
              width: height
              sourceSize.width: width
              sourceSize.height: height
              fillMode: Image.PreserveAspectFit
            }

            ColorOverlay {
              anchors.fill: userIcon
              source: userIcon
              color: "#FFFFFF"
            }
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

        onStateChanged: {
          __merginApi.pingMergin()
          refreshProjectList()
          pageFooter.setActiveButton( pageContent.state )
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
        }

        ComboBox {
          id: workspaceComboBox

          model: __merginApi.userInfo.workspaces
          onActivated: function(index) {
            console.log("Switch to", index)
          }
        }

        StackLayout {
          id: projectListLayout

          anchors.fill: parent
          currentIndex: pageFooter.currentIndex

          ProjectListPage {
            id: localProjectsPage

            projectModelType: ProjectsModel.LocalProjectsModel
            activeProjectId: root.activeProjectId
            list.visible: !stackView.pending

            onOpenProjectRequested: function( projectId, projectFilePath ) {
              setupProjectOpen( projectId, projectFilePath )
            }
            onShowLocalChangesRequested: function( projectId ) {
              showChanges( projectId )
            }
            list.onActiveProjectDeleted: setupProjectOpen( "", "" )
          }

          ProjectListPage {
            id: createdProjectsPage

            projectModelType: ProjectsModel.CreatedProjectsModel
            activeProjectId: root.activeProjectId
            list.visible: !stackView.pending

            onOpenProjectRequested: function( projectId, projectFilePath ) {
              setupProjectOpen( projectId, projectFilePath )
            }
            onShowLocalChangesRequested: function( projectId ) {
              showChanges( projectId )
            }
            list.onActiveProjectDeleted: setupProjectOpen( "", "" )
          }

          ProjectListPage {
            id: publicProjectsPage

            projectModelType: ProjectsModel.PublicProjectsModel
            activeProjectId: root.activeProjectId
            list.visible: !stackView.pending

            onOpenProjectRequested: function( projectId, projectFilePath ) {
              setupProjectOpen( projectId, projectFilePath )
            }
            onShowLocalChangesRequested: function( projectId ) {
              showChanges( projectId )
            }
            list.onActiveProjectDeleted: function() {
              setupProjectOpen( "", "" )
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

            text: qsTr("My projects")
            imageSource: InputStyle.accountIcon
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
          if (__merginApi.apiVersionStatus === MerginApiStatus.OK && stackView.currentItem.objectName === "authPanel") {
            if (__merginApi.userAuth.hasAuthData()) {
              refreshProjectList()
            } else if (pageContent.state !== 'local') {
              if (stackView.currentItem.objectName !== "authPanel") {
                root.openAuthPanel()
              }
            }
          }
        }
        function onAuthRequested() {
          stackView.pending = false
          root.openAuthPanel()
        }
        function onAuthChanged() {
          stackView.pending = false
          if ( __merginApi.userAuth.hasAuthData() ) {
            stackView.popOnePageOrClose()
            projectsPage.refreshProjectList()
            root.forceActiveFocus()
          }
        }
        function onAuthFailed() {
          stackView.pending = false
        }
        function onRegistrationFailed() {
          stackView.pending = false
        }
        function onRegistrationSucceeded() {
          stackView.pending = false
        }
      }
    }
  }

  Component {
    id: authPanelComp
    AuthPanel {
      id: authPanel
      objectName: "authPanel"
      visible: false
      pending: stackView.pending
      height: root.height
      width: root.width
      toolbarHeight: InputStyle.rowHeightHeader
      onBack: {
        stackView.popOnePageOrClose()
        if ( !__merginApi.userAuth.hasAuthData() ) {
          root.resetView()
        }
      }
    }
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
    id: accountPanelComp

    AccountPage {
      id: accountPanel
      height: root.height
      width: root.width
      visible: true
      onBack: {
        stackView.popOnePageOrClose()
      }
      onManagePlansClicked: manageSubscriptionPlans()
      onSignOutClicked: {
        if ( __merginApi.userAuth.hasAuthData() ) {
          __merginApi.clearAuth()
        }
        stackView.popOnePageOrClose()
        root.resetView()
      }
      onRestorePurchasesClicked: {
        __purchasing.restore()
      }
      onAccountDeleted: {
        stackView.popOnePageOrClose()
        root.resetView()
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
        stackView.popOnePageOrClose()
      }
      onSubscribeClicked: {
        stackView.popOnePageOrClose()
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
}
