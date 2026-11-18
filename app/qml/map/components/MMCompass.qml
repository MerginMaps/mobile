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
// Compass drawn on top of a map button, similar to the compass of other map apps: the ticks around the edge
// follow the rotation of the map (the north is the red one), and the letter in the middle is fixed and
// tells which cardinal point is at the top of the screen.
//

Item {
  id: root

  // size of the button the compass is drawn on
  property real size: __style.mapItemHeight

  // rotation of the map in degrees, the north tick is rotated with it
  property real mapRotation: 0

  readonly property color northColor: __style.grapeColor
  readonly property color mainColor: __style.forestColor
  readonly property color secondaryColor: __style.mediumGreyColor

  readonly property real tickMargin: size * 0.07 // distance of the ticks from the edge
  readonly property real mainTickLength: size * 0.12
  readonly property real secondaryTickLength: size * 0.07

  // bearing of the top of the screen, rounded to the closest cardinal point
  readonly property int cardinalIndex: Math.round( ( ( 360 - mapRotation ) % 360 + 360 ) % 360 / 90 ) % 4

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

  width: size
  height: size

  // smooth edges of the shapes
  layer.enabled: true
  layer.samples: 4

  // full size item, so that the ticks rotate around the center of the compass
  Item {
    anchors.fill: parent
    rotation: root.mapRotation

    Repeater {
      model: 24 // a tick every 15 degrees

      Item {
        required property int index

        readonly property bool isMain: index % 6 === 0 // north, east, south, west
        readonly property bool isNorth: index === 0

        anchors.fill: parent
        rotation: index * 15

        Rectangle {
          x: root.width / 2 - width / 2
          y: root.tickMargin
          width: {
            if ( parent.isNorth ) return root.size * 0.045
            if ( parent.isMain ) return root.size * 0.035
            return root.size * 0.025
          }
          height: parent.isMain ? root.mainTickLength : root.secondaryTickLength
          radius: width / 2
          antialiasing: true
          color: {
            if ( parent.isNorth ) return root.northColor
            if ( parent.isMain ) return root.mainColor
            return root.secondaryColor
          }
        }
      }
    }
  }

  Text {
    anchors.centerIn: parent
    text: root.cardinalPoints[root.cardinalIndex]
    color: root.mainColor
    font.family: __style.t1.family
    font.weight: __style.t1.weight
    font.pixelSize: root.size * 0.34
  }
}
