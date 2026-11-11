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
 * Audio form editor (external resource with the Audio document viewer) for QGIS Attribute Form.
 * Requires various global properties set to function, see featureform Loader section.
 * These properties are injected here via 'fieldXYZ' properties and captured with underscore `_`.
 *
 * Should be used only within feature form. See MMFormAudioViewer.
 *
 * The recording itself runs in the AudioRecorderController of the form page, because this editor is
 * a list delegate and gets destroyed when scrolled out of view. For the same reason the temporary files
 * are handled by the AttributeController (moved to the target folder on save, removed on cancel),
 * not by callbackOnFormSaved() / callbackOnFormCanceled().
 *
 * Paths are stored the same way as for photos, see the overview in MMFormPhotoEditor.
 */

MMFormAudioViewer {
  id: root

  property var _fieldValue: parent.fieldValue
  property var _fieldConfig: parent.fieldConfig
  property string _fieldItemId: parent.fieldItemId
  property bool _fieldValueIsNull: parent.fieldValueIsNull

  property string _fieldHomePath: parent.fieldHomePath
  property var _fieldActiveProject: parent.fieldActiveProject
  property MM.AttributeController _fieldController: parent.fieldController
  property AudioRecorderController _fieldAudioRecorder: parent.fieldAudioRecorder

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

  warningMsg: audioState === "notAvailable" ? qsTr( "Audio file is missing." ) : _fieldWarningMessage
  errorMsg: _fieldErrorMessage

  readOnly: _fieldFormIsReadOnly || !_fieldIsEditable
  shouldShowValidation: !_fieldFormIsReadOnly

  // audio is never reused for the next feature, see AttributeController
  hasCheckbox: false

  // the audio devices are not queried here, a missing microphone is reported when recording starts
  hasRecordCapability: !!root._fieldAudioRecorder

  isRecording: ( root._fieldAudioRecorder?.isRecording ?? false ) && root._fieldAudioRecorder.activeFieldItemId === root._fieldItemId
  recordingDuration: isRecording ? root._fieldAudioRecorder.duration : 0

  // no playback while any field records, the players must leave the audio session to the recorder
  playbackEnabled: !( root._fieldAudioRecorder?.isRecording ?? false )

  on_FieldValueChanged: internal.setAudioSource()
  on_FieldValueIsNullChanged: internal.setAudioSource()

  onRecordClicked: internal.startRecording()
  onChooseFromDeviceClicked: internal.chooseFromDevice()
  onStopRecordingClicked: root._fieldAudioRecorder.stopRecording()
  onRerecordClicked: {
    // while another field records, the recorder tells the form page, which shows a warning
    if ( root._fieldAudioRecorder?.isRecording ) {
      internal.startRecording()
    }
    else {
      replaceDialog.open()
    }
  }
  onTrashClicked: internal.removeAudio()

  Component.onCompleted: internal.setAudioSource()

  FileDialog {
    id: audioPicker

    title: qsTr( "Choose audio" )

    nameFilters: [ qsTr( "Audio files (%1)" ).arg( root._fieldAudioRecorder ? root._fieldAudioRecorder.audioNameFilters().join( " " ) : "" ) ]

    onAccepted: internal.audioChosen( selectedFile )
  }

  MMRemoveAudioDialog {
    id: removeDialog

    offerUnlinkOption: !root._fieldController.isNewFeature && !internal.isPending

    onDeleteAudio: {
      const wasPending = internal.isPending
      const path = removeDialog.audioPath

      // the value is reset first, so the player lets go of the file before it is removed
      resetValueAndClose()

      if ( wasPending ) {
        root._fieldController.discardPendingAudio( root._fieldItemId )
      }
      else {
        // the file is removed only when the feature is saved
        root._fieldController.scheduleFileRemoval( path )
      }
    }

    onUnlinkAudio: resetValueAndClose()

    function resetValueAndClose() {
      root.editorValueChanged( "", true )
      audioPath = ""
      close()
    }
  }

  MMReplaceAudioDialog {
    id: replaceDialog

    // a recording made in this form is dropped as soon as the new one is kept, a file from the project
    // is removed when the feature is saved. The recorder takes care of it, this editor may not exist by then
    onReplaceAudio: internal.startRecording( internal.isPending ? "" : internal.absolutePath )
  }

  Connections {
    target: root._fieldAudioRecorder

    function onRecordingFinished( fieldItemId ) {
      if ( fieldItemId === root._fieldItemId ) {
        internal.setAudioSource()
      }
    }
  }

  QtObject {
    id: internal

    /**
     * 0 - Relative path disabled
     * 1 - Relative path to project
     * 2 - Relative path to defaultRoot defined in the config - Default path field in the widget configuration form
     */
    property int relativeStorageMode: root._fieldConfig["RelativeStorage"]

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

    property string absolutePath // where the file is (or will be once saved) in the project
    property bool isPending: false // recorded or chosen in this form, still in the temporary folder

    function setAudioSource() {
      if ( !root._fieldValue || root._fieldValueIsNull ) {
        root.audioState = "notSet"
        root.audioUrl = ""
        root.fileName = ""
        absolutePath = ""
        isPending = false
        return
      }

      absolutePath = __inputUtils.getAbsolutePath( root._fieldValue, prefixToRelativePath )
      root.fileName = __inputUtils.getFileName( root._fieldValue )

      const playbackPath = root._fieldAudioRecorder ? root._fieldAudioRecorder.playbackPath( root._fieldItemId, absolutePath ) : absolutePath
      isPending = playbackPath !== absolutePath

      if ( __inputUtils.fileExists( playbackPath ) ) {
        root.audioState = "valid"
        root.audioUrl = __inputUtils.localFileToUrl( playbackPath )
      }
      else if ( __inputUtils.isValidUrl( absolutePath ) ) {
        root.audioState = "valid"
        root.audioUrl = absolutePath
      }
      else {
        root.audioState = "notAvailable"
        root.audioUrl = ""
      }
    }

    // fileToReplace: absolute path of the current file when re-recording, empty otherwise
    function startRecording( fileToReplace = "" ) {
      root.forceActiveFocus()

      if ( !root._fieldAudioRecorder ) {
        return
      }

      // only one recording at a time, otherwise the recorder emits recordingBusy()
      root._fieldAudioRecorder.startRecording( root._fieldItemId, root._fieldTitle, fileToReplace )
    }

    function chooseFromDevice() {
      root.forceActiveFocus()

      // the file picker would stop a running recording on Android
      if ( root._fieldAudioRecorder?.blockExternalActivity() ) {
        return
      }
      audioPicker.open()
    }

    function audioChosen( fileUrl ) {
      if ( !root._fieldAudioRecorder.importAudio( root._fieldItemId, fileUrl ) ) {
        __inputUtils.log( "Select audio", "Failed to import audio file " + fileUrl )
        __notificationModel.addError( qsTr( "Failed to process the audio file" ) )
        return
      }

      setAudioSource()
    }

    function removeAudio() {
      root.forceActiveFocus()

      if ( isPending || __inputUtils.fileExists( absolutePath ) ) {
        removeDialog.audioPath = absolutePath
        removeDialog.open()
      }
      else {
        root.editorValueChanged( "", true )
      }
    }
  }
}
