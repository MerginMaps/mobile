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
import "../../components"
import "../../inputs"

MMBaseInput {
  id: root

  property var parentValue: parent.value
  property bool isReadOnly: parent.readOnly ?? false

  property alias placeholderText: textField.placeholderText
  property alias text: textField.text

  signal editorValueChanged( var newValue, bool isNull )

  hasFocus: textField.activeFocus
  enabled: !root.isReadOnly

  content: TextField {
    id: textField

    anchors.fill: parent
    anchors.verticalCenter: parent.verticalCenter

    text: root.parentValue !== undefined ? root.parentValue : ''
    readOnly: !root.enabled
    color: root.enabled ? __style.nightColor : __style.mediumGreenColor
    placeholderTextColor: __style.nightAlphaColor
    font: __style.p5
    hoverEnabled: true
    background: Rectangle {
      color: __style.transparentColor
    }

    onTextChanged: root.editorValueChanged( text, text === "" )
  }

  rightAction: MMIcon {
    property bool pressed: false

    height: parent.height

    source: __style.qrCodeIcon
    color: root.enabled ? __style.forestColor : __style.mediumGreenColor
  }

  onRightActionClicked: {
    if ( !root.enabled )
      return
    if (!__inputUtils.acquireCameraPermission())
      return
    codeScannerLoader.active = true
    codeScannerLoader.focus = true
  }

  Loader {
    id: codeScannerLoader

    asynchronous: true
    active: false
    sourceComponent: readerComponent
  }

  Component {
    id: readerComponent

    MMCodeScanner {
      focus: true

      Component.onCompleted: open()
      onClosed: codeScannerLoader.active = false
      onScanFinished: function( captured ) {
        root.editorValueChanged( captured, false )
        codeScannerLoader.active = false
      }
    }
  }
}
