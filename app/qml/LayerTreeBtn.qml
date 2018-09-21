import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QgsQuick 0.1 as QgsQuick

Rectangle {
    id: layerTreeBtn
    height: width
    color: "transparent"

    property int size: width / 3
    property int offset: size / 2
    property real rot: 10

    signal activated()

    MouseArea {
        anchors.fill: parent
        onClicked: {
            layerTreeBtn.activated()
            animation.restart()
        }
    }

    SequentialAnimation {
        id: animation
        running: false
        NumberAnimation {
            target: layerTreeBtn
            property: "rot"
            from: 10
            to: 370
            duration: 1000
        }
    }

    Rectangle {
        id: thirdRec
        x: firstRec.x - offset
        y: firstRec.y - offset
        rotation: firstRec.rotation - rot
        width: size
        height: size
        color: "white"
        antialiasing: true
    }

    Rectangle {
        id: firstRec
        x: parent.width / 3
        y: parent.height / 3
        rotation: 0
        width: size
        height: size
        color: "#f3a67c"
        antialiasing: true
    }

    Rectangle {
        id: secondRec
        x: firstRec.x + offset
        y: firstRec.y + offset
        rotation: firstRec.rotation + rot
        width: size
        height: size
        color: "#fd5757"
        antialiasing: true
    }
}
