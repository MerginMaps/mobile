import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.2
import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "."  // import InputStyle singleton

Item {

  property int activeProjectIndex: -1
  property string activeProjectPath: __projectsModel.data(__projectsModel.index(activeProjectIndex), ProjectModel.Path)
  property string merginSearchExpression: ""
  property var busyIndicator

  property real rowHeight: InputStyle.rowHeightHeader * 1.2
  property real iconSize: rowHeight/3
  property bool showMergin: false
  property real panelMargin: InputStyle.panelMargin

  function openPanel() {
    homeBtn.activated()
    projectsPanel.visible = true
  }

  function getStatusIcon(status, pending) {
    if (pending) return "stop.svg"

    if (status === "noVersion") return "download.svg"
    else if (status === "outOfDate") return "sync.svg"
    else if (status === "upToDate") return "check.svg"
    else if (status === "modified") return "sync.svg"

    return "more_menu.svg"
  }

  Component.onCompleted: {
    // load model just after all components are prepared
    // otherwise GridView's delegate item is initialized invalidately
    grid.model = __projectsModel
    merginProjectsList.model = __merginProjectsModel
  }

  Connections {
    target: __merginApi
    onListProjectsFinished: {
      busyIndicator.running = false
    }
    onListProjectsFailed: {
      reloadList.visible = true
    }
    onApiVersionStatusChanged: {
      busyIndicator.running = false
      if (__merginApi.apiVersionStatus === MerginApiStatus.OK && authPanel.visible) {
        if (__merginApi.hasAuthData()) {
          authPanel.visible = false
          // filters should be set already
          __merginApi.listProjects(searchField.text)
        }
      } else if (toolbar.highlighted !== homeBtn.text) {
        authPanel.visible = true
      }
    }
    onAuthRequested: {
      busyIndicator.running = false
      authPanel.visible = true
    }
    onAuthChanged: {
      authPanel.pending = false
      if (__merginApi.hasAuthData()) {
        authPanel.close()
        myProjectsBtn.activated()
      } else {
        homeBtn.activated()
      }
    }
    onAuthFailed: authPanel.pending = false
  }

  id: projectsPanel
  visible: false
  focus: true

  Keys.onReleased: {
    if (!activeProjectPath) return

    if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
      event.accepted = true;
      projectsPanel.visible = false
    }
  }

  Keys.forwardTo: authPanel.visible ? authPanel : []

  // background
  Rectangle {
    width: parent.width
    height: parent.height
    color: InputStyle.clrPanelMain
  }

  BusyIndicator {
    id: busyIndicator
    width: parent.width/8
    height: width
    running: false
    visible: running
    anchors.centerIn: parent
    z: authPanel.z + 1
  }

  PanelHeader {
    id: header
    height: InputStyle.rowHeightHeader
    width: parent.width
    color: InputStyle.clrPanelMain
    rowHeight: InputStyle.rowHeightHeader
    titleText: qsTr("Projects")

    onBack: {
      if (authPanel.visible) {
        authPanel.visible = false
        homeBtn.activated()
      } else {
        projectsPanel.visible = false
      }
    }
    withBackButton: projectsPanel.activeProjectPath || authPanel.visible

    Item {
      id: avatar
      width: InputStyle.rowHeightHeader * 0.8
      height: InputStyle.rowHeightHeader
      anchors.right: parent.right
      anchors.rightMargin: projectsPanel.panelMargin

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
            if (__merginApi.hasAuthData() && __merginApi.apiVersionStatus === MerginApiStatus.OK) {
              __merginApi.getUserInfo(__merginApi.username)
              accountPanel.visible = true
              reloadList.visible = false
            }
            else
              myProjectsBtn.activated() // open auth form
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

  // SearchBar
  Rectangle {
    id: searchBar
    width: parent.width
    height: InputStyle.rowHeightHeader
    y: header.height
    color: InputStyle.panelBackgroundLight

    property color bgColor: InputStyle.panelBackgroundLight
    property color fontColor: InputStyle.panelBackgroundDarker

    /**
     * Used for deactivating focus on SearchBar when another component should have focus.
     * and the current element's forceActiveFocus() doesnt deactivates SearchBar focus.
     */
    function deactivate() {
      searchField.text = ""
      searchField.focus = false
    }

    Item {
      id: row
      width: searchBar.width
      height: searchBar.height

      TextField {
        id: searchField
        width: parent.width
        height: InputStyle.rowHeight
        font.pixelSize: InputStyle.fontPixelSizeNormal
        color: searchBar.fontColor
        placeholderText: qsTr("SEARCH")
        font.capitalization: Font.MixedCase
        inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
        background: Rectangle {
          color: searchBar.bgColor
        }
        leftPadding: projectsPanel.panelMargin
        rightPadding: projectsPanel.panelMargin

        onTextChanged: {
          if (toolbar.highlighted === homeBtn.text) {
            __projectsModel.searchExpression = searchField.text
          } else if (toolbar.highlighted === exploreBtn.text) {
            exploreBtn.activated()
          } else if (toolbar.highlighted === sharedProjectsBtn.text) {
            sharedProjectsBtn.activated()
          } else if (toolbar.highlighted === myProjectsBtn.text) {
            myProjectsBtn.activated()
          }else {
            merginSearchExpression = searchField.text
          }
        }
      }

      Item {
        id: iconContainer
        height: searchField.height
        width: projectsPanel.iconSize
        anchors.right: parent.right
        anchors.rightMargin: projectsPanel.panelMargin

        Image {
          id: cancelSearchBtn
          source: searchField.text ? "no.svg" : "search.svg"
          width: projectsPanel.iconSize
          height: width
          sourceSize.width: width
          sourceSize.height: height
          anchors.centerIn: parent
          fillMode: Image.PreserveAspectFit

          MouseArea {
            anchors.fill: parent
            onClicked: {
              if (searchField.text) {
                searchBar.deactivate()
              }
            }
          }
        }

        ColorOverlay {
          anchors.fill: cancelSearchBtn
          source: cancelSearchBtn
          color: searchBar.fontColor
        }
      }
    }

    Rectangle {
      id: searchFieldBorder
      color: searchBar.fontColor
      y: searchField.height - height * 4
      height: 2 * QgsQuick.Utils.dp
      opacity: searchField.focus ? 1 : 0.6
      width: parent.width - projectsPanel.panelMargin*2
      anchors.horizontalCenter: parent.horizontalCenter
    }
  }

  // Content
  ColumnLayout {
    id: contentLayout
    height: projectsPanel.height-header.height-searchBar.height-toolbar.height
    width: parent.width
    y: header.height + searchBar.height
    spacing: 0

    // Info label
    Item {
      id: infoLabel
      width: parent.width
      height: toolbar.highlighted === exploreBtn.text ? projectsPanel.rowHeight * 2 : 0
      visible: height

      Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
        color: InputStyle.panelBackgroundDarker
        font.pixelSize: InputStyle.fontPixelSizeNormal
        text: qsTr("Explore public Mergin projects!")
        visible: parent.height
      }

      // To not propagate click on canvas on background
      MouseArea {
        anchors.fill: parent
      }

      Item {
        id: infoLabelHideBtn
        height: projectsPanel.iconSize
        width: height
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: projectsPanel.panelMargin
        anchors.topMargin: projectsPanel.panelMargin

        MouseArea {
          anchors.fill: parent
          onClicked: infoLabel.visible = false
        }

        Image {
          id: infoLabelHide
          anchors.centerIn: infoLabelHideBtn
          source: 'no.svg'
          height: infoLabelHideBtn.height
          width: height
          sourceSize.width: width
          sourceSize.height: height
          fillMode: Image.PreserveAspectFit
        }

        ColorOverlay {
          anchors.fill: infoLabelHide
          source: infoLabelHide
          color: InputStyle.panelBackgroundDark
        }
      }

      Rectangle {
          id: borderLine
          color: InputStyle.panelBackground2
          width: parent.width
          height: 1 * QgsQuick.Utils.dp
          anchors.bottom: parent.bottom
      }
    }

    ListView {
      id: grid
      Layout.fillWidth: true
      Layout.fillHeight: true
      contentWidth: grid.width
      clip: true
      visible: !showMergin

      property int cellWidth: width
      property int cellHeight: projectsPanel.rowHeight
      property int borderWidth: 1

      delegate: delegateItem

      Text {
        anchors.fill: parent
        textFormat: Text.RichText
        text: "<style>a:link { color: " + InputStyle.fontColor + "; }</style>" +
              "No projects found.<br> See <a href='https://github.com/lutraconsulting/input/blob/master/docs/users/project_config.md'>how to create a project</a>" +
              " and <a href='https://github.com/lutraconsulting/input/blob/master/docs/users/data_sync.md'>how to download it</a> to your device."
        onLinkActivated: Qt.openUrlExternally(link)
        visible: grid.count === 0
        color: InputStyle.fontColor
        font.pixelSize: InputStyle.fontPixelSizeNormal
        font.bold: true
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        wrapMode: Text.WordWrap
        padding: InputStyle.panelMargin/2
      }
    }

    ListView {
      id: merginProjectsList
      visible: showMergin && !busyIndicator.running
      Layout.fillWidth: true
      Layout.fillHeight: true
      contentWidth: grid.width
      clip: true

      property int cellWidth: width
      property int cellHeight: projectsPanel.rowHeight
      property int borderWidth: 1

      Label {
        anchors.fill: parent
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
        visible: !merginProjectsList.contentHeight
        text: reloadList.visible ? qsTr("Unable to get the list of projects.") : qsTr("No projects found!")
        color: InputStyle.fontColor
        font.pixelSize: InputStyle.fontPixelSizeNormal
        font.bold: true
      }

      delegate: delegateItemMergin
    }
  }

  Component {
    id: delegateItem
    ProjectDelegateItem {
      cellWidth: projectsPanel.width
      cellHeight: projectsPanel.rowHeight
      iconSize: projectsPanel.iconSize
      width: cellWidth
      height: cellHeight
      visible: height ? true : false
      statusIconSource: "trash.svg"
      itemMargin: projectsPanel.panelMargin
      projectFullName: (projectNamespace && projectName) ? (projectNamespace + "/" + projectName) : folderName
      disabled: !isValid // invalid project
      highlight: {
        if (disabled) return true
        return path === projectsPanel.activeProjectPath ? true : false
      }

      onItemClicked: {
        if (showMergin) return

        projectsPanel.activeProjectIndex = index
        projectsPanel.visible = false
      }

      onMenuClicked: {
        deleteDialog.relatedProjectIndex = index
        deleteDialog.open()
      }
    }
  }

  Component {
    id: delegateItemMergin
    ProjectDelegateItem {
      cellWidth: projectsPanel.width
      cellHeight: projectsPanel.rowHeight
      width: cellWidth
      height: cellHeight
      visible: height ? true : false
      pending: pendingProject
      statusIconSource: getStatusIcon(status, pendingProject)
      iconSize: projectsPanel.iconSize
      projectFullName: projectNamespace + "/" + projectName
      progressValue: syncProgress

      onMenuClicked: {
        if (status === "upToDate") return

        if (pendingProject) {
          if (status === "modified") {
            __merginApi.uploadCancel(projectFullName)
          }
          if (status === "noVersion" || status === "outOfDate") {
            __merginApi.updateCancel(projectFullName)
          }
          return
        }

        if (status === "noVersion" || status === "outOfDate") {
          __merginApi.updateProject(projectNamespace, projectName)
        } else if (status === "modified") {
          __merginApi.uploadProject(projectNamespace, projectName)
        }
      }

    }
  }


  // Toolbar
  Rectangle {
    property int itemSize: toolbar.height * 0.8
    property string highlighted: homeBtn.text

    id: toolbar
    height: InputStyle.rowHeightHeader
    width: parent.width
    anchors.bottom: parent.bottom
    color: InputStyle.clrPanelBackground

    onHighlightedChanged: {
      searchField.text = ""
      if (toolbar.highlighted === homeBtn.text) {
        __projectsModel.searchExpression = ""
      } else {
        __merginApi.pingMergin()
      }
    }

    Row {
      height: toolbar.height
      width: parent.width
      anchors.bottom: parent.bottom

      Item {
        width: parent.width/parent.children.length
        height: parent.height

        MainPanelButton {

          id: homeBtn
          width: toolbar.itemSize
          text: qsTr("Home")
          imageSource: "home.svg"
          faded: toolbar.highlighted !== homeBtn.text

          onActivated: {
            toolbar.highlighted = homeBtn.text;
            if (authPanel.visible)
              authPanel.close()
            showMergin = false
          }
        }
      }

      Item {
        width: parent.width/parent.children.length
        height: parent.height
        MainPanelButton {
          id: myProjectsBtn
          width: toolbar.itemSize
          text: qsTr("My projects")
          imageSource: "account.svg"
          faded: toolbar.highlighted !== myProjectsBtn.text

          onActivated: {
            toolbar.highlighted = myProjectsBtn.text
            busyIndicator.running = true
            showMergin = true
            __merginProjectsModel.filterCreator = __merginApi.userId
            __merginProjectsModel.filterWriter = -1
            __merginApi.listProjects(searchField.text, "created")
          }
        }
      }

      Item {
        width: parent.width/parent.children.length
        height: parent.height
        MainPanelButton {
          id: sharedProjectsBtn
          width: toolbar.itemSize
          text: parent.width > sharedProjectsBtn.width * 2 ? qsTr("Shared with me") : qsTr("Shared")
          imageSource: "account-multi.svg"
          faded: toolbar.highlighted !== sharedProjectsBtn.text

          onActivated: {
            toolbar.highlighted = sharedProjectsBtn.text
            busyIndicator.running = true
            showMergin = true
            __merginProjectsModel.filterCreator = -1
            __merginProjectsModel.filterWriter = __merginApi.userId
            __merginApi.listProjects(searchField.text, "shared")
          }
        }
      }

      Item {
        width: parent.width/parent.children.length
        height: parent.height
        MainPanelButton {
          id: exploreBtn
          width: toolbar.itemSize
          text: qsTr("Explore")
          imageSource: "explore.svg"
          faded: toolbar.highlighted !== exploreBtn.text

          onActivated: {
            toolbar.highlighted = exploreBtn.text
            busyIndicator.running = true
            showMergin = true
            authPanel.visible = false
            __merginProjectsModel.filterCreator = -1
            __merginProjectsModel.filterWriter = -1
            __merginApi.listProjects(searchField.text)
          }
        }
      }
    }
  }


  // Other components
  AuthPanel {
    id: authPanel
    visible: false
    y: searchBar.y
    height: contentLayout.height + searchBar.height
    width: parent.width
    onAuthFailed: homeBtn.activated()
    toolbarHeight: toolbar.height
    onPendingChanged: busyIndicator.running = authPanel.pending
  }

  AccountPage {
    id: accountPanel
    height: window.height
    width: parent.width
    visible: false
  }

  MessageDialog {
    id: deleteDialog
    visible: false
    property int relatedProjectIndex

    title: qsTr( "Delete project" )
    text: qsTr( "Do you really want to delete project?" )
    icon: StandardIcon.Warning
    standardButtons: StandardButton.Ok | StandardButton.Cancel
    onAccepted: {
      __projectsModel.deleteProject(relatedProjectIndex)
      if (projectsPanel.activeProjectIndex === relatedProjectIndex) {
        __loader.load("")
        __loader.projectReloaded();
        projectsPanel.activeProjectIndex = -1
      }
      deleteDialog.relatedProjectIndex = -1
      visible = false
    }
    onRejected: {
      deleteDialog.relatedProjectIndex = -1
      visible = false
    }
  }

  Item {
    id: reloadList
    width: parent.width
    height: grid.cellHeight
    visible: false
    Layout.alignment: Qt.AlignVCenter
    y: projectsPanel.height/3 * 2

    Button {
        id: reloadBtn
        width: reloadList.width - 2* InputStyle.panelMargin
        height: reloadList.height
        text: qsTr("Try again")
        font.pixelSize: reloadBtn.height/2
        anchors.horizontalCenter: parent.horizontalCenter
        onClicked: {
          busyIndicator.running = true
          // filters suppose to not change
          __merginApi.listProjects(searchField.text)
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
}
