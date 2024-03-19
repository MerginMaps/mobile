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
  id: root

  property real size: 60 * __dp
  property alias running: root.visible
  property alias icon: iconComponent
  property int speed: 800

  width: root.size
  height: root.size
  visible: false

  Image {
    id: iconComponent
    anchors.fill: parent
    source: __style.loadingIndicatorImage
    sourceSize.width: width
    sourceSize.height: height
    fillMode: Image.PreserveAspectFit
  }

  RotationAnimation {
    target: iconComponent

    from: 0
    to: 360

    duration: root.speed

    running: root.running
    loops: Animation.Infinite
  }
}
