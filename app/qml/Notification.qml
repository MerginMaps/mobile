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
        x: 100
        y: 100
        width: 200
        height: 300
        modal: false
        focus: true
        closePolicy: Popup.CloseOnPressOutside

        Text {
            text: notification.text
        }
    }
}
