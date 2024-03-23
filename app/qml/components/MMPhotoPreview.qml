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
  width: ApplicationWindow.window.width
  height: ApplicationWindow.window.height
  closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside

  background: Rectangle {
    color: Qt.alpha(__style.nightAlphaColor, 0.9)
  }

  Item {
    id: photoFrame

    x: (root.width - imagePreview.width) / 2
    y: root.height / 4
    width: imagePreview.width
    height: imagePreview.height

    Image {
      id: imagePreview

      height: root.height / 2

      autoTransform: true;
      focus: true
      asynchronous: true
      fillMode: Image.PreserveAspectFit
    }

    PinchHandler {
      id: pinchHandler

      minimumRotation: -180
      maximumRotation: 180
      minimumScale: 0.5
      maximumScale: 10
    }

    DragHandler { }
  }

  MMRoundButton {
    id: closeButton

    anchors.top: parent.top
    anchors.right: parent.right
    anchors.topMargin: 2 * __style.pageMargins
    anchors.rightMargin: 2 * __style.pageMargins

    bgndColor: __style.lightGreenColor
    iconSource: __style.closeIcon

    onClicked: {
      previewLoader.active = false
    }
  }
}
