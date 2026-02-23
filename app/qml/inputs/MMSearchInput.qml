/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../components" as MMComponents
import "../components/private" as MMPrivateComponents

MMPrivateComponents.MMBaseSingleLineInput {
  id: root

  property bool delayedSearch: false
  property int emitInterval: 200
  property bool showClearIcon: true
  property string searchText: ""

  leftContent: MMComponents.MMIcon {
    id: searchIcon

    size: __style.icon24
    source: __style.searchIcon
    color: {
      if ( root.editState === "disabled" ) return __style.mediumGreyColor
      if ( root.validationState === "error" ) return __style.grapeColor
      if ( root.validationState === "warning" ) return __style.earthColor
      return __style.nightColor
    }
  }

  rightContent: MMComponents.MMIcon {
    id: rightIcon

    size: __style.icon24
    source: __style.closeIcon
    color: root.iconColor
  }

  rightContentVisible: root.showClearIcon && textField.activeFocus && root.text.length > 0

  onRightContentClicked: {
    if ( root.showClearIcon ) {
      textField.clear()
      root.searchText = ""
    }
    else {
      // if the clear button should not be there, let's open keyboard instead
      textField.forceActiveFocus()
    }
  }

  onTextEdited: {
    if ( root.delayedSearch ) {
      searchTimer.restart()
    }
    else
    {
      // trim the last whitespace from the user's input
      root.searchText = root.text.replace( /\s+$/, "" )
    }
  }

  Timer {
    id: searchTimer

    interval: root.emitInterval
    running: false

    onTriggered: { 
      // when using predictive text suggestions, a whitespace is sometimes added after the word
      // trim the last whitespace from the user's input
      root.searchText = root.text.replace( /\s+$/, "" )
    }
  }

  /**
    * Used for deactivating focus on MMSearchInput when another component should have focus.
    * and the current element's forceActiveFocus() doesnt deactivates SearchBar focus.
    */
  function deactivate() {
    root.textField.focus = false
    if ( root.text.length > 0 )
      root.textField.clear()
    root.searchText = ""
  }
}
