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

import "../components"
import "../form/components/media" as MMMediaComponents

/*
 * Quick capture saves a point at the GPS position without opening the feature form.
 * Field values come from the layer's default values, the behaviour is configured
 * in the field-profile.json file of the project, see QuickCaptureController.
 *
 * - capturePoint() lets the user pick a type (e.g. "Inaccessible road") and saves the point;
 *   types with "form": true (e.g. "Note") open the feature form instead, see featureFormRequested
 * - captureMedia() lets the user take a photo, record audio or record a video and saves a point with it
 */

Item {
  id: root

  readonly property bool pointCaptureEnabled: controller.enabled
  readonly property bool mediaCaptureEnabled: controller.mediaKinds.length > 0

  //! A new point to complete in the feature form, not saved yet
  signal featureFormRequested( var pair )

  function capturePoint() {
    const types = controller.types
    if ( types.length === 0 ) {
      controller.capturePoint( undefined )
    }
    else if ( types.length === 1 ) {
      internal.captureType( types[0] )
    }
    else {
      typesDrawerLoader.active = true
    }
  }

  function captureMedia() {
    const kinds = controller.mediaKinds
    if ( kinds.length === 1 ) {
      internal.startMediaCapture( kinds[0] )
    }
    else {
      mediaDrawerLoader.active = true
    }
  }

  MM.QuickCaptureController {
    id: controller

    qgsProject: __activeProject.qgsProject
    variablesManager: __variablesManager

    onCaptureSucceeded: function( message ) {
      __notificationModel.addSuccess( message )
    }

    onCaptureFailed: function( message ) {
      __notificationModel.addError( message )
    }
  }

  Connections {
    target: __activeProject

    function onProjectReloaded( qgsProject ) {
      controller.reload()
    }
  }

  Loader {
    id: typesDrawerLoader

    active: false
    sourceComponent: typesDrawerComponent
  }

  Component {
    id: typesDrawerComponent

    MMListDrawer {
      id: typesDrawer

      drawerHeader.title: qsTr( "Quick capture" )

      Component.onCompleted: open()
      onClosed: typesDrawerLoader.active = false

      list.model: controller.types

      list.delegate: MMListDelegate {
        text: modelData.label ?? modelData.value

        leftContent: MMIcon { source: __style.quickPointIcon }

        onClicked: {
          typesDrawer.close()
          internal.captureType( modelData )
        }
      }
    }
  }

  Loader {
    id: mediaDrawerLoader

    active: false
    sourceComponent: mediaDrawerComponent
  }

  Component {
    id: mediaDrawerComponent

    MMListDrawer {
      id: mediaDrawer

      drawerHeader.title: qsTr( "Quick capture" )

      Component.onCompleted: open()
      onClosed: mediaDrawerLoader.active = false

      list.model: controller.mediaKinds

      list.delegate: MMListDelegate {
        text: {
          if ( modelData === "photo" ) return qsTr( "Take a photo" )
          if ( modelData === "audio" ) return qsTr( "Record audio" )
          return qsTr( "Record a video" )
        }

        leftContent: MMIcon {
          source: {
            if ( modelData === "photo" ) return __style.cameraIcon
            if ( modelData === "audio" ) return __style.microphoneIcon
            return __style.addVideoIcon
          }
        }

        onClicked: {
          mediaDrawer.close()
          internal.startMediaCapture( modelData )
        }
      }
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
      targetDir: controller.mediaTargetDir( "audio" )

      Component.onCompleted: open()

      onRecordingFinished: function( path ) {
        controller.captureMedia( "audio", path )
      }

      onClosed: audioRecorderLoader.active = false
    }
  }

  // used only on desktop builds, where there is no camera
  FileDialog {
    id: desktopMediaPicker

    property string kind

    title: kind === "video" ? qsTr( "Open Video" ) : qsTr( "Open Image" )
    nameFilters: kind === "video" ? [ qsTr( "Video files (*.mp4 *.mov *.m4v *.3gp *.mkv *.webm *.avi)" ) ]
                                  : [ qsTr( "Image files (*.gif *.png *.jpg)" ) ]
    currentFolder: __inputUtils.imageGalleryLocation()

    onAccepted: controller.captureMedia( kind, selectedFile.toString() )
  }

  // codes identify our requests to the native camera, form editors use field indexes
  Connections {
    target: __androidUtils

    function onImageSelected( imagePath, code ) {
      if ( code === internal.photoCode ) {
        controller.captureMedia( "photo", imagePath )
      }
    }

    function onMediaSelected( mediaPath, code ) {
      if ( code === internal.videoCode ) {
        controller.captureMedia( "video", mediaPath )
      }
    }
  }

  Connections {
    target: __iosUtils

    function onImageSelected( imagePath, code ) {
      if ( code === internal.photoCode ) {
        controller.captureMedia( "photo", imagePath )
      }
    }

    function onMediaSelected( mediaPath, code ) {
      if ( code === internal.videoCode ) {
        controller.captureMedia( "video", mediaPath )
      }
    }
  }

  QtObject {
    id: internal

    readonly property string photoCode: "quickCapturePhoto"
    readonly property string videoCode: "quickCaptureVideo"

    function captureType( type ) {
      if ( type.form ) {
        const pair = controller.prepareFeature( type.value )
        if ( pair.valid ) {
          root.featureFormRequested( pair )
        }
      }
      else {
        controller.capturePoint( type.value )
      }
    }

    function startMediaCapture( kind ) {
      const targetDir = controller.mediaTargetDir( kind )
      if ( !__inputUtils.createDirectory( targetDir ) ) {
        __inputUtils.log( "Quick capture", "Could not create directory " + targetDir )
        __notificationModel.addError( qsTr( "Could not create directory %1." ).arg( targetDir ) )
        return
      }

      if ( kind === "audio" ) {
        audioRecorderLoader.active = true
      }
      else if ( __androidUtils.isAndroid ) {
        if ( kind === "photo" )
          __androidUtils.callCamera( targetDir, photoCode )
        else
          __androidUtils.callVideoCamera( targetDir, videoCode )
      }
      else if ( __iosUtils.isIos ) {
        if ( kind === "photo" )
          __iosUtils.callCamera( targetDir, photoCode )
        else
          __iosUtils.callVideoCamera( targetDir, videoCode )
      }
      else {
        desktopMediaPicker.kind = kind
        desktopMediaPicker.open()
      }
    }
  }
}
