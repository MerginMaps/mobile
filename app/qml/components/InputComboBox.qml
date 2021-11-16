/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.7
import QtQuick.Controls 2.2
import QtGraphicalEffects 1.0
import QgsQuick 0.1 as QgsQuick

/**
 * ComboBox used by valueMap and valueRelation
 * Do not use directly from Application QML
 */
ComboBox {
  id: comboBox
  property var comboStyle
  property real iconSize
  property bool readOnly: false

  signal itemClicked(var index)

  anchors {
    left: parent.left
    right: parent.right
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
    implicitWidth: 120 * QgsQuick.Utils.dp

    Rectangle {
      id: backgroundRect
      anchors.fill: parent
      border.color: comboBox.pressed ? comboStyle.activeColor : comboStyle.normalColor
      border.width: comboBox.visualFocus ? 2 : 1
      color: comboStyle.backgroundColor
      radius: comboStyle.cornerRadius
    }
  }
  contentItem: Text {
    color: comboStyle.fontColor
    elide: Text.ElideRight
    font.pointSize: customStyle.fields.fontPointSize
    height: comboBox.height * 0.8
    horizontalAlignment: Text.AlignLeft
    leftPadding: customStyle.fields.sideMargin
    text: comboBox.displayText
    verticalAlignment: Text.AlignVCenter
  }

  // [hidpi fixes]
  delegate: ItemDelegate {
    font.pointSize: customStyle.fields.fontPointSize
    font.weight: comboBox.currentIndex === index ? Font.DemiBold : Font.Normal
    height: comboBox.height * 0.8
    highlighted: comboBox.highlightedIndex === index
    leftPadding: customStyle.fields.sideMargin
    text: model.display
    width: comboBox.width

    onClicked: comboBox.itemClicked(model.FeatureId ? model.FeatureId : index)
  }
  // [/hidpi fixes]
  indicator: Item {
    anchors.right: parent.right
    anchors.verticalCenter: parent.verticalCenter

    Image {
      id: comboboxIndicatorIcon
      anchors.right: parent.right
      anchors.rightMargin: customStyle.fields.sideMargin
      anchors.verticalCenter: parent.verticalCenter
      autoTransform: true
      fillMode: Image.PreserveAspectFit
      height: iconSize ? iconSize : parent.height * 0.4
      source: customStyle.icons.combobox
      visible: false
      width: height / 2
    }
    ColorOverlay {
      anchors.fill: comboboxIndicatorIcon
      color: readOnly ? customStyle.toolbutton.backgroundColorInvalid : customStyle.toolbutton.activeButtonColor
      source: comboboxIndicatorIcon
    }
  }
}
