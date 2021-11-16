/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQml.Models 2.14
import QtQuick.Layouts 1.14
import QtGraphicalEffects 1.14
import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "../"

Item {
  id: root
  height: parent.height
  width: height

  signal clicked(var feature)

  Image {
    id: image
    property bool imageValid: true

    anchors.centerIn: parent
    autoTransform: true
    fillMode: Image.PreserveAspectFit
    height: imageValid ? parent.width : parent.width * 0.4
    horizontalAlignment: Image.AlignHCenter
    mipmap: true
    source: {
      let absolutePath = model.PhotoPath;
      if (image.status === Image.Error) {
        image.imageValid = false;
        customStyle.icons.notAvailable;
      } else if (absolutePath !== '' && __inputUtils.fileExists(absolutePath)) {
        "file://" + absolutePath;
      } else {
        image.imageValid = false;
        customStyle.icons.notAvailable;
      }
    }
    sourceSize.height: image.height
    sourceSize.width: image.width
    verticalAlignment: Image.AlignVCenter
    visible: imageValid
    width: imageValid ? parent.width : parent.width * 0.4
  }
  ColorOverlay {
    anchors.fill: image.imageValid ? undefined : image
    color: customStyle.relationComponent.iconColor
    source: image.imageValid ? undefined : image
  }
  Rectangle {
    // border
    anchors.fill: parent
    border.color: customStyle.relationComponent.photoBorderColor
    border.width: customStyle.relationComponent.photoBorderWidth
    color: "transparent"
  }
  MouseArea {
    anchors.fill: parent

    onClicked: root.clicked(model.FeaturePair)
  }
}
