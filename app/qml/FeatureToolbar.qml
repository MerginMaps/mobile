import QtQuick 2.7
import QtQuick.Controls 2.2
import "."  // import InputStyle singleton

Item {
    property int itemSize: toolbar.height * 0.8

    signal editClicked()
    signal saveClicked()
    signal addPhotoClicked()
    signal deleteClicked()

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
                imageSource: "edit.svg"

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
                imageSource: "trash.svg"
                disabled: true

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

                onActivated: toolbar.saveClicked()
            }

        }

        Item {
            width: parent.width/parent.children.length
            height: parent.height

            MainPanelButton {

                width: toolbar.itemSize
                text: qsTr("Add photo")
                imageSource: "add_photo.svg"
                disabled: true

                onActivated: {
                    toolbar.addPhotoClicked()
                }
            }
        }
    }
}
