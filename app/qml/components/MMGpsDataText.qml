import QtQuick
import "."
import ".."
import lc 1.0

Item {
    property string titleText: "Title"
    property string descriptionText: "Description goes here"
    property bool alignmentRight: false

    width: parent.width / 2
    height: parent.height

    Column {
        anchors.fill: parent
        spacing: 10

        Text {
            text: titleText
            color: __style.nightColor
            font: __style.p6
            wrapMode: Text.WordWrap
            horizontalAlignment: alignmentRight ? Text.AlignRight : Text.AlignLeft
            width: parent.width
        }

        Text {
            text: descriptionText
            color: __style.nightColor
            font: __style.t3
            wrapMode: Text.WordWrap
            horizontalAlignment: alignmentRight ? Text.AlignRight : Text.AlignLeft
            width: parent.width
        }
    }
}
