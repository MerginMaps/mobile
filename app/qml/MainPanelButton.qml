import QtQuick 2.7
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0

Item {
    id: root
    height: width
    anchors.centerIn: parent
    property string imageSource: ""
    property string imageSource2: ""
    property bool imageSourceCondition: true
    property string text
    property color fontColor: "white"

    signal activated()
    signal activatedOnHold()

    MouseArea {
        anchors.fill: parent
        onClicked: {
            root.activated()
        }
        onPressAndHold: {
            root.activatedOnHold()
        }
    }

    Image {
        id: image
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.fill: parent
        source: root.imageSourceCondition ? root.imageSource : root.imageSource2
        visible: source
        anchors.margins: root.width/4
        anchors.topMargin: 0
        fillMode: Image.PreserveAspectFit
    }

    ColorOverlay {
        anchors.fill: image
        source: image
        color: fontColor
    }

    Text {
        anchors.fill: parent
        color: fontColor
        text: root.text
        font.bold: true
        font.pixelSize: InputStyle.fontPixelSizeSmaller
        verticalAlignment: Text.AlignBottom
        horizontalAlignment: Text.AlignHCenter
        z: image.z + 1
    }
}
