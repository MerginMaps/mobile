import QtQuick
import QtQuick.Controls

ScrollView {
    id: root

    required property var colors
    required property var controller
    required property bool showEraseButton

    height: scrollRow.height
    ScrollBar.vertical.policy: ScrollBar.AlwaysOff
    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

    Row {
        id: scrollRow
        spacing: __style.margin12
        padding: __style.margin4
        anchors.centerIn: parent

        Repeater {
            model: colors ?? null

            MMRoundButton {
                anchors.verticalCenter: parent.verticalCenter

                contentItem: Rectangle {
                    color: modelData
                    radius: width / 2
                    anchors.fill: parent
                }

                background: Rectangle {
                    property bool isActive: modelData.toLowerCase() === controller.activeColor.toString().toLowerCase()

                    anchors.verticalCenter: parent.verticalCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    radius: width / 2
                    width: __style.margin48
                    height: __style.margin48
                    color: isActive ? __style.transparentColor : __style.lightGreenColor
                    border.width: 2
                    border.color: isActive ? __style.grassColor : __style.transparentColor
                }

                onClicked: {
                    if (showEraseButton) {
                        controller.eraserActive = false;
                    }
                    controller.activeColor = modelData;
                }
            }
        }

        MMButton {
            text: qsTr("Eraser")
            iconSourceLeft: __style.editIcon
            // in some instances the erase button is not needed, because there is an "undo" feature already implemented
            visible: showEraseButton

            type: MMButton.Types.Primary
            size: MMButton.Sizes.Small

            fontColor: controller?.eraserActive ? __style.negativeColor : __style.grapeColor
            iconColor: controller?.eraserActive ? __style.negativeColor : __style.grapeColor
            bgndColor: controller?.eraserActive ? __style.grapeColor : __style.negativeColor
            fontColorHover: controller?.eraserActive ? __style.grapeColor : __style.negativeColor
            iconColorHover: controller?.eraserActive ? __style.grapeColor : __style.negativeColor
            bgndColorHover: controller?.eraserActive ? __style.negativeColor : __style.grapeColor

            onClicked: {
                controller.activeColor = null;
                controller.eraserActive = true;
            }
        }
    }
}