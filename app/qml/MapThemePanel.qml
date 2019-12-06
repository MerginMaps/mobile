import QtQuick 2.7
import QtQuick.Controls 2.2

import lc 1.0
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton

Drawer {

    property int activeThemeIndex: 0
    property string activeMapThemeName: __mapThemesModel.data(__mapThemesModel.index(activeThemeIndex), MapThemesModel.Name)

    id: mapThemePanel
    visible: false
    modal: true
    interactive: false
    dragMargin: 0 // prevents opening the drawer by dragging.

    background: Rectangle {
        color: InputStyle.clrPanelMain
    }
    PanelHeader {
          id: header
          height: InputStyle.rowHeightHeader
          width: parent.width
          color: InputStyle.panelBackgroundLight
          rowHeight: InputStyle.rowHeightHeader
          titleText: qsTr("Map Themes")
          backTextVisible: false
          onBack: mapThemePanel.close()
          withBackButton: true
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
                   activeThemeIndex = index
                   activeThemeIndexChanged()
                   mapThemePanel.close()
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
