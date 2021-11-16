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
  /*required*/
  property bool isReadOnly: parent.readOnly

  /*required*/
  property var parentValue: parent.value
  /*required*/
  property bool parentValueIsNull: parent.valueIsNull

  signal editorValueChanged(var newValue, bool isNull)

  content: TextField {
    id: textField
    anchors.fill: parent
    bottomPadding: customStyle.fields.height * 0.25
    color: customStyle.fields.fontColor
    font.pointSize: customStyle.fields.fontPointSize
    inputMethodHints: field.isNumeric ? Qt.ImhFormattedNumbersOnly : Qt.ImhNone
    leftPadding: customStyle.fields.sideMargin
    readOnly: root.isReadOnly
    rightPadding: customStyle.fields.sideMargin
    text: root.parentValue === undefined || root.parentValueIsNull ? '' : root.parentValue
    topPadding: customStyle.fields.height * 0.25

    onPreeditTextChanged: Qt.inputMethod.commit() // to avoid Android's uncommited text
    onTextEdited: {
      let val = text;
      if (field.isNumeric) {
        val = val.replace(",", ".").replace(/ /g, ''); // replace comma with dot and remove spaces
      }
      editorValueChanged(val, val === "");
    }

    background: Rectangle {
      anchors.fill: parent
      color: customStyle.fields.backgroundColor
      radius: customStyle.fields.cornerRadius
    }
    states: [
      State {
        name: "limitedTextLengthState" // Make sure we do not input more characters than allowed for strings
        when: (!field.isNumeric) && (field.length > 0)

        PropertyChanges {
          maximumLength: field.length
          target: textField
        }
      }
    ]
  }
}
