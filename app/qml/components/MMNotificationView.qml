/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

Item {
  anchors.top: parent.top
  anchors.topMargin: 20 * __dp
  width: parent.width
  height: parent.height

  Repeater {
    id: repeater

    anchors.top: parent.top
    width: parent.width
    clip: true

    model: __notificationModel
    delegate: MMNotification {
      width: repeater.width - 40 * __dp
      visible: index === repeater.count - 1 // visible only the last notification
    }
  }
}
