import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import lc 1.0
import "."  // import InputStyle singleton

Rectangle {
    id: itemContainer
    color: itemContainer.highlight ? itemContainer.secondaryColor : itemContainer.primaryColor

    property color primaryColor: InputStyle.clrPanelMain
    property color secondaryColor: InputStyle.fontColor
    property int cellWidth: width
    property int cellHeight: height
    property int borderWidth: 1
    property bool highlight: false
    property bool pending: false
    property string statusIconSource: "more_menu.svg"
    property string projectName: name
    property bool disabled: false
    property real itemMargin: itemContainer.cellHeight/4

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
            spacing: 0

            Item {
                id: iconContainer
                height: itemContainer.cellHeight
                width: itemContainer.cellHeight

                Image {
                    anchors.margins: itemContainer.itemMargin
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
                height: itemContainer.cellHeight - row.bottomMargin
                width: itemContainer.cellWidth - (itemContainer.cellHeight * 2)
                Text {
                    id: mainText
                    text: itemContainer.projectName
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
                height: itemContainer.cellHeight
                width: itemContainer.cellHeight
                y: 0

                MouseArea {
                    anchors.fill: parent
                    onClicked: {
                        if (!pending) menuClicked()
                    }
                }

                Image {
                    anchors.margins: (itemContainer.cellHeight * 0.35)
                    id: statusIcon
                    anchors.fill: parent
                    source: statusIconSource
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
            width: row.width
            height: itemContainer.borderWidth
            anchors.bottom: parent.bottom
        }
    }
}
