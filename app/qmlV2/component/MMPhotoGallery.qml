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
import Qt5Compat.GraphicalEffects
import "../Style.js" as Style
import "."

Item {
  width: parent.width
  height: column.height

  property alias model: rowView.model

  Column {
    id: column

    padding: 20 * __dp
    spacing: 10 * __dp
    width: parent.width - 40 * __dp

    Item {
      width: parent.width
      height: 15 * __dp

      Text {
        text: "Gallery"
      }

      Text {
        anchors.right: parent.right
        text: "Show all"
      }
    }

    ScrollView {
      width: parent.width
      height: 130 + __dp

      ListView {
        id: rowView

        model: 5
        spacing: 20 * __dp
        orientation: ListView.Horizontal
        width: 200
        height: 120 * __dp
        delegate: Image {
          id: img
          width: 120 * __dp
          height: width
          source: modelData
          layer.enabled: true
          layer.effect: OpacityMask {
            maskSource: Item {
              width: img.width
              height: img.height
              Rectangle {
                anchors.centerIn: parent
                width: parent.width
                height: parent.height
                radius: 20 * __dp
              }
            }
          }
        }
      }
    }

    Text {
      text: "The size of image is too big"
      visible: false
    }
  }

}
