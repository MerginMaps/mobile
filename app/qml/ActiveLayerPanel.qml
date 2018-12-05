import QtQuick 2.7
import QtQuick.Controls 2.2
import lc 1.0
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton

Drawer {

    property alias state: stateManager.state
    property int activeLayerIndex: -1
    property QgsQuick.VectorLayer activeVectorLayer: __layersModel.data(__layersModel.index(activeLayerIndex), LayersModel.VectorLayer)
    property string activeLayerName: __layersModel.data(__layersModel.index(activeLayerIndex), LayersModel.Name)
    property string title: "Survey Layer"

    signal layerSettingChanged()

    function openPanel(state) {
        layerPanel.state = state
        if (state === "record") {
            if (activeLayerIndex >= 0 ) {
                layerPanel.layerSettingChanged()
                // record without opening panel
                return;
            }
        }
        layerPanel.visible = true
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

        layer.enabled: true
        layer.effect: Shadow {}
    }

    Item {
        id: cancelDefaultItem
        width: parent.width
        anchors.top: header.bottom
        implicitHeight: __appSettings.defaultLayer && stateManager.state === "setup" ? InputStyle.rowHeight : 0
        visible: implicitHeight

        ExtendedMenuItem {
            contentText: "Unselect default survey layer"
            imageSource: "no.svg"
            panelMargin: 0
            anchors.leftMargin: InputStyle.panelMargin
            anchors.rightMargin: InputStyle.panelMargin
            showBorder: layerPanel.activeLayerIndex !== 0

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    __appSettings.defaultLayer = ""
                    activeLayerIndex = -1
                    layerPanel.close()
                }
            }
        }
    }

    ListView {
        id: listView
        height: layerPanel.height - header.height - cancelDefaultItem.implicitHeight
        width: parent.width
        y: header.height + cancelDefaultItem.implicitHeight
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
            property color secondaryColor: InputStyle.fontColorBright
            width: listView.cellWidth
            height: !isVector ? 0 : listView.cellHeight
            visible: height ? true : false
            anchors.leftMargin: InputStyle.panelMargin
            anchors.rightMargin: InputStyle.panelMargin
            color: item.highlight ? secondaryColor : primaryColor

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
                id: item
                anchors.rightMargin: InputStyle.panelMargin
                anchors.leftMargin: InputStyle.panelMargin
                contentText: name ? name : ""
                imageSource: iconSource ? iconSource : ""
                overlayImage: false
                highlight: {
                    if (stateManager.state === "setup") {
                        __appSettings.defaultLayer === name
                    } else {
                        activeLayerIndex === index
                    }
                }
                showBorder: !__appSettings.defaultLayer || layerPanel.activeLayerIndex - 1 !== index
            }
        }

    }

}
