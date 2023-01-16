/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
pragma Singleton
import QtQuick
import QtMultimedia

/**
  * MMCamera class has been created to work around iOS camera crash in Qt 6.4.2
  * https://bugreports.qt.io/browse/QTBUG-110131
  *
  * It can be removed once the bug is fixed and Camera can be instantiated / destroyed
  * several times.
  * This way we create CaptureSession (with Camera obj) only once and reuse it from that point on.
  * MMCamera is registered as singleton class. One can access the camera either via `captureSession`
  * object or through `session()` function call.
  */
QtObject {
  id: root

  property CaptureSession captureSession: null

  function setActive( active ) {
    if ( captureSession && captureSession.camera ) {
      captureSession.camera.active = active;
    }
  }

  function session() {
    // create camera object if it does not exist yet
    if ( !captureSession ) {
      let component = Qt.createQmlObject( `
           import QtQuick
           import QtMultimedia

           CaptureSession {
             id: captureSession

             camera: Camera {
               id: camera

               active: false

               focusMode: Camera.FocusModeAutoNear
             }
           }`, root )

      root.captureSession = component;
    }

    return root.captureSession;
  }
}

