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

Popup {
  id: root

  property alias photoUrl: imagePreview.source

  parent: Overlay.overlay
  visible: true
  height: ApplicationWindow.window?.height ?? 0
  width: ApplicationWindow.window?.width ?? 0
  closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

  background: Rectangle {
    color: Qt.alpha(__style.darkGreyColor, 0.9)
  }

  contentItem: Item {
    anchors.fill: parent

    MMComponents.MMBusyIndicator {
      anchors.centerIn: parent
      visible: imagePreview.status === Image.Loading
    }

    // Do not propagate clicks to the background
    MouseArea {
      anchors.fill: parent
      onClicked: function( mouse ) {
        mouse.accepted = true
      }
    }

    Item {
      id: photoFrame

      width: Math.min(imagePreview.width, parent.width)
      height: Math.min(imagePreview.height, parent.height)

      y: parent.height / 2 - height / 2
      x: parent.width / 2 - width / 2

      Image {
        id: imagePreview

        height: root.height / 2

        clip: true

        focus: true
        asynchronous: true
        autoTransform: true
        fillMode: Image.PreserveAspectFit

        PinchArea {
          anchors.fill: parent
          pinch.target: imagePreview
          pinch.minimumRotation: -180
          pinch.maximumRotation: 180
          pinch.minimumScale: 0.5
          pinch.maximumScale: 10
        }

        DragHandler { }
      }
    }

    MMComponents.MMRoundButton {
      id: closeButton

      anchors {
        horizontalCenter: parent.horizontalCenter
        bottom: parent.bottom
        bottomMargin: __style.safeAreaBottom + __style.spacing20
      }

      bgndColor: __style.lightGreenColor
      iconSource: __style.closeIcon
      onClicked: {
        previewLoader.active = false
      }
    }
  }
}
