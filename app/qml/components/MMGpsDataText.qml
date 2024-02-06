import QtQuick
import "."
import ".."
import lc 1.0
import QtQuick.Layouts

Item {
    property string titleText: "Default title"
    property string descriptionText: "Default description"
    property bool alignmentRight: false
    property bool itemVisible: true

    width: parent.width / 2
    height: parent.height

    ColumnLayout {
        width: parent.width
        height: parent.height
        visible: itemVisible
        spacing: 0

        Text {
            text: titleText
            color: __style.nightColor
            font: __style.p6
            elide: Text.ElideRight
            horizontalAlignment: alignmentRight ? Text.AlignRight : Text.AlignLeft
            Layout.fillWidth: true
            width: parent.width
            Layout.topMargin: 8
        }

        Text {
            text: descriptionText
            color: __style.nightColor
            font: __style.t3
            elide: Text.ElideMiddle
            horizontalAlignment: alignmentRight ? Text.AlignRight : Text.AlignLeft
            Layout.fillWidth: true
            width: parent.width
            Layout.bottomMargin: 8
        }
    }
}
