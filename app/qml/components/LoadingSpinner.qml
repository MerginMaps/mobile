/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import ".."

Symbol {
  id: root

  property bool running: false
  property int speed: 800

  visible: running
  source: InputStyle.loadingIndicatorIcon

  RotationAnimation {
    target: root

    from: 0
    to: 360

    duration: root.speed

    running: root.running
    loops: Animation.Infinite
  }
}
