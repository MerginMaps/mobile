/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

//
// Compass drawn on top of a map button. It is built on top of the location icon: a dot in the middle
// with four ticks, and the cardinal points as far as possible from the center (near the edge of the button).
// Rotate the item to point the N to the north.
//

Item {
  id: root

  // size of the button the compass is drawn on
  property real size: __style.mapItemHeight

  readonly property color northColor: "#E8553A"
  readonly property color letterColor: __style.forestColor
  readonly property real margin: size * 0.06
  readonly property real tickLength: size * 0.07
  readonly property real tickOffset: size * 0.13 // distance of the tick from the center

  width: size
  height: size

  Rectangle {
    anchors.centerIn: parent
    width: root.size * 0.1
    height: width
    radius: width / 2
    color: __style.forestColor
  }

  Repeater {
    model: 4

    Rectangle {
      required property int index

      width: root.tickLength
      height: root.size * 0.04
      radius: height / 2
      color: index === 0 ? root.northColor : __style.forestColor

      // ticks are placed on the north, east, south and west
      x: root.width / 2 - width / 2 + Math.sin( index * Math.PI / 2 ) * ( root.tickOffset + width / 2 )
      y: root.height / 2 - height / 2 - Math.cos( index * Math.PI / 2 ) * ( root.tickOffset + width / 2 )
      rotation: index % 2 === 0 ? 90 : 0
    }
  }

  component Letter: Text {
    color: root.letterColor
    font.family: internal.fontFamily
    font.pixelSize: root.size * 0.23
    font.weight: Font.DemiBold
  }

  Letter {
    text: "N"
    color: root.northColor
    anchors.horizontalCenter: parent.horizontalCenter
    y: root.margin - ( height - contentHeight ) / 2
  }

  Letter {
    text: "S"
    anchors.horizontalCenter: parent.horizontalCenter
    y: root.height - root.margin - contentHeight - ( height - contentHeight ) / 2
  }

  Letter {
    text: "W"
    x: root.margin
    anchors.verticalCenter: parent.verticalCenter
  }

  Letter {
    text: "E"
    x: root.width - root.margin - width
    anchors.verticalCenter: parent.verticalCenter
  }

  QtObject {
    id: internal

    // font of the device, not the font of the app
    readonly property string fontFamily: {
      if ( Qt.platform.os === "ios" || Qt.platform.os === "osx" ) return ".AppleSystemUIFont"
      return "sans-serif"
    }
  }
}
