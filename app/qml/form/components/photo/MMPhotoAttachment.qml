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

import "../../../components" as MMComponents

Rectangle {
  id: root

  color: __style.polarColor
  radius: __style.radius20

  property bool hasCameraCapability: true

  signal capturePhotoClicked()
  signal chooseFromGalleryClicked()

  Row {
    anchors.centerIn: parent
    width: parent.width - 40 * __dp
    height: parent.height - 40 * __dp
    spacing: 20 * __dp

    Rectangle {
      id: takePictureBox

      width: hasCameraCapability ? parent.width / 2 - parent.spacing / 2 : 0
      height: parent.height
      color: __style.lightGreenColor
      radius: root.radius
      visible: hasCameraCapability

      Column {
        width: parent.width
        anchors.centerIn: parent
        spacing: 8 * __dp

        MMComponents.MMIcon {
          anchors.horizontalCenter: parent.horizontalCenter
          source: __style.addImageIcon
          color: root.enabled ? __style.forestColor : __style.mediumGreyColor
        }

        MMComponents.MMText {
          width: parent.width

          font: __style.p6
          text: qsTr("Take a picture")

          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          elide: Text.ElideMiddle

          visible: root.enabled
        }
      }

      MouseArea {
        anchors.fill: parent
        onClicked: root.capturePhotoClicked()
      }
    }

    Rectangle {
      id: chooseFromGaleryBox

      width: hasCameraCapability ? parent.width / 2 - parent.spacing / 2 : parent.width
      height: parent.height
      color: __style.lightGreenColor
      radius: root.radius

      Column {
        width: parent.width
        anchors.centerIn: parent
        spacing: 8 * __dp

        MMComponents.MMIcon {
          anchors.horizontalCenter: parent.horizontalCenter
          source: __style.morePhotosIcon
          color: root.enabled ? __style.forestColor : __style.mediumGreyColor
        }

        MMComponents.MMText {
          width: parent.width

          font: __style.p6
          text: qsTr("From gallery")

          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          elide: Text.ElideMiddle

          visible: root.enabled
        }
      }

      MouseArea {
        anchors.fill: parent
        onClicked: root.chooseFromGalleryClicked()
      }
    }
  }
}
