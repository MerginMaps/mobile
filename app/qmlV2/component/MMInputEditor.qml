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

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects
import ".."

//import lc 1.0

MMAbstractEditor {
  id: root

  /*required*/ property var parentValue: parent.value ?? ""
  /*required*/ property bool parentValueIsNull: parent.valueIsNull ?? false
  /*required*/ property bool isReadOnly: parent.readOnly ?? false

  property alias placeholderText: textField.placeholderText
  readonly property alias text: textField.text

  signal editorValueChanged( var newValue, var isNull )

  hasFocus: textField.activeFocus

  content: TextField {
    id: textField

    anchors.fill: parent

    text: root.parentValue
    color: root.enabled ? StyleV2.nightColor : StyleV2.mediumGreenColor
    placeholderTextColor: StyleV2.nightAlphaColor
    font: StyleV2.p5
    hoverEnabled: true

    background: Rectangle {
      color: StyleV2.transparentColor
    }
  }

  rightAction: MMIcon {
    id: rightIcon

    source: StyleV2.xMarkIcon
    color: root.enabled ? StyleV2.forestColor : StyleV2.mediumGreenColor
    height: parent.height
    visible: textField.activeFocus && textField.text.length>0
  }

  onRightActionClicked: textField.text = ""
}
