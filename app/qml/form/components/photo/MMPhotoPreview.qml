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

  property url photoUrl //passed url externally

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

      //Zoom limits
      property real minScale: 0.5
      property real maxScale: 10.0
      property real scale: 1.0
      // width: Math.min(imagePreview.width, parent.width)
      // height: Math.min(imagePreview.height, parent.height)

      Flickable {
        id: flick
        anchors.fill: parent
        clip: true
        interactive: true
        boundsBehavior: Flickable.StopAtBounds
        maximumFlickVelocity: 4000

        contentWidth:  Math.max(width,  imagePreview.paintedWidth  * photoFrame.scale)
        contentHeight: Math.max(height, imagePreview.paintedHeight * photoFrame.scale)

        Item {
          id: canvas
          width: flick.contentWidth
          height: flick.contentHeight

          Item {
            id: imageHolder
            width: imagePreview.width
            height: imagePreview.height
            anchors.centerIn: parent

            Image {
              id: imagePreview
              source: root.photoUrl
              height: root.height / 2

              clip: true

              focus: true
              asynchronous: true
              autoTransform: true
              fillMode: Image.PreserveAspectFit

              transform: Scale {
                origin.x: imagePreview.width  / 2
                origin.y: imagePreview.height / 2
                xScale: photoFrame.scale
                yScale: photoFrame.scale
              }
            }
          }
        }

        // Keep content in bounds; recenters when content smaller than viewport
        function _clamp() {
          const maxX = Math.max(0, contentWidth  - width)
          const maxY = Math.max(0, contentHeight - height)
          contentX = Math.max(0, Math.min(maxX, contentX))
          contentY = Math.max(0, Math.min(maxY, contentY))
        }

        PinchArea {
          id: pincher
          anchors.fill: parent
          pinch.target: imagePreview
          pinch.minimumRotation: -180
          pinch.maximumRotation: 180
          pinch.minimumScale: 0.5
          pinch.maximumScale: 10
          property real _startScale: 1

          onPinchStarted: function(pinch) {
            pinch.accepted = true
            _startScale = photoFrame.scale
          }
          // onPinchUpdated: function(pinch) {
          //   var newScale = Math.max(photoFrame.minScale, Math.min(photoFrame.maxScale, _startScale * pinch.scale))
          //   var p = pincher.mapToItem(flick.contentItem, pinch.center.x, pinch.center.y)

          //   var prevScale = photoFrame.scale
          //   photoFrame.scale = newScale

          //   var ratio = newScale / prevScale
          //   flick.contentX = (flick.contentX + p.x) * ratio - p.x
          //   flick.contentY = (flick.contentY + p.y) * ratio - p.y

          //   flick._clamp()
          // }

          onPinchFinished: function(pinch) {
            flick._clamp()
          }
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
