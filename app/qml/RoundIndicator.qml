import QtQuick 2.7

Item {
    id: gpsSignal
    width: parent.height
    height: width
    property int size: width
    property color color: "orange"
    property bool isActive: false

    Rectangle {
        anchors.centerIn: parent
        width: gpsSignal.size
        height: gpsSignal.size
        color: gpsSignal.color
        radius: width*0.5
        antialiasing: true
    }

    Rectangle {
        id: activeIndicator
        anchors.centerIn: parent
        width: gpsSignal.size/2
        height: gpsSignal.size/2
        color: "white"
        radius: width*0.5
        antialiasing: true
        visible: isActive
    }
}
