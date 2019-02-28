import QtQuick 2.7
import QtGraphicalEffects 1.0
import QgsQuick 0.1 as QgsQuick
import lc 1.0

Item {    
    id: root
    property real lineWidth: 3 * QgsQuick.Utils.dp
    property real lineLength: 100 * QgsQuick.Utils.dp
    property real childrenOpacity: 0.2

    Rectangle {
        anchors.centerIn: parent
        height: root.lineWidth
        width: root.lineLength
        color: "black"
        opacity: root.childrenOpacity
    }

    Rectangle {
        anchors.centerIn: parent
        width: root.lineWidth
        height: root.lineLength
        color: "black"
        opacity: root.childrenOpacity
    }
}
