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

  visible: false
  focus: true

  onFocusChanged: { // pass focus to stackview
    stackView.focus = true
  }

  StackView {
    id: stackView

    initialItem: projectsPanelComp
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

    Keys.onReleased: {
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

      function showChanges( projectId ) {
        if ( __merginProjectStatusModel.loadProjectInfo( projectId ) ) {
          stackView.push( statusPanelComp )
        }
        else __inputUtils.showNotification( qsTr( "No Changes" ) )
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
                  stackView.push( authPanelComp, { state: "login" })
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
          onVisibleChanged: {
            if ( root.visible ) { // projectsPanel opened
              pageContent.state = "local"
            }
            else {
              pageContent.state = ""
            }
          }

          onResetView: {
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

            onOpenProjectRequested: setupProjectOpen( projectId, projectFilePath )
            onShowLocalChangesRequested: showChanges( projectId )
            list.onActiveProjectDeleted: setupProjectOpen( "", "" )
          }

          ProjectListPage {
            id: createdProjectsPage

            projectModelType: ProjectsModel.CreatedProjectsModel
            activeProjectId: root.activeProjectId
            list.visible: !stackView.pending

            onOpenProjectRequested: setupProjectOpen( projectId, projectFilePath )
            onShowLocalChangesRequested: showChanges( projectId )
            list.onActiveProjectDeleted: setupProjectOpen( "", "" )
          }

          ProjectListPage {
            id: sharedProjectsPage

            projectModelType: ProjectsModel.SharedProjectsModel
            activeProjectId: root.activeProjectId
            list.visible: !stackView.pending

            onOpenProjectRequested: setupProjectOpen( projectId, projectFilePath )
            onShowLocalChangesRequested: showChanges( projectId )
            list.onActiveProjectDeleted: setupProjectOpen( "", "" )
          }

          ProjectListPage {
            id: publicProjectsPage

            projectModelType: ProjectsModel.PublicProjectsModel
            activeProjectId: root.activeProjectId
            list.visible: !stackView.pending

            onOpenProjectRequested: setupProjectOpen( projectId, projectFilePath )
            onShowLocalChangesRequested: showChanges( projectId )
            list.onActiveProjectDeleted: setupProjectOpen( "", "" )
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
        onProjectCreated: {
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
        onListProjectsFinished: stackView.pending = false
        onListProjectsByNameFinished: stackView.pending = false
        onApiVersionStatusChanged: {
          stackView.pending = false
          if (__merginApi.apiVersionStatus === MerginApiStatus.OK && stackView.currentItem.objectName === "authPanel") {
            if (__merginApi.userAuth.hasAuthData()) {
              refreshProjectList()
            } else if (pageContent.state !== 'local') {
              if (stackView.currentItem.objectName !== "authPanel") {
                stackView.push(authPanelComp, {state: "login"})
              }
            }
          }
        }
        onAuthRequested: {
          stackView.pending = false
          stackView.push(authPanelComp, {state: "login"})
        }
        onAuthChanged: {
          stackView.pending = false
          if ( __merginApi.userAuth.hasAuthData() ) {
            stackView.popOnePageOrClose()
            projectsPage.refreshProjectList()
            root.forceActiveFocus()
          }
        }
        onAuthFailed: stackView.pending = false
        onRegistrationFailed: stackView.pending = false
        onRegistrationSucceeded: stackView.pending = false
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
