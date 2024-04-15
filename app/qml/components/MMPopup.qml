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

Popup {
  id: root

  modal: true
  dim: false

  background: Rectangle {
    color: __style.polarColor
    radius: 20 * __dp

    layer.enabled: true
    layer.effect: MMShadow {
      radius: 20 * __dp
    }
  }
}
