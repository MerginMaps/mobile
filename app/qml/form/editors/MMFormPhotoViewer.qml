/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../../components" as MMComponents
import "../../components/private" as MMPrivateComponents
import "../components/photo" as MMPhotoComponents

/*
 * Photo viewer for feature form.
 * Its purpose is to show image based on the provided URL.
 * It is not combined with MMPhotoFormEditor as it would be not possible to use it in the gallery then.
 *
 * Serves as a base class for MMPhotoFormEditor.
 */

MMPrivateComponents.MMBaseInput {
  id: root

  property string photoUrl: ""
  property bool hasCameraCapability: true

  property var photoComponent: photo
  property alias photoState: photoStateGroup.state

  signal trashClicked()
  signal capturePhotoClicked()
  signal chooseFromGalleryClicked()

  StateGroup {
    id: photoStateGroup

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
  }


  inputContent: Rectangle {
    width: parent.width
    height: __style.row160

    color: __style.polarColor
    radius: __style.radius20

    MMComponents.MMPhoto {
      id: photo

      width: parent.width
      height: parent.height

      visible: photoStateGroup.state !== "notSet"

      photoUrl: root.photoUrl
      isLocalFile: root.photoUrl.startsWith( "file://" )

      fillMode: Image.PreserveAspectCrop

      onStatusChanged: {
      if ( status === Image.Error ) {
          __inputUtils.log( "Image Loading", "Could not load the image. It may be missing or invalid, the URL might be incorrect, or there may be no network connection: " + root.photoUrl )
        }
      }

      MouseArea {
        anchors.fill: parent
        onClicked: {
          if ( photo.status === Image.Ready ) {
            previewLoader.active = true
            previewLoader.focus = true
          }
        }
      }

      MMComponents.MMRoundButton {
        anchors {
          right: parent.right
          bottom: parent.bottom
          rightMargin: __style.margin10
          bottomMargin: __style.margin10
        }

        bgndColor: __style.negativeColor
        iconSource: __style.deleteIcon
        iconColor: __style.grapeColor

        visible: root.editState === "enabled" && photoStateGroup.state !== "notSet"

        onClicked: root.trashClicked()
      }
    }

    MMPhotoComponents.MMPhotoAttachment {
      width: parent.width
      height: parent.height

      visible: photoStateGroup.state === "notSet"
      enabled: root.editState === "enabled"

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

    MMPhotoComponents.MMPhotoPreview {
      photoUrl: root.photoUrl
    }
  }
}
