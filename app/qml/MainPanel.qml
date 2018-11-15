import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.1
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton

Item {
    signal openProjectClicked()
    signal openLayersClicked()
    signal myLocationClicked()
    signal myLocationHold()
    signal addFeatureClicked()
    signal openMapThemesClicked()
    signal openLogClicked()
    signal zoomToProject()
    property alias recordButton: recBtnIcon

    property string activeProjectName: "(none)"
    property string activeLayerName: "(none)"
    property string gpsStatus: "GPS \n (none)"
    property bool lockOnPosition: false

    property int itemSize: mainPanel.height * 0.8

    id: mainPanel

    Rectangle {
        anchors.fill: parent
        color: InputStyle.clrPanelBackground
        opacity: InputStyle.panelOpacity
    }

    Row {
        height: parent.height
        width: parent.width
        anchors.fill: parent

        Item {
            width: parent.width/parent.children.length
            height: parent.height

            MainPanelButton {

                id: openProjectBtn
                width: mainPanel.itemSize
                text: qsTr("Projects")
                imageSource: "ic_map_white_48px.svg"

                onActivated: mainPanel.openProjectClicked()
            }
        }

        Item {
            width: parent.width/parent.children.length
            height: parent.height
            MainPanelButton {
                id: myLocationBtn
                width: mainPanel.itemSize

                text: qsTr("GPS")
                imageSource: "ic_my_location_white_48px.svg"
                imageSource2: "baseline-gps_off-24px.svg"
                imageSourceCondition: mainPanel.lockOnPosition

                onActivated: mainPanel.myLocationClicked()
                onActivatedOnHold: mainPanel.myLocationHold()

                Item {
                    id: gpsSignal
                    width: parent.height/4
                    height: width
                    anchors.right: parent.right
                    anchors.top: parent.top
                    property int size: width

                    Rectangle {
                        anchors.centerIn: parent
                        width: gpsSignal.size
                        height: gpsSignal.size
                        color: "orange"
                        radius: width*0.5
                        antialiasing: true
                    }
                }
            }
        }

        Item {
            width: parent.width/parent.children.length
            height: parent.height
            MainPanelButton {
                id: recBtn
                width: mainPanel.itemSize
                text: qsTr("Record")

                RecordBtn {
                    id: recBtnIcon
                    width: mainPanel.itemSize
                    anchors.top: parent.top
                    anchors.margins: width/4
                    anchors.topMargin: -anchors.margins/2
                    enabled: true
                }

                onActivated: mainPanel.addFeatureClicked()
            }
        }

        Item {
            width: parent.width/parent.children.length
            height: parent.height
            MainPanelButton {
                id: menuBtn
                width: mainPanel.itemSize
                text: qsTr("More")
                imageSource: "ic_menu_48px.svg"
                onActivated: {
                    if (rootMenu.isOpen) {
                        rootMenu.close()
                    } else {
                        rootMenu.open()
                    }
                }
            }
        }
    }

    Menu {
        id: rootMenu
        title: "Menu"
        x:parent.width - rootMenu.width
        y: -rootMenu.height
        property bool isOpen: false
        width: 240 * QgsQuick.Utils.dp

        closePolicy: Popup.CloseOnPressOutside | Popup.CloseOnPressOutsideParent

        onClosed: isOpen = false
        onOpened: isOpen = true

        Button {
            height: InputStyle.rowHeight
            text: "Zoom to project"
            onClicked: {
                mainPanel.zoomToProject()
                rootMenu.close()
            }
        }

        Button {
            height: InputStyle.rowHeight
            text: "Map themes"
            onClicked: {
                mainPanel.openMapThemesClicked()
                rootMenu.close()
            }
        }
    }


}
