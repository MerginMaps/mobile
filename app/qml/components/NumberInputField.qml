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

import ".."

Item {
  id: root

  property real number
  property string suffix: ""

  signal valueChanged( var value )

  function getFocus() {
    numberInput.forceActiveFocus()
  }

  TextInput {
    id: numberInput

    onTextEdited: {
      let value = text.replace( ",", "." ).replace( / /g, '' ) // replace comma with dot and remove whitespaces

      if ( !isNaN( value ) ) // do not emit when value is not a number
      {
        root.valueChanged( Number( value ) )
      }
    }

    onPreeditTextChanged: if ( __androidUtils.isAndroid ) Qt.inputMethod.commit() // to avoid Android's uncommited text

    text: root.number

    height: parent.height
    width: parent.width * 0.8

    inputMethodHints: Qt.ImhFormattedNumbersOnly

    font.pixelSize: InputStyle.fontPixelSizeNormal
    color: InputStyle.fontColorBright
    selectionColor: InputStyle.fontColorBright
    selectedTextColor: "#ffffff"

    horizontalAlignment: Qt.AlignRight
    verticalAlignment: Qt.AlignVCenter

    clip: true
  }

  Text {
    id: suffixText

    font.pixelSize: InputStyle.fontPixelSizeNormal
    color: InputStyle.fontColorBright

    horizontalAlignment: Qt.AlignLeft
    verticalAlignment: Qt.AlignVCenter

    text: root.suffix

    height: parent.height
    width: parent.width * 0.2

    anchors {
      left: numberInput.right
      right: parent.right
      verticalCenter: parent.verticalCenter
    }

    MouseArea {
      anchors.fill: parent
      onClicked: numberInput.forceActiveFocus()
    }
  }
}

