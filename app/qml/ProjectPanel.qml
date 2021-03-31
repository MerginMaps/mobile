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

//      SearchBar {
//        id: searchBar
//        y: pageHeader.height
//        allowTimer: true

//        onSearchTextChanged: {
//          if (toolbar.highlighted === homeBtn.text) {
//            __localProjectsProxyModel.searchExpression = text
//          } else if (toolbar.highlighted === exploreBtn.text) {
//            // Filtered by request
//            exploreBtn.activated()
//          } else if (toolbar.highlighted === sharedProjectsBtn.text) {
//            __merginProjectsModel.searchExpression = text
//          } else if (toolbar.highlighted === myProjectsBtn.text) {
//            __merginProjectsModel.searchExpression = text
//          }
//        }
//      }

//      // Content
//      ColumnLayout {
//        id: contentLayout
//        height: projectsPanel.height-pageHeader.height-searchBar.height-toolbar.height
//        width: parent.width
//        y: pageHeader.height + searchBar.height
//        spacing: 0

        // Info label
//        Item {
//          id: infoLabel
//          width: parent.width
//          height: toolbar.highlighted === exploreBtn.text ? projectsPanel.rowHeight * 2 : 0
//          visible: height

//          Text {
//            anchors.horizontalCenter: parent.horizontalCenter
//            anchors.verticalCenter: parent.verticalCenter
//            horizontalAlignment: Text.AlignHCenter
//            verticalAlignment: Text.AlignVCenter
//            wrapMode: Text.WordWrap
//            color: InputStyle.panelBackgroundDarker
//            font.pixelSize: InputStyle.fontPixelSizeNormal
//            text: qsTr("Explore public projects.")
//            visible: parent.height
//          }

//          // To not propagate click on canvas on background
//          MouseArea {
//            anchors.fill: parent
//          }

//          Item {
//            id: infoLabelHideBtn
//            height: projectsPanel.iconSize
//            width: height
//            anchors.right: parent.right
//            anchors.top: parent.top
//            anchors.rightMargin: projectsPanel.panelMargin
//            anchors.topMargin: projectsPanel.panelMargin

//            MouseArea {
//              anchors.fill: parent
//              onClicked: infoLabel.visible = false
//            }

//            Image {
//              id: infoLabelHide
//              anchors.centerIn: infoLabelHideBtn
//              source: 'no.svg'
//              height: infoLabelHideBtn.height
//              width: height
//              sourceSize.width: width
//              sourceSize.height: height
//              fillMode: Image.PreserveAspectFit
//            }

//            ColorOverlay {
//              anchors.fill: infoLabelHide
//              source: infoLabelHide
//              color: InputStyle.panelBackgroundDark
//            }
//          }

//          Rectangle {
//              id: borderLine
//              color: InputStyle.panelBackground2
//              width: parent.width
//              height: 1 * QgsQuick.Utils.dp
//              anchors.bottom: parent.bottom
//          }
//        }

//        ProjectListPage {
//          id: localProjectsList


//        }

//        ListView {
//          id: grid
//          Layout.fillWidth: true
//          Layout.fillHeight: true
//          contentWidth: grid.width
//          clip: true
//          visible: !showMergin
//          maximumFlickVelocity: __androidUtils.isAndroid ? InputStyle.scrollVelocityAndroid : maximumFlickVelocity

//          model: ProjectsProxyModel {
//            projectSourceModel: ProjectsModel {
//              id: myModel
//              localProjectsManager: __localProjectsManager
//              modelType: ProjectsModel.LocalProjectsModel
//              merginApi: __merginApi
//            }
//          }

//          property int cellWidth: width
//          property int cellHeight: projectsPanel.rowHeight
//          property int borderWidth: 1

//          delegate: delegateItem

//          footer: DelegateButton {
//            height: projectsPanel.rowHeight
//            width: parent.width
//            text: qsTr("Create project")
//            onClicked: {
//              if (__inputUtils.hasStoragePermission()) {
//                stackView.push(projectWizardComp)
//              } else if (__inputUtils.acquireStoragePermission()) {
//                restartAppDialog.open()
//              }
//            }
//         }

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

//        ListView {
//          id: merginProjectsList

//          property int paginatedPage: 0

//          visible: showMergin && !busyIndicator.running
//          Layout.fillWidth: true
//          Layout.fillHeight: true
//          contentWidth: grid.width
//          clip: true
//          maximumFlickVelocity: __androidUtils.isAndroid ? InputStyle.scrollVelocityAndroid : maximumFlickVelocity

//          onCountChanged: {
//            if (merginProjectsList.visible || paginatedPage > 1) {
//              merginProjectsList.positionViewAtIndex(merginProjectsList.currentIndex, ListView.End)
//            }
//          }

