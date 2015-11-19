import QtQuick 2.4
import QtQuick.Window 2.2
import qgis 1.0

Window {
    visible: true

    TestItem {
        anchors.fill: parent
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            Qt.quit();
        }
    }

    Text {
        text: qsTr("Hello World")
        anchors.centerIn: parent
    }

    MapCanvas {
        anchors.fill: parent
    }
}
