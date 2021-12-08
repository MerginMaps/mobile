/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Dialogs 1.2

import "."  // import InputStyle singleton

Item {

    property alias handler: externalResourceHandler

    QtObject {
        id: externalResourceHandler

        // Has to be set for actions with callbacks
        property var itemWidget

        /**
         * Called when clicked on the camera icon to capture an image.
         * \param itemWidget editorWidget for modified field to send valueChanged signal.
         */
        property var capturePhoto: function capturePhoto(itemWidget) {
          externalResourceHandler.itemWidget = itemWidget
          if (__androidUtils.isAndroid) {
              __androidUtils.callCamera(itemWidget.targetDir)
          } else if (__iosUtils.isIos) {
              __iosUtils.callCamera(itemWidget.targetDir)
          } else {
            itemWidget.showDefaultPanel()
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
                fileDialog.open()
            }
        }

        /**
         * Called to show an image preview.
         * \param imagePath Absolute path to an image.
         */
        property var previewImage: function previewImage(imagePath) {
            imagePreview.source = "file://" +  imagePath
            imagePreview.width = window.width - 2 * InputStyle.panelMargin
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
                console.log("error: Unable to copy file " + imagePath + " to the project directory")
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
            var prefixPath = externalResourceHandler.itemWidget.targetDir.endsWith("/") ?
                  externalResourceHandler.itemWidget.targetDir :
                  externalResourceHandler.itemWidget.targetDir + "/"
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
        onImageSelected:externalResourceHandler.imageSelected(imagePath)
    }

    Connections {
        target: __iosUtils
        // used for both gallery and camera
        onImageSelected: externalResourceHandler.imageCaptured(imagePath)
    }

    Popup {
        id: previewImageWrapper
        modal: true
        focus: true
        closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutside
        background: Item {
            anchors.fill: parent
        }

        contentHeight: window.height
        contentWidth: window.width
        contentItem: Image {
            id: imagePreview
            anchors.centerIn: parent
            visible: true
            autoTransform: true
            fillMode: Image.PreserveAspectFit

            // on iOS automatic closePolicy does not work
            MouseArea {
              anchors.fill: parent
              onClicked: {
                previewImageWrapper.close()
              }
            }
        }
    }

    FileDialog {
        id: fileDialog
        title: qsTr( "Open Image" )
        visible: false
        nameFilters: [ qsTr( "Image files (*.gif *.png *.jpg)" ) ]
        width: window.width
        height: window.height
        folder: shortcuts.pictures // https://doc.qt.io/qt-5/ios-platform-notes.html#native-image-picker
        onAccepted: externalResourceHandler.imageSelected(fileDialog.fileUrl)
    }

    MessageDialog {
        property string imagePath

        id: imageDeleteDialog
        visible: false
        title: qsTr( "Remove photo reference" )
        text: qsTr( "Also permanently delete photo from device?" )
        icon: StandardIcon.Warning
        standardButtons: StandardButton.Yes | StandardButton.No | StandardButton.Cancel
        onYes: {
            externalResourceHandler.itemWidget.sourceToDelete = imageDeleteDialog.imagePath
            externalResourceHandler.itemWidget.editorValueChanged("", false)
            visible = false
        }
        onNo: {
            externalResourceHandler.itemWidget.editorValueChanged("", false)
            // visible = false called afterwards when onReject
        }
        onRejected: {
           visible = false
        }
    }

}


