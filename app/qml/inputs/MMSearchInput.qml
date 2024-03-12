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

MMBaseInput {
  id: root

  property alias placeholderText: textField.placeholderText
  property alias text: textField.text
  property bool allowTimer: false
  property int emitInterval: 200
  property bool showClearIcon: true

  hasFocus: textField.activeFocus

  signal searchTextChanged( string text )

  /**
    * Used for deactivating focus on MMSearchInput when another component should have focus.
    * and the current element's forceActiveFocus() doesnt deactivates SearchBar focus.
    */
  function deactivate() {
    textField.focus = false
    if ( textField.length > 0 )
      textField.clear()
    searchTextChanged("")
  }

  content: TextField {
    id: textField

    anchors.fill: parent
    anchors.verticalCenter: parent.verticalCenter

    color: root.enabled ? __style.nightColor : __style.mediumGreenColor
    placeholderTextColor: __style.nightAlphaColor
    font: __style.p5
    hoverEnabled: true
    background: Rectangle {
      color: __style.transparentColor
    }

    onDisplayTextChanged: {
      if ( root.allowTimer ) {
        if ( searchTimer.running )
          searchTimer.restart()
        else
          searchTimer.start()
      }
      else
      {
        root.searchTextChanged( textField.displayText )
      }
    }
  }

  leftAction: MMIcon {
    id: searchIcon

    property bool pressed: false

    anchors.verticalCenter: parent.verticalCenter

    size: __style.icon24
    source: __style.searchIcon
    color: root.enabled ? __style.nightColor : __style.mediumGreenColor
  }

  rightAction: MMIcon {
    id: rightIcon

    anchors.verticalCenter: parent.verticalCenter

    size: __style.icon24
    source: __style.closeIcon
    color: root.enabled ? __style.forestColor : __style.mediumGreenColor
    visible: root.showClearIcon && textField.activeFocus && textField.text.length > 0
  }

  onRightActionClicked: {
    if (root.showClearIcon) {
      textField.clear()
      root.searchTextChanged("")
    }
    else {
      // if the clear button should not be there, let's open keyboard instead
      textField.forceActiveFocus()
    }
  }

  Timer {
    id: searchTimer

    interval: root.emitInterval
    running: false

    onTriggered: {
      searchTextChanged( root.text )
    }
  }
}
