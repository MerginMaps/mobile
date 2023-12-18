/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import ".."

MMAbstractEditor {
  id: root

  property var parentValue: parent.value
  property bool parentValueIsNull: parent.valueIsNull ?? false
  property bool isReadOnly: parent.readOnly ?? false

  property int precision: 1 //config["Precision"]
  required property real from //getRange(config["Min"], -max_range)
  required property real to //getRange(config["Max"], max_range)
  property real step: 1 //config["Step"] ? config["Step"] : 1
  property string suffix: "" //config["Suffix"] ? config["Suffix"] : ""
  property var locale: Qt.locale()

  signal editorValueChanged( var newValue, var isNull )

  hasFocus: slider.activeFocus

  content: Item {
    id: input

    anchors.fill: parent

    RowLayout {
      id: rowLayout

      anchors.fill: parent

      Text {
        id: valueLabel

        Layout.preferredWidth: rowLayout.width / 2 - root.spacing
        Layout.maximumWidth: rowLayout.width / 2 - root.spacing
        Layout.preferredHeight: input.height
        Layout.maximumHeight: input.height

        elide: Text.ElideRight
        text: Number( slider.value ).toFixed( precision ).toLocaleString( root.locale ) + root.suffix

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
        font: __style.p5
        color: __style.nightColor
      }

      Slider {
        id: slider

        Layout.fillWidth: true
        Layout.maximumHeight: input.height
        Layout.preferredHeight: input.height

        to: root.to
        from: root.from
        stepSize: root.step
        value: root.parentValue ? root.parentValue : 0

        onValueChanged: { root.editorValueChanged( slider.value, false ); forceActiveFocus() }

        background: Rectangle {
          x: slider.leftPadding
          y: slider.topPadding + slider.availableHeight / 2 - height / 2
          width: slider.availableWidth
          height: 4 * __dp
          radius: 2 * __dp

          color: __style.lightGreenColor
        }

        handle: Rectangle {
          x: slider.leftPadding + slider.visualPosition * (slider.availableWidth - width)
          y: slider.topPadding + slider.availableHeight / 2 - height / 2
          width: 20 * __dp
          height: width
          radius: height / 2

          color: root.enabled ? __style.forestColor : __style.lightGreenColor
        }
      }
    }
  }
}
