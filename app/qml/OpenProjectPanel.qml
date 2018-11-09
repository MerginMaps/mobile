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
        header.height = projectsPanel.rowHeight
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

    //contentChildren: [

        ColumnLayout {
            id: contentLayout
            anchors.fill: parent
            spacing: 0


            Rectangle {
                id: projectMenu
                color: InputStyle.panelBackground2
                Layout.fillWidth: true
                height: 100 //projectsPanel.rowHeight
                width: projectsPanel.width

                Component.onCompleted: {
                    console.log("tralala!!!", projectsPanel.rowHeight, projectMenu.height)
                    projectMenu.height = projectsPanel.rowHeight
                }

                ButtonGroup {
                    buttons: projectMenuButtons.children
                }

                RowLayout {
                    id: projectMenuButtons
                    anchors.fill: parent
                    spacing: 0

                    Button {
                        Layout.fillHeight: true
                        Layout.fillWidth: true
                        id: myProjectsButton
                        Text {
                            anchors.fill: parent
                            text: "My projects"
                            color: InputStyle.fontColor
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            font.underline: true
                            font.bold: true
                        }
                    }

                    Button {
                        id: allProjectsButton
                        Layout.fillHeight: true
                        Layout.fillWidth: true

                        Text {
                            anchors.fill: parent
                            text: "All projects"
                            color: InputStyle.fontColor
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            font.bold: true
                        }
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

   // ]

    Component {
        id: delegateItem
        Rectangle {
            width: grid.cellWidth
            height: grid.cellHeight

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    projectsPanel.activeProjectIndex = index
                    projectsPanel.visible = false
                }
            }

            Column {
                anchors.fill: parent



                RowLayout {
                    id: row
                    anchors.fill: parent
                    spacing: 0

                    Rectangle {
                        id: iconContainer
                        height: grid.cellHeight
                        width: grid.cellHeight

                        Image {
                            anchors.margins: 20 // TODO @vsklencar
                            id: icon
                            anchors.fill: parent
                            source: 'file.svg'
                            fillMode: Image.PreserveAspectFit
                        }

                        ColorOverlay {
                            anchors.fill: icon
                            source: icon
                            color: InputStyle.fontColor
                        }

                    }

                    Rectangle {
                        id: textContainer
                        y: 0
                        height: grid.cellHeight - row.bottomMargin
                        width: grid.cellWidth - (grid.cellHeight * 2)
                        Text {
                            id: mainText
                            text: name
                            height: textContainer.height/2
                            leftPadding: 20
                            font.pointSize: 24
                            font.weight: Font.Bold
                            color: InputStyle.fontColor
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
                            leftPadding: 20
                            font.pointSize: 12
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignTop
                        }
                    }

                    Rectangle {
                        height: grid.cellHeight
                        width: grid.cellHeight
                        y: 0

                        Button {
                            id: icon2
                            anchors.fill: parent
                            anchors.margins: 20
                            text: "..."

                            background: Rectangle {
                                anchors.fill: parent
                                color: InputStyle.clrPanelMain
                            }
                        }
                    }
                }

                Rectangle {
                    id: borderLine
                    color: "grey"
                    width: row.width
                    height: grid.borderWidth
                    anchors.bottom: parent.bottom
                }

            }


        }

    }

}
