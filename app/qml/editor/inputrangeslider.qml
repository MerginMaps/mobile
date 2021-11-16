/***************************************************************************
 range.qml
  --------------------------------------
  Date                 : 2019
  Copyright            : (C) 2019 by Viktor Sklencar
  Email                : viktor.sklencar@lutraconsulting.co.uk
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtGraphicalEffects 1.14
import QgsQuick 0.1 as QgsQuick
import lc 1.0

Item {
  id: root
  property real from: getRange(config["Min"], -max_range)
  property var locale: Qt.locale()
  readonly property int max_range: 2000000000 // https://doc.qt.io/qt-5/qml-int.html

  /*required*/
  property var parentValue: parent.value
  property int precision: config["Precision"]
  property real step: config["Step"] ? config["Step"] : 1
  property string suffix: config["Suffix"] ? config["Suffix"] : ""
  property real to: getRange(config["Max"], max_range)

  enabled: !readOnly
  height: customStyle.fields.height

  signal editorValueChanged(var newValue, bool isNull)
  function getRange(rangeValue, defaultRange) {
    if (typeof rangeValue !== 'undefined' && rangeValue >= -max_range && rangeValue <= max_range)
      return rangeValue;
    else
      return defaultRange;
  }

  anchors {
    left: parent.left
    right: parent.right
  }

  // background
  Rectangle {
    anchors.fill: parent
    border.color: customStyle.fields.normalColor
    border.width: 1 * QgsQuick.Utils.dp
    color: customStyle.fields.backgroundColor
    radius: customStyle.fields.cornerRadius
  }
  Item {
    id: sliderContainer
    anchors.fill: parent

    RowLayout {
      id: rowLayout
      anchors.fill: parent

      Text {
        id: valueLabel
        Layout.maximumHeight: root.height
        Layout.maximumWidth: rowLayout.width / 3
        Layout.preferredHeight: root.height
        Layout.preferredWidth: rowLayout.width / 3
        color: customStyle.fields.fontColor
        elide: Text.ElideRight
        font.pointSize: customStyle.fields.fontPointSize
        horizontalAlignment: Text.AlignLeft
        leftPadding: customStyle.fields.sideMargin
        padding: 10 * QgsQuick.Utils.dp
        text: Number(slider.value).toFixed(precision).toLocaleString(root.locale) + root.suffix
        verticalAlignment: Text.AlignVCenter
      }
      Slider {
        id: slider
        Layout.fillWidth: true
        Layout.maximumHeight: root.height
        Layout.preferredHeight: root.height
        from: root.from
        rightPadding: customStyle.fields.sideMargin
        stepSize: root.step
        to: root.to
        value: root.parent.value ? root.parent.value : 0

        onValueChanged: root.editorValueChanged(slider.value, false)

        background: Rectangle {
          color: root.enabled ? customStyle.fields.fontColor : customStyle.fields.backgroundColorInactive
          height: slider.height * 0.1
          radius: 2 * QgsQuick.Utils.dp
          width: slider.availableWidth
          x: slider.leftPadding
          y: slider.topPadding + slider.availableHeight / 2 - height / 2
        }
        handle: Rectangle {
          border.color: customStyle.fields.backgroundColorInactive
          color: "white"
          height: width
          radius: height * 0.5
          width: slider.height * 0.6 * 0.66 + (2 * border.width) // Similar to indicator SwitchWidget of CheckBox widget
          x: slider.leftPadding + slider.visualPosition * (slider.availableWidth - width)
          y: slider.topPadding + slider.availableHeight / 2 - height / 2
        }
      }
    }
  }
}
