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

  hasFocus: textField.activeFocus

  signal searchTextChanged( string text )

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

    height: parent.height

    source: __style.searchIcon
    color: root.enabled ? __style.nightColor : __style.mediumGreenColor
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
