/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Layouts
import QtQuick.Shapes

import Input 0.1
import lc 1.0

import "../"
import "../components"

/**
  * SplittingTools is a set of tools that are used during recording/editing of a geometry.
  * These tools can be instantiated just for the time of recording and then destroyed.
  */
Item {
  id: root

  /*required*/ property var map
  /*required*/ property var featureToSplit

  signal canceled()
  signal done( bool success )

  SplittingMapTool {
    id: mapTool

    featureToSplit: root.featureToSplit
    mapSettings: root.map.mapSettings
  }

  GuidelineController {
    id: guidelineController

    mapSettings: root.map.mapSettings
    crosshairPosition: crosshair.screenPoint
    realGeometry: __inputUtils.transformGeometryToMapWithLayer( mapTool.recordedGeometry, __activeLayer.vectorLayer, root.map.mapSettings )
  }

  Highlight {
    id: guideline

    height: root.map.height
    width: root.map.width

    lineColor: __style.deepOceanColor

    mapSettings: root.map.mapSettings
    geometry: guidelineController.guidelineGeometry
  }

  Highlight {
    id: highlight

    height: map.height
    width: map.width

    markerColor: __style.deepOceanColor
    lineColor: __style.deepOceanColor
    lineStrokeStyle: ShapePath.DashLine

    mapSettings: root.map.mapSettings
    geometry: __inputUtils.transformGeometryToMapWithLayer( mapTool.recordedGeometry, __activeLayer.vectorLayer, root.map.mapSettings )
  }

  Crosshair {
    id: crosshair

    anchors.fill: parent

    qgsProject: __activeProject.qgsProject
    mapSettings: root.map.mapSettings
  }

  MMToolbar {
    y: parent.height

    width: parent.width

    model: ObjectModel {

      MMToolbarButton {
        text: qsTr( "Undo" )
        iconSource: __style.undoIcon
        onClicked: mapTool.removePoint()
      }

      MMToolbarButton {
        text: qsTr( "Add point" )
        iconSource: __style.addIcon
        onClicked: mapTool.addPoint( crosshair.recordPoint )
      }

      MMToolbarButton {
        text: qsTr( "Done" )
        iconSource: __style.doneIcon
        type: MMToolbarButton.Button.Emphasized;
        onClicked: {
          if ( mapTool.hasValidGeometry() )
          {
            let result = mapTool.commitSplit()
            root.done( result )
          }
          else
          {
            __notificationModel.addWarning( qsTr( "You need to add at least 2 points." ) )
          }
        }
      }
    }
  }
}
