/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.7

Item {
  id: gpsSignal
  property color color: InputStyle.softGreen
  property bool isActive: false
  property int size: width

  height: width
  width: parent.height

  Rectangle {
    anchors.centerIn: parent
    antialiasing: true
    color: gpsSignal.color
    height: gpsSignal.size
    radius: width * 0.5
    width: gpsSignal.size
  }
  Rectangle {
    id: activeIndicator
    anchors.centerIn: parent
    antialiasing: true
    color: "white"
    height: gpsSignal.size / 2.0
    radius: width * 0.5
    visible: isActive
    width: gpsSignal.size / 2.0
  }
}
