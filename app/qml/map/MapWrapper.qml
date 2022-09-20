/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.14

import lc 1.0
import QgsQuick 0.1 as QgsQuick
import QtQuick.Dialogs 1.3

import ".."
import "../components"
import "../dialogs"
import "../banners"

Item {
  id: root

  // mapExtentOffset represents a height (or a portion) of canvas which is occupied by some other component
  // like preview panel or stakeout panel. Map extent thus must be calculated regarding to this
  // offset in order to not highlight features in the occupied area, but rather move canvas
  property real mapExtentOffset

  readonly property alias gpsIndicatorColor: gpsStateGroup.indicatorColor

  readonly property alias mapSettings: mapCanvas.mapSettings
  readonly property alias compass: deviceCompass

  signal featureIdentified( var pair )
  signal nothingIdentified()

  signal recordingStarted()
  signal recordingFinished( var pair )
  signal recordingCanceled()

  signal editingGeometryStarted()
  signal editingGeometryFinished( var pair )
  signal editingGeometryCanceled()

  signal recordInLayerFeatureStarted()
  signal recordInLayerFeatureFinished( var pair )
  signal recordInLayerFeatureCanceled()

  signal splittingStarted()
  signal splittingFinished()
  signal splittingCanceled()

  signal notify( string message )

  signal stakeoutStarted( var pair )
  signal accuracyButtonClicked()

  signal signInRequested()
  signal localChangesPanelRequested()

  states: [
    State {
      name: "view"
    },
    State {
      name: "record"
    },
    State {
      // recording feature in specific layer without option to change the recording layer,
      // used to create linked features in relations, value relations and browse data
      name: "recordInLayer"
    },
    State {
      name: "edit" // of existing feature
    },
    State {
      name: "split" // of existing feature
    },
    State {
      name: "stakeout"
    },
    State {
      name: "inactive" // covered by other element
    }
  ]

  onStateChanged: {
    switch ( state ) {

      case "record": {
        recordingStarted()
        break
      }

      case "recordInLayer": {
        recordInLayerFeatureStarted()
        hideHighlight()
        break
      }

      case "edit": {
        editingGeometryStarted()
        hideHighlight()
        break
      }

      case "split": {
        howtoSplittingBanner.show()
        splittingStarted()
        break
      }

      case "view": {
        hideHighlight()

        // Stop/Start sync animation when user goes to map
        if ( __syncManager.hasPendingSync( __activeProject.projectFullName() ) )
        {
          syncInProgressAnimation.start()
        }
        else
        {
          syncInProgressAnimation.stop()
        }

        break
      }

      case "stakeout": {
        hideHighlight()
        stakeoutStarted( internal.stakeoutTarget )
        break
      }

      case "inactive": {
        break
      }
    }
  }

  state: "view"

  Rectangle {
    // background
    color: InputStyle.clrPanelMain
    anchors.fill: parent
  }

  QgsQuick.MapCanvas {
    id: mapCanvas

    height: root.height
    width: root.width
    visible: root.state !== "inactive"

    mapSettings.project: __activeProject.qgsProject

    IdentifyKit {
      id: identifyKit

      mapSettings: mapCanvas.mapSettings
      identifyMode: IdentifyKit.TopDownAll
    }

    onClicked: {
      if ( root.state === "view" )
      {
        let screenPoint = Qt.point( point.x, point.y )
        let pair = identifyKit.identifyOne( screenPoint )

        if ( pair.valid )
        {
          root.select( pair )
          root.featureIdentified( pair )
        }
        else
        {
          root.hideHighlight()
          root.nothingIdentified()
        }
      }
    }

    onLongPressed: {
      // Alter position of simulated provider
      if ( __positionKit.positionProvider && __positionKit.positionProvider.id() === "simulated" )
      {
        __positionKit.positionProvider.setPosition( __inputUtils.mapPointToGps( Qt.point( point.x, point.y ), mapCanvas.mapSettings ) )
      }
    }
  }

  MapPosition {
    id: mapPositioning

    mapSettings: mapCanvas.mapSettings
    positionKit: __positionKit
    onScreenPositionChanged: updatePosition()
  }

  Compass { id: deviceCompass }

  StateGroup {
    id: gpsStateGroup

    property color indicatorColor: InputStyle.softRed

    states: [
      State {
        name: "good" // GPS provides position AND horizontal accuracy is below set tolerance (threshold)
        when: __positionKit.hasPosition && __positionKit.horizontalAccuracy > 0 && __positionKit.horizontalAccuracy <= __appSettings.gpsAccuracyTolerance
        PropertyChanges {
          target: gpsStateGroup
          indicatorColor: InputStyle.softGreen
        }
      },
      State {
        name: "low" // below accuracy tolerance OR GPS does not provide horizontal accuracy
        when: __positionKit.hasPosition &&  (__positionKit.horizontalAccuracy < 0 || __positionKit.horizontalAccuracy > __appSettings.gpsAccuracyTolerance )
        PropertyChanges {
          target: gpsStateGroup
          indicatorColor: InputStyle.softOrange
        }
      },
      State {
        name: "unavailable" // GPS does not provide position
        when: !__positionKit.hasPosition
        PropertyChanges {
          target: gpsStateGroup
          indicatorColor: InputStyle.softRed
        }
      }
    ]
  }

  LoadingIndicator {
    width: mapCanvas.width
    height: InputStyle.mapLoadingIndicatorHeight

    visible: mapCanvas.isRendering && root.state !== "inactive"
  }

  ScaleBar {
    id: scaleBar

    mapSettings: mapCanvas.mapSettings

    height: InputStyle.scaleBarHeight
    preferredWidth: Math.min( window.width, 180 * __dp )

    anchors.horizontalCenter: parent.horizontalCenter
    anchors.top: parent.top
    anchors.topMargin: InputStyle.smallGap
  }

  Highlight {
    id: identifyHighlight

    visible: root.state === "view"
    anchors.fill: mapCanvas

    mapSettings: mapCanvas.mapSettings
  }

  Loader {
    id: recordingToolsLoader

    anchors.fill: parent

    asynchronous: true
    active: internal.isInRecordState

    sourceComponent: recordingToolsComponent
  }

  Loader {
    id: stakeoutLoader

    anchors.fill: mapCanvas

    asynchronous: true
    active: root.state === "stakeout"

    sourceComponent: stakeoutToolsComponent
  }

  Loader {
    id: splittingLoader

    anchors.fill: parent

    asynchronous: true
    active: root.state === "split"

    sourceComponent: splittingToolsComponent
  }

  PositionMarker {
    mapPosition: mapPositioning
    compass: deviceCompass
  }

  AutoHideBanner {
    id: syncSuccessfulBanner

    width: parent.width - InputStyle.innerFieldMargin * 2
    height: InputStyle.rowHeight

    bgColor: InputStyle.clrPanelBackground
    fontColor: "white"

    source: InputStyle.yesIcon

    text: qsTr( "Successfully synchronized" )
  }

  AutoHideBanner {
    id: upToDateBanner

    width: parent.width - InputStyle.innerFieldMargin * 2
    height: InputStyle.rowHeight

    bgColor: InputStyle.secondaryBackgroundColor
    fontColor: "white"

    source: InputStyle.yesIcon

    text: qsTr( "Up to date" )
  }

  AutoHideBanner {
    id: anotherProcessIsRunningBanner

    width: parent.width - InputStyle.innerFieldMargin * 2
    height: InputStyle.rowHeight

    text: qsTr( "Somebody else is syncing, we will try again later" )
  }

  AutoHideBanner {
    id: retryableSyncErrorBanner

    width: parent.width - InputStyle.innerFieldMargin * 2
    height: InputStyle.rowHeight

    visibleInterval: 10000
    text: qsTr( "There was an issue during synchronization, we will try again. Click to learn more" )

    onClicked: syncFailedDialog.open()
  }

  AutoHideBanner {
    id: splittingDoneBanner

    width: parent.width - InputStyle.innerFieldMargin * 2
    height: InputStyle.rowHeight

    bgColor: InputStyle.darkGreen
    fontColor: "white"

    source: InputStyle.yesIcon

    visibleInterval: 2000

    text: qsTr( "Splitting done successfully" )
  }

  AutoHideBanner {
    id: howtoSplittingBanner

    width: parent.width - InputStyle.innerFieldMargin * 2
    height: InputStyle.rowHeight

    bgColor: InputStyle.secondaryBackgroundColor
    fontColor: "white"

    source: InputStyle.infoIcon

    visibleInterval: 10000

    text: qsTr( "Create line to split the selected feature" )
  }

  AutoHideBanner {
    id: howtoEditingBanner

    width: parent.width - InputStyle.innerFieldMargin * 2
    height: InputStyle.rowHeight

    bgColor: InputStyle.secondaryBackgroundColor
    fontColor: "white"

    source: InputStyle.infoIcon

    visibleInterval: 10000

    text: qsTr( "Select some point to start editing the geometry" )
  }

  AutoHideBanner {
    id: redrawGeometryBanner

    width: parent.width - InputStyle.innerFieldMargin * 2
    height: InputStyle.rowHeight

    bgColor: InputStyle.secondaryBackgroundColor
    fontColor: "white"

    source: InputStyle.infoIcon

    visibleInterval: 10000

    text: qsTr( "Record new geometry for the feature" )
  }

  MissingAuthDialog {
    id: missingAuthDialog

    onSingInRequested: root.signInRequested()
  }

  SyncFailedDialog {
    id: syncFailedDialog
  }

  MigrateToMerginDialog {
    id: migrateToMerginDialog

    onMigrationRequested: __syncManager.migrateProjectToMergin( __activeProject.projectFullName() )
  }

  NoPermissionsDialog {
    id: noPermissionsDialog
  }

  SplittingFailedDialog {
    id: splittingFailedDialog
  }

  MapFloatButton {
    id: syncButton

    // Find out if sync would collide with acc button
    // based on distance between them
    function wouldCollideWithAccBtn()
    {
      let accBtnRightMostX = accuracyButton.x + accuracyButton.width
      let syncBtnLeftMostX = syncButton.x
      let distance = syncBtnLeftMostX - accBtnRightMostX
      return distance < InputStyle.smallGap / 2
    }

    onClicked: __activeProject.requestSync()
    onPressAndHold: root.localChangesPanelRequested()

    maxWidth: InputStyle.mapBtnHeight
    withImplicitMargins: false

    anchors.bottom: wouldCollideWithAccBtn() ? accuracyButton.top : parent.bottom
    anchors.bottomMargin: root.mapExtentOffset + InputStyle.smallGap
    anchors.right: parent.right
    anchors.rightMargin: InputStyle.smallGap

    visible: root.state === "view"

    content: Item {

      implicitWidth: InputStyle.mapBtnHeight
      height: parent.height

      anchors.horizontalCenter: parent.horizontalCenter

      Symbol {
        id: syncicon

        iconSize: parent.height / 2
        source: InputStyle.syncIcon

        anchors.centerIn: parent
      }

      RotationAnimation {
        id: syncInProgressAnimation

        target: syncicon

        from: 0
        to: 720
        duration: 1000

        alwaysRunToEnd: true
        loops: Animation.Infinite
        easing.type: Easing.InOutSine
      }
    }

    Connections {
      target: __syncManager
      enabled: root.state !== "inactive"

      function onSyncStarted( projectFullName )
      {
        if ( projectFullName === __activeProject.projectFullName() )
        {
          syncInProgressAnimation.start()
        }
      }

      function onSyncFinished( projectFullName, success )
      {
        if ( projectFullName === __activeProject.projectFullName() )
        {
          syncInProgressAnimation.stop()

          if ( success )
          {
            // just banner
            syncSuccessfulBanner.show()
            // refresh canvas
            mapCanvas.refresh()
          }
        }
      }

      function onSyncCancelled( projectFullName )
      {
        if ( projectFullName === __activeProject.projectFullName() )
        {
          syncInProgressAnimation.stop()
        }
      }

      function onSyncError( projectFullName, errorType, willRetry, errorMessage )
      {
        if ( projectFullName === __activeProject.projectFullName() )
        {
          if ( errorType === SyncError.NotAMerginProject )
          {
            migrateToMerginDialog.open()
          }
          else if ( errorType === SyncError.NoPermissions )
          {
            noPermissionsDialog.open()
          }
          else if ( errorType === SyncError.AnotherProcessIsRunning && willRetry )
          {
            // just banner that we will try again
            anotherProcessIsRunningBanner.show()
          }
          else
          {
            syncFailedDialog.detailedText = qsTr( "Details" ) + ": " + errorMessage
            if ( willRetry )
            {
              retryableSyncErrorBanner.show()
            }
            else
            {
              syncFailedDialog.open()
            }
          }
        }
      }
    }

    Connections {
      target: __merginApi
      enabled: root.state !== "inactive"

      function onMissingAuthorizationError( projectFullName )
      {
        if ( projectFullName === __activeProject.projectFullName() )
        {
          syncInProgressAnimation.stop()
          missingAuthDialog.open()
        }
      }

      function onProjectAlreadyOnLatestVersion( projectFullName )
      {
        if ( projectFullName === __activeProject.projectFullName() )
        {
          upToDateBanner.show()
        }
      }
    }
  }

  MapFloatButton {
    id: backButton

    onClicked: {
      if ( root.state === "edit" ) {
        cancelEditDialog.open()
      }
      else if ( root.state === "record" ) {
        cancelEditDialog.open()
      }
    }

    maxWidth: parent.width * 0.8

    anchors.top: howtoEditingBanner.showBanner ? howtoEditingBanner.bottom : parent.top
    anchors.topMargin: root.mapExtentOffset + InputStyle.smallGap
    anchors.left: parent.left
    anchors.leftMargin: InputStyle.smallGap

    visible: root.state != "view"

    content: Item {

      implicitWidth: backtext.implicitWidth + backicon.width + InputStyle.tinyGap
      height: parent.height

      anchors.horizontalCenter: parent.horizontalCenter

      Symbol {
        id: backicon

        iconSize: parent.height / 2
        source: InputStyle.backIcon

        anchors.verticalCenter: parent.verticalCenter
      }

      Text {
        id: backtext

        property real maxTextWidth: backButton.maxWidth - ( backicon.width + InputStyle.tinyGap + leftPadding ) // used offsets

        text: captionmetrics.elidedText
        elide: Text.ElideRight
        wrapMode: Text.NoWrap

        font.pixelSize: InputStyle.fontPixelSizeNormal
        color: InputStyle.fontColor

        height: parent.height

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        leftPadding: height / 3 // small gap between icon and caption

        TextMetrics { // element holding metrics about printed text to be able to scale text without binding loops
          id: captionmetrics

          font: backtext.font
          text: "Back"
          elide: backtext.elide
          elideWidth: backtext.maxTextWidth
        }

        anchors {
          left: backicon.right
          right: parent.right
          verticalCenter: parent.verticalCenter
        }
      }
    }
  }

  MessageDialog {
    id: cancelEditDialog

    title: qsTr( "Discard the changes?" )
    text: {
      if ( root.state === "edit" ) {
        return qsTr( "Clicking ‘Yes’ discards your changes to the geometry. If you would like " +
                    "to save the changes instead, hit ‘No’ and then ‘Done’ in the toolbar." )
      }
      else if ( root.state === "record" ) {
        return qsTr( "Clicking ‘Yes’ discards your new geometry and no feature will be saved. " +
                     "If you would like to save the geometry instead, hit ‘No’ and then ‘Done’ " +
                     "in the toolbar." )
      }
      return ""
    }

    standardButtons: StandardButton.Yes | StandardButton.No

    onButtonClicked: {
      if ( clickedButton === StandardButton.Yes ) {
        recordingToolsLoader.item.rollbackChanges()
      }
      else if ( clickedButton === StandardButton.No ) {
        cancelEditDialog.close()
      }
    }
  }


  MapFloatButton {
    id: accuracyButton

    property int accuracyPrecision: __positionKit.horizontalAccuracy > 1 ? 1 : 2

    onClicked: accuracyButtonClicked()

    maxWidth: parent.width - ( InputStyle.panelMargin * 2 )

    anchors.bottom: root.state === "record" ? activeLayerButton.top : parent.bottom
    anchors.bottomMargin: root.mapExtentOffset + InputStyle.smallGap
    anchors.horizontalCenter: parent.horizontalCenter

    visible: {
      if ( root.state === "inactive" )
      {
        return false
      }
      else if ( __positionKit.positionProvider && __positionKit.positionProvider.type() === "external" )
      {
        // for external receivers we want to show gps panel and accuracy button
        // even when the GPS receiver is not sending position data
        return true
      }
      else return ( gpsStateGroup.state !== "unavailable" )
    }

    content: Item {

      implicitWidth: acctext.implicitWidth + indicator.width + InputStyle.tinyGap
      height: parent.height

      anchors.horizontalCenter: parent.horizontalCenter

      Text {
        id: acctext

        text: {
          if ( !__positionKit.positionProvider )
          {
            return ""
          }
          else if ( __positionKit.positionProvider.type() === "external" )
          {
            if ( __positionKit.positionProvider.state === PositionProvider.Connecting )
            {
              return qsTr( "Connecting to %1" ).arg( __positionKit.positionProvider.name() )
            }
            else if ( __positionKit.positionProvider.state === PositionProvider.WaitingToReconnect )
            {
              return __positionKit.positionProvider.stateMessage
            }
            else if ( __positionKit.positionProvider.state === PositionProvider.NoConnection )
            {
              return __positionKit.positionProvider.stateMessage
            }
          }

          if ( !__positionKit.hasPosition )
          {
            return qsTr( "Connected, no position" )
          }
          else if ( Number.isNaN( __positionKit.horizontalAccuracy ) || __positionKit.horizontalAccuracy < 0 )
          {
            return qsTr( "Unknown accuracy" )
          }
          return __inputUtils.formatNumber( __positionKit.horizontalAccuracy, accuracyButton.accuracyPrecision ) + " m"
        }
        elide: Text.ElideRight
        wrapMode: Text.NoWrap

        font.pixelSize: InputStyle.fontPixelSizeNormal
        color: InputStyle.fontColor

        height: parent.height
        anchors.horizontalCenter: parent.horizontalCenter

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
      }

      RoundIndicator {
        id: indicator

        width: parent.height / 4
        height: width
        anchors.left: acctext.right
        anchors.leftMargin: InputStyle.tinyGap
        anchors.topMargin: InputStyle.tinyGap
        anchors.top: parent.top
        color: gpsStateGroup.indicatorColor
      }
    }
  }

  MapFloatButton {
    id: activeLayerButton

    onClicked: activeLayerPanel.openPanel()

    maxWidth: parent.width * 0.8

    anchors.bottom: parent.bottom
    anchors.bottomMargin: InputStyle.smallGap
    anchors.horizontalCenter: accuracyButton.horizontalCenter

    visible: root.state === "record"

    content: Item {

      implicitWidth: layername.implicitWidth + layericon.width + InputStyle.tinyGap
      height: parent.height

      anchors.horizontalCenter: parent.horizontalCenter

      Symbol {
        id: layericon

        iconSize: parent.height / 2
        source: __inputUtils.loadIconFromLayer( __activeLayer.layer )

        anchors.verticalCenter: parent.verticalCenter
      }

      Text {
        id: layername

        property real maxTextWidth: activeLayerButton.maxWidth - ( layericon.width + InputStyle.tinyGap + leftPadding ) // used offsets

        text: textmetrics.elidedText
        elide: Text.ElideRight
        wrapMode: Text.NoWrap

        font.pixelSize: InputStyle.fontPixelSizeNormal
        color: InputStyle.fontColor

        height: parent.height

        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        leftPadding: height / 3 // small gap between layer icon and layer name

        TextMetrics { // element holding metrics about printed text to be able to scale text without binding loops
          id: textmetrics

          font: layername.font
          text: __activeLayer.layerName
          elide: layername.elide
          elideWidth: layername.maxTextWidth
        }

        anchors {
          left: layericon.right
          right: parent.right
          verticalCenter: parent.verticalCenter
        }
      }
    }
  }

  ActiveLayerPanel {
    id: activeLayerPanel

    height: window.height/2
    width: window.width
    edge: Qt.BottomEdge

    onActiveLayerChangeRequested: __activeProject.setActiveLayer( __recordingLayersModel.layerFromLayerId( layerId ) )
  }

  Connections {
    target: mapCanvas.mapSettings
    onExtentChanged: {
      scaleBar.visible = true
    }
  }

  Component {
    id: recordingToolsComponent

    RecordingTools {
      anchors.fill: parent

      map: mapCanvas
      gpsState: gpsStateGroup
      activeFeature: root.state === "edit" ? internal.featurePairToEdit.feature : __inputUtils.emptyFeature()

      centerToGPSOnStartup: root.state !== "edit"

      onCanceled: {
        howtoEditingBanner.hide()

        if ( root.state === "record" )
        {
          root.recordingCanceled()
        }
        else if ( root.state === "edit" )
        {
          root.editingGeometryCanceled()
        }
        else if ( root.state === "recordInLayer" )
        {
          root.recordInLayerFeatureCanceled()
        }

        root.state = "view"
      }

      onDone: {
        howtoEditingBanner.hide()

        if ( root.state === "record" )
        {
          root.recordingFinished( featureLayerPair )
        }
        else if ( root.state === "edit" )
        {
          root.editingGeometryFinished( featureLayerPair )
        }
        else if ( root.state === "recordInLayer" )
        {
          root.recordInLayerFeatureFinished( featureLayerPair )
        }

        root.state = "view"
      }
    }
  }

  Component {
    id: stakeoutToolsComponent

    StakeoutTools {
      anchors.fill: parent

      map: mapCanvas
      mapExtentOffset: root.mapExtentOffset

      target: internal.stakeoutTarget
    }
  }

  Component {
    id: splittingToolsComponent

    SplittingTools {
      anchors.fill: parent

      map: mapCanvas
      featureToSplit: internal.featurePairToEdit

      onDone: {
        // close all feature forms, show banner if it went fine or not
        howtoSplittingBanner.hide()

        if ( success )
        {
          splittingDoneBanner.show()
        }
        else
        {
          splittingFailedDialog.open()
        }


        root.splittingFinished()

        root.state = "view"
      }

      onCanceled: {
        // go back to feature form
        howtoSplittingBanner.hide()

        root.splittingCanceled()

        root.state = "view"
      }
    }
  }

  QtObject {
    id: internal
    // private properties - not accessible by other components

    property var featurePairToEdit // we are editing geometry of this feature layer pair

    property var extentBeforeStakeout // extent that we return to once stakeout finishes
    property var stakeoutTarget

    property bool isInRecordState: root.state === "record" || root.state === "recordInLayer" || root.state === "edit"
  }

  function select( featurepair ) {
    centerToPair( featurepair, true )
    highlightPair( featurepair )
  }

  function record() {
    state = "record"
  }

  function recordInLayer( layer, parentpair ) {
    __activeProject.setActiveLayer( layer )
    centerToPair( parentpair )
    state = "recordInLayer"
  }

  function edit( featurepair ) {
    __activeProject.setActiveLayer( featurepair.layer )
    centerToPair( featurepair )
    howtoEditingBanner.show()

    internal.featurePairToEdit = featurepair
    state = "edit"
  }

  function redraw( featurepair ) {
    __activeProject.setActiveLayer( featurepair.layer )
    centerToPair( featurepair )
    redrawGeometryBanner.show()

    // clear feature geometry
    internal.featurePairToEdit = __inputUtils.changeFeaturePairGeometry( featurepair, __inputUtils.emptyGeometry() )

    state = "edit"
  }

  function split( featurepair ) {
    centerToPair( featurepair )

    internal.featurePairToEdit = featurepair
    state = "split"
  }

  function stakeout( featurepair )
  {
    internal.extentBeforeStakeout = mapCanvas.mapSettings.extent
    internal.stakeoutTarget = featurepair
    state = "stakeout"
  }

  function autoFollowStakeoutPath()
  {
    if ( state === "stakeout" )
    {
      stakeoutLoader.item.autoFollow()
    }
  }

  function stopStakeout()
  {
    state = "view"

    // go back to state before starting stakeout
    highlightPair( internal.stakeoutTarget )
    mapCanvas.mapSettings.extent = internal.extentBeforeStakeout
  }

  function centerToPair( pair, considerMapExtentOffset = false ) {
    if ( considerMapExtentOffset )
      var mapExtentOffsetRatio = mapExtentOffset / mapCanvas.height
    else
      mapExtentOffsetRatio = 0

    __inputUtils.setExtentToFeature( pair, mapCanvas.mapSettings, mapExtentOffsetRatio )
  }

  function highlightPair( pair ) {
    let geometry = __inputUtils.extractGeometry( pair, mapCanvas.mapSettings )
    identifyHighlight.geometry = __inputUtils.convertGeometryToMapCRS( geometry, pair.layer, mapCanvas.mapSettings )
  }

  function hideHighlight() {
    identifyHighlight.geometry = null
  }

  function centerToPosition() {
    if ( __positionKit.hasPosition ) {
      mapCanvas.mapSettings.setCenter( mapPositioning.mapPosition )
    }
    else {
      showMessage( qsTr( "GPS currently unavailable." ) )
    }
  }

  function isPositionOutOfExtent() {
    let border = InputStyle.mapOutOfExtentBorder
    return ( ( mapPositioning.screenPosition.x < border ) ||
            ( mapPositioning.screenPosition.y < border ) ||
            ( mapPositioning.screenPosition.x > mapCanvas.width - border ) ||
            ( mapPositioning.screenPosition.y > mapCanvas.height - border )
            )
  }

  function updatePosition() {
    if ( root.state === "view" )
    {
      if ( __appSettings.autoCenterMapChecked && isPositionOutOfExtent() )
      {
        centerToPosition()
      }
    }
  }

  function clear() {
    // clear all previous references to old project (if we don't clear references to the previous project,
    // highlights may end up with dangling pointers to map layers and cause crashes)

    identifyHighlight.geometry = null
  }
}
