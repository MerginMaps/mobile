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
import QtQuick.Controls.Basic
import "../Style.js" as Style

Item {
  id: control

  signal clicked

  required property var iconSource
  required property string text

  height: Style.toolbarHeight

  Button {
    width: {
      var w = parent.width - Style.commonSpacing/2
      if(w < Style.minimumToolbarLongButtonWidth)
        return Style.minimumToolbarLongButtonWidth
      else if(w > Style.maximumToolbarLongButtonWidth)
        return Style.maximumToolbarLongButtonWidth
      return w
    }
    height: Style.toolbarLongButtonWidth
    anchors.centerIn: parent

    contentItem: Item {
      width: parent.width
      anchors.centerIn: parent

      Row {
        id: row

        spacing: 5 * __dp
        height: parent.height
        anchors.centerIn: parent

        MMIcon {
          source: control.iconSource
          color: text.color
        }
        Text {
          id: text

          text: control.text
          color: Style.forest
          font: Qt.font(Style.t3)
          verticalAlignment: Text.AlignVCenter
          topPadding: 10 * __dp
          bottomPadding: 10 * __dp
        }
      }
    }

    background: Rectangle {
      color: Style.grass
      radius: height / 2
    }

    onClicked: control.clicked()
  }
}
