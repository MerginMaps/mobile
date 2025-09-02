/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Dialogs

import mm 1.0 as MM
import MMInput

import "../../dialogs"

/*
 * Photo form editor (external resource) for QGIS Attribute Form
 * Requires various global properties set to function, see featureform Loader section.
 * These properties are injected here via 'fieldXYZ' properties and captured with underscore `_`.
 *
 * Should be used only within feature form.
 * Replaced previous ExternalResourceBundle and some code from inputexternalresource.
 * See MMFormPhotoViewer.
 *
 *
 * Overview of path handling
 * -------------------------
 *
 * Project home path: comes from QgsProject::homePath() - by default it points to the folder where
 * the .qgs/.qgz project file is stored, but can be changed manually by the user.
 *
 * Default path: defined in the field's configuration. This is the path where newly captured images will be stored.
 * It has to be an absolute path. It can be defined as an expression (e.g. @project_home || '/photos') or
 * by plain path (e.g. /home/john/photos). If not defined, project home path is used.
 *
 * In the field's configuration, there are three ways how path to pictures is stored in field values:
 * absolute paths, relative to default path and relative to project path. Below is an example of how
 * the final field values of paths are calculated.
 *
 *   variable         |     value
 * -------------------+--------------------------------
 * project home path  |  /home/john
 * default path       |  /home/john/photos
 * image path         |  /home/john/photos/img0001.jpg
 *
 *
 *    storage type          |   calculation of field value    |      final field value
 * -------------------------+---------------------------------+--------------------------------
 * absolute path            |  image path                     |   /home/john/photos/img0001.jpg
 * relative to default path |  image path - default path      |   img0001.jpg
 * relative to project path |  image path - project home path |   photos/img0001.jpg
 */

