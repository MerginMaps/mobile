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
import "../"  // import InputStyle singleton

Text {
  color: InputStyle.fontColor
  font.bold: true
  font.pixelSize: InputStyle.fontPixelSizeNormal
  height: parent.height
  horizontalAlignment: Qt.AlignHCenter
  linkColor: InputStyle.highlightColor
  text: "(no-text)"
  textFormat: Text.StyledText
  verticalAlignment: Qt.AlignVCenter
  width: parent.width
  wrapMode: Text.WordWrap

  onLinkActivated: Qt.openUrlExternally(link)
}
