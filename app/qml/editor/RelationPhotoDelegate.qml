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
import QtQml.Models
import QtQuick.Layouts
import Qt5Compat.GraphicalEffects

import lc 1.0
import ".."

Item {
  id: root

  signal clicked( var feature )

  height: parent.height
  width: height

  Image {
    id: image

    property bool imageValid: true

    anchors.centerIn: parent
    width: imageValid ? parent.width : parent.width * 0.4
    height: imageValid ? parent.width : parent.width * 0.4
    sourceSize.width: image.width
    sourceSize.height: image.height
    visible: imageValid

    autoTransform: true
    source: {
      let absolutePath = model.PhotoPath

      if (image.status === Image.Error) {
        image.imageValid = false
        customStyle.icons.notAvailable
      }
      else if (absolutePath !== '' && __inputUtils.fileExists(absolutePath)) {
        "file://" + absolutePath
      }
      else {
        image.imageValid = false
        customStyle.icons.notAvailable
      }
    }

    horizontalAlignment: Image.AlignHCenter
    verticalAlignment: Image.AlignVCenter
    mipmap: true
    fillMode: Image.PreserveAspectFit
  }

  ColorOverlay {
    source: image.imageValid ? undefined : image
    anchors.fill: image.imageValid ? undefined : image
    color: customStyle.relationComponent.iconColor
  }

  Rectangle { // border
    anchors.fill: parent

    border.color: customStyle.relationComponent.photoBorderColor
    border.width: customStyle.relationComponent.photoBorderWidth
    color: "transparent"
  }

  MouseArea {
    anchors.fill: parent
    onClicked: function() {
      root.clicked( model.FeaturePair )
    }
  }
}
