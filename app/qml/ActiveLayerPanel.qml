import QtQuick 2.7
import QtQuick.Controls 2.2
import lc 1.0
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton

Drawer {

    property QgsQuick.VectorLayer activeVectorLayer: (__layersModel.activeLayerIndex >= 0) ?
                                                         __layersModel.data(__layersModel.index(__layersModel.activeLayerIndex), LayersModel.VectorLayer) :
                                                         null
    property string title: "Survey Layer"

    function openPanel() {
        layerPanel.visible = true
    }

    id: layerPanel
    visible: false
    modal: true
    interactive: false
    dragMargin: 0 // prevents opening the drawer by dragging.

    signal updateRecordPanel

    background: Rectangle {
        color: InputStyle.clrPanelMain
    }

    PanelHeader {
      id: header
      height: InputStyle.rowHeightHeader
      width: parent.width
      color: InputStyle.panelBackgroundLight
      rowHeight: InputStyle.rowHeightHeader
      titleText: qsTr("Select Active Layer")
      backTextVisible: false
      onBack: layerPanel.close()
      withBackButton: true
      layer.enabled: true
      layer.effect: Shadow {}
    }

    ListView {
        id: listView
        height: layerPanel.height - header.height
        width: parent.width
        y: header.height
        implicitWidth: parent.width
        implicitHeight: contentHeight
        model: __layersModel
        clip: true
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
            height: isVector && !isReadOnly && hasGeometry ? listView.cellHeight : 0
            visible: height ? true : false
            anchors.leftMargin: InputStyle.panelMargin
            anchors.rightMargin: InputStyle.panelMargin
            color: item.highlight ? secondaryColor : primaryColor

            MouseArea {
              anchors.fill: parent
              onClicked: {
                __layersModel.activeIndex = index
                layerPanel.visible = false
                updateRecordPanel()
                // TODO emit singals
              }
            }

            ExtendedMenuItem {
                id: item
                anchors.rightMargin: InputStyle.panelMargin
                anchors.leftMargin: InputStyle.panelMargin
                contentText: name ? name : ""
                imageSource: iconSource ? iconSource : ""
                overlayImage: false
                highlight: __layersModel.activeIndex === index
                showBorder: !__appSettings.defaultLayer || __layersModel.activeIndex - 1 !== index
            }
        }

    }

}
