/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import Qt5Compat.GraphicalEffects

import mm 1.0 as MM
import "../../components"

Item {
    id: root

    required property string text
    /*required*/ property var qgsProject
    /*required*/ property var mapSettings

    property bool canCloseShape: false
    property bool shouldUseSnapping: false

    property point center: Qt.point( root.width / 2, root.height / 2 )

    property var recordPoint: snapUtils.recordPoint

    property point screenPoint: snapUtils.snapped && __activeLayer.vectorLayer ? __inputUtils.transformPointToScreenCoordinates(__activeLayer.vectorLayer.crs, mapSettings, recordPoint) : center

    property real outerSize: 60 * __dp
    property real innerDotSize: 10 * __dp

    implicitWidth: row.width
    implicitHeight: __style.mapItemHeight

    signal closeShapeClicked

    MM.SnapUtils {
      id: snapUtils

      centerPosition: root.center
      mapSettings: root.mapSettings
      qgsProject: root.qgsProject
      useSnapping: root.shouldUseSnapping
      destinationLayer: __activeLayer.vectorLayer
    }

    MMCrosshair {
      id: crosshair

      anchors.fill: parent
      qgsProject: __activeProject.qgsProject
      mapSettings: root.mapSettings
    }

    Rectangle {
      y: crosshair.crosshairForeground.y + crosshair.crosshairForeground.height + 2 * __dp
      x: crosshair.crosshairForeground.x - ( ( width - crosshair.crosshairForeground.width ) / 2 )

      width: Math.max( root.outerSize - 10 * __dp , row.width )
      height: root.outerSize * 0.6
      radius: root.height / 2
      color: textBg.color

      layer.enabled: true
      layer.effect: MMShadow {}

      Row {
        id: row

        anchors.centerIn: parent
        leftPadding: 8 * __dp
        rightPadding: leftPadding
        spacing: 4 * __dp
        height: parent.height

        MMIcon {
          id: icon
          anchors.verticalCenter: parent.verticalCenter
          source: root.canCloseShape ? __style.closeShapeIcon : ""
          size: root.canCloseShape ? __style.icon24 : 0
        }

        Rectangle {
          id: textBg
          property real spacing:  5 * __dp
          anchors.verticalCenter: parent.verticalCenter
          color: root.canCloseShape ? __style.grassColor : __style.forestColor
          height: text.height + spacing
          width: text.width + 3 * spacing
          radius: height / 2

          Text {
            id: text

            property real textSurroundingItemsWidth: textBg.spacing + icon.width + row.spacing + 2 * row.leftPadding

            width: ( implicitWidth + textSurroundingItemsWidth ) > root.maxWidth ? root.maxWidth - textSurroundingItemsWidth : implicitWidth
            anchors.centerIn: parent
            color: root.canCloseShape ? __style.forestColor: __style.polarColor
            text: root.canCloseShape ? qsTr( "Close shape" ) : root.text
            font: __style.t3
            elide: Text.ElideRight
          }
        }
      }

      MouseArea {
        anchors.fill: parent
        onClicked: {
          if ( root.canCloseShape )
            root.closeShapeClicked()
        }
      }
    }

    Connections { // future snapping? _(ツ)_/¯
      target: __activeProject

      function onProjectWillBeReloaded() {
        snapUtils.clear()
      }

      function onProjectReloaded( project ) {
        // We need to re-assign qgs project to snaputils, because
        // even though we loaded a different project,
        // internally we keep the same pointer for QgsProject.
        snapUtils.qgsProject = __activeProject.qgsProject
        snapUtils.mapSettings = root.mapSettings
      }
    }
}
