/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.14

import lc 1.0
import ".."

Item {    
    id: root

    property real size: 100 * __dp

    property point center: Qt.point( root.width / 2, root.height / 2 )

    Image {
        anchors.centerIn: parent
        height: root.size
        width: height
        source: InputStyle.crosshairIcon
        sourceSize.width: width
        sourceSize.height: height
    }
}
