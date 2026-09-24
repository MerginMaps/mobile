/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
pragma ComponentBehavior: Bound

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
  layer.effect: MultiEffect {
    maskEnabled: true
    maskSource: maskRect
    autoPaddingEnabled: false
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

  // On Android, backgrounding/resuming the app (e.g. while picking a photo) can tear down and
  // recreate the EGL surface. If this item's layer renders while that surface is still settling,
  // the resulting texture can be corrupted (seen as a blank/white photo on some Android 9 devices),
  // and nothing else triggers a re-render afterwards. Force the layer to regenerate once the app
  // is confirmed active again.
  Connections {
    target: Qt.application
    function onStateChanged() {
      if ( Qt.application.state === Qt.ApplicationActive ) {
        root.layer.enabled = false
        maskRect.layer.enabled = false
        Qt.callLater( function() {
          root.layer.enabled = true
          maskRect.layer.enabled = true
        } )
      }
    }
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
