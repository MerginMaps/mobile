/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.7
import QtQuick.Controls 2.2
import "."  // import InputStyle singleton

Popup {
  id: popup
  property string text: ""

  focus: true
  margins: -7
  modal: false
  opacity: 1

  onOpened: timer.start()

  Text {
    anchors.fill: parent
    color: InputStyle.fontColor
    horizontalAlignment: Text.AlignHCenter
    text: popup.text
    verticalAlignment: Text.AlignVCenter
    wrapMode: Text.Wrap
  }
  Timer {
    id: timer
    interval: 3000
    repeat: false
    running: false

    onTriggered: popup.close()
  }

  background: Rectangle {
    anchors.fill: parent
    border.color: InputStyle.fontColor
    border.width: 5
    color: InputStyle.panelBackgroundLight
    opacity: 1
  }
}
