import QtQuick 2.7
import QgsQuick 0.1 as QgsQuick
import lc 1.0

Item {    
    id: root
    property real size: 100 * QgsQuick.Utils.dp

    Image {
        anchors.centerIn: parent
        height: root.size
        width: height
        source: "crosshair.svg"
        sourceSize.width: width
        sourceSize.height: heightß
    }
}
