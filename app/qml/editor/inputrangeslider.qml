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

import lc 1.0

Item {
  id: root

  /*required*/ property var parentValue: parent.value

  readonly property int max_range: 2000000000 // https://doc.qt.io/qt-5/qml-int.html

  property int precision: config["Precision"]
  property real from: getRange(config["Min"], -max_range)
  property real to: getRange(config["Max"], max_range)
  property real step: config["Step"] ? config["Step"] : 1
  property var locale: Qt.locale()
  property string suffix: config["Suffix"] ? config["Suffix"] : ""

  signal editorValueChanged( var newValue, bool isNull )

  function getRange(rangeValue, defaultRange) {
    if ( typeof rangeValue !== 'undefined' && rangeValue >= -max_range && rangeValue <= max_range )
      return rangeValue
    else
      return defaultRange
  }

  enabled: !readOnly
  height: customStyle.fields.height

  anchors {
    left: parent.left
    right: parent.right
  }

  // background
  Rectangle {
    anchors.fill: parent
    border.color: customStyle.fields.normalColor
    border.width: 1 * __dp
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

        Layout.preferredWidth: rowLayout.width / 3
        Layout.maximumWidth: rowLayout.width / 3
        Layout.preferredHeight: root.height
        Layout.maximumHeight: root.height

        elide: Text.ElideRight
        text: Number( slider.value ).toFixed( precision ).toLocaleString( root.locale ) + root.suffix

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        font.pixelSize: customStyle.fields.fontPixelSize
        color: customStyle.fields.fontColor
        padding: 10 * __dp
        leftPadding: customStyle.fields.sideMargin
      }

      Slider {
        id: slider

        to: root.to
        from: root.from
        stepSize: root.step
        value: root.parent.value ? root.parent.value : 0

        Layout.fillWidth: true
        Layout.maximumHeight: root.height
        Layout.preferredHeight: root.height
        rightPadding: customStyle.fields.sideMargin

        onValueChanged: root.editorValueChanged( slider.value, false )

        background: Rectangle {
          x: slider.leftPadding
          y: slider.topPadding + slider.availableHeight / 2 - height / 2
          width: slider.availableWidth
          height: slider.height * 0.1
          radius: 2 * __dp

          color: root.enabled ? customStyle.fields.fontColor : customStyle.fields.backgroundColorInactive
        }

        handle: Rectangle {
          x: slider.leftPadding + slider.visualPosition * (slider.availableWidth - width)
          y: slider.topPadding + slider.availableHeight / 2 - height / 2

          width: slider.height * 0.6 * 0.66 + (2 * border.width) // Similar to indicator SwitchWidget of CheckBox widget
          height: width

          radius: height * 0.5

          color: "white"
          border.color: customStyle.fields.backgroundColorInactive
        }
      }
    }
  }
}
