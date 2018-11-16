import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QgsQuick 0.1 as QgsQuick

Item {
    id: recordBtn
    height: width
    property int size: width / 2

    property bool recording: false

    function activated() {
        animation.start()
    }

    Rectangle {
        id: recBtn
        anchors.centerIn: parent
        property int borderWidth: 10 * QgsQuick.Utils.dp
        width: size
        height: size
        color: recordBtn.enabled ? "#fd5757" : "#aaaaaa"
        border.color: "white"
        border.width: borderWidth
        radius: width*0.5
        antialiasing: true

        Label {
            id: label
            font.pixelSize: parent.width / 5
            anchors.centerIn: parent
            text: "REC"
            color: "white"
            visible: recording
        }

        SequentialAnimation {
            id: animation
            loops: Animation.Infinite
            running: recording
            NumberAnimation {
                target: recBtn
                property: "borderWidth"
                from: 10
                to: 7
                duration: 200
            }
            NumberAnimation {
                target: recBtn
                property: "borderWidth"
                from: 7
                to: 10
                duration: 200
            }
        }
    }

}
