/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Controls
import ".."
import "../components"

Item {
    id: toolbar

    property int itemSize: toolbar.height * 0.8
    property bool isFeaturePoint: false
    property bool isSpatialLayer: false

    signal editClicked()
    signal deleteClicked()
    signal editGeometryClicked()
    signal splitGeometryClicked()
    signal redrawGeometryClicked()

    states: [
        State {
            name: "edit" // edit existing feature
            PropertyChanges { target: editRow; visible: true }
            PropertyChanges { target: readOnlyRow; visible: false }
            PropertyChanges { target: addRow; visible: false }
        }
        ,State {
            name: "add" // add new feature
            PropertyChanges { target: editRow; visible: false }
            PropertyChanges { target: readOnlyRow; visible: false }
            PropertyChanges { target: addRow; visible: true }
        }
        ,State {
            name: "readOnly"
            PropertyChanges { target: editRow; visible: false }
            PropertyChanges { target: readOnlyRow; visible: true }
            PropertyChanges { target: addRow; visible: false }
        }
    ]

    Rectangle {
        anchors.fill: parent
        color: InputStyle.clrPanelBackground
    }

    Row {
        id: readOnlyRow
        height: parent.height
        width: parent.width
        anchors.fill: parent

        Item {
            width: parent.width/parent.children.length
            height: parent.height

            MainPanelButton {
                id: openProjectBtn
                width: toolbar.itemSize
                text: qsTr("Edit")
                imageSource: InputStyle.editIcon

                onActivated: {
                    toolbar.editClicked()
                }
            }
        }
    }

    Row {
        id: editRow
        height: parent.height
        width: parent.width
        anchors.fill: parent

        Item {
            width: parent.width/parent.children.length
            height: parent.height

            MainPanelButton {
                width: visible ? toolbar.itemSize : 0
                text: qsTr("Delete")
                imageSource: InputStyle.removeIcon

                onActivated: {
                    toolbar.deleteClicked()
                }
            }
        }

        Item {
            width: visible ? parent.width/parent.children.length : 0
            height: parent.height
            visible: isSpatialLayer

            MainPanelButton {
                width: parent.visible ? toolbar.itemSize : 0
                text: qsTr("Edit geometry")
                imageSource: InputStyle.editIcon

                onActivated: {
                    toolbar.editGeometryClicked()
                }
            }
        }

        Item {
            width: visible ? parent.width/parent.children.length : 0
            height: parent.height
            visible: isSpatialLayer

            MainPanelButton {
              id: menuBtn
              width: parent.visible ? toolbar.itemSize : 0
              text: qsTr("Advanced")
              imageSource: InputStyle.moreMenuIcon

              onActivated: {
                if ( !rootMenu.visible ) rootMenu.open()
                else rootMenu.close()
              }
            }
        }
    }

    Row {
        id: addRow
        height: parent.height
        width: parent.width
        anchors.fill: parent


        Item {
            width: parent.width/parent.children.length
            height: parent.height
            visible: isSpatialLayer

            MainPanelButton {
                width: toolbar.itemSize
                text: qsTr("Edit geometry")
                imageSource: InputStyle.editIcon

                onActivated: {
                    toolbar.editGeometryClicked()
                }
            }
        }
    }

    Menu {
        id: rootMenu
        title: qsTr("Advanced")
        x:parent.width - rootMenu.width
        y: -rootMenu.height
        width: parent.width < 300 * __dp ? parent.width : 300 * __dp
        closePolicy: Popup.CloseOnReleaseOutsideParent | Popup.CloseOnEscape

        MenuItem {
            width: parent.width
            height: visible ? toolbar.itemSize : 0
            visible: !isFeaturePoint

            ExtendedMenuItem {
                height: toolbar.itemSize
                rowHeight: height
                width: parent.width
                contentText: qsTr("Split geometry")
                imageSource: InputStyle.scissorsIcon
            }

            onClicked: {
                toolbar.splitGeometryClicked()
                rootMenu.close()
            }
        }

        MenuItem {
            width: parent.width
            height: toolbar.itemSize

            ExtendedMenuItem {
                height: toolbar.itemSize
                rowHeight: height
                width: parent.width
                contentText: qsTr("Redraw geometry")
                imageSource: InputStyle.eraserIcon
            }

            onClicked: {
                toolbar.redrawGeometryClicked()
                rootMenu.close()
            }
        }
   }

}
