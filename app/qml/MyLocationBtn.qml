import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QgsQuick 0.1 as QgsQuick

Item {
    id: myLocationBtn
    height: width
    property int size: width / 2
    property bool lockOnPosition: false

    signal activated()
    signal activatedOnHold()

    MouseArea {
        anchors.fill: parent
        onClicked: {
            myLocationBtn.activated()
            animation.restart()
        }
        onPressAndHold: {
            myLocationBtn.activatedOnHold()
        }
    }

    SequentialAnimation {
        id: animation
        running: false
        NumberAnimation {
            target: image
            property: "scale"
            from: 1
            to: 1.2
        }
        NumberAnimation {
            target: image
            property: "scale"
            from: 1.2
            to: 1
        }
    }

    Image {
        id: image
        width: size
        height: size
        anchors.centerIn: parent
        source: lockOnPosition ? "ic_my_location_white_48px.svg" : "baseline-gps_off-24px.svg"
    }
}
