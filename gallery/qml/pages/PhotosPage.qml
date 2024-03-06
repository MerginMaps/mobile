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

import "../../app/qml/components"
import "../../app/qml/form/editors"

Page {

  Rectangle {
    anchors.fill: parent
    color: __style.lightGreenColor
  }

  Column {
    width: parent.width
    spacing: 20
    anchors.horizontalCenter: parent.horizontalCenter
    anchors.top: parent.top
    anchors.topMargin: 20
    anchors.left: parent.left
    anchors.leftMargin: 20

    MMFormPhotoViewer {
      width: parent.width

      onCapturePhotoClicked: console.log("onCapturePhotoClicked")
      onChooseFromGalleryClicked: console.log("onChooseFromGalleryClicked")
    }

    MMFormPhotoViewer {
      width: parent.width

      photoUrl: "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg"
      state: "valid"
    }

    MMFormPhotoViewer {
      width: 200

      photoUrl: "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg"
      state: "valid"
    }

    MMFormPhotoViewer {
      width: 200

      state: "notAvailable"
    }
  }
}
