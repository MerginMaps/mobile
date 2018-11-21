import QtQuick 2.11
import QtQuick.Controls 2.2
import "."

ApplicationWindow {
    id: window
    visible: true
    width:  __appwindowwidth
    height: __appwindowheight
    visibility: __appwindowvisibility
    title: qsTr("Input")

    Component.onCompleted: {
        console.log("App started")
    }

    Loader {
        id: splashLoader
        anchors.fill: parent
        source: "SplashScreen.qml"
        asynchronous: false
        visible: true

        onStatusChanged: {
            console.log("tralala status changed")
            if (status === Loader.Ready) {
                appLoader.setSource("main.qml");
            }
        }
    }

    Connections {
        target: splashLoader.item
        onReadyToGo: {
            appLoader.visible = true
            splashLoader.visible = false
            splashLoader.setSource("")
            appLoader.item.forceActiveFocus();
            InputStyle.deviceRatio = window.screen.devicePixelRatio
        }
    }

    Loader {
        id: appLoader
        anchors.fill: parent
        visible: false
        asynchronous: true
        onStatusChanged: {
            if (status === Loader.Ready)
                splashLoader.item.appReady()
            if (status === Loader.Error) {
                console.log("Error"); splashLoader.item.errorInLoadingApp();
            }
        }
    }
}
