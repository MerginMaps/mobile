import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import lc 1.0
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton

Drawer {

    property int activeLayerIndex: 0
    property QgsQuick.VectorLayer activeVectorLayer: __layersModel.data(__layersModel.index(activeLayerIndex), LayersModel.VectorLayer)
    property string activeLayerName: __layersModel.data(__layersModel.index(activeLayerIndex), LayersModel.Name)


    signal layerSettingChanged()

    id: layerPanel
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
        color: InputStyle.clrPanelBackground2


        Text {
            anchors.fill: parent
            anchors.leftMargin: InputStyle.panelMargin
            anchors.rightMargin: InputStyle.panelMargin
            text: "Survey layer"
            color: InputStyle.fontColor
            font.pixelSize: InputStyle.fontPixelSizeBig
            font.bold: true
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }
    }

    ListView {
        id: listView
        height: layerPanel.height - header.height
        width: parent.width
        y: header.height
        implicitWidth: parent.width
        implicitHeight: contentHeight
        model: __layersModel
        delegate: delegateItem

        property int cellWidth: width
        property int cellHeight: InputStyle.rowHeight
        property int borderWidth: 1

    }

    Component {
        id: delegateItem
        Rectangle {
            id: itemContainer
            property color primaryColor: InputStyle.clrPanelMain
            property color secondaryColor: InputStyle.fontColor
            width: listView.cellWidth
            height: name === "(none)" ? 0 : listView.cellHeight
            visible: height ? true : false
            anchors.leftMargin: InputStyle.panelMargin
            anchors.rightMargin: InputStyle.panelMargin

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    layerPanel.activeLayerIndex = index
                    layerPanel.visible = false
                }
            }

            Item {
                anchors.fill: parent
                anchors.rightMargin: InputStyle.panelMargin
                anchors.leftMargin: InputStyle.panelMargin

                RowLayout {
                    id: row
                    anchors.fill: parent
                    spacing: 20

                    Rectangle {
                        id: iconContainer
                        height: InputStyle.rowHeight
                        width: InputStyle.rowHeight

                        Image {
                            id: icon
                            anchors.fill: parent
                            source: iconSource ? iconSource : ""
                            sourceSize.width: 48
                            sourceSize.height: 48
                            fillMode: Image.PreserveAspectFit
                        }

                    }

                    Item {
                        id: textContainer
                        y: 0
                        x: iconContainer.width + InputStyle.panelMargin
                        height: listView.cellHeight
                        width: listView.cellWidth - listView.cellHeight // minus icon

                        Text {
                            id: mainText
                            text: name
                            height: parent.height
                            width: parent.width

                            font.pointSize: InputStyle.scaleFontPointSize(InputStyle.fontPointSizeBig)
                            font.weight: Font.Bold
                            color: index === activeLayerIndex ? itemContainer.primaryColor : itemContainer.secondaryColor
                            horizontalAlignment: Text.AlignLeft
                            verticalAlignment: Text.AlignVCenter
                        }
                    }
                }

                Rectangle {
                    id: borderLine
                    color: InputStyle.fontColor
                    width: row.width
                    height: listView.borderWidth
                    anchors.bottom: parent.bottom
                }
            }
        }

    }

}
