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

  /*required*/
  property var config: parent.config
  /*required*/
  property bool isReadOnly: parent.readOnly
  /*required*/
  property var parentValue: parent.value

  height: textArea.topPadding + textArea.bottomPadding + textArea.contentHeight

  signal editorValueChanged(var newValue, bool isNull)
  signal importDataRequested

  onRightActionClicked: root.importDataRequested()

  content: TextArea {
    id: textArea
    anchors.fill: parent
    bottomPadding: customStyle.fields.height * 0.25
    color: customStyle.fields.fontColor
    font.pointSize: customStyle.fields.fontPointSize
    leftPadding: customStyle.fields.sideMargin
    readOnly: root.isReadOnly
    rightPadding: customStyle.fields.sideMargin
    text: root.parentValue !== undefined ? root.parentValue : ''
    textFormat: config['UseHtml'] ? TextEdit.RichText : TextEdit.PlainText
    topPadding: customStyle.fields.height * 0.25
    wrapMode: Text.Wrap

    onLinkActivated: Qt.openUrlExternally(link)
    onTextChanged: root.editorValueChanged(text, text === "")
  }
  rightAction: Item {
    anchors.fill: parent

    Image {
      id: importDataBtnIcon
      source: customStyle.icons.importData
      sourceSize.width: parent.width * 0.6
      visible: !root.isReadOnly
      width: parent.width * 0.6
      x: parent.x + parent.width - 1.5 * width
      y: parent.y + parent.height / 2 - height / 2
    }
    ColorOverlay {
      anchors.fill: importDataBtnIcon
      color: root.parent.readOnly ? customStyle.toolbutton.backgroundColorInvalid : customStyle.fields.fontColor
      source: importDataBtnIcon
    }
  }
}
