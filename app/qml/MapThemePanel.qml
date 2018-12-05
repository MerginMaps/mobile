import QtQuick 2.7
import QtQuick.Controls 2.2

import lc 1.0
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton

Drawer {

    property int activeThemeIndex: 0

    id: mapThemePanel
    visible: false
    modal: false
    interactive: true
    dragMargin: 0 // prevents opening the drawer by dragging.

    background: Rectangle {
        color: InputStyle.clrPanelMain
    }
    Rectangle {
        id: header
        height: InputStyle.rowHeightHeader
        width: parent.width
        color: InputStyle.panelBackgroundLight

        Text {
            anchors.fill: parent
            anchors.leftMargin: InputStyle.panelMargin
            anchors.rightMargin: InputStyle.panelMargin
            text: "Map themes"
            color: InputStyle.fontColor
            font.pixelSize: InputStyle.fontPixelSizeTitle
            font.bold: true
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }

        layer.enabled: true
        layer.effect: Shadow {}
    }

    ListView {
        id: listView
        height: mapThemePanel.height - header.height
        width: parent.width
        y: header.height
        implicitWidth: parent.width
        implicitHeight: contentHeight
        model: __mapThemesModel
        delegate: delegateItem
        clip: true

        property int cellWidth: width
        property int cellHeight: InputStyle.rowHeight
        property int borderWidth: 1

        Label {
            anchors.fill: parent
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            visible: parent.count == 0
            text: qsTr("No themes in the project!")
            color: InputStyle.fontColor
            font.pixelSize: InputStyle.fontPixelSizeNormal
            font.bold: true
        }

    }

    Component {
        id: delegateItem
        Rectangle {
            id: itemContainer
            property color primaryColor: InputStyle.clrPanelMain
            property color secondaryColor: InputStyle.fontColorBright
            width: listView.cellWidth
            height: listView.cellHeight
            anchors.leftMargin: InputStyle.panelMargin
            anchors.rightMargin: InputStyle.panelMargin
            color: item.highlight ? secondaryColor : primaryColor

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    __mapThemesModel.applyTheme(name)
                    activeThemeIndex = index
                }
            }

            ExtendedMenuItem {
                id: item
                panelMargin: InputStyle.panelMargin
                contentText: name
                imageSource: "map_styles.svg"
                anchors.rightMargin: panelMargin
                anchors.leftMargin: panelMargin
                highlight: activeThemeIndex === index
                showBorder: activeThemeIndex - 1 !== index
            }
        }

    }

}
