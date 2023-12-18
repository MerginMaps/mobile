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
import "../components"

MMAbstractEditor {
  id: root

  property var parentValue: parent.value ?? false
  property bool parentValueIsNull: parent.valueIsNull ?? false
  property bool isReadOnly: parent.readOnly ?? false

  property string textOn: qsTr("True")
  property string textOff: qsTr("False")
  property alias checked: rightSwitch.checked

  signal editorValueChanged( var newValue, var isNull )

  hasFocus: rightSwitch.focus

  content: Text {
    id: textField

    width: parent.width + rightSwitch.x
    anchors.verticalCenter: parent.verticalCenter

    text: root.checked ? root.textOn : root.textOff
    color: root.enabled ? __style.nightColor : __style.mediumGreenColor
    font: __style.p5
    elide: Text.ElideRight
  }

  rightAction: MMSwitch {
    id: rightSwitch

    width: 50
    height: parent.height
    x: -30 * __dp

    checked: root.checked
    textOn: root.textOn
    textOff: root.textOff
    visibleText: false

    onCheckedChanged: focus = true
  }
}
