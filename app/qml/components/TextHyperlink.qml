/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.7
import QtQuick.Controls 2.2
import ".."  // import InputStyle singleton

Text {
    width: parent.width
    height: parent.height
    color: InputStyle.fontColor
    onLinkActivated: Qt.openUrlExternally(link)
    textFormat: Text.StyledText
    wrapMode: Text.WordWrap
    font.pixelSize: InputStyle.fontPixelSizeNormal
    font.bold: true
    horizontalAlignment: Qt.AlignHCenter
    verticalAlignment: Qt.AlignVCenter
    text: "(no-text)"
    linkColor: InputStyle.highlightColor
}
