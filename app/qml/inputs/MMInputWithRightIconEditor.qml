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
import "../components"

MMAbstractEditor {
  id: root

  property var parentValue: parent.value ?? ""
  property bool parentValueIsNull: parent.valueIsNull ?? false
  property bool isReadOnly: parent.readOnly ?? false

  property alias placeholderText: textField.placeholderText
  property alias text: textField.text
  property alias iconSource: rightIcon.source

  signal editorValueChanged( var newValue, var isNull )

  hasFocus: textField.activeFocus

  content: TextField {
    id: textField

    anchors.fill: parent

    text: root.parentValue
    color: root.enabled ? __style.nightColor : __style.mediumGreenColor
    placeholderTextColor: __style.nightAlphaColor
    font: __style.p5
    hoverEnabled: true

    background: Rectangle {
      color: __style.transparentColor
    }
  }

  rightAction: MMIcon {
    id: rightIcon

    height: parent.height

    color: root.enabled ? __style.forestColor : __style.mediumGreenColor
  }
}
