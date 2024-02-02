import QtQuick
import "."
import ".."
import lc 1.0

Item {
    property string titleText: "Title"
    property string descriptionText: "Description goes here"
    property bool alignmentRight: false
    property bool itemVisible: true

    width: parent.width / 2
    height: parent.height

    Column {
        width: parent.width
        visible: itemVisible

        Text {
            text: titleText
            color: __style.nightColor
            font: __style.p6
            wrapMode: Text.WordWrap
            horizontalAlignment: alignmentRight ? Text.AlignRight : Text.AlignLeft
            anchors.top: parent.top
            anchors.topMargin: 8
            width: parent.width
        }

        Text {
            text: descriptionText
            color: __style.nightColor
            font: __style.t3
            wrapMode: Text.WordWrap
            horizontalAlignment: alignmentRight ? Text.AlignRight : Text.AlignLeft
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 8
            width: parent.width
        }
    }
}
