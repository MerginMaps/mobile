import QtQuick
import QtQuick.Controls

ScrollView {
    id: root

    required property list<color> colors
    required property bool showEraseButton

    property color activeColor
    property bool eraserActive: false

    height: scrollRow.height
    ScrollBar.vertical.policy: ScrollBar.AlwaysOff
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

    Row {
        id: scrollRow
        spacing: __style.margin12
        padding: __style.margin4
        anchors.centerIn: parent

        Repeater {
            model: root.colors
            MMColorButton{
                required property color modelData
                required property int index
                
                chosenColor: modelData
                isSelected: !root.eraserActive && (root.activeColor === modelData)
                
                onClicked: {
                    if (root.showEraseButton) {
                        root.eraserActive = false;
                    }   
                    root.activeColor = modelData;
                }
                Component.onCompleted: {
                    // set the initial color to be the first one in the list
                    if ( index === 0 )
                    {
                        root.activeColor = modelData
                    }
                }  
            } 
        }

        MMButton {
            text: qsTr("Eraser")
            iconSourceLeft: __style.editIcon
            // in some instances the erase button is not needed, because there is an "undo" feature already implemented
            visible: root.showEraseButton

            type: MMButton.Types.Primary
            size: MMButton.Sizes.Small

            fontColor: root.eraserActive ? __style.negativeColor : __style.grapeColor
            iconColor: root.eraserActive ? __style.negativeColor : __style.grapeColor
            bgndColor: root.eraserActive ? __style.grapeColor : __style.negativeColor
            fontColorHover: root.eraserActive ? __style.grapeColor : __style.negativeColor
            iconColorHover: root.eraserActive ? __style.grapeColor : __style.negativeColor
            bgndColorHover: root.eraserActive ? __style.negativeColor : __style.grapeColor

            onClicked: {
                root.activeColor = null;
                root.eraserActive = true;
            }
        }
    }
}