MMFormPhotoViewer {
  id: root

  property var _fieldValue: parent.fieldValue
  property var _fieldConfig: parent.fieldConfig
  property var _fieldIndex: parent.fieldIndex
  property bool _fieldValueIsNull: parent.fieldValueIsNull

  property string _fieldHomePath: parent.fieldHomePath
  property var _fieldActiveProject: parent.fieldActiveProject
  property var _fieldFeatureLayerPair: parent.fieldFeatureLayerPair

  property bool _fieldShouldShowTitle: parent.fieldShouldShowTitle
  property bool _fieldFormIsReadOnly: parent.fieldFormIsReadOnly
  property bool _fieldIsEditable: parent.fieldIsEditable

  property string _fieldTitle: parent.fieldTitle
  property string _fieldErrorMessage: parent.fieldErrorMessage
  property string _fieldWarningMessage: parent.fieldWarningMessage

  property bool _fieldRememberValueSupported: parent.fieldRememberValueSupported
  property bool _fieldRememberValueState: parent.fieldRememberValueState

  signal editorValueChanged( var newValue, bool isNull )
  signal rememberValueBoxClicked( bool state )

  title: _fieldShouldShowTitle ? _fieldTitle : ""

  warningMsg: photoState === "notAvailable" ? qsTr( "Photo is missing." ) : _fieldWarningMessage
  errorMsg: _fieldErrorMessage

  readOnly: _fieldFormIsReadOnly || !_fieldIsEditable
  shouldShowValidation: !_fieldFormIsReadOnly

  hasCheckbox: _fieldRememberValueSupported
  checkboxChecked: _fieldRememberValueState

  photoUrl: internal.tempSketchedImageSource ? internal.tempSketchedImageSource : internal.resolvedImageSource
  hasCameraCapability: __androidUtils.isAndroid || __iosUtils.isIos

  on_FieldValueChanged: internal.setImageSource()
  on_FieldValueIsNullChanged: internal.setImageSource()

  onCapturePhotoClicked: internal.capturePhoto()
  onChooseFromGalleryClicked: internal.chooseFromGallery()
  onTrashClicked: internal.removeImage( __inputUtils.getAbsolutePath( root._fieldValue, internal.prefixToRelativePath ) )

  onCheckboxCheckedChanged: {
    root.rememberValueBoxClicked( checkboxChecked )
  }

  sketchingController: photoSketchingLoader.item

  // used only on desktop builds
  FileDialog {
    id: desktopGalleryPicker

    title: qsTr( "Open Image" )

    nameFilters: [ qsTr( "Image files (*.gif *.png *.jpg)" ) ]

    currentFolder: __inputUtils.imageGalleryLocation()
    onAccepted: {
      internal.imageSelected( selectedFile )
    }
  }

  MMRemovePhotoDialog {
    id: imageDeleteDialog

    property string imagePath

    onDeleteImage: {
      // schedule the image for deletion
      internal.imageSourceToDelete = imageDeleteDialog.imagePath
      resetValueAndClose()
    }

    onUnlinkImage: resetValueAndClose()

    function resetValueAndClose() {
      root.editorValueChanged( "", true )

      errorMsg = ""
      imagePath = ""
      close()
    }
  }

  Loader {
    id: photoSketchingLoader
    active: __activeProject.photoSketchingEnabled

    sourceComponent: photoSketchingComponent

    onLoaded: {
      item.prepareController()
    }
  }

  Component {
    id: photoSketchingComponent

    PhotoSketchingController {
      photoSource: root.photoUrl
      projectName: root._fieldActiveProject.homePath
    }
  }

  Connections {
    target: root.sketchingController

    function onTempPhotoSourceChanged( newPath ){
      if ( internal.tempSketchedImageSource === "file://" + newPath ) {
        internal.tempSketchedImageSource = ""
      }
      internal.tempSketchedImageSource = "file://" + newPath
    }
  }

  Connections {
    target: __androidUtils

    // used for both gallery and camera
    function onImageSelected( imagePath, index ) {
      if ( root._fieldIndex.toString() === index.toString() ) {
        internal.imageSelected( imagePath )
      }
    }
  }

  Connections {
    target: __iosUtils

    // used for both gallery and camera
    function onImageSelected( imagePath, index ) {
      if ( root._fieldIndex.toString() === index.toString() ) {
        internal.imageCaptured( imagePath )
      }
    }
  }

  function callbackOnFormSaved() {
    if ( internal.imageSourceToDelete ) {
      __inputUtils.removeFile( internal.imageSourceToDelete )
      internal.imageSourceToDelete = ""
    }
  }

  function callbackOnFormCanceled() {
    internal.imageSourceToDelete = ""
  }

  QtObject {
    id: internal
    //! This object is a combination of previous "ExternalResourceBundle" and some functions from "inputexternalresource" editor

    /**
     * 0 - Relative path disabled
     * 1 - Relative path to project
     * 2 - Relative path to defaultRoot defined in the config - Default path field in the widget configuration form
     */
    property int relativeStorageMode: root._fieldConfig["RelativeStorage"]

    /**
     * This evaluates the "default path" with the following order:
     * 1. evaluate default path expression if defined,
     * 2. use default path value if not empty,
     * 3. use project home folder
     */
    property string targetDir: __inputUtils.resolveTargetDir(
                                 root._fieldHomePath,
                                 root._fieldConfig,
                                 root._fieldFeatureLayerPair,
                                 root._fieldActiveProject
                                 )

    property string prefixToRelativePath: __inputUtils.resolvePrefixForRelativePath(
                                            relativeStorageMode,
                                            root._fieldHomePath,
                                            targetDir
                                            )

    property string resolvedImageSource

    property string tempSketchedImageSource

    property string imageSourceToDelete // used to postpone image deletion to when the form is saved

    //
    // Sets path or url of the assigned photo to the resolvedImageSource.
    //  - resolvedImageSource is the actual path on the device or a remote url,
    //  - and is used by QML Image to show the image
    //
    function setImageSource() {
      if ( !root._fieldValue || root._fieldValueIsNull ) {
        root.photoState = "notSet"
        resolvedImageSource = ""
        return
      }

      let absolutePath = __inputUtils.getAbsolutePath( root._fieldValue, internal.prefixToRelativePath )

      if ( __inputUtils.fileExists( absolutePath ) ) {
        root.photoState = "valid"
        resolvedImageSource = "file://" + absolutePath
        tempSketchedImageSource = ""
      }
      else if ( __inputUtils.isValidUrl( absolutePath ) ) {
          root.photoState = "valid";
          resolvedImageSource = absolutePath;
      }
      else {
        root.photoState = "notAvailable"
        resolvedImageSource = ""
      }
    }

    /**
     * Called when clicked on the camera icon to capture an image.
     */
    function capturePhoto() {
      if ( !__inputUtils.createDirectory( targetDir ) )
      {
        __inputUtils.log( "Capture photo", "Could not create directory " + targetDir );
        __notificationModel.addError( qsTr( "Could not create directory %1." ).arg( targetDir ) )
      }

      if ( __androidUtils.isAndroid ) {
        __androidUtils.callCamera( targetDir, root._fieldIndex )
      }
      else if ( __iosUtils.isIos ) {
        __iosUtils.callCamera( targetDir, root._fieldIndex )
      }
      else {
        // This should never happen
        console.error( "Camera not implemented on this platform." )
      }
    }

    /**
     * Called when clicked on the gallery icon to choose a file from a gallery.
     * Ambiguity issues are handled with Connection's enabled property. We enable it
     * only when waiting for image. All usecases and bundle itself counts with one interaction
     * per one time.
     *
     * The workflow of choosing an image from a gallery starts here and goes as follows:
     * Android gallery even is evoked. When a user chooses image, "imageSelected( selectedImagePath )" is emitted.
     * Then "imageSelected" caught the signal, handles changes and sends signal "valueChanged".
     */
    function chooseFromGallery() {
      if ( __androidUtils.isAndroid ) {
        __androidUtils.callImagePicker( targetDir, root._fieldIndex )
      }
      else if ( __iosUtils.isIos ) {
        __iosUtils.callImagePicker( targetDir, root._fieldIndex )
      }
      else {
        desktopGalleryPicker.open()
      }
    }

    /**
     * Called to remove an image from a widget. A confirmation dialog is open first if a file exists.
     * ItemWidget reference is set here to delete an image for certain widget.
     * \param itemWidget editorWidget for modified field to send valueChanged signal.
     * \param imagePath Absolute path to an image.
     */
    function removeImage( path ) {
      if ( __inputUtils.fileExists( path ) ) {
        imageDeleteDialog.imagePath = path
        imageDeleteDialog.open()
      }
      else {
        root.editorValueChanged( "", true )
      }
    }

    /**
     * Called when an image is either selected from a gallery or captured by native camera. If the image doesn't exist in a folder
     * set in widget's config, it is copied to the destination and value is set according a new copy (only when chosen from gallery).
     * \param imgPath Absolute path to a selected image
     *
     * Used for Android and desktop builds
     */
    function imageSelected( imgPath ) {
      let filename = __inputUtils.getFileName( imgPath )

      //! final absolute location of an image.
      let absolutePath = __inputUtils.getAbsolutePath( filename, targetDir )

      if ( !__inputUtils.fileExists( absolutePath ) ) { // we need to copy it!

        let success = __inputUtils.copyFile( imgPath, absolutePath )

        if ( !success ) {
          __inputUtils.log( "Select image", "Failed to copy image file to " + absolutePath )
          __notificationModel.addError( qsTr( "Failed to process the image" ) )
        }
      }
      confirmImage( prefixToRelativePath, absolutePath )
    }

    /**
     * Called when an image is captured by a camera. Method sets proper value according given absolute path of the image
     * and prefixPath set in thd project settings.
     * \param imgPath Absolute path to a captured image
     *
     * Only used for iOS!
     */
    function imageCaptured( imgPath ) {
      if ( imgPath ) {

        let prefixPath = prefixToRelativePath.endsWith("/") ? prefixToRelativePath : prefixToRelativePath + "/"

        confirmImage( prefixPath, imgPath )
      }
    }

    /**
     * Called when a photo is taken and confirmed (clicked on check/ok button).
     * Original photo file is renamed with current date time to avoid name conflicts.
     * ItemWidget reference is always set here to avoid ambiguity in case of
     * multiple external resource (attachment) fields.
     * \param prefixToRelativePath depends on widget's config, see more inputexternalwidget.qml
     * \param imgPath
     */
    function confirmImage( prefixToRelativePath, imgPath ) {
      if ( imgPath ) {
        __inputUtils.rescaleImage( imgPath, __activeProject.qgsProject )
        let newImgPath = __inputUtils.getRelativePath( imgPath, prefixToRelativePath )

        root.editorValueChanged( newImgPath, newImgPath === "" || newImgPath === null )
        if ( photoSketchingLoader.active ) {
          sketchingController.prepareController()
        }
      }
    }
  }
}
