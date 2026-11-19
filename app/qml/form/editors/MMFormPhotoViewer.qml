/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import mm 1.0 as MM
import MMInput

import "../../components" as MMComponents
import "../../components/private" as MMPrivateComponents
import "../components/photo" as MMPhotoComponents
import "../components/signature" as MMSignatureComponents
import "../components" as MMFormComponents

/*
 * Photo viewer for feature form.
 * Its purpose is to show image based on the provided URL.
 * It is not combined with MMPhotoFormEditor as it would be not possible to use it in the gallery then.
 *
 * Serves as a base class for MMPhotoFormEditor, which also edits signatures.
 */

MMPrivateComponents.MMBaseInput {
  id: root

  property string photoUrl: ""
  property bool hasCameraCapability: true
  property bool canEditPhoto: true
  property bool signatureMode: false // signature instead of photo, it can be signed or deleted but not drawn on

  property var photoComponent: photo
  property PhotoSketchingController sketchingController
  property alias photoState: photoStateGroup.state
  readonly property bool hasOpenDialog: previewLoader.active || sketchingLoader.active // keeps the form delegate alive, see MMFormPage

  signal trashClicked()
  signal capturePhotoClicked()
  signal chooseFromGalleryClicked()
  signal drawOnPhotoClicked()
  signal signNowClicked()
  signal sketchingCanceled()

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
      isLocalFile: root.photoUrl.startsWith( "file:///")
      cache: false

      fillMode: root.signatureMode ? Image.PreserveAspectFit : Image.PreserveAspectCrop

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

      MMComponents.MMRoundButton {
        anchors {
          right: parent.right
          top: parent.top
          rightMargin: __style.margin10
          topMargin: __style.margin10
        }

        bgndColor: __style.lightGreenColor
        iconSource: __style.drawIcon
        iconColor: __style.forestColor

        visible: root.editState === "enabled" && photoStateGroup.state !== "notSet" && !root.signatureMode && __activeProject.photoSketchingEnabled && root.photoUrl.startsWith("file:///")

        onClicked: root.openSketching()
      }
    }

    MMPhotoComponents.MMPhotoAttachment {
      width: parent.width
      height: parent.height

      visible: photoStateGroup.state === "notSet" && root.canEditPhoto && !root.signatureMode
      enabled: root.editState === "enabled"

      hasCameraCapability: root.hasCameraCapability

      onCapturePhotoClicked: root.capturePhotoClicked()
      onChooseFromGalleryClicked: root.chooseFromGalleryClicked()
    }

    MMSignatureComponents.MMSignatureAttachment {
      width: parent.width
      height: parent.height

      visible: photoStateGroup.state === "notSet" && root.canEditPhoto && root.signatureMode
      enabled: root.editState === "enabled"

      onSignNowClicked: root.signNowClicked()
    }

    Rectangle {
      width: parent.width - 2 * __style.margin20
      height: parent.height - 2 * __style.margin20

      anchors.centerIn: parent
      radius: parent.radius
      color: __style.lightGreenColor

      visible: photoStateGroup.state === "notSet" && !root.canEditPhoto

      MMComponents.MMText {
        anchors.centerIn: parent
        text: root.signatureMode ? qsTr( "No signature" ) : qsTr( "No image" )
        font: __style.t4Italic
        color: __style.darkGreyColor
      }
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

  Loader {
    id: sketchingLoader

    active: false
    sourceComponent: sketchingComponent
  }

  Component {
    id: sketchingComponent

    MMFormComponents.MMFormPhotoSketchingPageDialog {
      photoUrl: root.photoUrl
      controller: root.sketchingController
      signatureMode: root.signatureMode

      Component.onCompleted: open()

      onClosed: {
        sketchingLoader.active = false
      }
      onRejected: root.sketchingCanceled()
    }
  }

  function openSketching() {
    root.forceActiveFocus()
    sketchingLoader.active = true
    sketchingLoader.focus = true
  }
}
