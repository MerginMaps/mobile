import QtQuick 2.7
import QtQuick.Controls 2.2

Item {

    id: notification
    property string text: ""

    function open() {
        popup.open()
    }

    Popup {
        id: popup
        modal: false
        focus: true
        opacity: 1
        width: notification.width
        height: notification.height
        background: Rectangle {
            anchors.fill: parent
            color: "white"
            opacity: 0.8
        }
        onOpened: timer.start()

        Text {
            anchors.fill: parent
            text: notification.text
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
        }
    }

    Timer {
        id: timer
        interval: 2000
        onTriggered: popup.close()
        running: false
        repeat: false
    }
}
