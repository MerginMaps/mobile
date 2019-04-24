import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import lc 1.0
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton

Rectangle {
    id: itemContainer
    color: itemContainer.highlight ? itemContainer.secondaryColor : itemContainer.primaryColor

    property color primaryColor: InputStyle.clrPanelMain
    property color secondaryColor: InputStyle.fontColor
    property int cellWidth: width
    property int cellHeight: height
    property real iconSize: height/2
    property real borderWidth: 1 * QgsQuick.Utils.dp
    property bool highlight: false
    property bool pending: false
    property string statusIconSource: "more_menu.svg"
    property string projectName: name
    property bool disabled: false
    property real itemMargin: InputStyle.panelMargin

    signal itemClicked();
    signal menuClicked()

    MouseArea {
        anchors.fill: parent
        onClicked: if (!disabled) itemClicked()
    }

    Rectangle {
        id: backgroundRect
        visible: disabled
        width: parent.width
        height: parent.height
        color: InputStyle.panelBackgroundDark
    }

    Item {
        width: parent.width
        height: parent.height

        RowLayout {
            id: row
            anchors.fill: parent
            anchors.rightMargin: itemContainer.itemMargin
            anchors.leftMargin: itemContainer.itemMargin
            spacing: InputStyle.panelMargin

            Item {
                id: iconContainer
                height: itemContainer.cellHeight
                width: itemContainer.iconSize

                Image {
                    id: icon
                    anchors.centerIn: parent
                    source: 'project.svg'
                    width: itemContainer.iconSize
                    height: width
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
                height: itemContainer.cellHeight
                width: row.width - iconContainer.width - statusContainer.width - (2* row.spacing)

                Text {
                    id: mainText
                    text: itemContainer.projectName
                    height: textContainer.height/2
                    width: textContainer.width
                    font.pixelSize: InputStyle.fontPixelSizeNormal
                    font.weight: Font.Bold
                    color: itemContainer.highlight? itemContainer.primaryColor : itemContainer.secondaryColor
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignBottom
                    elide: Text.ElideRight
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
                id: statusContainer
                height: itemContainer.cellHeight
                width: itemContainer.iconSize
                y: 0

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (!pending) menuClicked()
                    }
                }

                Image {
                    id: statusIcon
                    anchors.centerIn: parent
                    source: statusIconSource
                    height: itemContainer.iconSize
                    width: height
                    sourceSize.width: width
                    sourceSize.height: height
                    fillMode: Image.PreserveAspectFit
                    visible: !pending
                }

                ColorOverlay {
                    anchors.fill: statusIcon
                    source: statusIcon
                    color: itemContainer.highlight ? itemContainer.primaryColor : itemContainer.secondaryColor
                    visible: !pending
                }

                BusyIndicator {
                    id: busyIndicator
                    implicitHeight: itemContainer.cellHeight/2
                    implicitWidth: implicitHeight
                    running: pending
                    anchors.centerIn: parent
                }
            }
        }

        Rectangle {
            id: borderLine
            color: InputStyle.panelBackground2
            width: itemContainer.width
            height: itemContainer.borderWidth
            anchors.bottom: parent.bottom
        }
    }
}