//          property int cellWidth: width
//          property int cellHeight: projectsPanel.rowHeight
//          property int borderWidth: 1

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

//          delegate: delegateItemMergin

//          footer:  Button {
//            text: "ListProjects API"
//            onClicked: {
//              __myProjectsModel.listProjects(merginProjectsList.paginatedPage + 1)
//              merginProjectsList.paginatedPage++
//            }
//          }
//        }

//      }

//      Component {
//        id: delegateItem

//        ProjectDelegateItem {
//          id: delegateItemContent

//          projectFullName: model.ProjectFullName
//          projectId: model.ProjectId
//          projectDescription: model.ProjectDescription
//          projectStatus: model.ProjectSyncStatus
//          projectIsValid: model.ProjectIsValid
//          projectIsPending: model.ProjectPending ? true : false
//          projectSyncProgress: model.ProjectSyncProgress ? ProjectSyncProgress : 0
//          projectIsLocal: model.ProjectIsLocal
//          projectIsMergin: model.ProjectIsMergin

//          width: parent.width
//          height: projectsPanel.rowHeight
//          viewContentY: ListView.view.contentY
//          viewHeight: ListView.view.height

//          onOpenRequested: console.log( "PMR: Open", projectId )
//          onSyncRequested: console.log( "PMR: Sync", projectId )
//          onMigrateRequested: console.log( "PMR: Upload", projectId )
//          onRemoveRequested: console.log( "PMR: Remove", projectId )
//          onStopSyncRequested: console.log( "PMR: Stop", projectId )
//          onShowChangesRequested: console.log( "PMR: Show changes", projectId )
//        }
//      }

//        ProjectDelegateItem {
//          id: delegateItemContent
//          cellWidth: projectsPanel.width
//          cellHeight: projectsPanel.rowHeight
//          iconSize: projectsPanel.iconSize
//          width: cellWidth
//          height: cellHeight
//          visible: height ? true : false
//          statusIconSource: "more_menu.svg"
//          itemMargin: projectsPanel.panelMargin
//          projectFullName: ProjectFullName
//          projectDescription: getStatusIcon( ProjectSyncStatus, ProjectPending )
//          disabled: !ProjectIsValid // invalid project
//          highlight: {
//            if (disabled) return true
//            return ProjectFilePath === projectsPanel.activeProjectPath ? true : false
//          }

//          Menu {
//            property real menuItemHeight: projectsPanel.rowHeight * 0.8
//            id: contextMenu
//            height: menuItemHeight * 2
//            width:Math.min( parent.width, 300 * QgsQuick.Utils.dp )
//            leftMargin: Math.max(parent.width - width, 0)

//            //! sets y-offset either above or below related item according relative position to end of the list
//            onAboutToShow: {
//              var itemRelativeY = parent.y - grid.contentY
//              if (itemRelativeY + contextMenu.height >= grid.height)
//                contextMenu.y = -contextMenu.height
//              else
//                contextMenu.y = parent.height
//            }

//            MenuItem {
//              height: ProjectIsMergin ? contextMenu.menuItemHeight : 0
//              ExtendedMenuItem {
//                  height: parent.height
//                  rowHeight: parent.height
//                  width: parent.width
//                  contentText: qsTr("Status")
//                  imageSource: InputStyle.infoIcon
//                  overlayImage: true
//              }
//              onClicked: {
//                if (__merginProjectStatusModel.loadProjectInfo(delegateItemContent.projectFullName)) {
//                  stackView.push(statusPanelComp)
//                } else __inputUtils.showNotification(qsTr("No Changes"))
//              }
//            }

//            MenuItem {
//              height: ProjectIsMergin ? 0: contextMenu.menuItemHeight
//              ExtendedMenuItem {
//                  height: parent.height
//                  rowHeight: parent.height
//                  width: parent.width
//                  contentText: qsTr("Upload to Mergin")
//                  imageSource: InputStyle.uploadIcon
//                  overlayImage: true
//              }
//              onClicked: {
//                if (!ProjectIsMergin) {
//                  __merginApi.migrateProjectToMergin(projectName)
//                }
//              }
//            }

//            MenuItem {
//              height: contextMenu.menuItemHeight
//              ExtendedMenuItem {
//                  height: parent.height
//                  rowHeight: parent.height
//                  width: parent.width
//                  contentText: qsTr("Remove from device")
//                  imageSource: InputStyle.removeIcon
//                  overlayImage: true
//              }
//              onClicked: {
//                deleteDialog.relatedProjectDirectory = ProjectDirectory
//                deleteDialog.open()
//              }
//            }
//          }

//          onItemClicked: {
//            if (showMergin) return

//            projectsPanel.activeProjectIndex = index
//            projectsPanel.visible = false
//          }

