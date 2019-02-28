import QtQuick 2.12
import QtGraphicalEffects 1.12
import QgsQuick 0.1 as QgsQuick
import lc 1.0

Item {    
    id: root
    property int lineWidth: 3 * QgsQuick.Utils.dp
    property real childrenOpacity: 0.2

    Rectangle {
        id: background
        opacity: root.childrenOpacity
        color: "black"
        anchors.fill: parent
        visible: false
    }

    Rectangle {
        id: crosshair
        anchors.centerIn: parent
        radius: width*0.5
        width: 100 * QgsQuick.Utils.dp
        height: width
        color: "white"
        visible: false
    }

    Rectangle {
        anchors.centerIn: parent
        height: root.lineWidth
        width: crosshair.width
        color: "black"
        opacity: root.childrenOpacity
    }

    Rectangle {
        anchors.centerIn: parent
        width: root.lineWidth
        height: crosshair.height
        color: "black"
        opacity: root.childrenOpacity
    }

//    OpacityMask {
//        anchors.fill: background
//        source: background
//        maskSource: crosshair
//    }

}
