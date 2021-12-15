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
import QtQuick.Layouts 1.14
import QtGraphicalEffects 1.14

import lc 1.0

AbstractEditor {
  id: root

  /*required*/ property var parentValue: parent.value
  /*required*/ property bool parentValueIsNull: parent.valueIsNull
  /*required*/ property bool isReadOnly: parent.readOnly

  property var locale: Qt.locale()
  property real precision: config['Precision'] ? config['Precision'] : 0
  property string suffix: config['Suffix'] ? config['Suffix'] : ''

  // don't ever use a step smaller than would be visible in the widget
  // i.e. if showing 2 decimals, smallest increment will be 0.01
  // https://github.com/qgis/QGIS/blob/a038a79997fb560e797daf3903d94c7d68e25f42/src/gui/editorwidgets/qgsdoublespinbox.cpp#L83-L87
  property real step: Math.max(config["Step"], Math.pow( 10.0, 0.0 - precision ))

  signal editorValueChanged( var newValue, bool isNull )

  enabled: !isReadOnly

  leftAction: Item {
    id: minusSign

    anchors.fill: parent
    enabled: Number( numberInput.text ) - root.step >= config["Min"]

    Image {
      id: imgMinus

      anchors.centerIn: parent

      width: parent.width / 3
      sourceSize.width: parent.width / 3

      source: customStyle.icons.minus
    }

    ColorOverlay {
      source: imgMinus
      color: minusSign.enabled ? customStyle.fields.fontColor : customStyle.toolbutton.backgroundColorInvalid
      anchors.fill: imgMinus
    }
  }

  onLeftActionClicked: {
    if ( minusSign.enabled )
    {
      let decremented = Number( numberInput.text ) - root.step
      root.editorValueChanged( decremented.toFixed( root.precision ), false )
    }
  }

  content: Item {
    id: contentContainer

    anchors.fill: parent

    Row {
      id: inputAndSuffixContainer

      x: parent.width / 2 - width / 2

      width: numberInput.contentWidth + suffix.contentWidth > parent.width ? parent.width : numberInput.contentWidth + suffix.contentWidth

      height: parent.height

      TextInput {
        id: numberInput

        onTextEdited: {
          let val = text.replace( ",", "." ).replace( / /g, '' ) // replace comma with dot
          root.editorValueChanged( val, val  === "" )
        }

        text: root.parentValue === undefined || root.parentValueIsNull ? "" : root.parentValue

        height: parent.height
        width: contentWidth < ( parent.width - suffix.width ) ? contentWidth : parent.width - suffix.width

        inputMethodHints: Qt.ImhFormattedNumbersOnly

        font.pixelSize: customStyle.fields.fontPixelSize
        color: customStyle.fields.fontColor
        selectionColor: customStyle.fields.fontColor
        selectedTextColor: "#ffffff"

        horizontalAlignment: Qt.AlignRight
        verticalAlignment: Qt.AlignVCenter

        clip: true
      }

      Text {
        id: suffix

        text: root.suffix

        visible: root.suffix !== "" && numberInput.text !== ""

        height: parent.height
        width: paintedWidth
        horizontalAlignment: Qt.AlignLeft
        verticalAlignment: Qt.AlignVCenter

        color: customStyle.fields.fontColor
        font.pixelSize: customStyle.fields.fontPixelSize
      }
    }
  }

  onContentClicked: {
    numberInput.forceActiveFocus()
  }

  rightAction: Item {
    id: plusSign

    anchors.fill: parent

    enabled: Number( numberInput.text ) + root.step <= config["Max"]

    Image {
      id: imgPlus

      anchors.centerIn: parent

      width: parent.width / 3
      sourceSize.width: parent.width / 3

      source: customStyle.icons.plus
    }

    ColorOverlay {
      source: imgPlus
      color: plusSign.enabled ? customStyle.fields.fontColor : customStyle.toolbutton.backgroundColorInvalid
      anchors.fill: imgPlus
    }
  }

  onRightActionClicked: {
    if ( plusSign.enabled )
    {
      let incremented = Number( numberInput.text ) + root.step
      root.editorValueChanged( incremented.toFixed( root.precision ), false )
    }

    // on press and hold behavior can be used from here:
    // https://github.com/mburakov/qt5/blob/93bfa3874c10f6cb5aa376f24363513ba8264117/qtquickcontrols/src/controls/SpinBox.qml#L306-L309
  }
}
