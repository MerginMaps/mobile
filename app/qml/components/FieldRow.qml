import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.2
import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "./.."

// import InputStyle singleton
Item {
  id: root
  signal removeClicked

  property real rowHeight: InputStyle.rowHeightHeader
  property real iconSize: rowHeight
  property color color: InputStyle.fontColor
  property var widgetList: []

  RowLayout {
    id: row
    height: root.rowHeight
    width: root.width
    spacing: InputStyle.panelSpacing

    TextField {
      id: textField
      height: row.height
      topPadding: 10 * QgsQuick.Utils.dp
      bottomPadding: 10 * QgsQuick.Utils.dp
      font.pixelSize: InputStyle.fontPixelSizeNormal
      color: root.color
      placeholderText: FieldName ? FieldName : qsTr("Field name")
      Layout.fillWidth: true
      Layout.fillHeight: true

      background: Rectangle {
        anchors.fill: parent
        border.color: textField.activeFocus ? InputStyle.fontColor : InputStyle.panelBackgroundLight
        border.width: textField.activeFocus ? 2 : 1
        color: InputStyle.clrPanelMain
        radius: InputStyle.cornerRadius
      }
    }

    ComboBox {
      id: comboBox
      height: row.height
      Layout.fillWidth: true
      Layout.fillHeight: true
      model: root.widgetList
    }

    Item {
      id: imageBtn
      height: root.iconSize
      width: height
      Layout.fillWidth: true
      Layout.fillHeight: true

      MouseArea {
        anchors.fill: parent
        onClicked: root.removeClicked()
      }

      Image {
        id: image
        anchors.centerIn: imageBtn
        source: InputStyle.noIcon
        height: imageBtn.height
        width: height
        sourceSize.width: width
        sourceSize.height: height
        fillMode: Image.PreserveAspectFit
      }

      ColorOverlay {
        anchors.fill: image
        source: image
        color: root.color
      }
    }
  }
}
