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
    /*required*/ property var mapSettings
    property bool shouldUseSnapping: false

    property point center: Qt.point( root.width / 2, root.height / 2 )

    property var recordPoint: snapUtils.recordPoint

    property point screenPoint: snapUtils.snapped ? __inputUtils.transformPointToScreenCoordinates(__activeLayer.vectorLayer.crs, mapSettings, recordPoint) : center

    property real outerSize: 60 * __dp
    property real innerDotSize: 10 * __dp

    SnapUtils {
      id: snapUtils

      mapSettings: root.mapSettings
      qgsProject: root.qgsProject
      useSnapping: root.shouldUseSnapping
      destinationLayer: __activeLayer.vectorLayer

      centerPosition: root.center
    }

    Image {
      id: crosshairBackground // white background of the crosshair

      x: root.screenPoint.x - width / 2
      y: root.screenPoint.y - height / 2

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

      x: root.screenPoint.x - width / 2
      y: root.screenPoint.y - height / 2

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

      x: root.screenPoint.x - width / 2
      y: root.screenPoint.y - height / 2

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

      x: root.screenPoint.x - width / 2
      y: root.screenPoint.y - height / 2

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

      x: root.screenPoint.x - width / 2
      y: root.screenPoint.y - height / 2

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

    Connections {
      target: __activeProject

      function onProjectWillBeReloaded() {
        snapUtils.clear()
      }

      function onProjectReloaded() {
        // We need to re-assign qgs project to snaputils, because
        // even though we loaded a different project,
        // internally we keep the same pointer for QgsProject.
        snapUtils.qgsProject = __activeProject.qgsProject
        snapUtils.mapSettings = root.mapSettings
      }
    }
}
