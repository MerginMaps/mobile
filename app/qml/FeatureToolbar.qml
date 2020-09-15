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
import "."  // import InputStyle singleton

Item {
    property int itemSize: toolbar.height * 0.8
    property bool isFeaturePoint: false
    property bool saveBtnEnabled: true

    signal editClicked()
    signal saveClicked()
    signal addPhotoClicked()
    signal deleteClicked()
    signal editGeometryClicked()

    id: toolbar

    states: [
        State {
            name: "Edit"
            PropertyChanges { target: editRow; visible: true }
            PropertyChanges { target: readOnlyRow; visible: false }
        }
        ,State {
            name: "Add"
            PropertyChanges { target: editRow; visible: true }
            PropertyChanges { target: readOnlyRow; visible: false }
        }
        ,State {
            name: "ReadOnly"
            PropertyChanges { target: editRow; visible: false }
            PropertyChanges { target: readOnlyRow; visible: true }
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
                    toolbar.state="Edit"
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
                width: toolbar.itemSize
                text: qsTr("Delete")
                imageSource: InputStyle.removeIcon

                onActivated: {
                    toolbar.deleteClicked()
                }
            }
        }

        Item {
            width: parent.width/parent.children.length
            height: parent.height

            MainPanelButton {

                width: toolbar.itemSize
                text: qsTr("Save")
                imageSource: "yes.svg"
                enabled: toolbar.saveBtnEnabled

                onActivated: toolbar.saveClicked()
            }

        }

        Item {
            width: parent.width/parent.children.length
            height: parent.height

            MainPanelButton {

                width: toolbar.itemSize
                text: qsTr("Edit geometry")
                imageSource: InputStyle.editIcon
                enabled: isFeaturePoint

                onActivated: {
                    toolbar.editGeometryClicked()
                }
            }
        }
    }
}
