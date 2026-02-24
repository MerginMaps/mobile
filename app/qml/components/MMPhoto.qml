/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Effects

Image {
  id: root

  property url photoUrl
  property bool isLocalFile: true

  signal clicked( url path )

  height: width
  source: root.photoUrl
  asynchronous: true
  autoTransform: true
  layer.enabled: true
  layer.effect: Component {
    MultiEffect {
      maskEnabled: true
      maskSource: maskRect
      autoPaddingEnabled: false
    }
  }

  // The mask shape
  Rectangle {
    id: maskRect
    width: root.width
    height: root.height
    radius: 20 * __dp
    visible: false
    layer.enabled: true 
  }

  Rectangle {
    anchors.fill: root
    color: __style.polarColor
    z: -1
    visible: root.photoUrl.toString() === "" || root.status === Image.Error // if image has transparent background, we would still see it

    MMIcon {
      anchors.centerIn: parent
      source: root.photoUrl && root.isLocalFile ? __style.morePhotosIcon : __style.remoteImageLoadErrorIcon
      color: __style.mediumGreenColor
      size: __style.icon32
    }
  }

  MMSingleClickMouseArea {
    anchors.fill: root
    onSingleClicked: root.clicked(root.photoUrl)
  }

  MMBusyIndicator {
    id: busyIndicator
    anchors.centerIn: root
    visible: root.status === Image.Loading
  }

  onStatusChanged: {
    if (status === Image.Error) {
      console.error("MMPhoto: Error loading image: " + root.photoUrl);
    }
  }
}
