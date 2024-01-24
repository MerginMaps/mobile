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

Page {

  Rectangle {
    anchors.fill: parent
    color: __style.lightGreenColor
  }

  Column {
    width: parent.width

    MMPhotoGallery {
      title: "Gallery 7 photos - show just 5"
      maxVisiblePhotos: 5
      model: [
        "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg",
        "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg",
        "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg",
        "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg",
        "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg",
        "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg",
        "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg"
      ]
      onShowAll: console.log("Open Gallery")
      onClicked: function( path ) { console.log("Open " + path) }
    }

    MMPhotoGallery {
      title: "Gallery 7 photos - show all"
      maxVisiblePhotos: -1
      showAddImage: true
      model: [
        "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg",
        "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg",
        "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg",
        "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg",
        "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg",
        "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg",
        "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg"
      ]
      onShowAll: console.log("Open Gallery")
      onClicked: function( path ) { console.log("Open " + path) }
      onAddImage: console.log("Open camera")
    }

    MMPhotoGallery {
      title: "Gallery 3 photos"
      warningMsg: "The size of image is too big"
      showAddImage: true
      model: [
        "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg",
        "https://images.pexels.com/photos/955656/pexels-photo-955656.jpeg",
        "https://images.pexels.com/photos/559422/pexels-photo-559422.jpeg"
      ]
      onShowAll: console.log("Open Gallery")
      onClicked: function( path ) { console.log("Open " + path) }
      onAddImage: console.log("Open camera")
    }
  }
}
