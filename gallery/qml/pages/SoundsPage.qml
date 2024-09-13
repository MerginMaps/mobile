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
import QtMultimedia

import "../../app/qml/components"
import "../components" as GalleryComponents

ScrollView {

  Column {
    padding: 20

    Row {
      Column {
        spacing: 5

        GroupBox {
          title: "MMSound"
          background: Rectangle {
            color: "white"
            border.color: "gray"
          }
          label: Text {
            color: "black"
            text: parent.title
            padding: 5
          }

          Column {
            spacing: 5

            SoundEffect {
                id: successSound
                source: __style.successSound
            }

            Button {
              text: "Success"
              onClicked: successSound.play()
            }
          }
        }
      }
    }
  }
}
