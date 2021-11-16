/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.14
import "../"

Symbol {
  id: root
  property bool running: false

  source: InputStyle.loadingIndicatorIcon
  visible: running

  RotationAnimation {
    duration: 800
    from: 0
    loops: Animation.Infinite
    running: root.running
    target: root
    to: 360
  }
}
