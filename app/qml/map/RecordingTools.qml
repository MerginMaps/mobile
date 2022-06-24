/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.14

import QgsQuick 0.1
import lc 1.0

import "../"

/**
  * RecordingTools is a set of tools that are used during recording/editing of a geometry.
  * These tools can be instantiated just for the time of recording and then destroyed.
  */
Item {
  id: root

  /*required*/ property var mapSettings
  /*required*/ property var gpsState

  property var initialGeometry

  signal canceled()
  signal done( var geometry )

  Banner {
    id: gpsAccuracyBanner

    // potentially might miss this code (when started/ended recording):
//    if ( _gpsAccuracyBanner.showBanner ) {
//      _gpsAccuracyBanner.state = visible ? "show" : "fade"
//    }

    property bool shouldShowAccuracyWarning: {
      let isLowAccuracy = gpsState.state === "low" || gpsState.state === "unavailable"
      let isBannerAllowed = __appSettings.gpsAccuracyWarning
//      let isRecording = root.isInRecordState
      let isUsingPosition = mapTool.isUsingPosition
      let isGpsWorking = __positionKit.hasPosition

      return isLowAccuracy  &&
          isBannerAllowed   &&
          isGpsWorking      &&
          isUsingPosition
//          isRecording       &&
    }

    width: parent.width - InputStyle.innerFieldMargin * 2
    height: InputStyle.rowHeight * 2

    text: qsTr( "Low GPS position accuracy (%1 m)<br><br>Please make sure you have good view of the sky." )
    .arg( __inputUtils.formatNumber( __positionKit.horizontalAccuracy ) )
    withLink: true
    link: __inputHelp.gpsAccuracyHelpLink

    showBanner: shouldShowAccuracyWarning
  }

  RecordingMapTool {
    id: mapTool

    property bool isUsingPosition: centeredToGPS || mapTool.recordingType == RecordingMapTool.StreamMode

    centeredToGPS: false
    mapSettings: root.mapSettings

    recordingType: RecordingMapTool.Manual
    recordingInterval: __appSettings.lineRecordingInterval

    positionKit: __positionKit
    layer: __activeLayer.vectorLayer
  }

  Crosshair {
    id: crosshair

    anchors.fill: parent

    qgsProject: __activeProject.qgsProject
    mapSettings: root.mapSettings
    shouldUseSnapping: !mapTool.isUsingPosition

    // Might be needed vv
    // TODO: move to record tools
    // we set project here as at this point it is surely valid and
    // snapping settings can be read
//    _crosshair.qgsProject = __activeProject.qgsProject
  }

  Highlight {
    id: highlight

    anchors.fill: parent

    mapSettings: root.mapSettings
  }

  RecordToolbar {
    id: toolbar

    y: parent.height
    width: parent.width
    height: InputStyle.rowHeightHeader

    gpsIndicatorColor: root.gpsIndicatorColor

    manualRecording: {}

    pointLayerSelected: __inputUtils.isPointLayer( __activeLayer.vectorLayer )

    onAddClicked: {}

    onGpsSwitchClicked: {
      if ( _gpsState.state === "unavailable" ) {
        showMessage( qsTr( "GPS currently unavailable." ) )
        return
      }
      mapCanvas.mapSettings.setCenter( mapPositioning.mapPosition )

      {}
    }

    onManualRecordingClicked: { /* record point immediately after turning on the streaming mode */ }

    onCancelClicked: {
      if ( root.state === "recordFeature" )
        root.recordingCanceled()
      else if ( root.state === "editGeometry" )
        root.editingGeometryCanceled()
      else if ( root.state === "recordInLayerFeature" )
        root.recordInLayerFeatureCanceled()

      root.state = "view"
    }

    onRemovePointClicked: {}

    onStopRecordingClicked: {}
  }

  Connections {
    target: mapCanvas.mapSettings
    onExtentChanged: {
      mapTool.centeredToGPS = false
    }
  }
}
