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
import "."

Rectangle {
  id: root

  color: __style.polarColor
  radius: 20 * __dp

  property bool canDeviceTakePictures: true

  signal capturePhotoClicked()
  signal chooseFromGalleryClicked()

  Row {
    anchors.centerIn: parent
    width: parent.width - 40 * __dp
    height: parent.height - 40 * __dp
    spacing: 20 * __dp

    Rectangle {
      id: takePictureBox

      width: canDeviceTakePictures ? parent.width / 2 - parent.spacing / 2 : 0
      height: parent.height
      color: __style.lightGreenColor
      radius: root.radius
      visible: canDeviceTakePictures

      Column {
        width: parent.width
        anchors.centerIn: parent
        spacing: 8 * __dp

        MMIcon {
          anchors.horizontalCenter: parent.horizontalCenter
          source: __style.addImageIcon
          color: __style.forestColor
        }

        Text {
          width: parent.width
          height: 24 * __dp
          font: __style.p6
          text: qsTr("Take a picture")
          color: __style.nightColor
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          elide: Text.ElideMiddle
        }
      }

      MouseArea {
        anchors.fill: parent
        onClicked: root.capturePhotoClicked()
      }
    }

    Rectangle {
      id: chooseFromGaleryBox

      width: canDeviceTakePictures ? parent.width / 2 - parent.spacing / 2 : parent.width
      height: parent.height
      color: __style.lightGreenColor
      radius: root.radius

      Column {
        width: parent.width
        anchors.centerIn: parent
        spacing: 8 * __dp

        MMIcon {
          anchors.horizontalCenter: parent.horizontalCenter
          source: __style.morePhotosIcon
          color: __style.forestColor
        }

        Text {
          width: parent.width
          height: 24 * __dp
          font: __style.p6
          text: qsTr("From gallery")
          color: __style.nightColor
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          elide: Text.ElideMiddle
        }
      }

      MouseArea {
        anchors.fill: parent
        onClicked: root.chooseFromGalleryClicked()
      }
    }
  }
}
