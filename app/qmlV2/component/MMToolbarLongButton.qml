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
import ".."

Item {
  id: control

  signal clicked

  required property var iconSource
  required property string text

  readonly property double toolbarLongButtonWidth: 50 * __dp
  readonly property double minimumToolbarLongButtonWidth: 200 * __dp
  readonly property double maximumToolbarLongButtonWidth: 500 * __dp

  height: styleV2.toolbarHeight

  StyleV2 { id: styleV2 }

  Button {
    width: {
      var w = parent.width - 10 * __dp
      if(w < control.minimumToolbarLongButtonWidth)
        return control.minimumToolbarLongButtonWidth
      else if(w > control.maximumToolbarLongButtonWidth)
        return control.maximumToolbarLongButtonWidth
      return w
    }
    height: control.toolbarLongButtonWidth
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
          color: styleV2.forestColor
          font: styleV2.t3
          verticalAlignment: Text.AlignVCenter
          topPadding: 10 * __dp
          bottomPadding: 10 * __dp
        }
      }
    }

    background: Rectangle {
      color: styleV2.grassColor
      radius: height / 2
    }

    onClicked: control.clicked()
  }
}
