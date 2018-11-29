import QtQuick 2.7
import QtQuick.Controls 2.2
import lc 1.0
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton

Drawer {

    property alias state: stateManager.state
    property int activeLayerIndex: 0
    property QgsQuick.VectorLayer activeVectorLayer: __layersModel.data(__layersModel.index(activeLayerIndex), LayersModel.VectorLayer)
    property string activeLayerName: __layersModel.data(__layersModel.index(activeLayerIndex), LayersModel.Name)

    property string title: "Survey Layer"

    signal layerSettingChanged()

    function openPanel(state) {
        activeLayerPanel.state = state
        if (state === "record") {
            // overwrite -> resets activeIndex according default
            //activeLayerIndex = __layersModel.defaultLayerIndex

            if (activeLayerIndex !== 0) {
                layerPanel.layerSettingChanged()
                // record without opening panel
                return;
            }
        }
        activeLayerPanel.visible = true
    }

    id: layerPanel
    visible: false
    modal: false
    interactive: true
    dragMargin: 0 // prevents opening the drawer by dragging.

    background: Rectangle {
        color: InputStyle.clrPanelMain
    }

    Item {
        id: stateManager
        states: [
            State {
                name: "setup"
            },
            State {
                name: "record"
            }
        ]
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
            text: stateManager.state === "setup"? qsTr("Default survey layer") : qsTr("Survey layer")
            color: InputStyle.fontColor
            font.pixelSize: InputStyle.fontPixelSizeTitle
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
            height: !isVector ? 0 : listView.cellHeight
            visible: height ? true : false
            anchors.leftMargin: InputStyle.panelMargin
            anchors.rightMargin: InputStyle.panelMargin

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    layerPanel.activeLayerIndex = index
                    layerPanel.visible = false
                    if (stateManager.state === "record") {
                        layerPanel.layerSettingChanged()
                    } else if (stateManager.state === "setup") {
                        __appSettings.defaultLayer = name
                    }
                }
            }

            ExtendedMenuItem {
                anchors.rightMargin: InputStyle.panelMargin
                anchors.leftMargin: InputStyle.panelMargin
                imageSource: iconSource ? iconSource : ""
                overlayImage: false
                contentText: name
                highlight: {
                    if (stateManager.state === "setup") {
                        __appSettings.defaultLayer === name
                    } else {
                        activeLayerIndex === index
                    }
                }
//                fontColor: highlight ? InputStyle.clrPanelMain : InputStyle.fontColor
//                panelColor: highlight ? InputStyle.fontColor : InputStyle.clrPanelMain
            }
        }

    }

}
