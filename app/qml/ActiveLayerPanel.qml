import QtQuick 2.7
import QtQuick.Controls 2.2
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
            font.pixelSize: InputStyle.fontPixelSizeSmall
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

        Label {
            anchors.fill: parent
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            visible: parent.count == 0
            text: qsTr("No editable layers in the project!")
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
            property color secondaryColor: InputStyle.fontColor
            width: listView.cellWidth
            // first item in the model is "none" layer
            height: index === 0 || !isVector ? 0 : listView.cellHeight
            visible: height ? true : false
            anchors.leftMargin: InputStyle.panelMargin
            anchors.rightMargin: InputStyle.panelMargin

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    layerPanel.activeLayerIndex = index
                    layerPanel.visible = false
                    layerPanel.layerSettingChanged()
                }
            }

            Item {
                anchors.fill: parent
                anchors.rightMargin: InputStyle.panelMargin
                anchors.leftMargin: InputStyle.panelMargin

                Item {
                    id: row
                    anchors.fill: parent

                    Rectangle {
                        id: iconContainer
                        height: InputStyle.rowHeight
                        width: InputStyle.rowHeight

                        Image {
                            id: icon
                            anchors.fill: parent
                            anchors.margins: InputStyle.rowHeight/4
                            source: iconSource ? iconSource : ""
                            sourceSize.width: width
                            sourceSize.height: height
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

                            font.pixelSize: InputStyle.fontPixelSizeSmall
                            font.weight: Font.Bold
                            color: itemContainer.secondaryColor
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
