import QtQuick 2.0
import QtQuick.Controls 1.2

Item {
    width: label.x+label.width
    height: label.height

    property int scaleBarWidth: 3  // line width of the scale bar
    property int scaleBarHeight: 10 // height of the left and right notch
    property int labelBarMargin: 8 // how far is the label from the scale bar

    property var engine  // should be assigned in order to follow map
    property int length: engine ? engine.scaleBarLength : 100      // length of the scale bar in pixels
    property string text: engine ? engine.scaleBarText : "? km"  // text of the label next to the scale bar

    Rectangle {
        anchors.fill: parent
        color: "white"
        opacity: 0.5
    }

    Rectangle {
        id: rectBar
        anchors.bottom: label.baseline
        width: length
        height: scaleBarWidth
        color: "black"
    }

    Rectangle {
        id: leftBar
        anchors.left: rectBar.left
        anchors.bottom: rectBar.bottom
        width: scaleBarWidth
        height: scaleBarHeight
        color: "black"
    }

    Rectangle {
        id: rightBar
        anchors.right: rectBar.right
        anchors.bottom: rectBar.bottom
        width: scaleBarWidth
        height: scaleBarHeight
        color: "black"
    }

    Label {
        id: label
        text: parent.text

        anchors.left: rectBar.right
        anchors.leftMargin: labelBarMargin
    }

}
