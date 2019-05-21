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
  property string activeProjectName: __projectsModel.data(__projectsModel.index(activeProjectIndex), ProjectModel.Name)
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

  function getStatusIcon(status) {
    if (status === "noVersion") return "download.svg"
    else if (status === "outOfDate") return "update.svg"
    else if (status === "upToDate") return "check.svg"
    else if (status === "modified") return "upload.svg"

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
    onApiVersionStatusChanged: {
      busyIndicator.running = false
      if (__merginApi.apiVersionStatus === MerginApiStatus.OK && authPanel.visible) {
        authPanel.visible = false
      }
    }
    onApiIncompatibilityOccured: {
      busyIndicator.running = false
      if (projectsPanel.visible && toolbar.highlighted !== homeBtn.text) {
        authPanel.visible = true
      }
    }
    onAuthRequested: {
      busyIndicator.running = false
      authPanel.visible = true
    }
    onAuthChanged: {
      if (__merginApi.hasAuthData()) {
        authPanel.close()
        myProjectsBtn.activated()
      } else {
        homeBtn.activated()
      }
    }
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

      Label {
        anchors.fill: parent
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
        visible: parent.count == 0
        text: qsTr("No projects found!")
        color: InputStyle.fontColor
        font.pixelSize: InputStyle.fontPixelSizeNormal
        font.bold: true
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
      height: passesFilter ? cellHeight : 0
      visible: height ? true : false
      statusIconSource: "trash.svg"
      itemMargin: projectsPanel.panelMargin
      projectName: projectNamespace + "/" + folderName
      disabled: !isValid // invalid project
      highlight: {
        if (disabled) return true
        return path === projectsPanel.activeProjectPath ? true : false
      }

      onItemClicked: {
        if (showMergin) return
        projectsPanel.activeProjectIndex = index
        __appSettings.defaultProject = path
        projectsPanel.visible = false
        projectsPanel.activeProjectIndexChanged()
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
      height: passesFilter ? cellHeight : 0
      visible: height ? true : false
      pending: pendingProject
      statusIconSource: getStatusIcon(status)
      iconSize: projectsPanel.iconSize
      projectName: name

      onMenuClicked: {
        if (status === "upToDate") return

        __merginProjectsModel.setPending(index, true)

        if (status === "noVersion") {
          __merginApi.downloadProject(name)
        } else if (status === "outOfDate") {
          __merginApi.updateProject(name)
        } else if (status === "modified") {
          __merginApi.uploadProject(name)
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
            __merginApi.listProjects(searchField.text)
          }
        }
      }

      Item {
        width: parent.width/parent.children.length
        height: parent.height
        MainPanelButton {
          id: sharedProjectsBtn
          width: toolbar.itemSize
          text: qsTr("Shared with me")
          imageSource: "account-multi.svg"
          faded: toolbar.highlighted !== sharedProjectsBtn.text

          onActivated: {
            toolbar.highlighted = sharedProjectsBtn.text
            busyIndicator.running = true
            showMergin = true
            __merginProjectsModel.filterCreator = -1
            __merginProjectsModel.filterWriter = __merginApi.userId
            __merginApi.listProjects(searchField.text)
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
    onAuthFailed: myProjectsBtn.activated()
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
}
