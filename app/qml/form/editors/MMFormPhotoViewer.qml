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
import "../../components"
import "../../inputs"

/*
 * Photo viewer for feature form.
 * Its purpose is to show image based on the provided URL.
 * It is not combined with MMPhotoFormEditor as it would be not possible to use it in the gallery then.
 *
 * Serves as a base class for MMPhotoFormEditor.
 */

MMBaseInput {
  id: root

  // TODO:
  // - handle "photo notAvailable" state
  // - handle empty state - add "capture photo" and "choose from gallery" signals
  // - scale images well - based on the root.size

  property url photoUrl: ""
  property bool hasCameraCapability: true

  property alias photoComponent: photo

  signal trashClicked()
  signal capturePhotoClicked()
  signal chooseFromGalleryClicked()

  contentItemHeight: 160 * __dp
  spacing: 0
  radius: 20 * __dp

  states: [
    State {
      name: "valid"
    },
    State {
      name: "notSet"
    },
    State {
      name: "notAvailable"
    }
  ]

  state: "notSet"

  onContentClicked: {
    if ( root.enabled && photo.status === Image.Ready ) {
      previewLoader.active = true
      previewLoader.focus = true
    }
  }

  content: Item {
    MMPhoto {
      id: photo

      width: root.width
      height: root.contentItemHeight
      visible: root.state !== "notSet"
      photoUrl: root.photoUrl

      fillMode: Image.PreserveAspectCrop

      MouseArea {
        anchors.fill: parent
        onClicked: root.contentClicked()
      }

      MMRoundButton {
        anchors {
          right: parent.right
          bottom: parent.bottom
          rightMargin: 10 * __dp
          bottomMargin: 10 * __dp
        }

        bgndColor: __style.negativeColor
        iconSource: __style.deleteIcon
        iconColor: __style.grapeColor
        visible: root.enabled && photo.status === Image.Ready
        onClicked: root.trashClicked()
      }
    }

    MMPhotoAttachment {
      width: root.width
      height: root.contentItemHeight
      visible: root.state === "notSet"

      hasCameraCapability: root.hasCameraCapability

      onCapturePhotoClicked: root.capturePhotoClicked()
      onChooseFromGalleryClicked: root.chooseFromGalleryClicked()
    }
  }

  Loader {
    id: previewLoader

    asynchronous: true
    active: false
    sourceComponent: previewComponent
  }

  Component {
    id: previewComponent

    MMPhotoPreview {
      photoUrl: root.photoUrl
    }
  }
}
