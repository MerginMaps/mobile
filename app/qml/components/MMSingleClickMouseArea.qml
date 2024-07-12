/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

/**
  * MMSingleClickMouseArea enhances MouseArea by preventing multiple clicks within a
  * specified time frame, ensuring consistent behavior and avoiding unintended actions
  * Similar to MouseArea, but it should be used with onSingleClicked signal handling instead of onClicked
*/

MouseArea {
  id: root

  signal singleClicked()

  onClicked: {
    if ( !root.enabled ) {
      mouse.accepted = true;
      return;
    }

    root.enabled = false;
    singleClicked()
    timer.start()
  }

  Timer {
    id: timer
    interval: 2000
    repeat: false
    onTriggered: root.enabled = true
  }
}
