import QtQuick 2.7
import QtQuick.Controls 2.2

import yd 1.0
import QgsQuick 0.1 as QgsQuick
import "."  // import YdnpaStyle singleton

Drawer {

    property int activeLayerIndex: 0
    property QgsQuick.VectorLayer activeVectorLayer: __layersModel.data(__layersModel.index(activeLayerIndex), LayersModel.VectorLayer)
    property string activeLayerName: __layersModel.data(__layersModel.index(activeLayerIndex), LayersModel.Name)

    id: activeLayerPanel
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
            text: "Active Layer"
            color: YdnpaStyle.clrPanelMain
            font.pixelSize: YdnpaStyle.fontPixelSizeBig
            font.bold: true
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }

        ListView {
            implicitWidth: parent.width
            implicitHeight: contentHeight
            model: __layersModel
            delegate: ItemDelegate {
                id: control
                text: name
                visible: isVector && !isReadOnly // show only vector and editable layers

                contentItem: Text {
                    color: index === activeLayerIndex ? YdnpaStyle.clrPanelHighlight : YdnpaStyle.clrPanelMain
                    rightPadding: control.spacing
                    text: control.text
                    font.pixelSize: YdnpaStyle.fontPixelSizeNormal
                    elide: Text.ElideRight
                    visible: control.text
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: {
                    console.log("active layer:", name)
                    activeLayerIndex = index
                    activeLayerPanel.visible = false
                }
            }
        }
    }

}
