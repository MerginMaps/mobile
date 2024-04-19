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

Item {
  implicitHeight: ApplicationWindow.window?.height ?? 0
  implicitWidth: ApplicationWindow.window?.width ?? 0

  // Make sure it is always rendered in front of everything else in the scene, including popups
  parent: Overlay.overlay
  z: 1

  Repeater {
    id: repeater

    anchors.top: parent.top
    width: parent.width
    clip: true

    model: __notificationModel
    delegate: MMNotification {
      x: __style.safeAreaLeft
      y: __style.safeAreaTop > __style.margin12 ? __style.safeAreaTop : __style.margin12
      width: repeater.width - 2 * __style.pageMargins - __style.safeAreaLeft - __style.safeAreaRight
      visible: index === repeater.count - 1 // visible only the last notification
    }
  }
}
