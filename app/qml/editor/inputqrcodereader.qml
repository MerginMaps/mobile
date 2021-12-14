/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.14
import QtQuick.Controls 2.14
import QtGraphicalEffects 1.14

AbstractEditor {
  id: root

  /*required*/ property var config: parent.config
  /*required*/ property var parentValue: parent.value
  /*required*/ property bool isReadOnly: parent.readOnly

  signal editorValueChanged( var newValue, bool isNull )
  signal importDataRequested()

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

    onLinkActivated: Qt.openUrlExternally( link )

    onTextChanged: root.editorValueChanged( text, text === "" )
  }

  rightAction: Item {
    anchors.fill: parent

    Image {
      id: importDataBtnIcon

      y: parent.y + parent.height / 2 - height / 2
      x: parent.x + parent.width - 1.5 * width

      width: parent.width * 0.6
      sourceSize.width: parent.width * 0.6

      source: customStyle.icons.importData
      visible: !root.isReadOnly
    }

    ColorOverlay {
      source: importDataBtnIcon
      color: root.parent.readOnly ? customStyle.toolbutton.backgroundColorInvalid : customStyle.fields.fontColor
      anchors.fill: importDataBtnIcon
    }
  }

  onRightActionClicked: root.importDataRequested()
}
