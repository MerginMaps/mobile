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
import QtQuick.Controls.Basic
import "../components"

MMAbstractEditor {
  id: root

  property var parentValue: parent.value ?? 0
  property bool parentValueIsNull: parent.valueIsNull ?? false
  property bool isReadOnly: parent.readOnly ?? false

  property var locale: Qt.locale()
  // TODO: uncomment in Input app
  property real precision//: config['Precision'] ? config['Precision'] : 0
  property string suffix//: config['Suffix'] ? config['Suffix'] : ''
  property real from //: config["Min"]
  property real to //: config["Max"]

  property alias placeholderText: numberInput.placeholderText

  // don't ever use a step smaller than would be visible in the widget
  // i.e. if showing 2 decimals, smallest increment will be 0.01
  // https://github.com/qgis/QGIS/blob/a038a79997fb560e797daf3903d94c7d68e25f42/src/gui/editorwidgets/qgsdoublespinbox.cpp#L83-L87
  property real step//: Math.max(config["Step"], Math.pow( 10.0, 0.0 - precision ))

  signal editorValueChanged( var newValue, var isNull )

  enabled: !isReadOnly
  hasFocus: numberInput.activeFocus

  leftAction: MMIcon {
    id: leftIcon

    height: parent.height

    source: __style.minusIcon
    color: enabled ? __style.forestColor : __style.mediumGreenColor
    enabled: Number( numberInput.text ) - root.step >= root.from
  }

  content: Item {
    anchors.fill: parent
    Row {
      height: parent.height
      anchors.horizontalCenter: parent.horizontalCenter
      clip: true

      TextField {
        id: numberInput

        height: parent.height

        clip: true
        text: root.parentValue === undefined || root.parentValueIsNull ? "" : root.parentValue
        color: root.enabled ? __style.nightColor : __style.mediumGreenColor
        placeholderTextColor: __style.nightAlphaColor
        font: __style.p5
        hoverEnabled: true
        verticalAlignment: Qt.AlignVCenter
        inputMethodHints: Qt.ImhFormattedNumbersOnly

        onTextEdited: {
          let val = text.replace( ",", "." ).replace( / /g, '' ) // replace comma with dot

          root.editorValueChanged( val, val  === "" )
        }

        background: Rectangle {
          color: __style.transparentColor
        }
      }

      Text {
        id: suffix

        text: root.suffix

        visible: root.suffix !== "" && numberInput.text !== ""

        height: parent.height
        verticalAlignment: Qt.AlignVCenter

        font: __style.p5
        color: numberInput.color
      }
    }
  }

  rightAction: MMIcon {
    id: rightIcon

    height: parent.height

    source: __style.plusIcon
    color: enabled ? __style.forestColor : __style.mediumGreenColor
    enabled: Number( numberInput.text ) + root.step <= root.to
  }

  onLeftActionClicked: {
    numberInput.forceActiveFocus()
    if ( leftIcon.enabled ) {
      let decremented = Number( numberInput.text ) - root.step
      root.editorValueChanged( decremented.toFixed( root.precision ), false )
    }
  }
  onRightActionClicked: {
    numberInput.forceActiveFocus();
    if ( rightIcon.enabled ) {
      let incremented = Number( numberInput.text ) + root.step
      root.editorValueChanged( incremented.toFixed( root.precision ), false )
    }
  }
}
