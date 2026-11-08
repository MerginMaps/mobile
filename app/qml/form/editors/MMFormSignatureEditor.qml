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

import "../../dialogs"
import "../components" as MMFormComponents

/*
 * Signature form editor (external resource) for QGIS Attribute Form.
 * The parent delegate (see featureform Loader section in MMFormPage.qml) sets properties such as
 * fieldValue, fieldConfig and fieldIndex, re-exposed here as local _fieldValue, _fieldConfig, etc.
 *
 * Draws the signature with PhotoSketchingController and MMFormPhotoSketchingPageDialog on a blank
 * canvas image. A confirmed signature can only be deleted, not redrawn.
 */

MMFormSignatureViewer {
  id: root

  readonly property int canvasWidth: 1200
  readonly property int canvasHeight: 600

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

  signal editorValueChanged( var newValue, bool isNull )

  title: _fieldShouldShowTitle ? _fieldTitle : ""

  warningMsg: signatureState === "notAvailable" ? qsTr( "Signature is missing." ) : _fieldWarningMessage
  errorMsg: _fieldErrorMessage

  readOnly: _fieldFormIsReadOnly || !_fieldIsEditable
  shouldShowValidation: !_fieldFormIsReadOnly

  signatureUrl: internal.tempSketchedImageSource ? internal.tempSketchedImageSource : internal.resolvedImageSource

  on_FieldValueChanged: {
    internal.setImageSource()
    // after editing geometry this needs to be triggered to recover an unsaved drawing for components which were not in view
    signatureSketchingController.prepareController()
  }
  on_FieldValueIsNullChanged: internal.setImageSource()

  onSignNowClicked: internal.signNow()
  onTrashClicked: internal.removeSignature( __inputUtils.getAbsolutePath( root._fieldValue, internal.prefixToRelativePath ) )

  MMRemovePhotoDialog {
    id: signatureDeleteDialog

    property string signaturePath

    offerUnlinkOption: false
    title: qsTr( "Delete signature?" )
    description: qsTr( "Deleting removes the signature from this feature entirely. The feature will need to be signed again." )
    primaryButton.text: qsTr( "Delete signature" )

    onDeleteImage: {
      internal.signatureSourceToDelete = signatureDeleteDialog.signaturePath
      signatureSketchingController.removeBackupSketches()
      signatureSketchingController.clear()
      root.editorValueChanged( "", true )

      errorMsg = ""
      signaturePath = ""
      close()
    }
  }

  // draws on a blank canvas image
  PhotoSketchingController {
    id: signatureSketchingController

    photoSource: internal.canvasUrl
    projectName: root._fieldActiveProject.homePath
  }

  Connections {
    target: signatureSketchingController

    function onTempPhotoSourceChanged( newPath ) {
      internal.tempSketchedImageSource = __inputUtils.localFileToUrl( newPath )

      // commit the field value once per sign session, saveSketches() finalizes it on form save
      if ( !internal.signatureConfirmed ) {
        internal.signatureConfirmed = true
        const newValue = __inputUtils.getRelativePath( internal.pendingSignaturePath, internal.prefixToRelativePath )
        root.editorValueChanged( newValue, newValue === "" || newValue === null )
      }
    }

    function onSketchesSavingError() {
      __notificationModel.addError( qsTr( "Signature could not be saved, please contact support." ) )
    }
  }

  Loader {
    id: signatureDialogLoader
    active: false
    sourceComponent: signatureDialogComponent

    onLoaded: item.open()
  }

  Component {
    id: signatureDialogComponent

    MMFormComponents.MMFormPhotoSketchingPageDialog {
      photoUrl: internal.canvasUrl
      controller: signatureSketchingController

      onClosed: signatureDialogLoader.active = false
    }
  }

  function callbackOnFormSaved() {
    if ( internal.signatureSourceToDelete ) {
      __inputUtils.removeFile( internal.signatureSourceToDelete )
      internal.signatureSourceToDelete = ""
    }
  }

  function callbackOnFormCanceled() {
    internal.signatureSourceToDelete = ""
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

    property string resolvedImageSource

    property string tempSketchedImageSource

    property string signatureSourceToDelete // used to postpone file deletion to when the form is saved

    property string pendingSignaturePath // blank canvas created for the sign session currently in progress
    property string canvasUrl
    property bool signatureConfirmed

    function setImageSource() {
      if ( !root._fieldValue || root._fieldValueIsNull ) {
        root.signatureState = "notSet"
        resolvedImageSource = ""
        return
      }

      const absolutePath = __inputUtils.getAbsolutePath( root._fieldValue, internal.prefixToRelativePath )

      if ( __inputUtils.fileExists( absolutePath ) ) {
        root.signatureState = "valid"
        resolvedImageSource = __inputUtils.localFileToUrl( absolutePath )
        tempSketchedImageSource = ""

        // point the controller back at this file so prepareController() can recover a pending temp backup
        pendingSignaturePath = absolutePath
        signatureConfirmed = true
        canvasUrl = resolvedImageSource
      }
      else {
        root.signatureState = "notAvailable"
        resolvedImageSource = ""
      }
    }

    /**
     * Called when "Sign now" is clicked.
     */
    function signNow() {
      forceActiveFocus()
      updateTargetDir()
      if ( !__inputUtils.createDirectory( targetDir ) )
      {
        __inputUtils.log( "Sign feature", "Could not create directory " + targetDir );
        __notificationModel.addError( qsTr( "Could not create directory %1." ).arg( targetDir ) )
        return
      }

      const username = __merginApi.userInfo && __merginApi.userInfo.username.length > 0 ? __merginApi.userInfo.username : "anonymous"
      const timestamp = Qt.formatDateTime( new Date(), "yyyyMMdd_HHmmsszzz" )
      const filename = "signature-" + username + "-" + timestamp + ".png"
      const absolutePath = __inputUtils.getAbsolutePath( filename, targetDir )

      if ( !__inputUtils.createSignatureCanvas( absolutePath, root.canvasWidth, root.canvasHeight ) ) {
        __notificationModel.addError( qsTr( "Signature could not be created, please contact support." ) )
        return
      }
      __inputUtils.writeSignatureMetadata( absolutePath, PositionKit.hasPosition, PositionKit.latitude, PositionKit.longitude, PositionKit.altitude )

      pendingSignaturePath = absolutePath
      signatureConfirmed = false
      canvasUrl = __inputUtils.localFileToUrl( absolutePath )

      signatureSketchingController.clear()
      signatureSketchingController.activeColor = __style.photoSketchingBlackColor
      signatureSketchingController.prepareController()

      signatureDialogLoader.active = true
    }

    /**
     * Called to remove a signature. A confirmation dialog is shown first if the file exists.
     */
    function removeSignature( path ) {
      forceActiveFocus()
      if ( __inputUtils.fileExists( path ) ) {
        signatureDeleteDialog.signaturePath = path
        signatureDeleteDialog.open()
      }
      else {
        root.editorValueChanged( "", true )
      }
    }

    /**
     * Function updates the targetDir property with a new value. Necessary when the signature field uses
     * a QGIS expression, which references another field in the same form, to save signatures in a certain directory.
     */
    function updateTargetDir() {
      targetDir = __inputUtils.resolveTargetDir( root._fieldHomePath, root._fieldConfig, root._fieldController.featureLayerPair, root._fieldController.parentController?.featureLayerPair ?? __inputUtils.createFeatureLayerPair(), root._fieldActiveProject )
    }
  }
}
