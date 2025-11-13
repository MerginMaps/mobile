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

import mm 1.0 as MM

import "../components"
import "./components"

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

  MM.SplittingMapTool {
    id: mapTool

    featureToSplit: root.featureToSplit
    mapSettings: root.map.mapSettings
  }

  MM.GuidelineController {
    id: guidelineController

    mapSettings: root.map.mapSettings
    crosshairPosition: crosshair.screenPoint
    realGeometry: __inputUtils.transformGeometryToMapWithLayer( mapTool.recordedGeometry, __activeLayer.vectorLayer, root.map.mapSettings )
  }

  MMHighlight {
    id: guideline

    height: root.map.height
    width: root.map.width

    markerColor: __style.deepOceanColor
    lineColor: __style.deepOceanColor
    lineStrokeStyle: ShapePath.DashLine
    lineWidth: MMHighlight.LineWidths.Narrow

    mapSettings: root.map.mapSettings
    geometry: guidelineController.guidelineGeometry
  }

  MMHighlight {
    id: highlight

    height: map.height
    width: map.width

    markerColor: __style.deepOceanColor
    lineColor: __style.deepOceanColor
    lineWidth: MMHighlight.LineWidths.Narrow

    mapSettings: root.map.mapSettings
    geometry: __inputUtils.transformGeometryToMapWithLayer( mapTool.recordedGeometry, __activeLayer.vectorLayer, root.map.mapSettings )
  }

  MMCrosshair {
    id: crosshair

    anchors.fill: parent

    qgsProject: __activeProject.qgsProject
    mapSettings: root.map.mapSettings
  }

  MMToolbar {
    y: parent.height

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
        iconSource: __style.doneCircleIcon
        onClicked: {
          if ( mapTool.hasValidGeometry() )
          {
            if ( !mapTool.canCommitSplit() )
            {
              __notificationModel.addWarning( qsTr( "The split line does not properly cross the feature. Please adjust the line to cross the feature boundary." ) )
              return
            }

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
