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

  property url photoUrl

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
      anchors.fill: parent
      clip: true

      // Zoom limits
      property real minScale: 0.5
      property real maxScale: 10.0
      property real scale: 1.0

      Flickable {
        id: flick
        anchors.fill: parent
        clip: true
        interactive: true
        boundsBehavior: Flickable.StopAtBounds
        maximumFlickVelocity: 4000

        contentWidth:  Math.max(width,  imagePreview.width  * imagePreview.scale)
        contentHeight: Math.max(height, imagePreview.height * imagePreview.scale)

        Image {
          id: imagePreview
          source: root.photoUrl
          anchors.centerIn: parent
          width:  (implicitWidth  >= implicitHeight) ? root.width  * 0.85 : undefined
          height: (implicitHeight >  implicitWidth)  ? root.height * 0.85 : undefined
          smooth: true
          clip: true
          focus: true
          asynchronous: true
          autoTransform: true
          fillMode: Image.PreserveAspectFit
          scale: photoFrame.scale
        }

        // Keep content in bounds; recenters when content smaller or bigger than viewport
        function _clamp() {
          const maxX = Math.max(0, contentWidth  - width)
          const maxY = Math.max(0, contentHeight - height)
          contentX = Math.max(0, Math.min(maxX, contentX))
          contentY = Math.max(0, Math.min(maxY, contentY))
        }

        PinchArea {
          id: pincher
          anchors.fill: parent
          pinch.minimumRotation: 0
          pinch.maximumRotation: 0
          pinch.minimumScale: photoFrame.minScale
          pinch.maximumScale: photoFrame.maxScale
          property real startScale: 1

          //holds the value
          onPinchStarted: function(pinch) {
            pinch.accepted = true
            startScale = photoFrame.scale
          }

          onPinchUpdated: function(pinch) {
            //to keep the new scaled value after the user zooms in, calculating
            const newScale = Math.max(photoFrame.minScale, Math.min(photoFrame.maxScale, startScale * pinch.scale))
            const local = pincher.mapToItem(imagePreview, pinch.center.x, pinch.center.y)
            const before = imagePreview.mapToItem(flick.contentItem, local.x, local.y)
            const old = photoFrame.scale
            if (newScale !== old) {
              photoFrame.scale = newScale

              // same local point sits in content -after- scaling
              const after = imagePreview.mapToItem(flick.contentItem, local.x, local.y)

              //shift scroll so the point stays under the fingers
              flick.contentX += (after.x - before.x)
              flick.contentY += (after.y - before.y)
            }

            flick._clamp()
          }

          onPinchFinished: function() { flick._clamp() }
        }
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
