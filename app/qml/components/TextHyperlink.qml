/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Controls
import ".."  // import InputStyle singleton

Text {
    width: parent.width
    height: parent.height
    color: InputStyle.fontColor
    onLinkActivated: function( link ) {
      Qt.openUrlExternally(link)
    }
    textFormat: Text.StyledText
    wrapMode: Text.WordWrap
    font.pixelSize: InputStyle.fontPixelSizeNormal
    font.bold: true
    horizontalAlignment: Qt.AlignHCenter
    verticalAlignment: Qt.AlignVCenter
    text: "(no-text)"
    linkColor: InputStyle.highlightColor
}
