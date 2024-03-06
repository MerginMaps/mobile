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
import "../components"

Item {
  id: root

  property var _fieldValue: parent.fieldValue
  property var _fieldConfig: parent.fieldConfig

  property real padding: 11 * __dp

  height: textArea.height
  width: parent.width

  Rectangle { // background
    width: root.width
    height: root.height
    border.width: 2 * __dp
    border.color: __style.transparentColor
    color: __style.whiteColor
    radius: __style.radius12
  }

  Text {
    id: textArea

    wrapMode: Text.Wrap
    font: __style.p5
    color: __style.nightColor

    text: root._fieldValue !== undefined ? root._fieldValue : ''
    textFormat: _fieldConfig['UseHtml'] ? TextEdit.RichText : TextEdit.PlainText

    width: root.width

    topPadding: root.padding
    bottomPadding: root.padding
    leftPadding: root.padding
    rightPadding: root.padding

    onLinkActivated: function( link ) {
      Qt.openUrlExternally( link )
    }
  }
}
