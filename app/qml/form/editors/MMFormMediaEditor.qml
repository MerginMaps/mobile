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

import "../../components" as MMComponents
import "../../components/private" as MMPrivateComponents
import "../../dialogs"
import "../components/photo" as MMPhotoComponents
import "../components/media" as MMMediaComponents

/*
 * Audio and video form editor (external resource / attachment widget) for QGIS Attribute Form.
 * The editor is used when the "Integrated document viewer" of the attachment widget is set to Audio or Video
 * in QGIS (stored as "DocumentViewer" in the widget config, available since QGIS 3.30).
 *
 * Audio can be recorded directly in the app or chosen from files, video can be recorded
 * with the native camera or chosen from gallery. Both can be played back in the form.
 *
 * Path handling (default path, relative storage) follows the photo editor, see MMFormPhotoEditor.
 */

MMPrivateComponents.MMBaseInput {
  id: root

  property var _fieldValue: parent.fieldValue
  property var _fieldConfig: parent.fieldConfig
  property var _fieldIndex: parent.fieldIndex
  property bool _fieldValueIsNull: parent.fieldValueIsNull

  property string _fieldHomePath: parent.fieldHomePath
  property var _fieldActiveProject: parent.fieldActiveProject
  property MM.AttributeController _fieldController: parent.fieldController

  property bool _fieldShouldShowTitle: parent.fieldShouldShowTitle
  property bool _fieldFormIsReadOnly: parent.fieldFormIsReadOnly
  property bool _fieldIsEditable: parent.fieldIsEditable

  property string _fieldTitle: parent.fieldTitle
  property string _fieldErrorMessage: parent.fieldErrorMessage
  property string _fieldWarningMessage: parent.fieldWarningMessage

  property bool _fieldRememberValueSupported: parent.fieldRememberValueSupported
  property bool _fieldRememberValueState: parent.fieldRememberValueState

  // QgsExternalResourceWidget::DocumentViewerContent - Audio = 3, Video = 4
  readonly property bool isVideo: Number( _fieldConfig["DocumentViewer"] ) === 4

  property alias mediaState: mediaStateGroup.state

  signal editorValueChanged( var newValue, bool isNull )
  signal rememberValueBoxClicked( bool state )

  title: _fieldShouldShowTitle ? _fieldTitle : ""

  warningMsg: {
    if ( mediaState === "notAvailable" ) {
      return root.isVideo ? qsTr( "Video is missing." ) : qsTr( "Audio is missing." )
    }
    return _fieldWarningMessage
  }
  errorMsg: _fieldErrorMessage

  readOnly: _fieldFormIsReadOnly || !_fieldIsEditable
  shouldShowValidation: !_fieldFormIsReadOnly

  hasCheckbox: _fieldRememberValueSupported
  checkboxChecked: _fieldRememberValueState

  on_FieldValueChanged: internal.setMediaSource()
  on_FieldValueIsNullChanged: internal.setMediaSource()

  onCheckboxCheckedChanged: {
    root.rememberValueBoxClicked( checkboxChecked )
  }

  StateGroup {
    id: mediaStateGroup

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
    height: root.isVideo || mediaStateGroup.state === "notSet" ? __style.row160 * 1.5 : __style.row80

    color: __style.polarColor
    radius: __style.radius20

    MMMediaComponents.MMMediaPlayer {
      id: player

      anchors.fill: parent

      visible: mediaStateGroup.state === "valid"

      source: visible ? internal.resolvedMediaSource : ""
      showVideo: root.isVideo

      // keep the audio controls clear of the delete button
      controlsRightInset: !root.isVideo && deleteButton.visible ? deleteButton.width + __style.margin10 : 0
    }

    MMComponents.MMText {
      anchors.centerIn: parent
      width: parent.width - 2 * __style.margin20

      visible: mediaStateGroup.state === "notAvailable"

      font: __style.p5
      color: __style.nightColor
      horizontalAlignment: Text.AlignHCenter
      wrapMode: Text.WordWrap

      text: root._fieldValue ? root._fieldValue : ""
    }

    MMComponents.MMRoundButton {
      id: deleteButton

      anchors {
        right: parent.right
        top: parent.top
        rightMargin: __style.margin10
        topMargin: root.isVideo ? __style.margin10 : ( parent.height - height ) / 2
      }

      z: 1

      bgndColor: __style.negativeColor
      iconSource: __style.deleteIcon
      iconColor: __style.grapeColor

      visible: root.editState === "enabled" && mediaStateGroup.state !== "notSet"

      onClicked: internal.removeMedia( __inputUtils.getAbsolutePath( root._fieldValue, internal.prefixToRelativePath ) )
    }

    MMPhotoComponents.MMPhotoAttachment {
      width: parent.width
      height: parent.height

      visible: mediaStateGroup.state === "notSet"
      enabled: root.editState === "enabled"

      captureIconSource: root.isVideo ? __style.addVideoIcon : __style.addAudioIcon
      captureText: root.isVideo ? qsTr( "Record a video" ) : qsTr( "Record audio" )
      chooseIconSource: __style.morePhotosIcon
      chooseText: root.isVideo ? qsTr( "From gallery" ) : qsTr( "From files" )

      // audio is recorded within the app, video needs a native camera
      hasCameraCapability: !root.isVideo || __androidUtils.isAndroid || __iosUtils.isIos
      // there is no audio file picker on iOS
      hasChooseCapability: root.isVideo || !__iosUtils.isIos

      onCapturePhotoClicked: internal.captureMedia()
      onChooseFromGalleryClicked: internal.chooseMedia()
    }
  }

  // used only on desktop builds
  FileDialog {
    id: desktopMediaPicker

    title: root.isVideo ? qsTr( "Open Video" ) : qsTr( "Open Audio" )

    nameFilters: root.isVideo ? [ qsTr( "Video files (*.mp4 *.mov *.m4v *.3gp *.mkv *.webm *.avi)" ) ]
                              : [ qsTr( "Audio files (*.m4a *.mp3 *.aac *.wav *.ogg *.opus *.flac *.amr *.3gp)" ) ]

    currentFolder: __inputUtils.imageGalleryLocation()
    onAccepted: {
      internal.mediaSelected( selectedFile.toString() )
    }
  }

  MMRemovePhotoDialog {
    id: mediaDeleteDialog

    property string mediaPath
    offerUnlinkOption: !root._fieldController.isNewFeature

    title: root.isVideo ? qsTr( "Delete video?" ) : qsTr( "Delete audio?" )
    description: {
      if ( root.isVideo ) {
        return offerUnlinkOption ? qsTr( "Would you like to delete or unlink the video? Deleting removes the video from your project entirely, while unlinking keeps the video in your project but removes it from this specific feature." ) : qsTr( "Would you like to delete the video?" )
      }
      return offerUnlinkOption ? qsTr( "Would you like to delete or unlink the audio? Deleting removes the audio from your project entirely, while unlinking keeps the audio in your project but removes it from this specific feature." ) : qsTr( "Would you like to delete the audio?" )
    }

    primaryButton.text: root.isVideo ? qsTr( "Delete video" ) : qsTr( "Delete audio" )
    secondaryButton.text: root.isVideo ? qsTr( "Unlink video" ) : qsTr( "Unlink audio" )

    onDeleteImage: {
      // schedule the file for deletion
      internal.mediaSourceToDelete = mediaDeleteDialog.mediaPath
      resetValueAndClose()
    }

    onUnlinkImage: resetValueAndClose()

    function resetValueAndClose() {
      player.stop()
      root.editorValueChanged( "", true )

      mediaPath = ""
      close()
    }
  }

  Loader {
    id: audioRecorderLoader

    active: false
    sourceComponent: audioRecorderComponent
  }

  Component {
    id: audioRecorderComponent

    MMMediaComponents.MMAudioRecorderDrawer {
      targetDir: internal.targetDir

      Component.onCompleted: open()

      onRecordingFinished: function( path ) {
        internal.confirmMedia( path )
      }

      onClosed: {
        audioRecorderLoader.active = false
      }
    }
  }

  Connections {
    target: __androidUtils

    // used for both file picker and camera
    function onMediaSelected( mediaPath, index ) {
      if ( root._fieldIndex.toString() === index.toString() ) {
        internal.mediaSelected( mediaPath )
      }
    }
  }

  Connections {
    target: __iosUtils

    // used for both gallery and camera
    function onMediaSelected( mediaPath, index ) {
      if ( root._fieldIndex.toString() === index.toString() ) {
        internal.mediaSelected( mediaPath )
      }
    }
  }

  function callbackOnFormSaved() {
    if ( internal.mediaSourceToDelete ) {
      __inputUtils.removeFile( internal.mediaSourceToDelete )
      internal.mediaSourceToDelete = ""
    }
  }

  function callbackOnFormCanceled() {
    internal.mediaSourceToDelete = ""
  }

  QtObject {
    id: internal

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
                                 root._fieldController.featureLayerPair,
                                 root._fieldController.parentController?.featureLayerPair ?? __inputUtils.createFeatureLayerPair(),
                                 root._fieldActiveProject
                                 )

    property string prefixToRelativePath: __inputUtils.resolvePrefixForRelativePath(
                                            relativeStorageMode,
                                            root._fieldHomePath,
                                            targetDir
                                            )

    property string resolvedMediaSource

    property string mediaSourceToDelete // used to postpone file deletion to when the form is saved

    //
    // Sets url of the assigned audio/video to the resolvedMediaSource.
    //  - resolvedMediaSource is the actual file url on the device or a remote url
    //
    function setMediaSource() {
      if ( !root._fieldValue || root._fieldValueIsNull ) {
        root.mediaState = "notSet"
        resolvedMediaSource = ""
        return
      }

      const absolutePath = __inputUtils.getAbsolutePath( root._fieldValue, internal.prefixToRelativePath )

      if ( __inputUtils.fileExists( absolutePath ) ) {
        root.mediaState = "valid"
        resolvedMediaSource = __inputUtils.localFileToUrl( absolutePath )
      }
      else if ( __inputUtils.isValidUrl( absolutePath ) ) {
        root.mediaState = "valid"
        resolvedMediaSource = absolutePath
      }
      else {
        root.mediaState = "notAvailable"
        resolvedMediaSource = ""
      }
    }

    function prepareTargetDir() {
      updateTargetDir()
      if ( !__inputUtils.createDirectory( targetDir ) ) {
        __inputUtils.log( "Capture media", "Could not create directory " + targetDir )
        __notificationModel.addError( qsTr( "Could not create directory %1." ).arg( targetDir ) )
        return false
      }
      return true
    }

    /**
     * Called when clicked on the record button - opens audio recorder or native camera in video mode.
     */
    function captureMedia() {
      root.forceActiveFocus()
      if ( !prepareTargetDir() ) {
        return
      }

      if ( !root.isVideo ) {
        audioRecorderLoader.active = true
        audioRecorderLoader.focus = true
      }
      else if ( __androidUtils.isAndroid ) {
        __androidUtils.callVideoCamera( targetDir, root._fieldIndex )
      }
      else if ( __iosUtils.isIos ) {
        __iosUtils.callVideoCamera( targetDir, root._fieldIndex )
      }
      else {
        // This should never happen
        console.error( "Video camera not implemented on this platform." )
      }
    }

    /**
     * Called when clicked on the gallery/files button to choose an existing audio or video file.
     */
    function chooseMedia() {
      root.forceActiveFocus()
      if ( !prepareTargetDir() ) {
        return
      }

      if ( __androidUtils.isAndroid ) {
        __androidUtils.callMediaPicker( targetDir, root.isVideo ? "video/*" : "audio/*", root._fieldIndex )
      }
      else if ( __iosUtils.isIos ) {
        __iosUtils.callVideoPicker( targetDir, root._fieldIndex )
      }
      else {
        desktopMediaPicker.open()
      }
    }

    /**
     * Called to remove the attachment from the widget. A confirmation dialog is open first if the file exists.
     */
    function removeMedia( path ) {
      root.forceActiveFocus()
      if ( __inputUtils.fileExists( path ) ) {
        mediaDeleteDialog.mediaPath = path
        mediaDeleteDialog.open()
      }
      else {
        root.editorValueChanged( "", true )
      }
    }

    /**
     * Called when a file is either selected from gallery/files or recorded by native camera.
     * If the file doesn't exist in the target folder, it is copied there first.
     * \param mediaPath Absolute path or file url of the selected file
     */
    function mediaSelected( mediaPath ) {
      updateTargetDir()
      if ( !targetDir ) {
        __inputUtils.log( "Select media", "Failed to resolve target dir for new media file" )
        __notificationModel.addError( qsTr( "Failed to process the file, directory resolving failed" ) )
        return
      }

      const isUrl = mediaPath.startsWith( "file://" )
      const filename = isUrl ? decodeURIComponent( __inputUtils.getFileName( mediaPath ) ) : __inputUtils.getFileName( mediaPath )

      //! final absolute location of the file
      const absolutePath = __inputUtils.getAbsolutePath( filename, targetDir )

      if ( !__inputUtils.fileExists( absolutePath ) ) { // we need to copy it!
        const sourcePath = isUrl ? __inputUtils.getRelativePath( mediaPath, "" ) : mediaPath
        const success = __inputUtils.copyFile( sourcePath, absolutePath )

        if ( !success ) {
          __inputUtils.log( "Select media", "Failed to copy media file to " + absolutePath )
          __notificationModel.addError( qsTr( "Failed to process the file" ) )
          return
        }
      }
      confirmMedia( absolutePath )
    }

    /**
     * Sets the field value to the path of the file relative to the configured storage mode.
     */
    function confirmMedia( absolutePath ) {
      if ( absolutePath ) {
        const newPath = __inputUtils.getRelativePath( absolutePath, prefixToRelativePath )
        root.editorValueChanged( newPath, newPath === "" || newPath === null )
      }
    }

    /**
     * Function updates the targetDir property with a new value. Necessary when the field uses QGIS expression,
     * which references another field in the same form, to save files in certain directory.
     */
    function updateTargetDir() {
      targetDir = __inputUtils.resolveTargetDir( root._fieldHomePath, root._fieldConfig, root._fieldController.featureLayerPair, root._fieldController.parentController?.featureLayerPair ?? __inputUtils.createFeatureLayerPair(), root._fieldActiveProject )
    }
  }
}
