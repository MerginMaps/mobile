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
import Qt5Compat.GraphicalEffects

import lc 1.0
import ".."

AbstractEditor {
  id: root

  /*required*/ property var config: parent.config
  /*required*/ property var parentValue: parent.value
  /*required*/ property bool isReadOnly: parent.readOnly
  property StackView formView: parent.formView

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

  rightAction: Item {
    anchors.fill: parent

    Image {
      id: importDataBtnIcon

      y: parent.y + parent.height / 2 - height / 2
      x: parent.x + parent.width - 1.5 * width

      width: parent.width * 0.6
      sourceSize.width: parent.width * 0.6

      source: InputStyle.qrCodeIcon
      visible: !root.isReadOnly
    }

    ColorOverlay {
      source: importDataBtnIcon
      color: root.parent.readOnly ? customStyle.toolbutton.backgroundColorInvalid : customStyle.fields.fontColor
      anchors.fill: importDataBtnIcon
    }
  }

  onRightActionClicked: {
    if ( root.parent.readOnly ) return

    let page = root.formView.push( readerComponent, {} )
    page.forceActiveFocus()
  }

  Component {
    id: readerComponent

    CodeScanner {
      id: codeScannerPage

      focus: true

      onBackButtonClicked: {
        root.formView.pop()
      }

      onScanFinished: function( captured ) {
        root.editorValueChanged( captured, false )
        root.formView.pop()
      }
    }
  }
}
