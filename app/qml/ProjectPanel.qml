import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "."  // import InputStyle singleton

Popup {

    property int activeProjectIndex: -1
    property string activeProjectPath: __projectsModel.data(__projectsModel.index(activeProjectIndex), ProjectModel.Path)
    property string activeProjectName: __projectsModel.data(__projectsModel.index(activeProjectIndex), ProjectModel.Name)
    property var busyIndicator

    property real rowHeight: InputStyle.rowHeightHeader * 1.2
    property bool showMergin: false

    function openPanel(state) {
        stateManager.state = state
        projectsPanel.visible = true
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

    id: projectsPanel
    visible: false
    contentWidth: projectsPanel.width
    margins: 0
    padding: 0
    closePolicy: activeProjectName ? Popup.CloseOnEscape : Popup.NoAutoClose

    background: Rectangle {
        color: InputStyle.clrPanelMain
    }

    // TODO make one global indicator if somewhere else will be needed
    BusyIndicator {
      id: busyIndicator
      width: parent.width/8
      height: width
      running: false
      visible: running
      anchors.centerIn: parent
    }

    Item {
        id: stateManager
        states: [
            State {
                name: "setup"
            },
            State {
                name: "view"
            }
        ]
    }

    PanelHeader {
        id: header
        height: InputStyle.rowHeightHeader
        width: parent.width
        color: InputStyle.clrPanelMain
        rowHeight: InputStyle.rowHeightHeader
        titleText: stateManager.state === "setup"? qsTr("Default project") : qsTr("Projects")

        onBack: projectsPanel.close()
        withBackButton: projectsPanel.activeProjectPath
    }

    ColumnLayout {
        id: contentLayout
        height: projectsPanel.height-header.height
        width: parent.width
        y: header.height
        spacing: 0

        TabBar {
            id: projectMenuButtons
            Layout.fillWidth: true
            spacing: 0
            implicitHeight: InputStyle.rowHeightHeader
            z: grid.z + 1

            background: Rectangle {
                color: InputStyle.panelBackgroundLight
            }

            PanelTabButton {
                height: projectMenuButtons.height
                text: qsTr("MY PROJECTS")
                horizontalAlignment: Text.AlignLeft

                onClicked: showMergin = false
            }

            PanelTabButton {
                height: projectMenuButtons.height
                text: qsTr("ALL PROJECTS")
                horizontalAlignment: Text.AlignRight

                onClicked: {
                    busyIndicator.running = true
                    showMergin = true
                    __merginApi.listProjects()
                }
            }
        }

        // TODO: must be wrapped in item due to ColumnLayout
        Item {
            width: parent.width
            implicitHeight: __appSettings.defaultProject && stateManager.state === "setup" ? InputStyle.rowHeight : 0
            visible: implicitHeight

            ExtendedMenuItem {
                contentText: "Unselect default project"
                imageSource: "no.svg"
                panelMargin: 0

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        __appSettings.defaultProject = ""
                        projectsPanel.close()
                    }
                }
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

            delegate: delegateItem
        }
    }

    Component {
        id: delegateItem

        Rectangle {
            id: itemContainer
            property color primaryColor: InputStyle.clrPanelMain
            property color secondaryColor: InputStyle.fontColor
            property bool highlight: {
                if (stateManager.state === "setup") {
                    return path === __appSettings.defaultProject ? true : false
                } else {
                    return index === projectsPanel.activeProjectIndex ? true : false
                }
            }

            width: grid.cellWidth
            height: grid.cellHeight
            color: itemContainer.highlight ? itemContainer.secondaryColor : itemContainer.primaryColor

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (showMergin) return

                    if (stateManager.state === "setup") {
                        __appSettings.defaultProject = path ? path : ""
                    }

                    else if (stateManager.state === "view") {
                        projectsPanel.activeProjectIndex = index
                    }

                    projectsPanel.visible = false
                }
            }

            Item {
                width: parent.width
                height: parent.height

                RowLayout {
                    id: row
                    anchors.fill: parent
                    spacing: 0

                    Item {
                        id: iconContainer
                        height: grid.cellHeight
                        width: grid.cellHeight

                        Image {
                            anchors.margins: (grid.cellHeight/4)
                            id: icon
                            anchors.fill: parent
                            source: 'project.svg'
                            sourceSize.width: width
                            sourceSize.height: height
                            fillMode: Image.PreserveAspectFit
                        }

                        ColorOverlay {
                            anchors.fill: icon
                            source: icon
                            color: itemContainer.highlight ? itemContainer.primaryColor : itemContainer.secondaryColor
                        }

                    }

                    Item {
                        id: textContainer
                        y: 0
                        height: grid.cellHeight - row.bottomMargin
                        width: grid.cellWidth - (grid.cellHeight * 2)
                        Text {
                            id: mainText
                            text: name
                            height: textContainer.height/2
                            font.pixelSize: InputStyle.fontPixelSizeNormal
                            font.weight: Font.Bold
                            color: itemContainer.highlight? itemContainer.primaryColor : itemContainer.secondaryColor
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignBottom
                        }

                        Text {
                            height: textContainer.height/2
                            text: projectInfo ? projectInfo : ""
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            anchors.left: parent.left
                            anchors.top: mainText.bottom
                            font.pixelSize: InputStyle.fontPixelSizeSmall
                            color: itemContainer.highlight ? itemContainer.primaryColor : InputStyle.panelBackgroundDark
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignTop
                        }
                    }

                    Item {
                        height: grid.cellHeight
                        width: grid.cellHeight
                        y: 0

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                projectsPanel.activeProjectIndex = index
                                projectsPanel.visible = false
                            }
                        }

                        Text {
                            anchors.fill: parent
                            text: "..."
                            height: textContainer.height/2
                            font.pixelSize: InputStyle.fontPixelSizeSmall
                            font.weight: Font.Bold
                            color: itemContainer.highlight ? itemContainer.primaryColor : itemContainer.secondaryColor
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }

                Rectangle {
                    id: borderLine
                    color: InputStyle.panelBackground2
                    width: row.width
                    height: grid.borderWidth
                    anchors.bottom: parent.bottom
                }
            }
        }
    }
}
