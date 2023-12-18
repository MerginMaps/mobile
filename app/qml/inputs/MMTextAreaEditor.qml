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

  property var parentValue: parent.value ?? ""
  property bool parentValueIsNull: parent.valueIsNull ?? false
  property bool isReadOnly: parent.readOnly ?? false

  property alias placeholderText: textArea.placeholderText
  property alias text: textArea.text

  signal editorValueChanged( var newValue, var isNull )

  hasFocus: textArea.activeFocus
  innerHeight: textArea.vertSpace + textArea.contentHeight + 2 * textArea.vertSpace

  content: TextArea {
    id: textArea

    property real vertSpace: 11 * __dp

    y: textArea.vertSpace
    height: contentHeight + textArea.vertSpace
    width: parent.width

    hoverEnabled: true
    placeholderTextColor: __style.nightAlphaColor
    color: root.enabled ? __style.nightColor : __style.mediumGreenColor
    font: __style.p5
    wrapMode: Text.WordWrap
  }
}
