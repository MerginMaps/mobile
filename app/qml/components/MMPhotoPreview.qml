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

Popup {
  id: root

  property alias photoUrl: imagePreview.source

  parent: Overlay.overlay
  visible: true
  height: ApplicationWindow.window?.height ?? 0
  width: ApplicationWindow.window?.width ?? 0
  closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

  background: Rectangle {
    color: Qt.alpha(__style.nightAlphaColor, 0.9)
  }

  contentItem: Item {
    anchors.fill: parent

    MMBusyIndicator {
      anchors.centerIn: parent
      visible: true
    }

    Item {
      id: photoFrame

      anchors.centerIn: parent
      width: Math.min(imagePreview.width, parent.width)
      height: Math.min(imagePreview.height, parent.height)

      Image {
        id: imagePreview

        height: root.height / 2

        autoTransform: true
        focus: true
        asynchronous: true
        fillMode: Image.PreserveAspectFit
      }

      PinchHandler {
        minimumRotation: -180
        maximumRotation: 180
        minimumScale: 0.5
        maximumScale: 10
      }

      DragHandler { }
    }

    Item {
      x: __style.safeAreaLeft
      y: __style.safeAreaTop
      width: parent.width - __style.safeAreaLeft - __style.safeAreaRight
      height: parent.height - __style.safeAreaBottom - __style.safeAreaTop

      MMRoundButton {
        id: closeButton

        anchors {
          horizontalCenter: parent.horizontalCenter
          bottom: parent.bottom
          bottomMargin: __style.spacing20
        }

        bgndColor: __style.lightGreenColor
        iconSource: __style.closeIcon
        onClicked: {
          previewLoader.active = false
        }
      }

    }
  }
}
