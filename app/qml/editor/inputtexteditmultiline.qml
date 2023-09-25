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

AbstractEditor {
  id: root

  /*required*/ property bool isReadOnly: parent.readOnly
  /*required*/ property var parentValue: parent.value
  /*required*/ property var config: parent.config

  signal editorValueChanged( var newValue, bool isNull )

  height: textArea.topPadding + textArea.bottomPadding + textArea.contentHeight

  content: TextArea {
    id: textArea

    readOnly: root.isReadOnly

    anchors.fill: parent

    topPadding: customStyle.fields.height * 0.25
    bottomPadding: customStyle.fields.height * 0.25
    leftPadding: customStyle.fields.sideMargin
    rightPadding: customStyle.fields.sideMargin

    wrapMode: Text.Wrap
    color: customStyle.fields.fontColor
    font.pixelSize: customStyle.fields.fontPixelSize

    text: root.parentValue !== undefined ? root.parentValue : ''
    textFormat: config['UseHtml'] ? TextEdit.RichText : TextEdit.PlainText

    onLinkActivated: function( link ) {
      Qt.openUrlExternally( link )
    }

    onTextChanged: root.editorValueChanged( text, text === "" )
  }
}
