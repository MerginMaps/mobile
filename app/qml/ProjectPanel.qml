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
import "./components/"

Item {
  id: root

  property string activeProjectId: ""
  property string activeProjectPath: ""

  property real rowHeight: InputStyle.rowHeightHeader * 1.2
  property real panelMargin: InputStyle.panelMargin

  signal openProjectRequested( string projectId, string projectPath )

  function openPanel() {
    root.visible = true
    stackView.visible = true
  }

  function hidePanel() {
    root.visible = false
    stackView.clearStackAndClose()
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
          stackView.clearStackAndClose()
          root.visible = false
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

      function refreshProjectList() {

        stackView.pending = true
        switch( pageContent.state ) {
          case "local":
            localProjectsPage.refreshProjectsList()
            break
          case "created":
            createdProjectsPage.refreshProjectsList()
            break
          case "shared":
            sharedProjectsPage.refreshProjectsList()
            break
          case "public":
            publicProjectsPage.refreshProjectsList()
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
          anchors.rightMargin: root.panelMargin

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
                  stackView.push( accountPanelComp )
                  reloadList.visible = false
                }
                else
                  stackView.push( authPanelComp, { state: "login" })
              }
            }

            Image {
              id: userIcon

              anchors.centerIn: avatarImage
              source: 'account.svg'
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
          refreshProjectList()
          console.log("New state: ", pageContent.state)
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
            imageSource: "home.svg"
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
            imageSource: "account.svg"
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

            imageSource: "account-multi.svg"
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
            imageSource: "explore.svg"
            width: pageFooter.itemSize

            handleClicks: false
            faded: pageFooter.currentIndex !== publicProjectsBtn.TabBar.index
          }

          onClicked: pageContent.state = "public"
        }
      }

//-------------------------------------------------
//          Text {
//            id: noProjectsText
//            anchors.fill: parent
//            textFormat: Text.RichText
//            text: "<style>a:link { color: " + InputStyle.fontColor + "; }</style>" +
//                  qsTr("No downloaded projects found.%1Learn %2how to create projects%3 and %4download them%3 onto your device.")
//                  .arg("<br/>")
//                  .arg("<a href='"+ __inputHelp.howToCreateNewProjectLink +"'>")
//                  .arg("</a>")
//                  .arg("<a href='"+ __inputHelp.howToDownloadProjectLink +"'>")

//            onLinkActivated: Qt.openUrlExternally(link)
//            visible: grid.count === 0 && !storagePermissionText.visible
//            color: InputStyle.fontColor
//            font.pixelSize: InputStyle.fontPixelSizeNormal
//            font.bold: true
//            verticalAlignment: Text.AlignVCenter
//            horizontalAlignment: Text.AlignHCenter
//            wrapMode: Text.WordWrap
//            padding: InputStyle.panelMargin/2
//          }

//          Text {
//            id: storagePermissionText
//            anchors.fill: parent
//            textFormat: Text.RichText
//            text: "<style>a:link { color: " + InputStyle.fontColor + "; }</style>" +
//                  qsTr("Input needs a storage permission, %1click to grant it%2 and then restart application.")
//                  .arg("<a href='missingPermission'>")
//                  .arg("</a>")

//            onLinkActivated: {
//              if ( __inputUtils.acquireStoragePermission() ) {
//                restartAppDialog.open()
//              }
//            }
//            visible: !__inputUtils.hasStoragePermission()
//            color: InputStyle.fontColor
//            font.pixelSize: InputStyle.fontPixelSizeNormal
//            font.bold: true
//            verticalAlignment: Text.AlignVCenter
//            horizontalAlignment: Text.AlignHCenter
//            wrapMode: Text.WordWrap
//            padding: InputStyle.panelMargin/2
//          }
//        }
//-------------------------------------------------

//-------------------------------------------------
//          TODO: unable to get list of the projects
//          Label {
//            anchors.fill: parent
//            horizontalAlignment: Qt.AlignHCenter
//            verticalAlignment: Qt.AlignVCenter
//            visible: !merginProjectsList.contentHeight
//            text: reloadList.visible ? qsTr("Unable to get the list of projects.") : qsTr("No projects found!")
//            color: InputStyle.fontColor
//            font.pixelSize: InputStyle.fontPixelSizeNormal
//            font.bold: true
//          }
//-------------------------------------------------

// Toolbar
//-------------------------------------------------

//        onHighlightedChanged: {
////          searchBar.deactivate()
//          if (toolbar.highlighted === homeBtn.text) {
////            __projectsModel.searchExpression = ""
//          } else {
//            __merginApi.pingMergin()  <------------------!!!
//          }
//        }
//-------------------------------------------------

      // Other components

      Item {
        id: reloadList
        width: parent.width
        height: root.rowHeight
        visible: false
        Layout.alignment: Qt.AlignVCenter
        y: root.height/3 * 2

        Button {
            id: reloadBtn
            width: reloadList.width - 2* InputStyle.panelMargin
            height: reloadList.height
            text: qsTr("Retry")
            font.pixelSize: reloadBtn.height/2
            anchors.horizontalCenter: parent.horizontalCenter
            onClicked: {
              stackView.pending = true
              // filters suppose to not change
              __merginApi.listProjects( searchBar.text )
              reloadList.visible = false
            }
            background: Rectangle {
                color: InputStyle.highlightColor
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

      Connections {
        target: __projectWizard
        onProjectCreated: {
          if  (stackView.currentItem.objectName === "projectWizard") {
            stackView.popOnePageOrClose()
          }
        }
      }

      Connections {
        target: __merginApi
        onListProjectsFinished: {
          stackView.pending = false
        }
        onListProjectsFailed: {
          reloadList.visible = true
        }
        onListProjectsByNameFinished: stackView.pending = false
        onApiVersionStatusChanged: {
          stackView.pending = false
          if (__merginApi.apiVersionStatus === MerginApiStatus.OK && stackView.currentItem.objectName === "authPanel") {
            if (__merginApi.userAuth.hasAuthData()) {
              refreshProjectList()
            } else if (toolbar.highlighted !== homeBtn.text) {
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
          if (__merginApi.userAuth.hasAuthData()) {
            refreshProjectList()
            root.forceActiveFocus()
          }
          stackView.popOnePageOrClose()

        }
        onAuthFailed: {
          homeBtn.activated()
          stackView.pending = false
          root.forceActiveFocus()
        }
        onRegistrationFailed: stackView.pending = false
        onRegistrationSucceeded: stackView.pending = false

        //TODO: on sync project failed: push auth panel too
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
        if  (stackView.currentItem.objectName === "projectsPanel") {
          __merginApi.authFailed() // activate homeBtn
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
      onManagePlansClicked: {
        if (__purchasing.hasInAppPurchases && (__purchasing.hasManageSubscriptionCapability || !__merginApi.userInfo.ownsActiveSubscription )) {
          stackView.push( subscribePanelComp)
        } else {
          Qt.openUrlExternally(__purchasing.subscriptionManageUrl);
        }
      }
      onSignOutClicked: {
        if (__merginApi.userAuth.hasAuthData()) {
          __merginApi.clearAuth()
          stackView.popOnePageOrClose()
        }
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
