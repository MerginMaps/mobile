/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

pragma ComponentBehavior: Bound

import QtQuick

//
// Compass drawn on top of a map button, similar to the compass of other map apps: the ticks around the edge
// follow the rotation of the map (the north is the red one), and the letter in the middle is fixed and
// tells which cardinal point is at the top of the screen.
//

Item {
  id: root

  // length, width and color of one kind of tick
  component TickStyle: QtObject {
    property real length
    property real width
    property color color
  }

  // size of the button the compass is drawn on
  property real size: __style.mapItemHeight

  // rotation of the map in degrees, the north tick is rotated with it
  property real mapRotation: 0

  readonly property var cardinalPoints: [
    //: Short name of the north, shown in the compass on the map
    qsTr( "N" ),
    //: Short name of the east, shown in the compass on the map
    qsTr( "E" ),
    //: Short name of the south, shown in the compass on the map
    qsTr( "S" ),
    //: Short name of the west, shown in the compass on the map
    qsTr( "W" )
  ]

  // a tick every 15 degrees, N, E, S and W get the main ticks
  readonly property int tickCount: 24
  readonly property int ticksPerCardinal: tickCount / cardinalPoints.length

  // all sizes are relative to the size of the compass
  readonly property real tickMargin: size * 0.07 // distance of the ticks from the edge
  readonly property real letterSize: size * 0.34

  readonly property TickStyle northTick: TickStyle { length: root.size * 0.12; width: root.size * 0.045; color: __style.grapeColor }
  readonly property TickStyle mainTick: TickStyle { length: root.size * 0.12; width: root.size * 0.035; color: __style.forestColor }
  readonly property TickStyle secondaryTick: TickStyle { length: root.size * 0.07; width: root.size * 0.025; color: __style.mediumGreyColor }

  // cardinal point closest to the top of the screen (the map rotating clockwise turns the top towards the west),
  // the rotation is within a full turn (the map settings keep it in (-180, 180])
  readonly property int cardinalIndex: ( Math.round( -mapRotation / 360 * cardinalPoints.length ) + cardinalPoints.length ) % cardinalPoints.length

  width: size
  height: size

  // the only rotating node: the ticks turn with the map, the letter stays upright
  Item {
    id: ticks

    anchors.fill: parent
    rotation: root.mapRotation

    Repeater {
      model: root.tickCount

      // each tick is placed at the top of the compass and turned around the compass center
      Rectangle {
        id: tick

        required property int index

        readonly property TickStyle tickStyle: {
          if ( index === 0 ) return root.northTick
          if ( index % root.ticksPerCardinal === 0 ) return root.mainTick
          return root.secondaryTick
        }

        x: ( root.size - width ) / 2
        y: root.tickMargin
        width: tickStyle.width
        height: tickStyle.length

        radius: width / 2
        antialiasing: true
        color: tickStyle.color
        transform: Rotation {
          origin.x: tick.width / 2
          origin.y: root.size / 2 - root.tickMargin // center of the compass, relative to the tick
          angle: tick.index * 360 / root.tickCount
        }
      }
    }
  }

  Text {
    anchors.centerIn: parent

    text: root.cardinalPoints[root.cardinalIndex]
    color: root.mainTick.color
    font {
      family: __style.t1.family
      weight: __style.t1.weight
      pixelSize: root.letterSize
    }
  }
}
