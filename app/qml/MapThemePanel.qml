import QtQuick 2.7
import QtQuick.Controls 2.2

import lc 1.0
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton

Drawer {

    property int activeThemeIndex: 0

    id: mapThemePanel
    visible: false
    modal: true
    interactive: true
    dragMargin: 0 // prevents opening the drawer by dragging.

    background: Rectangle {
        color: InputStyle.clrPanelBackground
        opacity: InputStyle.panelOpacity
    }

    Column {
        spacing: InputStyle.panelSpacing
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: InputStyle.panelSpacing * 2

        Text {
            text: "Map Theme"
            color: InputStyle.clrPanelMain
            font.pixelSize: InputStyle.fontPixelSizeBig
            font.bold: true
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }

        ListView {
            implicitWidth: parent.width
            implicitHeight: contentHeight
            model: __mapThemesModel
            delegate: ItemDelegate {
                id: control
                text: name

                contentItem: Text {
                    color: index === activeThemeIndex ? InputStyle.clrPanelHighlight : InputStyle.clrPanelMain
                    rightPadding: control.spacing
                    text: control.text
                    font.pixelSize: InputStyle.fontPixelSizeNormal
                    elide: Text.ElideRight
                    visible: control.text
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: {
                    __mapThemesModel.applyTheme(name)
                    activeThemeIndex = index
                }
            }
        }
    }

}
