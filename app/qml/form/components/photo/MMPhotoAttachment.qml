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
  property bool hasChooseCapability: true

  // allows reusing the component for other attachment types (audio, video)
  property url captureIconSource: __style.addImageIcon
  property string captureText: qsTr("Take a picture")
  property url chooseIconSource: __style.morePhotosIcon
  property string chooseText: qsTr("From gallery")

  signal capturePhotoClicked()
  signal chooseFromGalleryClicked()

  Row {
    anchors.centerIn: parent
    width: parent.width - 40 * __dp
    height: parent.height - 40 * __dp
    spacing: 20 * __dp

    Rectangle {
      id: takePictureBox

      width: hasCameraCapability ? ( hasChooseCapability ? parent.width / 2 - parent.spacing / 2 : parent.width ) : 0
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
          source: root.captureIconSource
          color: root.enabled ? __style.forestColor : __style.mediumGreyColor
        }

        MMComponents.MMText {
          width: parent.width

          font: __style.p6
          text: root.captureText

          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          elide: Text.ElideMiddle

          visible: root.enabled
        }
      }

      MMComponents.MMSingleClickMouseArea{
        anchors.fill: parent
        onSingleClicked: root.capturePhotoClicked()
      }
    }

    Rectangle {
      id: chooseFromGaleryBox

      width: hasChooseCapability ? ( hasCameraCapability ? parent.width / 2 - parent.spacing / 2 : parent.width ) : 0
      height: parent.height
      color: __style.lightGreenColor
      radius: root.radius
      visible: hasChooseCapability

      Column {
        width: parent.width
        anchors.centerIn: parent
        spacing: 8 * __dp

        MMComponents.MMIcon {
          anchors.horizontalCenter: parent.horizontalCenter
          source: root.chooseIconSource
          color: root.enabled ? __style.forestColor : __style.mediumGreyColor
        }

        MMComponents.MMText {
          width: parent.width

          font: __style.p6
          text: root.chooseText

          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          elide: Text.ElideMiddle

          visible: root.enabled
        }
      }

      MMComponents.MMSingleClickMouseArea{
        anchors.fill: parent
        onSingleClicked: root.chooseFromGalleryClicked()
      }
    }
  }
}
