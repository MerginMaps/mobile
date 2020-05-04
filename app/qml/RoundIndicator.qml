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
    width: parent.height
    height: width
    property int size: width
    property color color: InputStyle.softGreen
    property bool isActive: false

    Rectangle {
        anchors.centerIn: parent
        width: gpsSignal.size
        height: gpsSignal.size
        color: gpsSignal.color
        radius: width*0.5
        antialiasing: true
    }

    Rectangle {
        id: activeIndicator
        anchors.centerIn: parent
        width: gpsSignal.size/2.0
        height: gpsSignal.size/2.0
        color: "white"
        radius: width*0.5
        antialiasing: true
        visible: isActive
    }
}
