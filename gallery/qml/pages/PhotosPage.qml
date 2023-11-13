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

import "../../app/qmlV2/component"
import "../../app/qmlV2/Style.js" as Style

Page {

  Rectangle {
    anchors.fill: parent
    color: Style.lightGreen
  }

  Column {
    width: parent.width

    MMPhotoGallery {
      model: [
        "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg",
        "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg",
        "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg",
        "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg",
        "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg",
      ]
    }

    MMPhotoGallery {
      model: [
        "https://images.pexels.com/photos/615348/forest-fog-sunny-nature-615348.jpeg",
        "https://images.pexels.com/photos/955656/pexels-photo-955656.jpeg",
        "https://images.pexels.com/photos/559422/pexels-photo-559422.jpeg"]
    }
  }
}
