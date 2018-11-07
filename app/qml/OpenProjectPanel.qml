import QtQuick 2.7
import QtQuick.Controls 2.2
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

    id: projectsPanel
    visible: false
    contentWidth: parent.width
    contentHeight: parent.height - header.height

    background: Rectangle {
        color: InputStyle.clrPanelMain
        opacity: InputStyle.panelOpacity
    }

//    header: Rectangle {
//        height: 48 // TODO bug???
//        width: parent.width
//        color: InputStyle.clrPanelMain
//        Text {
//            anchors.fill: parent
//            text: "Projects"
//            color: InputStyle.fontColor
//            font.pixelSize: InputStyle.fontPixelSizeNormal
//            verticalAlignment: Text.AlignVCenter
//            horizontalAlignment: Text.AlignHCenter
//        }
//    }
    footer: Item {}

    contentChildren: [

        ColumnLayout {
            id: contentLayout
            anchors.fill: parent
            Layout.fillWidth: true

            Rectangle {
                id: projectMenu
                color: InputStyle.panelBackground2
                Layout.fillWidth: true
                height: projectsPanel.rowHeight

                RowLayout {
                    anchors.fill: parent
                    Text {
                        text: "My projects"
                    }

                    Text {
                        text: "My projects"
                    }
                }
            }

            ListView {
                id: listView
                model: __projectsModel
                height: projectsPanel.height - projectsPanel.rowHeight
                width: projectsPanel.width
                delegate: ItemDelegate {
                    id: itemDelegate
                    //text: name
                    width: listView.width
                    height: projectsPanel.rowHeight

                    RowLayout {
                        id: row
                        anchors.fill: parent
                        //height: projectsPanel.rowHeight


                        Image {
                            id: icon
                            height: row.height
                            width: height
                            source: 'file.svg'
                            fillMode: Image.PreserveAspectCrop
                        }

                        ColorOverlay {
                            anchors.fill: icon
                            source: icon
                            color: InputStyle.fontColor
                        }

                        Text {
                            anchors.left: icon.right
                            height: row.height
                            text: name
                        }
                    }



                    onClicked: {
                        console.log("active project:", name, projectMenu.height, projectsPanel.rowHeight)
                        //projectsPanel.activeProjectIndex = index
                       // projectsPanel.visible = false
                    }
                }
            }
        }

    ]

}
