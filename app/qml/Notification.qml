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
    modal: false
    focus: true
    opacity: 1
    property string text: ""
    margins: -7
    background: Rectangle {
        anchors.fill: parent
        color: InputStyle.fontColorBright
        opacity: 0.5
    }
    onOpened: timer.start()

    Text {
        anchors.fill: parent
        text: popup.text
        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    Timer {
        id: timer
        interval: 2000
        onTriggered: popup.close()
        running: false
        repeat: false
    }
}
