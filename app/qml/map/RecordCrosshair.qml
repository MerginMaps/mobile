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
    property bool shouldUseSnapping

    property point center: Qt.point( root.width / 2, root.height / 2 )

    property point recordPoint: snapUtils.snapped ? snapUtils.snappedPosition : center

    property real outerSize: 60 * __dp
    property real innerDotSize: 10 * __dp

    SnapUtils {
      id: snapUtils

      mapSettings: root.mapsettings
      qgsProject: root.qgsProject
      useSnapping: root.shouldUseSnapping

      centerPosition: root.center
    }

    Image {
      id: crosshairBackground // white background of the crosshair

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

      height: root.outerSize
      width: height

      source: InputStyle.crosshairBakcgroundIcon
      sourceSize.width: width
      sourceSize.height: height
    }

    Image {
      id: crosshairForeground // green / purple outer circle of the crosshair

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

      height: root.outerSize
      width: height

      source: InputStyle.crosshairForegroundIcon
      sourceSize.width: width
      sourceSize.height: height
    }

    ColorOverlay {
      anchors.fill: crosshairForeground
      source: crosshairForeground
      color: snapUtils.snapped ? "#8a2be2" : InputStyle.fontColor
    }

    Image {
      id: crossCenterDot // Center dot - visible when not snapped

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

      opacity: snapUtils.snapped ? 0 : 100

      Behavior on opacity {
        PropertyAnimation {
          properties: "opacity"
          duration: 100
          easing.type: Easing.InQuad
        }
      }

      height: root.innerDotSize
      width: height
      sourceSize.width: width
      sourceSize.height: height

      source: InputStyle.crosshairCenterDotIcon
    }

    Image {
      id: crossCenterPlus // Center dot - visible when not snapped

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

      opacity: snapUtils.snapped && ( snapUtils.snapType === SnapUtils.Vertex || snapUtils.snapType === SnapUtils.Other ) ? 100 : 0

      Behavior on opacity {
        PropertyAnimation {
          properties: "opacity"
          duration: 100
          easing.type: Easing.InQuad
        }
      }

      rotation: snapUtils.snapType === SnapUtils.Other ? 0 : 45

      Behavior on rotation {
        PropertyAnimation {
          properties: "rotation"
          duration: 50
          easing.type: Easing.InQuad
        }
      }

      height: root.innerDotSize * 2
      width: height
      sourceSize.width: width
      sourceSize.height: height

      source: InputStyle.crosshairCenterPlusIcon
    }

    Image {
      id: crossCenterCircle // Center circle - visible when snapped to segment

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

      opacity: snapUtils.snapped && snapUtils.snapType === SnapUtils.Segment ? 100 : 0

      Behavior on opacity {
        PropertyAnimation {
          properties: "opacity"
          duration: 100
          easing.type: Easing.InQuad
        }
      }

      height: root.innerDotSize * 2
      width: height
      sourceSize.width: width
      sourceSize.height: height

      source: InputStyle.crosshairCenterCircleIcon
    }
}