//          onMenuClicked:contextMenu.open()
//        }

//      Component {
//        id: delegateItemMergin

//        Rectangle {
//          width: 20
//          height: 20
//          color: "green"
//        }

//        ProjectDelegateItem {
//          cellWidth: projectsPanel.width
//          cellHeight: projectsPanel.rowHeight
//          width: cellWidth
//          height: cellHeight
//          visible: height ? true : false
//          pending: ProjectPending
//          statusIconSource: getStatusIcon(ProjectSyncStatus, ProjectPending)
//          iconSize: projectsPanel.iconSize
//          projectFullName: ProjectFullName
//          progressValue: ProjectSyncProgress
//          projectDescription: ProjectDescription
//          isAdditional: __myProjectsModel.canFetchMore() // TODO: replace with delegate button on listview footer

//          onMenuClicked: {
//            if ( !__inputUtils.hasStoragePermission() ) {
//              if ( __inputUtils.acquireStoragePermission() )
//                restartAppDialog.open() // TODO: replace with reload data!
//              return
//            }

//            if (ProjectSyncStatus === ProjectStatus.UpToDate) return

//            if ( ProjectPending ) {
//              __myProjectsModel.stopProjectSync(ProjectNamespace, ProjectName)
//              return
//            }

//            __myProjectsModel.syncProject(ProjectNamespace, ProjectName)
//          }

//          onDelegateButtonClicked: { // TODO: replace with footer property
//            var searchText = searchBar.text

//            // Note that current index used to save last item position
//            merginProjectsList.currentIndex = merginProjectsList.count - 1 // TODO: huh?

//            __myProjectsModel.listProjects(merginProjectsList.paginatedPage + 1, searchText)
//          }

//        }
//      }


      // Toolbar
//      Rectangle {
//        property int itemSize: toolbar.height * 0.8
//        property string highlighted: homeBtn.text

//        id: toolbar
//        height: InputStyle.rowHeightHeader
//        width: parent.width
//        anchors.bottom: parent.bottom
//        color: InputStyle.clrPanelBackground
//        visible: false

//        MouseArea {
//          anchors.fill: parent
//          onClicked: {} // dont do anything, just do not let click event propagate
//        }

//        onHighlightedChanged: {
////          searchBar.deactivate()
//          if (toolbar.highlighted === homeBtn.text) {
////            __projectsModel.searchExpression = ""
//          } else {
//            __merginApi.pingMergin()
//          }
//        }

//        Row {
//          height: toolbar.height
//          width: parent.width
//          anchors.bottom: parent.bottom

//          Item {
//            width: parent.width/parent.children.length
//            height: parent.height

//            MainPanelButton {

//              id: homeBtn
//              width: toolbar.itemSize
//              text: qsTr("Home")
//              imageSource: "home.svg"
//              faded: toolbar.highlighted !== homeBtn.text

//              onActivated: {
//                toolbar.highlighted = homeBtn.text;
//                showMergin = false
////                stackView.pending = true
//                myModel.listProjectsByName()
//              }
//            }
//          }

//          Item {
//            width: parent.width/parent.children.length
//            height: parent.height
//            MainPanelButton {
//              id: myProjectsBtn
//              width: toolbar.itemSize
//              text: qsTr("My projects")
//              imageSource: "account.svg"
//              faded: toolbar.highlighted !== myProjectsBtn.text

//              onActivated: {
//                toolbar.highlighted = myProjectsBtn.text
//                stackView.pending = true
//                showMergin = true
//                __myProjectsModel.listProjects()
//              }
//            }
//          }

//          Item {
//            width: parent.width/parent.children.length
//            height: parent.height
//            MainPanelButton {
//              id: sharedProjectsBtn
//              width: toolbar.itemSize
//              text: parent.width > sharedProjectsBtn.width * 2 ? qsTr("Shared with me") : qsTr("Shared")
//              imageSource: "account-multi.svg"
//              faded: toolbar.highlighted !== sharedProjectsBtn.text

//              onActivated: {
//                toolbar.highlighted = sharedProjectsBtn.text
//                stackView.pending = true
//                showMergin = true
//                __merginApi.listProjects("", "shared")
//              }
//            }
//          }

//          Item {
//            width: parent.width/parent.children.length
//            height: parent.height
//            MainPanelButton {
//              id: exploreBtn
//              width: toolbar.itemSize
//              text: qsTr("Explore")
//              imageSource: "explore.svg"
//              faded: toolbar.highlighted !== exploreBtn.text

//              onActivated: {
//                toolbar.highlighted = exploreBtn.text
//                stackView.pending = true
//                showMergin = true
//                __merginApi.listProjects( searchBar.text )
//              }
//            }
//          }
//        }
//      }

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
