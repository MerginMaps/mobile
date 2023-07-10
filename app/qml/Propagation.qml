/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

pragma Singleton
import QtQuick

// workaround about propagation issues (can be removed after fix: dev-2.3_CU-861mp5x42_propagationFix)
Timer {
  id: propagationTimer
  interval: 300

  property bool canClick: true

  onTriggered: canClick = true
  onCanClickChanged: {
    if(!canClick) {
      propagationTimer.running = true
    }
  }
}
