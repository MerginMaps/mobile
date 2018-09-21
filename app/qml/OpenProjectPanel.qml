import QtQuick 2.7
import QtQuick.Controls 2.2
import QgsQuick 0.1 as QgsQuick
import yd 1.0
import "."  // import YdnpaStyle singleton

Drawer {

    property int activeProjectIndex: -1
    property string activeProjectPath: __projectsModel.data(__projectsModel.index(activeProjectIndex), ProjectModel.Path)
    property string activeProjectName: __projectsModel.data(__projectsModel.index(activeProjectIndex), ProjectModel.Name)

    id: projectsPanel
    visible: false
    modal: true
    interactive: true
    dragMargin: 0 // prevents opening the drawer by dragging.

    background: Rectangle {
        color: YdnpaStyle.clrPanelBackground
        opacity: YdnpaStyle.panelOpacity
    }

    Column {
        spacing: YdnpaStyle.panelSpacing

        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: YdnpaStyle.panelSpacing * 2

        Text {
            text: "Project"
            color: YdnpaStyle.clrPanelMain
            font.pixelSize: YdnpaStyle.fontPixelSizeBig
            font.bold: true
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }

        ListView {
            implicitWidth: parent.width
            implicitHeight: contentHeight
            model: __projectsModel
            delegate: ItemDelegate {
                id: control
                text: name

                contentItem: Text {
                    color: index === activeProjectIndex ? YdnpaStyle.clrPanelHighlight : YdnpaStyle.clrPanelMain
                    rightPadding: control.spacing
                    text: control.text
                    font.pixelSize: YdnpaStyle.fontPixelSizeNormal
                    elide: Text.ElideRight
                    visible: control.text
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: {
                    console.log("active project:", name)
                    projectsPanel.activeProjectIndex = index
                    projectsPanel.visible = false
                }
            }
        }
    }

}
