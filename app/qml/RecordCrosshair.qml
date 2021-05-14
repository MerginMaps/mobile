/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.7
import QgsQuick 0.1 as QgsQuick
import lc 1.0

Item {    
    id: root
    property real size: 100 * QgsQuick.Utils.dp

    Image {
        anchors.centerIn: parent
        height: root.size
        width: height
        source: "crosshair.svg"
        sourceSize.width: width
        sourceSize.height: height
    }
}
