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
import QtQuick.Controls.Basic
import "../../components"
import "../../inputs"

// TODO: turn into MMPhotoFormViewer and make it as a base class to MMPhotoFormEditor

MMBaseInput {
  id: root

  property var _field: parent.field
  property var _fieldValue: parent.fieldValue
  property bool _fieldValueIsNull: parent.fieldValueIsNull

  property var _fieldConfig: parent.fieldConfig
  property bool _fieldShouldShowTitle: parent.fieldShouldShowTitle
  property bool _fieldIsReadOnly: parent.fieldIsReadOnly

  property string _fieldTitle: parent.fieldTitle
  property string _fieldErrorMessage: parent.fieldErrorMessage
  property string _fieldWarningMessage: parent.fieldWarningMessage

  signal editorValueChanged( var newValue, var isNull )

  title: _fieldShouldShowTitle ? _fieldTitle : ""

  warningMsg: _fieldWarningMessage
  errorMsg: _fieldErrorMessage

  enabled: !_fieldIsReadOnly

  hasFocus: rightSwitch.focus


//  property var parentValue: parent.value ?? ""
//  property bool parentValueIsNull: parent.valueIsNull ?? false
//  property bool isReadOnly: parent.readOnly ?? false

//  property url photoUrl

//  signal trashClicked()

  contentItemHeight: 160 * __dp
  spacing: 0
  radius: 20 * __dp

  content: MMPhoto {
    id: photo

    width: root.width
    height: root.contentItemHeight
    photoUrl: root.photoUrl

    MouseArea {
      anchors.fill: parent
      onClicked: root.contentClicked()
    }

    Rectangle {
      width: 40 * __dp
      height: width
      radius: width / 2
      color: __style.negativeColor
      anchors.right: parent.right
      anchors.bottom: parent.bottom
      anchors.rightMargin: 10 * __dp
      anchors.bottomMargin: 10 * __dp
      visible: photo.status === Image.Ready

      MMIcon {
        anchors.centerIn: parent
        source: __style.deleteIcon
        useCustomSize: true
        width: 30 * __dp
        height: width
        color: __style.grapeColor
      }

      MouseArea {
        anchors.centerIn: parent
        width: parent.width + 20 * __dp
        height: width
        onClicked: root.trashClicked()
      }
    }
  }

  QtObject {
    id: externalResourceHandler

    // Has to be set for actions with callbacks
    property var itemWidget

    // Whether we have camera available on this platform
    property var hasCameraCapability:__androidUtils.isAndroid || __iosUtils.isIos

    /**
       * Called when clicked on the camera icon to capture an image.
       * \param itemWidget editorWidget for modified field to send valueChanged signal.
       */
    property var capturePhoto: function capturePhoto(itemWidget) {
      externalResourceHandler.itemWidget = itemWidget
      if ( !__inputUtils.createDirectory( itemWidget.targetDir ) )
      {
        __inputUtils.log("Capture photo", "Could not create directory " + itemWidget.targetDir);
        errorDialog.errorText = qsTr( "Could not create directory %1." ).arg( itemWidget.targetDir )
        errorDialog.open()
      }

      if (__androidUtils.isAndroid) {
        __androidUtils.callCamera(itemWidget.targetDir)
      } else if (__iosUtils.isIos) {
        __iosUtils.callCamera(itemWidget.targetDir)
      } else {
        // This should never happen
        console.log("Camera not implemented on this platform.")
      }
    }

    /**
       * Called when clicked on the gallery icon to choose a file from a gallery.
       * ItemWidget reference is set here and kept for the whole workflow to avoid ambiguity in case of
       * multiple external resource (attachment) fields. All usecases and bundle itself counts with one interaction
       * per one time.
       *
       * The workflow of choosing an image from a gallery starts here and goes as follows:
       * Android gallery even is evoked. When a user chooses image, "imageSelected( selectedImagePath )" is emitted.
       * Then "imageSelected" caught the signal, handles changes and sends signal "valueChanged".
       * \param itemWidget editorWidget for modified field to send valueChanged signal.
       */
    property var chooseImage: function chooseImage(itemWidget) {
      externalResourceHandler.itemWidget = itemWidget
      if (__androidUtils.isAndroid) {
        __androidUtils.callImagePicker()
      } else if (__iosUtils.isIos) {
        __iosUtils.callImagePicker(itemWidget.targetDir)
      } else {
        desktopGalleryPicker.open()
      }
    }

    /**
       * Called to show an image preview.
       * \param imagePath Absolute path to an image.
       */
    property var previewImage: function previewImage(imagePath) {
      imagePreview.source = "file://" +  imagePath
      imagePreview.width = window.width
      previewImageWrapper.open()
    }

    /**
       * Called to remove an image from a widget. A confirmation dialog is open first if a file exists.
       * ItemWidget reference is set here to delete an image for certain widget.
       * \param itemWidget editorWidget for modified field to send valueChanged signal.
       * \param imagePath Absolute path to an image.
       */
    property var removeImage: function removeImage(itemWidget, imagePath) {
      if (__inputUtils.fileExists(imagePath)) {
        externalResourceHandler.itemWidget = itemWidget
        imageDeleteDialog.imagePath = imagePath
        imageDeleteDialog.open()
      } else {
        itemWidget.editorValueChanged("", false)
      }
    }

    /**
       * Called when a photo is taken and confirmed (clicked on check/ok button).
       * Original photo file is renamed with current date time to avoid name conflicts.
       * ItemWidget reference is always set here to avoid ambiguity in case of
       * multiple external resource (attachment) fields.
       * \param itemWidget editorWidget for a modified field to send valueChanged signal.
       * \param prefixToRelativePath depends on widget's config, see more inputexternalwidget.qml
       * \param value depends on widget's config, see more in inputexternalwidget.qml
       */
    property var confirmImage: function confirmImage(itemWidget, prefixToRelativePath, value) {
      if (value) {
        __inputUtils.rescaleImage(value, __activeProject.qgsProject)
        var newCurrentValue = __inputUtils.getRelativePath(value, prefixToRelativePath)
        itemWidget.editorValueChanged(newCurrentValue, newCurrentValue === "" || newCurrentValue === null)
      }
    }

    /**
       * Called when an image is either selected from a gallery or captured by native camera. If the image doesn't exist in a folder
       * set in widget's config, it is copied to the destination and value is set according a new copy (only when chosen from gallery).
       * \param imagePath Absolute path to a selected image
       */
    property var imageSelected: function imageSelected(imagePath) {
      var filename = __inputUtils.getFileName(imagePath)
      //! final absolute location of an image.
      var absolutePath = __inputUtils.getAbsolutePath( filename, externalResourceHandler.itemWidget.targetDir )

      if (!__inputUtils.fileExists(absolutePath)) { // we need to copy it!
        var success = __inputUtils.copyFile(imagePath, absolutePath)
        if (!success)
        {
          __inputUtils.log("Select image", "Failed to copy image file to " + absolutePath);
          errorDialog.errorText = qsTr( "Failed to copy image file to %1." ).arg( absolutePath )
          errorDialog.open()
        }
      }
      externalResourceHandler.confirmImage(externalResourceHandler.itemWidget, externalResourceHandler.itemWidget.prefixToRelativePath, absolutePath)
    }

    /**
       * Called when an image is captured by a camera. Method sets proper value according given absolute path of the image
       * and prefixPath set in thd project settings.
       * \param imagePath Absolute path to a captured image
       */
    property var imageCaptured: function imageCaptured(absoluteImagePath) {
      if (absoluteImagePath) {
        var prefixPath = externalResourceHandler.itemWidget.prefixToRelativePath.endsWith("/") ?
              externalResourceHandler.itemWidget.prefixToRelativePath :
              externalResourceHandler.itemWidget.prefixToRelativePath + "/"
        externalResourceHandler.confirmImage(externalResourceHandler.itemWidget, prefixPath, absoluteImagePath)
      }
    }

    property var onFormSave: function onFormSave(itemWidget) {
      __inputUtils.removeFile(itemWidget.sourceToDelete)
      itemWidget.sourceToDelete = ""
    }

    property var onFormCanceled: function onFormCanceled(itemWidget) {
      itemWidget.sourceToDelete = ""
    }
  }

  Connections {
    target: __androidUtils
    // used for both gallery and camera
    function onImageSelected( imagePath ) {
      externalResourceHandler.imageSelected(imagePath)
    }
  }

  Connections {
    target: __iosUtils
    // used for both gallery and camera
    function onImageSelected( imagePath ) {
      externalResourceHandler.imageCaptured(imagePath)
    }
  }
}
