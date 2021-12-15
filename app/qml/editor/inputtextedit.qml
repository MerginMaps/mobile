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

/**
 * Text Edit for QGIS Attribute Form
 * Requires various global properties set to function, see featureform Loader section
 * Do not use directly from Application QML
 */

AbstractEditor {
  id: root

  /*required*/ property var parentValue: parent.value
  /*required*/ property bool parentValueIsNull: parent.valueIsNull
  /*required*/ property bool isReadOnly: parent.readOnly

  signal editorValueChanged( var newValue, bool isNull )

  content: TextField {
    id: textField

    anchors.fill: parent

    topPadding: customStyle.fields.height * 0.25
    bottomPadding: customStyle.fields.height * 0.25
    leftPadding: customStyle.fields.sideMargin
    rightPadding: customStyle.fields.sideMargin

    readOnly: root.isReadOnly

    font.pixelSize: customStyle.fields.fontPixelSize
    color: customStyle.fields.fontColor

    text: root.parentValue === undefined || root.parentValueIsNull ? '' : root.parentValue
    inputMethodHints: field.isNumeric ? Qt.ImhFormattedNumbersOnly : Qt.ImhNone

    states: [
      State {
        name: "limitedTextLengthState" // Make sure we do not input more characters than allowed for strings
        when: ( !field.isNumeric ) && ( field.length > 0 )
        PropertyChanges {
          target: textField
          maximumLength: field.length
        }
      }
    ]

    background: Rectangle {
      anchors.fill: parent

      radius: customStyle.fields.cornerRadius
      color: customStyle.fields.backgroundColor
    }

    onTextEdited: {
      let val = text
      if ( field.isNumeric )
      {
        val = val.replace( ",", "." ).replace( / /g, '' ) // replace comma with dot and remove spaces
      }

      editorValueChanged( val, val === "" )
    }

    onPreeditTextChanged: if ( __androidUtils.isAndroid ) Qt.inputMethod.commit() // to avoid Android's uncommited text
  }
}
