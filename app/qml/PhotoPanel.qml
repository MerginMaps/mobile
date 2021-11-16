/***************************************************************************
 photopanel.qml
  --------------------------------------
  Date                 : Dec 2017
  Copyright            : (C) 2017 by Viktor Sklencar
  Email                : vsklencar at gmail dot com
 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.3
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.2
import QtQml 2.2
import QtMultimedia 5.8
import QtGraphicalEffects 1.0
import QgsQuick 0.1 as QgsQuick

Drawer {
  id: photoPanel
  property var backButtonSource: __inputUtils.getThemeIcon("ic_back")
  property color bgColor: "black"
  property real bgOpacity: 1
  property color borderColor: "black"
  property real buttonSize: imageButtonSize * 1.2
  property var buttonsPosition
  property var cancelButtonIcon: __inputUtils.getThemeIcon("ic_clear_black")

  // icons:
  property var captureButtonIcon: __inputUtils.getThemeIcon("ic_camera_alt_border")
  property var confirmButtonIcon: __inputUtils.getThemeIcon("ic_check_black")
  property var fieldItem
  property int iconSize: photoPanel.width / 20
  property real imageButtonSize: 45 * QgsQuick.Utils.dp
  property var lastPhotoName
  // Along with lastPhotoName creates an absolute path to a photo. Its either project path or defaultRoot.
  property var prefixToRelativePath
  // Capture path
  property var targetDir

  dragMargin: 0 // prevents opening the drawer by dragging.
  interactive: true
  modal: true
  visible: false

  signal confirmButtonClicked(string path, string filename)
  function discardCapturedImage() {
    captureItem.saveImage = false;
    photoPreview.visible = false;
    if (camera.imageCapture.capturedImagePath != "") {
      __inputUtils.removeFile(camera.imageCapture.capturedImagePath);
    }
  }

  onVisibleChanged: {
    if (visible) {
      camera.setCameraState(Camera.ActiveState);
      camera.start();
    } else {
      camera.stop();
      photoPreview.visible = false;
    }
  }

  // PhotoCapture item
  Item {
    id: captureItem
    property bool saveImage: false

    height: window.height
    width: window.width

    Component.onDestruction: {
      if (!captureItem && camera.imageCapture.capturedImagePath != "") {
        captureItem.saveImage = false;
        Utils.removeFile(camera.imageCapture.capturedImagePath);
      }
      captureItem.saveImage = false;
    }

    Camera {
      id: camera
      cameraState: Camera.UnloadedState

      imageCapture {
        onImageCaptured: {
          // Show the preview in an Image
          photoPreview.source = preview;
        }
      }
      focus {
        focusMode: Camera.FocusContinuous
        focusPointMode: Camera.FocusPointAuto
      }
    }
    VideoOutput {
      id: videoOutput
      anchors.fill: parent
      autoOrientation: true
      focus: visible // to receive focus and capture key events when visible
      source: camera
      visible: !photoPreview.visible

      Item {
        id: captureButton
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        antialiasing: true
        height: buttonSize
        width: buttonSize

        MouseArea {
          id: mouseArea
          anchors.fill: parent

          onClicked: {
            if (targetDir !== "") {
              camera.imageCapture.captureToLocation(photoPanel.targetDir);
            } else {
              // saved to default location - TODO handle this case
              camera.imageCapture.capture();
            }
            photoPreview.visible = true;
          }
        }
        Image {
          id: captureButtonImage
          anchors.centerIn: parent
          fillMode: Image.PreserveAspectFit
          height: imageButtonSize
          source: photoPanel.captureButtonIcon
          sourceSize.height: imageButtonSize
          sourceSize.width: imageButtonSize
        }
      }
    }
    Image {
      id: photoPreview
      fillMode: Image.PreserveAspectFit
      height: parent.height
      visible: false
      width: parent.width

      onVisibleChanged: if (!photoPreview.visible)
        photoPreview.source = ""

      // Cancel button
      Rectangle {
        id: cancelButton
        property int borderWidth: 5 * QgsQuick.Utils.dp

        anchors.right: parent.right
        anchors.top: confirmButton.bottom
        antialiasing: true
        border.color: photoPanel.borderColor
        border.width: borderWidth
        color: "white"
        height: buttonSize
        radius: width * 0.5
        visible: camera.imageCapture.capturedImagePath != ""
        width: buttonSize

        MouseArea {
          anchors.fill: parent

          onClicked: photoPanel.discardCapturedImage()
        }
        Image {
          anchors.centerIn: parent
          fillMode: Image.PreserveAspectFit
          height: imageButtonSize
          source: photoPanel.cancelButtonIcon
          sourceSize.height: imageButtonSize
          sourceSize.width: imageButtonSize
        }
      }

      // Confirm button
      Rectangle {
        id: confirmButton
        property int borderWidth: 5 * QgsQuick.Utils.dp

        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        antialiasing: true
        border.color: photoPanel.borderColor
        border.width: borderWidth
        color: "white"
        height: buttonSize
        radius: width * 0.5
        visible: camera.imageCapture.capturedImagePath != ""
        width: buttonSize

        MouseArea {
          anchors.fill: parent

          onClicked: {
            captureItem.saveImage = true;
            photoPanel.visible = false;
            confirmButtonClicked(photoPanel.prefixToRelativePath, camera.imageCapture.capturedImagePath);
          }
        }
        Image {
          anchors.centerIn: parent
          fillMode: Image.PreserveAspectFit
          height: imageButtonSize
          source: photoPanel.confirmButtonIcon
          sourceSize.height: imageButtonSize
          sourceSize.width: imageButtonSize
          width: imageButtonSize
        }
      }
    }
    Item {
      id: backButton
      property int borderWidth: 50 * QgsQuick.Utils.dp

      antialiasing: true
      height: width
      width: imageButtonSize * 1.5

      MouseArea {
        anchors.fill: parent

        onClicked: {
          cancelButton.visible ? photoPanel.discardCapturedImage() : photoPanel.close();
        }
      }
      Image {
        id: backBtnIcon
        anchors.centerIn: parent
        fillMode: Image.PreserveAspectFit
        height: imageButtonSize / 2
        source: photoPanel.backButtonSource
        sourceSize.height: height
        sourceSize.width: height
      }
      ColorOverlay {
        anchors.centerIn: parent
        anchors.fill: backBtnIcon
        color: "white"
        smooth: true
        source: backBtnIcon
      }
    }
  }

  background: Rectangle {
    color: photoPanel.bgColor
    opacity: photoPanel.bgOpacity
  }
}
