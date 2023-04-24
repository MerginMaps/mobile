/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Controls
import QtMultimedia
import QtQml.Models
import QtPositioning
import QtQuick.Dialogs

import Qt.labs.settings

import lc 1.0 as InputClass
import "./map"
import "./misc"
import "./dialogs"
import "./layers"
import "./popups"

ApplicationWindow {
    id: window
    visible: true
    width:  __appwindowwidth
    height: __appwindowheight
    visibility: __appwindowvisibility
    title: "Mergin Maps" // Do not translate

    Item {
      id: stateManager
      state: "map"

      states: [
        State {
          name: "map" // Working with map in an opened project - view, record, stakeout, form, ...
        },
        State {
          name: "projects" // Listing projects
        },
        State {
          name: "misc" // Settings, GPS panel, ..
        }
      ]

      onStateChanged: {
        if ( stateManager.state === "map" ) {
          map.state = "view"
        }
        else if ( stateManager.state === "projects" ) {
          projectPanel.openPanel()
        }

        if ( stateManager.state !== "map" ) {
          map.state = "inactive";
        }
      }
    }

    Settings {
      // start window where it was closed last time
      property alias x: window.x
      property alias y: window.y
      property alias width: window.width
      property alias height: window.height
    }

    //! Must stay in main.qml, it is used from different nested components
    function showMessage(message) {
        if ( !__androidUtils.isAndroid ) {
            popup.text = message
            popup.open()
        } else {
            __androidUtils.showToast( message )
        }
    }

    function showProjError(message) {
      projDialog.text  = message
      projDialog.open()
    }

    function selectFeature( pair ) {
      let hasNullGeometry = pair.feature.geometry.isNull

      if ( hasNullGeometry ) {
        formsStackManager.openForm( pair, "readOnly", "form" )
      }
      else if ( pair.valid ) {
        map.select( pair )
        formsStackManager.openForm( pair, "readOnly", "preview")
      }
    }

    Component.onCompleted: {

      // load default project
      if ( __appSettings.defaultProject ) {
        let path = __appSettings.defaultProject

        if ( __localProjectsManager.projectIsValid( path ) && __activeProject.load( path ) ) {
          projectPanel.activeProjectPath = path
          projectPanel.activeProjectId = __localProjectsManager.projectId( path )
          __appSettings.activeProject = path
        }
        else {
          // if default project load failed, delete default setting
          __appSettings.defaultProject = ""
          stateManager.state = "projects"
        }
      }
      else {
        stateManager.state = "projects"
      }

      // get focus when any project is active, otherwise let focus to project panel
      if ( __appSettings.activeProject )
        mainPanel.forceActiveFocus()


      // Catch back button click (if no other component catched it so far)
      // to prevent QT from quitting the APP immediately
      contentItem.Keys.released.connect( function( event ) {
        if ( event.key === Qt.Key_Back ) {
          event.accepted = true
          window.backButtonPressed()
        }
      } )

        console.log("Application initialized!")
    }

    MapWrapper {
      id: map

      height: window.height - mainPanel.height
      width: window.width

      mapExtentOffset: {
        // offset depends on what panels are visible.
        // we need to subtract mainPanel (toolbar)'s height from any visible panel
        // because panels start at the bottom of the screen, but map canvas's height is lowered
        // by mainPanels's height.
        if ( stakeoutPanelLoader.active )
        {
          // if stakeout panel is opened
          return stakeoutPanelLoader.item.panelHeight - mainPanel.height
        }
        else if ( formsStackManager.takenVerticalSpace > 0 )
        {
          // if feature preview panel is opened
          return formsStackManager.takenVerticalSpace - mainPanel.height
        }
        else if ( stateManager.state === "projects" || stateManager.state === "misc" )
        {
          //
          // Due to an upstream bug in Qt, see #2387 and #2425 for more info
          //
          return window.height
        }
        else if ( gpsDataPageLoader.active )
        {
          //
          // Block also GPS data page clicks propagation.
          // Due to an upstream bug in Qt, see #2387 and #2425 for more info
          //
          return window.height
        }

        return 0
      }

      onFeatureIdentified: function( pair ) {
        formsStackManager.openForm( pair, "readOnly", "preview" );
      }

      onNothingIdentified: {
        formsStackManager.closeDrawer()
      }

      onRecordingFinished: function( pair ) {
        formsStackManager.openForm( pair, "add", "form" )
        map.highlightPair( pair )
      }

      onEditingGeometryStarted: formsStackManager.geometryEditingStarted()
      onEditingGeometryFinished: function( pair ) {
        formsStackManager.geometryEditingFinished( pair )
      }
      onEditingGeometryCanceled: {
        formsStackManager.geometryEditingFinished( null, false )
      }

      onRecordInLayerFeatureStarted: formsStackManager.geometryEditingStarted()
      onRecordInLayerFeatureFinished: function( pair ) {
        formsStackManager.recordInLayerFinished( pair )
      }
      onRecordInLayerFeatureCanceled: {
        formsStackManager.recordInLayerFinished( null, false )
      }

      onSplittingStarted: formsStackManager.hideAll()
      onSplittingFinished: {
        formsStackManager.closeAll()
      }
      onSplittingCanceled: {
        formsStackManager.reopenAll()
      }

      onNotify: showMessage( message )
      onAccuracyButtonClicked: {
        gpsDataPageLoader.active = true
        gpsDataPageLoader.focus = true
      }

      onStakeoutStarted: function( pair ) {
        stakeoutPanelLoader.active = true
        stakeoutPanelLoader.focus = true
        stakeoutPanelLoader.item.targetPair = pair
      }

      onSignInRequested: {
        stateManager.state = "projects"
        projectPanel.openAuthPanel()
      }

      onLocalChangesPanelRequested: {
          if ( __merginProjectStatusModel.loadProjectInfo( __activeProject.projectFullName() ) )
          {
            stateManager.state = "projects"
            projectPanel.openChangesPanel()
          }
          else
          {
            __inputUtils.showNotification( qsTr( "No Changes" ) )
          }
      }

      Component.onCompleted: {
        __activeProject.mapSettings = map.mapSettings
        __iosUtils.positionKit = __positionKit
        __iosUtils.compass = map.compass
        __variablesManager.compass = map.compass
        __variablesManager.positionKit = __positionKit
      }
    }

    MainPanel {
        id: mainPanel

        width: window.width
        height: InputStyle.rowHeightHeader

        //
        // In order to workaround the QTBUG-108689 - we need to disable main panel's buttons when something else occupies the space
        //
        enabled: mainPanel.height > map.mapExtentOffset

        y: window.height - height

        visible: map.state === "view"

        gpsIndicatorColor: map.gpsIndicatorColor

        onOpenProjectClicked: stateManager.state = "projects"
        onOpenMapThemesClicked: {
          mapThemesPanel.visible = true
          stateManager.state = "misc"
        }
        onMyLocationClicked: map.centerToPosition()

        onMyLocationHold: {
            __appSettings.autoCenterMapChecked = !__appSettings.autoCenterMapChecked
            showMessage( __appSettings.autoCenterMapChecked ? qsTr("GPS auto-center mode on") : qsTr("GPS auto-center mode off") )
        }
        onOpenSettingsClicked: settingsPanel.visible = true
        onZoomToProject: {
          if ( __appSettings.autoCenterMapChecked ) {
            mainPanel.myLocationHold()
          }
          __inputUtils.zoomToProject( __activeProject.qgsProject, map.mapSettings )
        }
        onRecordClicked: {
            if ( __recordingLayersModel.rowCount() > 0 ) {
              stateManager.state = "map"
              map.record()
            } else {
                showMessage( qsTr( "No editable layers found." ) )
            }
        }
        onLocalChangesClicked: {
          if ( __merginProjectStatusModel.loadProjectInfo( __activeProject.projectFullName() ) )
          {
            stateManager.state = "projects"
            projectPanel.openChangesPanel()
          }
          else
          {
            __inputUtils.showNotification( qsTr( "No Changes" ) )
          }
        }
        onLayersClicked: {
          stateManager.state = "misc"
          let layerspanel = mapPanelsStackView.push( layersPanelComponent, {}, StackView.PushTransition )
        }

        onPositionTrackingClicked: {
          trackingPanelLoader.active = true
        }
    }

    NotificationBanner {
      id: failedToLoadProjectBanner

      width: parent.width - failedToLoadProjectBanner.anchors.margins * 2
      height: InputStyle.rowHeight * 2

      onDetailsClicked: {
        projectIssuesPanel.projectLoadingLog = __activeProject.projectLoadingLog();
        projectIssuesPanel.visible = true;
      }
    }

    SettingsPanel {
      id: settingsPanel

      height: window.height
      width: window.width
      rowHeight: InputStyle.rowHeight

      onVisibleChanged: {
        if (settingsPanel.visible) {
          settingsPanel.focus = true; // get focus
          stateManager.state = "misc"
        }
        else {
          mainPanel.focus = true; // pass focus back to main panel
          stateManager.state = "map"
        }
      }

      gpsIndicatorColor: map.gpsIndicatorColor
    }

    ProjectPanel {
        id: projectPanel

        height: window.height
        width: window.width

        onVisibleChanged: {
          if ( projectPanel.visible ) {
            projectPanel.forceActiveFocus()
          }
          else {
            mainPanel.forceActiveFocus()
          }
        }

        onOpenProjectRequested: function( projectId, projectPath ) {
          __appSettings.defaultProject = projectPath
          __appSettings.activeProject = projectPath
          __activeProject.load( projectPath )
        }

        onClosed: stateManager.state = "map"
    }

    StackView {
      id: mapPanelsStackView

      //
      // View that can show panels on top of the map,
      // like layers panel, settings and similar
      //

      anchors.fill: parent

      pushEnter: Transition {
        YAnimator {
          to: 0
          from: mapPanelsStackView.height
          duration: 400
          easing.type: Easing.OutCubic
        }
      }

      pushExit: Transition {}

      popEnter: Transition {}

      popExit: Transition {
        YAnimator {
          to: mapPanelsStackView.height
          from: 0
          duration: 400
          easing.type: Easing.OutCubic
        }
      }
    }

    Component {
      id: layersPanelComponent

      LayersPanelV2 {

        onClose: function() {
          mainPanel.forceActiveFocus()
          mapPanelsStackView.clear( StackView.PopTransition )
          stateManager.state = "map"
        }

        onSelectFeature: function( featurePair ) {
          // close layers panel if the feature has geometry
          if ( __inputUtils.isSpatialLayer( featurePair.layer ) )
          {
            close()
          }

          window.selectFeature( featurePair )
        }

        onAddFeature: function( targetLayer ) {
          let newPair = __inputUtils.createFeatureLayerPair( targetLayer, __inputUtils.emptyGeometry(), __variablesManager )
          formsStackManager.openForm( newPair, "add", "form" )

          // If we start supporting addition of spatial features from the layer's list,
          // make sure to change the root state here to "map"
        }
      }
    }

    Component {
      id: gpsDataPageComponent

      GpsDataPage {
        id: gpsDataPage

        onBack: {
          mainPanel.focus = true
          gpsDataPageLoader.active = false
        }

        mapSettings: map.mapSettings

        height: window.height
        width: window.width
      }
    }

    Loader {
      id: gpsDataPageLoader

      asynchronous: true
      active: false
      focus: true
      sourceComponent: gpsDataPageComponent
      onActiveChanged: {
        if ( gpsDataPageLoader.active )
        {
          formsStackManager.closeDrawer();

          if ( stakeoutPanelLoader.active )
          {
            // if we are in stakeout mode
            stakeoutPanelLoader.item.hide()
          }
        }
        else
        {
          if ( stakeoutPanelLoader.active )
          {
            // user closed GPS panel and we are in stakeout mode - reopen stakeout panel
            stakeoutPanelLoader.item.restore()
          }
        }
      }
    }

    MapThemePanel {
        id: mapThemesPanel

        height: window.height/2
        width: window.width
        edge: Qt.BottomEdge

        onClosed: stateManager.state = "map"
    }

    Loader {
      id: trackingPanelLoader

      focus: true
      active: false
      asynchronous: true

      sourceComponent: Component {

        PositionTrackingDrawer {

          width: window.width

          trackingActive: __activeProject.isTrackingPosition
          distanceTraveled: map.trackingManager ? __inputUtils.geometryLengthAsString( map.trackingManager?.trackedGeometry ) : qsTr( "not tracking" )
          trackingStartedAt: {
            if ( map.trackingManager?.startTime )
            {
              let date = map.trackingManager?.startTime
              return date.getHours() + ":" + date.getMinutes() + ":" + date.getSeconds()
            }
            return qsTr( "not tracking" )
          }

          onTrackingBtnClicked: __activeProject.isTrackingPosition = !__activeProject.isTrackingPosition

          onClosed: {
            trackingPanelLoader.active = false
          }
        }
      }

      onActiveChanged: {
        if ( active )
        {
          trackingPanelLoader.item?.open()
        }
      }
    }

    ProjectIssuesPanel {
      id: projectIssuesPanel

      height: window.height
      width: window.width
      rowHeight: InputStyle.rowHeight
      visible: false;

      onVisibleChanged: {
        if (projectIssuesPanel.visible)
          projectIssuesPanel.focus = true; // get focus
        else
          mainPanel.focus = true; // pass focus back to main panel
      }
    }

    Loader {
      id: stakeoutPanelLoader

      focus: true
      active: false
      asynchronous: true

      sourceComponent: stakeoutPanelComponent
    }

    Component {
      id: stakeoutPanelComponent

      StakeoutPanel {
        id: stakeoutPanel

        height: window.height
        width: window.width

        mapCanvas: map

        onStakeoutFinished: {
          map.stopStakeout()
          formsStackManager.openForm( targetPair, "readOnly", "preview" )
          stakeoutPanelLoader.active = false
        }

        onAutoFollowClicked: map.autoFollowStakeoutPath()
        onPanelHeightUpdated: map.updatePosition()
      }
    }

    Notification {
        id: popup

        text: ""
        width: 400 * __dp
        height: 160 * __dp
        x: ( parent.width - width ) / 2
        y: ( parent.height - height ) / 2
    }

    StorageLimitDialog {
        id: storageLimitDialog
        onOpenSubscriptionPlans: {
          storageLimitDialog.close()
          if (__merginApi.apiSupportsSubscriptions) {
            projectPanel.manageSubscriptionPlans()
          }
        }
    }

    MessageDialog {
        id: projDialog
        onAccepted: projDialog.close()
        title: qsTr("PROJ Error")
        buttons: MessageDialog.Ignore | MessageDialog.Help
        onButtonClicked: function(clickedButton) {
          if (clickedButton === MessageDialog.Help) {
            Qt.openUrlExternally(__inputHelp.howToSetupProj)
          }
          close()
        }
    }

    FormsStackManager {
      id: formsStackManager

      height: window.height
      width: window.width
      previewHeight: window.height / 3

      project: __activeProject.qgsProject

      onCreateLinkedFeatureRequested: function( targetLayer, parentPair )  {
        if ( __inputUtils.isNoGeometryLayer( targetLayer) ) {
          let newPair = __inputUtils.createFeatureLayerPair( targetLayer, __inputUtils.emptyGeometry(), __variablesManager )
          recordInLayerFinished( newPair, true )
        }
        else { // we will record geometry
          stateManager.state = "map"
          map.recordInLayer( targetLayer, parentPair )
        }
      }

      onEditGeometryRequested: function( pair ) {
        stateManager.state = "map"
        map.edit( pair )
      }

      onSplitGeometryRequested: function( pair ) {
        stateManager.state = "map"
        map.split( pair )
      }

      onRedrawGeometryRequested: function( pair ) {
        stateManager.state = "map"
        map.redraw( pair )
      }

      onClosed: {
        if ( mapPanelsStackView.depth ) {
          // this must be layers panel as it is the only thing on the stackview currently
          const item = mapPanelsStackView.get( 0 )
          item.forceActiveFocus()
          stateManager.state = "misc"
        }
        else if ( gpsDataPageLoader.active )
        {
          stateManager.state = "misc"
          // do nothing, gps page already has focus
        }
        else {
          stateManager.state = "map"
          mainPanel.focus = true
        }

        map.hideHighlight()
      }

      onStakeoutFeature: function( feature ) {
        if ( !__inputUtils.isPointLayerFeature( feature ) )
          return;
        if ( !__positionKit.hasPosition )
        {
          showMessage( qsTr( "Stake out is disabled because location is unavailable!" ) );
          return;
        }

        map.stakeout( feature )
        closeDrawer()
      }
    }

    MessageDialog {
      id: projectErrorDialog

      title: qsTr("Failed to open the project")
      buttons: MessageDialog.Close | MessageDialog.Help

      onButtonClicked: function(button, role) {
        if ( button === MessageDialog.Help ) {
          Qt.openUrlExternally(__inputHelp.projectLoadingErrorHelpLink)
        }
        projectLoadingScreen.visible = false
        projectPanel.openPanel()
        close()
      }
    }

    WhatsNewDialog {
      id: whatsNewDialog

      width: parent.width / 2 < InputStyle.minDialogWidth ? parent.width - 2 * InputStyle.panelMargin : parent.width / 2
      height: parent.height / 2 < InputStyle.minDialogHeight ? parent.height - 2 * InputStyle.panelMargin : parent.height / 2

      anchors.centerIn: parent
      informativeText: qsTr("We've made it easier for teams to collaborate on Mergin Maps! To find out more, check out our latest blog post about workspaces by clicking the button below.")
      infoUrl: __inputHelp.whatsNewPostLink
    }

    ProjectLoadingScreen {
      id: projectLoadingScreen

      anchors.fill: parent
      visible: false
    }

    Connections {
        target: __merginApi
        function onNetworkErrorOccurred( message, topic, httpCode, projectFullName ) {
          if ( stateManager.state === "projects" )
          {
            var msg = message ? message : qsTr( "Failed to communicate with Mergin.%1Try improving your network connection." ).arg( "\n" )
            showMessage( msg )
          }
        }

        function onStorageLimitReached( uploadSize ) {
          __merginApi.getUserInfo()
          if (__merginApi.apiSupportsSubscriptions) {
            __merginApi.getWorkspaceInfo()
          }
          storageLimitDialog.uploadSize = uploadSize
          storageLimitDialog.open()
        }

        function onNotify( message ) {
          showMessage(message)
        }

        function onProjectDataChanged( projectFullName ) {
          //! if current project has been updated, refresh canvas
          if ( projectFullName === projectPanel.activeProjectId ) {
            map.mapSettings.extentChanged()
          }
        }

        function onServerWasUpgraded() {
          if (!__appSettings.ignoreWhatsNew) {
            whatsNewDialog.open()
          }
        }
    }

    Connections {
        target: __inputProjUtils
        function onProjError( message ) {
          showProjError(message)
        }
    }

    Connections {
        target: __inputUtils
        function onShowNotificationRequested( message ) {
            showMessage(message)
        }
    }

    Connections {
      target: __activeProject

      function onLoadingStarted() {
        projectLoadingScreen.visible = true;
        failedToLoadProjectBanner.reset();
        projectIssuesPanel.clear();
      }

      function onLoadingFinished() {
        projectLoadingScreen.visible = false
      }

      function onLoadingErrorFound() {
        failedToLoadProjectBanner.pushNotificationMessage( qsTr( "There were issues loading the project." ) )
      }

      function onReportIssue( layerName, message ) {
        projectIssuesPanel.reportIssue( layerName, message )
      }

      function onProjectReloaded( project ) {
        map.clear()
      }

      function onProjectWillBeReloaded() {
        formsStackManager.reload()
      }

      function onProjectReadingFailed( message ) {
        projectErrorDialog.informativeText = qsTr( "Could not read the project file:" ) + "\n" + message
        projectErrorDialog.open()
      }
    }

    LegacyFolderMigration {
      id: lfm

      z: 1000 // unfortunatelly we need this hack because some parts of application still sets z coord
      anchors.fill: parent
    }

    Timer {
      id: closeAppTimer

      interval: 3000
      running: false
      repeat: false
    }

    function backButtonPressed() {

      if ( closeAppTimer.running ) {
        __inputUtils.quitApp()
      }
      else {
        closeAppTimer.start()
        showMessage( qsTr( "Press back again to quit the app" ) )
      }
    }
}
