import QtQuick 2.11
import QtGraphicalEffects 1.0
import "."

Item {
    id: root
    anchors.fill: parent

    property bool appIsReady: false
    property bool splashIsReady: false

    property bool ready: appIsReady && splashIsReady && !splashTimer.running
    onReadyChanged: if (ready) readyToGo();

    signal readyToGo()

    function appReady()
    {
        appIsReady = true
    }

    function errorInLoadingApp()
    {
        Qt.quit()
    }

    Rectangle {
        anchors.fill: parent
        color: InputStyle.fontColor
    }

    Image {
        id: logo
        anchors.centerIn: parent
        source: "input.svg"
        width: parent.width/2
        sourceSize.height: 0
        fillMode: Image.PreserveAspectFit
        sourceSize.width: width
    }

    Text {
        text: "Collect map data in the field"
        anchors.verticalCenterOffset: parent.height/6
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter
        font.italic: true
        font.pixelSize: InputStyle.fontPixelSizeNormal
        color: "white"
        opacity: 0.75
    }

    Text {
        text: "Developed by Lutra Consulting"
        anchors.bottomMargin: InputStyle.panelMargin
        anchors.fill: parent
        verticalAlignment: Text.AlignBottom
        horizontalAlignment: Text.AlignHCenter
        font.bold: true
        font.pixelSize: InputStyle.fontPixelSizeSmall
        color: "white"
        opacity: 0.5
    }

    Timer {
        id: splashTimer
        interval: 2000
        onTriggered: splashIsReady = true
    }

    Component.onCompleted: splashTimer.start()
}
