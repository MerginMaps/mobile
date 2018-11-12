import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "."  // import InputStyle singleton

Page {

    property int activeProjectIndex: -1
    property string activeProjectPath: __projectsModel.data(__projectsModel.index(activeProjectIndex), ProjectModel.Path)
    property string activeProjectName: __projectsModel.data(__projectsModel.index(activeProjectIndex), ProjectModel.Name)

    property real rowHeight: InputStyle.scale(InputStyle.buttonSize)

    Component.onCompleted: {
        // load model just after all components are prepared
        // otherwise GridView's delegate item is initialized invalidately
        grid.model = __projectsModel
    }

    id: projectsPanel
    visible: false
    contentWidth: projectsPanel.width

    background: Rectangle {
        color: InputStyle.clrPanelMain
    }

    header: Rectangle {
        height: projectsPanel.rowHeight
        width: parent.width
        color: InputStyle.clrPanelMain
        Text {
            anchors.fill: parent
            text: "Projects"
            color: InputStyle.fontColor
            font.pixelSize: InputStyle.fontPixelSizeNormal
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }
    }
    footer: Item {}

    ColumnLayout {
        id: contentLayout
        anchors.fill: parent
        spacing: 0

        TabBar {
            id: projectMenuButtons
            Layout.fillWidth: true
            spacing: 0
            implicitHeight: rowHeight

            background: Rectangle {
                color: InputStyle.panelBackground2
            }

            TabButton {
                height: projectMenuButtons.height
                id: button1
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0
                anchors.top: parent.top
                anchors.topMargin: 0
                background: Rectangle {
                    anchors.fill: parent
                    color: "transparent"
                }

                contentItem: Text {
                    anchors.fill: parent
                    text: qsTr("My projects")
                    color: InputStyle.fontColor
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.underline: button1.checked
                    font.bold: true
                }

            }
            TabButton {
                height: parent.height
                id: button2
                y:0
                background: Rectangle {
                    anchors.fill: parent
                    height: parent.height
                    color: "transparent"
                }
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 0
                anchors.top: parent.top
                anchors.topMargin: 0
                Text {
                    anchors.fill: parent
                    height: parent.height
                    text: qsTr("All projects")
                    color: InputStyle.fontColor
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    font.underline: button2.checked
                    font.bold: true
                }
            }
        }

        ListView {
            id: grid
            //model: __projectsModel
            Layout.fillWidth: true
            Layout.fillHeight: true
            contentWidth: grid.width

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
            width: grid.cellWidth
            height: grid.cellHeight
            color:index === activeProjectIndex ? itemContainer.secondaryColor : itemContainer.primaryColor

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    projectsPanel.activeProjectIndex = index
                    projectsPanel.visible = false
                }
            }

            Rectangle {
                width: parent.width
                height: parent.height
                color: 'transparent'

                RowLayout {
                    id: row
                    anchors.fill: parent
                    spacing: 0

                    Rectangle {
                        id: iconContainer
                        height: grid.cellHeight
                        width: grid.cellHeight
                        color: 'transparent'

                        Image {
                            anchors.margins: (grid.cellHeight/4)*1.25 // TODO @vsklencar
                            id: icon
                            anchors.fill: parent
                            source: 'file.svg'
                            fillMode: Image.PreserveAspectFit
                        }

                        ColorOverlay {
                            anchors.fill: icon
                            source: icon
                            color: index === activeProjectIndex ? itemContainer.primaryColor : itemContainer.secondaryColor
                        }

                    }

                    Rectangle {
                        id: textContainer
                        y: 0
                        height: grid.cellHeight - row.bottomMargin
                        width: grid.cellWidth - (grid.cellHeight * 2)
                        color: 'transparent'
                        Text {
                            id: mainText
                            text: name
                            height: textContainer.height/2
                            font.pointSize: 24
                            font.weight: Font.Bold
                            color: index === activeProjectIndex ? itemContainer.primaryColor : itemContainer.secondaryColor
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignBottom
                        }

                        Text {
                            height: textContainer.height/2
                            text: projectInfo
                            anchors.right: parent.right
                            anchors.bottom: parent.bottom
                            anchors.left: parent.left
                            anchors.top: mainText.bottom
                            font.pointSize: 12
                            color: index === activeProjectIndex ? itemContainer.primaryColor : "grey"
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignTop
                        }
                    }

                    Rectangle {
                        height: grid.cellHeight
                        width: grid.cellHeight
                        color: 'transparent'
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
                            font.pointSize: 24
                            font.weight: Font.Bold
                            color: index === activeProjectIndex ? itemContainer.primaryColor : itemContainer.secondaryColor
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
