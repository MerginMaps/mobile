import QtQuick
import QtQuick.Controls

MMRoundButton {
    id: root

    required property color chosenColor
    property bool isSelected: false

    anchors.verticalCenter: parent.verticalCenter

    contentItem: Rectangle {
        color: root.chosenColor
        radius: width / 2
        anchors.fill: parent
    }

    background: Rectangle {
        anchors{
            verticalCenter: parent.verticalCenter
            horizontalCenter: parent.horizontalCenter
        }

        radius: width / 2
        width: __style.margin48
        height: __style.margin48

        color: root.isSelected ? __style.transparentColor : __style.lightGreenColor
        border{
            width: 2
            color: root.isSelected ? __style.grassColor : __style.transparentColor
        }
    }
}