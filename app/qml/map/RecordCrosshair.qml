/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.14
import QtGraphicalEffects 1.14

import lc 1.0
import ".."

Item {    
    id: root

    /*required*/ property var qgsProject
    /*required*/ property var mapsettings

    property point center: Qt.point( root.width / 2, root.height / 2 )

    property point recordPoint: snapUtils.snapped ? snapUtils.snappedPosition : center

    property real size: 50 * __dp

    SnapUtils {
      id: snapUtils

      mapSettings: root.mapsettings
      qgsProject: root.qgsProject

      centerPosition: root.center
    }

    Image {
      id: crossBorder

//      anchors.centerIn: parent

      x: root.recordPoint.x - width / 2
      y: root.recordPoint.y - height / 2

      Behavior on x {
        PropertyAnimation {
          properties: "x"
          duration: 50
          easing.type: Easing.InQuad
        }
      }

      Behavior on y {
        PropertyAnimation {
          properties: "y"
          duration: 50
          easing.type: Easing.InQuad
        }
      }

      height: root.size
      width: height
      source: InputStyle.crosshairAltIcon
      sourceSize.width: width
      sourceSize.height: height
    }

    ColorOverlay {
      anchors.fill: crossBorder
      source: crossBorder
      color: snapUtils.snapped ? "#8a2be2" : InputStyle.fontColor
    }

    Image {
      id: crossCenter

      x: root.recordPoint.x - width / 2
      y: root.recordPoint.y - height / 2

      Behavior on x {
        PropertyAnimation {
          properties: "x"
          duration: 50
          easing.type: Easing.InQuad
        }
      }

      Behavior on y {
        PropertyAnimation {
          properties: "y"
          duration: 50
          easing.type: Easing.InQuad
        }
      }

      height: root.size
      width: height
      sourceSize.width: width
      sourceSize.height: height

      visible: false

      source: InputStyle.crosshairCenterIcon
    }

    ColorOverlay {
      anchors.fill: crossCenter
      source: crossCenter
      color: snapUtils.snapped ? "#8a2be2" : InputStyle.fontColor

      visible: snapUtils.snapped
    }
}
