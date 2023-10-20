/***************************************************************************
 checkbox.qml
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
import "../components"

Item {
  id: root
  /*required*/ property var parentValue: parent.value
  /*required*/ property var config: parent.config

  height: textArea.height
  width: parent.width

  Rectangle { // background
    width: root.width
    height: root.height
    border.color: customStyle.fields.normalColor
    border.width: 1 * __dp
    color: customStyle.fields.backgroundColor
    radius: customStyle.fields.cornerRadius
  }

  Text {
    id: textArea

    wrapMode: Text.Wrap
    color: customStyle.fields.fontColor
    font.pixelSize: customStyle.fields.fontPixelSize

    text: root.parentValue !== undefined ? root.parentValue : ''
    textFormat: config['UseHtml'] ? TextEdit.RichText : TextEdit.PlainText

    width: root.width

    topPadding: customStyle.fields.height * 0.25
    bottomPadding: customStyle.fields.height * 0.25
    leftPadding: customStyle.fields.sideMargin
    rightPadding: customStyle.fields.sideMargin

    onLinkActivated: function( link ) {
      Qt.openUrlExternally( link )
    }
  }
}
