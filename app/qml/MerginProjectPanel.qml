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
  property var busyIndicator

  property real rowHeight: InputStyle.rowHeightHeader * 1.2
  property bool showMergin: false
  property real panelMargin: InputStyle.panelMargin/2

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
  }

  Connections {
    target: __merginApi
    onAuthRequested: {
      busyIndicator.running = false
      authPanel.visible = true
    }
  }

  Connections {
    target: __merginApi
    onAuthChanged: {
      if (__merginApi.hasAuthData()) {
        authPanel.close()
        homeBtn.clicked()
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

    onBack: projectsPanel.visible = false
    withBackButton: projectsPanel.activeProjectPath

    Item {
      id: avatar
      width: InputStyle.rowHeightHeader
      height: width
      anchors.right: parent.right
      anchors.rightMargin: projectsPanel.panelMargin

      Rectangle {
        id: avatarImage
        anchors.centerIn: parent
        width: InputStyle.rowHeightHeader * 0.7
        height: width
        color: InputStyle.fontColor
        radius: width*0.5
        antialiasing: true

        MouseArea {
          anchors.fill: parent
          onClicked: {
            if (__merginApi.hasAuthData())
              accountPanel.visible = true
            else
              myProjectsBtn.activated() // open auth form
          }
        }

        Image {
          id: userIcon
          anchors.fill: parent
          source: 'account.svg'
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

    property real fieldHeight: InputStyle.rowHeightHeader
    property color bgColor: InputStyle.panelBackgroundLight
    property color fontColor: InputStyle.panelBackgroundDark

    /**
     * Used for deactivating focus on SearchBar when another component should have focus.
     * and the current element's forceActiveFocus() doesnt deactivates SearchBar focus.
     */
    function deactivate() {
      searchField.text = ""
      searchField.focus = false
      //listView.visible = false
    }

    Item {
      id: row
      width: searchBar.width
      height: searchBar.height

      TextField {
        id: searchField
        width: parent.width
        height: searchBar.fieldHeight * 0.8
        font.pixelSize: InputStyle.fontPixelSizeNormal
        color: searchBar.fontColor
        placeholderText: qsTr("SEARCH")
        font.capitalization: Font.MixedCase
        inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
        background: Rectangle {
          color: searchBar.bgColor
        }

        onTextChanged: {
          if (toolbar.highlighted === homeBtn.text) {
            __projectsModel.searchExpression = searchField.text
          } else {
            __merginProjectsModel.searchExpression = searchField.text
          }
        }
      }

      Item {
        id: iconContainer
        height: searchField.height
        width: searchField.height
        anchors.right: parent.right
        anchors.rightMargin: projectsPanel.panelMargin

        Image {
          id: cancelSearchBtn
          source: searchField.text ? "no.svg" : "search.svg"
          sourceSize.width: width
          sourceSize.height: height
          width: searchField.height
          height: width
          fillMode: Image.PreserveAspectFit

          MouseArea {
            anchors.fill: parent
            onClicked: {
              if (searchField.text) {
                searchBar.deactivate()
              } else {
                // perform search
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
      y: searchField.height - height
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
      visible: showMergin
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
      width: cellWidth
      height: passesFilter ? cellHeight : 0
      visible: height ? true : false
      statusIconSource: "trash.svg"
      itemMargin: projectsPanel.panelMargin
      projectName: folderName
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

          onActivated: {toolbar.highlighted = homeBtn.text; showMergin = false}
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
            __merginApi.listProjects()
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
          imageSource: "account-multiple.svg"
          faded: toolbar.highlighted !== sharedProjectsBtn.text

          onActivated: {
            toolbar.highlighted = sharedProjectsBtn.text
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
          imageSource: "cloud-search.svg"
          faded: toolbar.highlighted !== exploreBtn.text

          onActivated: {
            toolbar.highlighted = exploreBtn.text
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
    onAuthFailed: myProjectsBtn.clicked()
  }

  AccountPage {
    id: accountPanel
    height: window.height
    width: parent.width
    visible: false
    //onAuthFailed: myProjectsBtn.clicked()
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
