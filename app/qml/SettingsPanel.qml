import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton

Drawer {
    id: settingsPanel
    visible: false
    modal: true
    interactive: true
    dragMargin: 0 // prevents opening the drawer by dragging.

    property alias autoCenterMapChecked: autoCenterMapCheckBox.checked

    background: Rectangle {
        color: InputStyle.clrPanelBackground
        opacity: InputStyle.panelOpacity
    }

    ColumnLayout {
        x: 10 * QgsQuick.Utils.dp
        y: 10 * QgsQuick.Utils.dp
        width: parent.width - 20 * QgsQuick.Utils.dp
        height: parent.height - 20 * QgsQuick.Utils.dp
        spacing: 10 * QgsQuick.Utils.dp

        Text {
            id: settingsText
            text: "Settings"
            Layout.fillWidth: true
            color: InputStyle.clrPanelMain
            font.pixelSize: InputStyle.fontPixelSizeBig
            font.bold: true
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }

        CheckBox {
            id: autoCenterMapCheckBox
            checked: false
            text: "Auto-center map"
        }

        Text {
            id: logTitle
            text: "Message Log"
            Layout.fillWidth: true
            color: InputStyle.clrPanelMain
            font.pixelSize: InputStyle.fontPixelSizeBig
            font.bold: true
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
        }


        QgsQuick.MessageLog {
            id: messageLog
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: QgsQuick.MessageLogModel {}
            visible: true
        }
    }

}
