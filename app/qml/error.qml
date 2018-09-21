import QtQuick 2.7
import QtQuick.Controls 2.2
import QgsQuick 0.1 as QgsQuick
import "."  // import YdnpaStyle singleton

ApplicationWindow {
    id: window
    color: "white"
    visible: true
    visibility: "Maximized"
    title: qsTr("YDNPA Surveys")

    Rectangle {
        id: headerRect
        color: YdnpaStyle.clrPanelBackground
        opacity: YdnpaStyle.panelOpacity
        x: 0
        height: 140
        width: parent.width
    }

    Row {
        id: rowLayout
        y: 10
        x: 20
        height: 120
        spacing: YdnpaStyle.panelSpacing

        Image {
            id: image
            fillMode: Image.PreserveAspectFit
            height: 100
            anchors.verticalCenter: parent.verticalCenter
            source: "logo_yd.png"
        }

        Text {
            id: text1
            width: window.width - image.width - image1.width - 2*rowLayout.spacing - 30
            height: 100
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            elide: Text.ElideRight
            color: YdnpaStyle.clrPanelMain
            anchors.verticalCenter: parent.verticalCenter
            text: qsTr("YDNPA Surveys")
            font.bold: true
            font.pixelSize: 30 * QgsQuick.Utils.dp
        }

        Image {
            id: image1
            anchors.verticalCenter: parent.verticalCenter
            fillMode: Image.PreserveAspectFit
            height: 100
            source: "logo_lutra.png"
        }
    }

    Text {
        id: msgText
        x: 25
        y: headerRect.height + 10
        width: parent.width
        height: parent.height - 2*headerRect.height - 20
        text: __msg
        wrapMode: Text.WordWrap
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
        font.pixelSize: 23 * QgsQuick.Utils.dp
    }

    Rectangle {
        y: headerRect.height + 20 + msgText.height
        color: YdnpaStyle.clrPanelBackground
        opacity: YdnpaStyle.panelOpacity
        height: 140
        width: parent.width
    }
}
