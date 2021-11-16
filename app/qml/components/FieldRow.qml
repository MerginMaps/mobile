import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import QtQuick.Dialogs 1.2
import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "../" // import InputStyle singleton

Item {
  id: fieldDelegate
  property color color: InputStyle.fontColor
  property real iconSize: rowHeight * 0.4
  property real rowHeight: InputStyle.rowHeightHeader
  property var widgetList: []

  signal removeClicked(var index)

  RowLayout {
    id: row
    property real itemSize: (parent.width - imageBtn.width - (2 * row.spacing)) / 2

    height: fieldDelegate.rowHeight
    spacing: InputStyle.panelSpacing
    width: fieldDelegate.width

    InputTextField {
      id: textField
      Layout.fillHeight: true
      Layout.fillWidth: true
      Layout.preferredWidth: row.itemSize
      color: fieldDelegate.color

      Component.onCompleted: text = AttributeName
      onTextChanged: AttributeName = text
    }
    ComboBox {
      id: comboBox
      Layout.fillHeight: true
      Layout.fillWidth: true
      Layout.preferredWidth: row.itemSize
      height: row.height
      model: widgetList
      textRole: "display"
      valueRole: "widget"

      Component.onCompleted: {
        comboBox.currentIndex = comboBox.indexOfValue(WidgetType);
      }

      MouseArea {
        anchors.fill: parent
        propagateComposedEvents: true

        onClicked: mouse.accepted = false
        onDoubleClicked: mouse.accepted = false
        onPositionChanged: mouse.accepted = false
        onPressAndHold: mouse.accepted = false
        onPressed: {
          forceActiveFocus();
          mouse.accepted = false;
        }
        onReleased: mouse.accepted = false
      }

      background: Item {
        implicitHeight: comboBox.height * 0.8

        Rectangle {
          id: backgroundRect
          anchors.fill: parent
          border.color: comboBox.pressed ? InputStyle.fontColor : InputStyle.panelBackgroundLight
          border.width: comboBox.visualFocus ? 2 : 1
          color: InputStyle.panelBackgroundLight
          radius: InputStyle.cornerRadius
        }
      }
      contentItem: Text {
        color: InputStyle.fontColor
        elide: Text.ElideRight
        font.pixelSize: InputStyle.fontPixelSizeNormal
        height: comboBox.height * 0.8
        horizontalAlignment: Text.AlignLeft
        leftPadding: textField.leftPadding
        text: comboBox.displayText
        verticalAlignment: Text.AlignVCenter
      }
      delegate: ItemDelegate {
        font.pixelSize: InputStyle.fontPixelSizeNormal
        font.weight: comboBox.currentIndex === index ? Font.DemiBold : Font.Normal
        height: comboBox.height * 0.8
        highlighted: comboBox.highlightedIndex === index
        leftPadding: textField.leftPadding
        text: model.display.replace('&', "&&") // issue ampersand character showing up as underscore
        width: comboBox.width

        onClicked: {
          WidgetType = model.widget;
          comboBox.currentIndex = index;
        }
      }
      indicator: Item {
        anchors.right: parent.right
        height: parent.height

        Image {
          id: comboboxIndicatorIcon
          anchors.right: parent.right
          anchors.rightMargin: InputStyle.innerFieldMargin
          anchors.verticalCenter: parent.verticalCenter
          autoTransform: true
          fillMode: Image.PreserveAspectFit
          height: fieldDelegate.iconSize
          source: InputStyle.comboboxIcon
          visible: false
          width: height / 2
        }
        ColorOverlay {
          anchors.fill: comboboxIndicatorIcon
          color: InputStyle.activeButtonColor
          source: comboboxIndicatorIcon
        }
      }
    }
    Symbol {
      id: imageBtn
      Layout.fillHeight: true
      height: fieldDelegate.height / 2
      iconSize: fieldDelegate.iconSize
      source: InputStyle.noIcon

      MouseArea {
        anchors.fill: parent

        onClicked: {
          fieldDelegate.removeClicked(index);
        }
      }
    }
  }
}
