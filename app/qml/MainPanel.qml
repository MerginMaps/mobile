import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.1
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton

Item {
    signal openProjectClicked()
    signal myLocationClicked()
    signal myLocationHold()
    signal addFeatureClicked()
    signal setDefaultLayerClicked()
    signal setDefaultProjectClicked()
    signal openMapThemesClicked()
    signal openLogClicked()
    signal zoomToProject()
    property alias recordButton: recBtnIcon

    property string activeProjectName: "(none)"
    property string activeLayerName: "(none)"
    property string gpsStatus: "GPS \n (none)"
    property int gpsAccuracyTolerance
    property real gpsAccuracy
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
                imageSource: "project.svg"

                onActivated: mainPanel.openProjectClicked()
                onActivatedOnHold: mainPanel.setDefaultProjectClicked()
            }
        }

        Item {
            width: parent.width/parent.children.length
            height: parent.height
            MainPanelButton {
                id: myLocationBtn
                width: mainPanel.itemSize

                text: qsTr("GPS")
                imageSource: "ic_gps_fixed_48px.svg"
                imageSource2: "ic_gps_not_fixed_48px.svg"
                imageSourceCondition: __appSettings.autoCenterMapChecked

                onActivated: mainPanel.myLocationClicked()
                onActivatedOnHold: mainPanel.myLocationHold()

                RoundIndicator {
                    width: parent.height/4
                    height: width
                    anchors.right: parent.right
                    anchors.top: parent.top
                    color: {
                        if (gpsAccuracy <= 0) return InputStyle.softRed
                        return gpsAccuracy < gpsAccuracyTolerance ? InputStyle.softGreen : InputStyle.softOrange
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
                onActivatedOnHold: mainPanel.setDefaultLayerClicked()
            }
        }

        Item {
            width: parent.width/parent.children.length
            height: parent.height
            MainPanelButton {
                id: menuBtn
                width: mainPanel.itemSize
                text: qsTr("More")
                imageSource: "more_menu.svg"
                //isHighlighted: rootMenu.isOpen
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
        width: parent.width < 300 * QgsQuick.Utils.dp ? parent.width : 300 * QgsQuick.Utils.dp
        closePolicy: Popup.CloseOnPressOutside | Popup.CloseOnPressOutsideParent

        onClosed: isOpen = false
        onOpened: isOpen = true

        MenuItem {
            height: mainPanel.itemSize
            width: parent.width

            ExtendedMenuItem {
                height: mainPanel.itemSize
                rowHeight: height
                width: parent.width
                contentText: qsTr("Zoom to project")
                imageSource: "zoom_to_project.svg"
            }

            onClicked: {
                mainPanel.zoomToProject()
                rootMenu.close()
            }
        }


        MenuItem {
            height: mainPanel.itemSize
            width: parent.width

            ExtendedMenuItem {
                height: mainPanel.itemSize
                rowHeight: height
                width: parent.width
                contentText: qsTr("Map themes")
                imageSource: "map_styles.svg"
            }

            onClicked: {
                mainPanel.openMapThemesClicked()
                rootMenu.close()
            }
        }

        MenuItem {
            height: mainPanel.itemSize
            width: parent.width

            ExtendedMenuItem {
                anchors.fill: parent
                rowHeight: parent.height
                width: parent.width
                contentText: qsTr("Settings")
                imageSource: "settings.svg"
            }

            onClicked: {
                mainPanel.openLogClicked()
                rootMenu.close()
            }
        }
    }


}
