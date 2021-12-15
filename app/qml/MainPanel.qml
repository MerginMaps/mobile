/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.1
import QtQuick.Layouts 1.3

import "."  // import InputStyle singleton
import "./components"

Item {
    signal openProjectClicked()
    signal myLocationClicked()
    signal myLocationHold()
    signal recordClicked()
    signal openMapThemesClicked()
    signal openBrowseDataClicked()
    signal openSettingsClicked()
    signal zoomToProject()

    property real itemSize: mainPanel.height * 0.8
    property color gpsIndicatorColor: InputStyle.softRed

    id: mainPanel
    focus: true

    Rectangle {
        anchors.fill: parent
        color: InputStyle.clrPanelBackground
        opacity: InputStyle.panelOpacity
    }

    Row {
        id: panelRow
        anchors.fill: parent
        height: mainPanel.itemSize
        property real itemWidth: mainPanel.height * 1.2
        property int minItemNumber: 4
        property int itemsToShow: {
          var possibleItems = Math.min((width / panelRow.itemWidth), children.length) - 1
          return minItemNumber >= possibleItems ? minItemNumber : possibleItems
        }
        property real calculatedItemWidth: itemsToShow ? parent.width/itemsToShow : parent.width

        Item {
            id: openProjectsItem
            height: parent.height
            visible: panelRow.itemsToShow > 1
            width: visible ? panelRow.calculatedItemWidth : 0

            MainPanelButton {
                id: openProjectBtn
                width: mainPanel.itemSize
                text: qsTr("Projects")
                imageSource: InputStyle.projectIcon

                onActivated: {
                  rootMenu.close()
                  mainPanel.openProjectClicked()
                }
            }
        }

        Item {
            id: myLocationItem
            height: parent.height
            visible: panelRow.itemsToShow > 2
            width: visible ? panelRow.calculatedItemWidth : 0

            MainPanelButton {
                id: myLocationBtn
                width: mainPanel.itemSize

                text: qsTr("GPS")
                imageSource: InputStyle.gpsFixedIcon
                imageSource2: InputStyle.gpsNotFixedIcon
                imageSourceCondition: __appSettings.autoCenterMapChecked

                onActivated: {
                  rootMenu.close()
                  mainPanel.myLocationClicked()
                }

                onActivatedOnHold: {
                  rootMenu.close()
                  mainPanel.myLocationHold()
                }

                RoundIndicator {
                    width: parent.height/4
                    height: width
                    anchors.right: parent.right
                    anchors.top: parent.top
                    color: gpsIndicatorColor
                }
            }
        }

        Item {
            id: recItem
            height: parent.height
            visible: panelRow.itemsToShow > 3
            width: visible ? panelRow.calculatedItemWidth : 0

            MainPanelButton {
                id: recBtn

                width: mainPanel.itemSize
                text: qsTr("Record")
                imageSource: InputStyle.recordIcon

                onActivated: {
                  rootMenu.close()
                  mainPanel.recordClicked()
                }
            }
        }

        Item {
            id: zoomToProjectItem
            height: parent.height
            visible: panelRow.itemsToShow > 4
            width: visible ? panelRow.calculatedItemWidth : 0

            MainPanelButton {

                id: zoomToProjectBtn
                width: mainPanel.itemSize
                text: qsTr("Zoom to project")
                imageSource: InputStyle.zoomToProjectIcon

                onActivated: {
                  rootMenu.close()
                  mainPanel.zoomToProject()
                }
            }
        }

        Item {
            id: browseDataItem
            height: parent.height
            visible: panelRow.itemsToShow > 5
            width: visible ? panelRow.calculatedItemWidth : 0

            MainPanelButton {
                id: browseDataBtn
                width: mainPanel.itemSize
                text: qsTr("Browse data")
                imageSource: InputStyle.tableIcon

                onActivated: {
                  rootMenu.close()
                  mainPanel.openBrowseDataClicked()
                }
            }
        }

        Item {
            id: mapThemesItem
            height: parent.height
            visible: panelRow.itemsToShow > 6
            width: visible ? panelRow.calculatedItemWidth : 0

            MainPanelButton {

                id: mapThemesBtn
                width: mainPanel.itemSize
                text: qsTr("Map themes")
                imageSource: InputStyle.mapThemesIcon
                onActivated: {
                  rootMenu.close()
                  mainPanel.openMapThemesClicked()
                }
            }
        }

        // Last item
        Item {
            id: settingsItem
            height: parent.height
            visible: panelRow.itemsToShow > 6
            width: visible ? panelRow.calculatedItemWidth : 0

            MainPanelButton {

                id: settingsBtn
                width: mainPanel.itemSize
                text: qsTr("Settings")
                imageSource: InputStyle.settingsIcon
                onActivated: {
                  rootMenu.close()
                  mainPanel.openSettingsClicked()
                }
            }
        }

        Item {
            width: panelRow.calculatedItemWidth
            height: parent.height
            visible: !settingsItem.visible

            MainPanelButton {
              id: menuBtn
              width: mainPanel.itemSize
              text: qsTr("More")
              imageSource: InputStyle.moreMenuIcon

              onActivated: {
                if ( !rootMenu.visible ) rootMenu.open()
                else rootMenu.close()
              }
            }
        }
    }

    Menu {
        id: rootMenu
        title: qsTr("Menu")
        x:parent.width - rootMenu.width
        y: -rootMenu.height
        width: parent.width < 300 * __dp ? parent.width : 300 * __dp
        closePolicy: Popup.CloseOnReleaseOutsideParent | Popup.CloseOnEscape

        MenuItem {
            width: parent.width
            visible: !openProjectsItem.visible
            height: visible ? mainPanel.itemSize : 0

            ExtendedMenuItem {
                height: mainPanel.itemSize
                rowHeight: height
                width: parent.width
                contentText: qsTr("Projects")
                imageSource: InputStyle.projectIcon
            }

            onClicked: {
                openProjectBtn.activated()
                rootMenu.close()
            }
        }

        MenuItem {
            width: parent.width
            visible: !myLocationItem.visible
            height: visible ? mainPanel.itemSize : 0

            ExtendedMenuItem {
                height: mainPanel.itemSize
                rowHeight: height
                width: parent.width
                contentText: qsTr("GPS")
                imageSource: __appSettings.autoCenterMapChecked ? InputStyle.gpsFixedIcon : InputStyle.gpsNotFixedIcon

                RoundIndicator {
                    width: parent.height/4
                    height: width
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.margins : parent.height/8
                    color: gpsIndicatorColor
                }
            }

            onClicked: {
                myLocationBtn.activated()
                rootMenu.close()
            }
            onPressAndHold: mainPanel.myLocationHold()
        }

        MenuItem {
            width: parent.width
            visible: !recItem.visible
            height: visible ? mainPanel.itemSize : 0

            ExtendedMenuItem {
                height: mainPanel.itemSize
                rowHeight: height
                width: parent.width
                contentText: qsTr("Record")
                imageSource: InputStyle.recordIcon
            }

            onClicked: {
                recBtn.activated()
                rootMenu.close()
            }
        }

        MenuItem {
            width: parent.width
            visible: !zoomToProjectItem.visible
            height: visible ? mainPanel.itemSize : 0

            ExtendedMenuItem {
                height: mainPanel.itemSize
                rowHeight: height
                width: parent.width
                contentText: qsTr("Zoom to project")
                imageSource: InputStyle.zoomToProjectIcon
            }

            onClicked: {
                zoomToProjectBtn.activated()
                rootMenu.close()
            }
        }

        MenuItem {
            width: parent.width
            visible: !mapThemesItem.visible
            height: visible ? mainPanel.itemSize : 0

            ExtendedMenuItem {
                height: mainPanel.itemSize
                rowHeight: height
                width: parent.width
                contentText: qsTr("Browse features")
                imageSource: InputStyle.tableIcon
            }

            onClicked: {
                browseDataBtn.activated()
                rootMenu.close()
            }
        }

        MenuItem {
            width: parent.width
            visible: !mapThemesItem.visible
            height: visible ? mainPanel.itemSize : 0

            ExtendedMenuItem {
                height: mainPanel.itemSize
                rowHeight: height
                width: parent.width
                contentText: qsTr("Map themes")
                imageSource: InputStyle.mapThemesIcon
            }

            onClicked: {
                mapThemesBtn.activated()
                rootMenu.close()
            }
        }

        MenuItem {
            visible: !settingsItem.visible
            height: visible ? mainPanel.itemSize : 0
            width: parent.width

            ExtendedMenuItem {
                anchors.fill: parent
                rowHeight: parent.height
                width: parent.width
                contentText: qsTr("Settings")
                imageSource: InputStyle.settingsIcon
            }

            onClicked: {
                settingsBtn.activated()
                rootMenu.close()
            }
        }
    }

    // Menu shadow
    Rectangle {
        x: rootMenu.x
        y: rootMenu.y
        width: rootMenu.width
        height: rootMenu.height
        layer.enabled: true
        layer.effect: Shadow {}
        visible: rootMenu.opened
    }
}
