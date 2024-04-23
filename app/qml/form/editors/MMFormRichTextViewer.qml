/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../../components/private" as MMPrivateComponents

MMPrivateComponents.MMBaseInput {
  id: root

  property var _fieldValue: parent.fieldValue
  property var _fieldConfig: parent.fieldConfig

  property bool _fieldShouldShowTitle: parent.fieldShouldShowTitle

  property string _fieldTitle: parent.fieldTitle
  property string _fieldHomePath: parent.fieldHomePath

  title: _fieldShouldShowTitle ? _fieldTitle : ""

  inputContent: Rectangle { // background
    width: parent.width
    height: textArea.implicitHeight
    color: __style.polarColor
    radius: __style.radius12

    Text { // intentionally not MMText to that bottom padding works correctly in rich text mode
      id: textArea

      width: parent.width

      wrapMode: Text.Wrap
      font: __style.p5
      color: __style.nightColor

      text: root._fieldValue !== undefined ? root._fieldValue : ''
      textFormat: root._fieldConfig['UseHtml'] ? TextEdit.RichText : TextEdit.PlainText


      topPadding: __style.margin12
      bottomPadding: __style.margin12
      leftPadding: __style.margin20
      rightPadding: __style.margin20

      onLinkActivated: ( link ) => __inputUtils.openLink( root._fieldHomePath, link.toString() )
    }
  }
}
