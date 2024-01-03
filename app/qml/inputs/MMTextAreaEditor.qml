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

  property alias placeholderText: textArea.placeholderText
  property alias text: textArea.text

  signal editorValueChanged( var newValue, var isNull )

  hasFocus: textArea.activeFocus
  contentItemHeight: {
    const minHeight = 34 * __dp + metrics.height * 3 // min height for 3 rows
    var realHeight = textArea.y + textArea.contentHeight + 2 * textArea.verticalPadding
    return realHeight < minHeight ? minHeight : realHeight
  }

  content: TextArea {
    id: textArea

    property real verticalPadding: 11 * __dp

    y: textArea.verticalPadding
    height: contentHeight + textArea.verticalPadding
    width: parent.width

    hoverEnabled: true
    placeholderTextColor: __style.nightAlphaColor
    color: root.enabled ? __style.nightColor : __style.mediumGreenColor
    font: __style.p5
    wrapMode: Text.WordWrap
  }

  FontMetrics {
    id: metrics
    font: textArea.font
  }
}
