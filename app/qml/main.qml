/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtCore
import QtQuick.Controls
import QtMultimedia
import QtQml.Models
import QtPositioning
import QtQuick.Dialogs
import QtQuick.Layouts

import lc 1.0 as InputClass
import "./map"
import "./dialogs"
import "./layers"
import "./components"
import "./misc"
import "./project"
import "./settings"
import "./gps"
import "./form"

import notificationType 1.0

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

          // Stop/Start sync animation when user goes to map
          if ( __syncManager.hasPendingSync( __activeProject.projectFullName() ) )
          {
            syncInProgressAnimation.start()
          }
          else
          {
            syncInProgressAnimation.stop()
          }
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

    function showMessage(message) {
      __notificationModel.add(
        message,
        3,
        NotificationType.Information,
        NotificationType.None
      )
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

      height: window.height - mapToolbar.height
      width: window.width

      mapExtentOffset: {
        // offset depends on what panels are visible.
        // we need to subtract mapToolbar's height from any visible panel
        // because panels start at the bottom of the screen, but map canvas's height is lowered
        // by mapToolbar's height.
        if ( stakeoutPanelLoader.active )
        {
          // if stakeout panel is opened
          return stakeoutPanelLoader.item.panelHeight - mapToolbar.height
        }
        else if ( formsStackManager.takenVerticalSpace > 0 )
        {
          // if feature preview panel is opened
          return formsStackManager.takenVerticalSpace - mapToolbar.height
        }

        return 0
      }

      onFeatureIdentified: function( pair ) {
        formsStackManager.openForm( pair, "readOnly", "preview" );
      }

      onFeaturesIdentified: function( pairs ) {
        formsStackManager.closeDrawer()
        featurePairSelection.showPairs( pairs );
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

      onNotify: function ( message ) {
        showMessage( message )
      }
      onAccuracyButtonClicked: {
        gpsDataDrawerLoader.active = true
        gpsDataDrawerLoader.focus = true
      }

      onStakeoutStarted: function( pair ) {
        stakeoutPanelLoader.active = true
        stakeoutPanelLoader.focus = true
        stakeoutPanelLoader.item.targetPair = pair
      }

      onLocalChangesPanelRequested: {
        stateManager.state = "projects"
        projectPanel.openChangesPanel( __activeProject.projectFullName() )
      }

      onOpenTrackingPanel: {
        trackingPanelLoader.active = true
      }

      onOpenStreamingPanel: {
        console.log("TODO")
      }

      Component.onCompleted: {
        __activeProject.mapSettings = map.mapSettings
        __iosUtils.positionKit = __positionKit
        __iosUtils.compass = map.compass
        __variablesManager.compass = map.compass
        __variablesManager.positionKit = __positionKit
      }
    }

    LocationPermission {
      id: locationPermission
      accuracy: LocationPermission.Precise

      function requestPermissionAsync() {
        if ( locationPermission.status === Qt.Granted ) {
          return true;
        }
        else if ( locationPermission.status === Qt.Undetermined ) {
          locationPermission.request()
        }
        else if ( locationPermission.status === Qt.Denied ) {
          __inputUtils.log("Permissions", "Location permission is denied")
          showMessage( qsTr( "Location permission is required to show your location on map. Please enable it in system settings." ) );
        }
        return false;
      }
    }

    MMToolbar {
      id: mapToolbar

      anchors {
        left: parent.left
        bottom: parent.bottom
        right: parent.right
      }

      visible: map.state === "view"

      model: ObjectModel {

        MMToolbarButton {
          id: syncButton

          text: qsTr("Sync")
          iconSource: __style.syncIcon
          onClicked: {
            __activeProject.requestSync()
          }

          RotationAnimation {
            id: syncInProgressAnimation

            target: syncButton.buttonIcon

            from: 0
            to: 720
            duration: 1000

            alwaysRunToEnd: true
            loops: Animation.Infinite
            easing.type: Easing.InOutSine
          }
        }

        MMToolbarButton {
          text: qsTr("Add")
          iconSource: __style.addIcon
          onClicked: {
            if ( __recordingLayersModel.rowCount() > 0 ) {
              stateManager.state = "map"
              map.record()
            }
            else {
              showMessage( qsTr( "No editable layers found." ) )
            }
          }
        }

        MMToolbarButton {
          text: qsTr("Layers")
          iconSource: __style.layersIcon
          onClicked: {
            stateManager.state = "misc"
            let layerspanel = mapPanelsStackView.push( layersPanelComponent, {}, StackView.PushTransition )
          }
        }

        MMToolbarButton {
          text: qsTr("Back home")
          iconSource: __style.homeIcon
          onClicked: {
            stateManager.state = "projects"
          }
        }

        MMToolbarButton {
          text: qsTr("Zoom to project")
          iconSource: __style.zoomToProjectIcon
          onClicked: {
            __inputUtils.zoomToProject( __activeProject.qgsProject, map.mapSettings )
          }
        }

        MMToolbarButton {
          text: qsTr("Map themes")
          iconSource: __style.mapThemesIcon
          onClicked: {
            mapThemesPanel.visible = true
            stateManager.state = "misc"
          }
        }

        MMToolbarButton {
          text: qsTr("Position tracking")
          iconSource: __style.positionTrackingIcon
          menuButtonRightText: map.isTrackingPosition ? "Active" : ""

          onClicked: {
            trackingPanelLoader.active = true
          }

        }

        MMToolbarButton {
          text: qsTr("Local changes")
          iconSource: __style.localChangesIcon
          onClicked: {
            stateManager.state = "projects"
            projectPanel.openChangesPanel( __activeProject.projectFullName() )
          }
        }

        MMToolbarButton {
          text: qsTr("Settings")
          iconSource: __style.settingsIcon
          onClicked: {
            settingsPanel.visible = true
          }
        }
      }
    }

    NotificationBanner {
      id: failedToLoadProjectBanner

      // TODO: replace with notifications

      width: parent.width - failedToLoadProjectBanner.anchors.margins * 2
      height: InputStyle.rowHeight * 2

      onDetailsClicked: {
        projectIssuesPanel.projectLoadingLog = __activeProject.projectLoadingLog();
        projectIssuesPanel.visible = true;
      }
    }

    MMSettingsController {
      id: settingsPanel

      height: window.height
      width: window.width

      function openConnectGps() {
        settingsPanel.open("gps")
      }

      onVisibleChanged: {
        if (settingsPanel.visible) {
          settingsPanel.focus = true; // get focus
          stateManager.state = "misc"
        }
        else {
          stateManager.state = "map"
        }
      }
    }

    MMProjectPanel {
        id: projectPanel

        height: window.height
        width: window.width

        activeProjectId: __activeProject.localProject.id() ?? ""

        onVisibleChanged: {
          if ( projectPanel.visible ) {
            projectPanel.forceActiveFocus()
          }
        }

        onOpenProjectRequested: function( projectPath ) {
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

      MMLayersPanel {

        onClose: function() {
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
      id: gpsDataDrawerComponent

      MMGpsDataDrawer {
        id: gpsDataDrawer
        title: qsTr("GPS info")
        mapSettings: map.mapSettings

        onManageGpsClicked: {
          gpsDataDrawer.close()
          settingsPanel.openConnectGps()
        }

        onClosed: {
          gpsDataDrawerLoader.active = false
        }
      }
    }

    Loader {
      id: gpsDataDrawerLoader

      asynchronous: true
      active: false
      focus: true

      sourceComponent: gpsDataDrawerComponent

      onActiveChanged: {
        if ( active )
        {
          gpsDataDrawerLoader.item?.open()
        }
      }
    }

    MMMapThemePanel {
      id: mapThemesPanel

      height: ( window.height / 2 )
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

        MMPositionTrackingDrawer {

          width: window.width

          trackingActive: map.isTrackingPosition

          distanceTraveled: trackingPrivate.getDistance()
          trackingStartedAt: trackingPrivate.getStartingTime()

          onTrackingBtnClicked: map.setTracking( !trackingActive )

          onClosed: {
            trackingPanelLoader.active = false
          }

          QtObject {
            id: trackingPrivate

            function getDistance() {
              if ( map.isTrackingPosition ) {
                return __inputUtils.geometryLengthAsString( map.trackingManager?.trackedGeometry )
              }
              return qsTr( "not tracking" )
            }

            function getStartingTime() {
              if ( map.isTrackingPosition )
              {
                let date = map.trackingManager?.startTime
                if ( date ) {
                  return date.getHours() + ":" + date.getMinutes() + ":" + date.getSeconds()
                }
              }
              return qsTr( "not tracking" )
            }
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

    MMProjectIssuesPanel {
      id: projectIssuesPanel

      height: window.height
      width: window.width
      visible: false

      onVisibleChanged: {
        if (projectIssuesPanel.visible)
          projectIssuesPanel.focus = true; // get focus
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

    MMFormStackManager {
      id: formsStackManager

      height: window.height
      width: window.width
      previewHeight: window.height * 0.4

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
        else if ( gpsDataDrawerLoader.active )
        {
          stateManager.state = "misc"
          // do nothing, gps page already has focus
        }
        else {
          stateManager.state = "map"
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

    MMProjectLoadingScreen {
      id: projectLoadingScreen

      anchors.fill: parent
      visible: false
    }

    MMStorageLimitDialog {
        id: storageLimitDialog

        plan: __merginApi.subscriptionInfo.planAlias
        dataUsing: "%1 / %2".arg(__inputUtils.bytesToHumanSize(__merginApi.workspaceInfo.diskUsage)).arg(__inputUtils.bytesToHumanSize(__merginApi.workspaceInfo.storageLimit))
        usedData: __merginApi.workspaceInfo.storageLimit > 0 ? __merginApi.workspaceInfo.diskUsage / __merginApi.workspaceInfo.storageLimit : 0
        apiSupportsSubscription: __merginApi.apiSupportsSubscriptions

        onManageAccountClicked: {
          storageLimitDialog.close()
          if (__merginApi.apiSupportsSubscriptions) {
            projectPanel.manageSubscriptionPlans()
          }
        }
    }

    ProjectLimitDialog {
        id: projectLimitDialog
        onOpenSubscriptionPlans: {
          projectLimitDialog.close()
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

    MessageDialog {
      id: migrationDialog

      property string version

      onAccepted: migrationDialog.close()
      title: qsTr("Your server will soon be out of date")
      text: qsTr("Please contact your server administrator to upgrade your server to the latest version. Subsequent releases of our mobile app may not be compatible with your current server version.")
      buttons: MessageDialog.Close | MessageDialog.Help | MessageDialog.Ignore
      onButtonClicked: function(clickedButton) {
        if (clickedButton === MessageDialog.Help) {
          Qt.openUrlExternally(__inputHelp.migrationGuides)
        }
        else if (clickedButton === MessageDialog.Ignore) {
          // don't show this dialog for this version
          __appSettings.ignoreMigrateVersion = version
        }
        close()
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

    MigrateToMerginDialog {
      id: migrateToMerginDialog

      onMigrationRequested: __syncManager.migrateProjectToMergin( __activeProject.projectFullName() )
    }

    NoPermissionsDialog {
      id: noPermissionsDialog
    }

    SyncFailedDialog {
      id: syncFailedDialog
    }

    MissingAuthDialog {
      id: missingAuthDialog

      onSingInRequested: {
        stateManager.state = "projects"
        projectPanel.showLogin()
      }
    }

    // Should be the top-most visual item
    MMNotificationView {
      anchors {
        top: parent.top
        left: parent.left
        right: parent.right
        topMargin: 20 * __dp
      }
    }

    MMDropdownDrawer {
      id: featurePairSelection

      title: qsTr( "Select feature" )
      withSearchbar: false
      model: InputClass.FeaturesModel {}
      valueRole: "FeaturePair"
      textRole: "FeatureTitle"

      onSelectionFinished: function( pairs ) {
        var pair = pairs[0]
        featurePairSelection.close()
        map.highlightPair( pair )
        formsStackManager.openForm( pair, "readOnly", "preview" );
      }

      function showPairs( pairs ) {
        if ( pairs.length > 0 )
        {
          model.populateStaticModel( pairs )
          open()
        }
      }
    }

    Connections {
      target: __syncManager
      enabled: stateManager.state === "map"

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
            __notificationModel.addSuccess( qsTr( "Successfully synchronized" ) )

            // refresh canvas
            map.refreshMap()
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
            __notificationModel.addInfo( qsTr( "Somebody else is syncing, we will try again later" ) )
          }
          else
          {
            syncFailedDialog.detailedText = qsTr( "Details" ) + ": " + errorMessage
            if ( willRetry )
            {
              // TODO: open sync failed dialogue when clicked on the notification
              __notificationModel.addError( qsTr( "There was an issue during synchronization, we will try again. Click to learn more" ) )
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
          storageLimitDialog.dataToSync = __inputUtils.bytesToHumanSize(uploadSize)
          storageLimitDialog.open()
        }

        function onProjectLimitReached( maxProjects, errorMsg ) {
          __merginApi.getUserInfo()
          if (__merginApi.apiSupportsSubscriptions) {
            __merginApi.getWorkspaceInfo()
          }
          projectLimitDialog.maxProjectNumber = maxProjects
          projectLimitDialog.open()
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

        function onMigrationRequested( version ) {
          if( __appSettings.ignoreMigrateVersion !== version ) {
            migrationDialog.version = version
            migrationDialog.open()
          }
        }

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
            __notificationModel.addInfo( qsTr( "Up to date" ) )
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

        // check location permission
        if ( locationPermission.status === Qt.Undetermined ) {
          locationPermission.request();
        }
        else if ( locationPermission.status === Qt.Denied ) {
          __inputUtils.log("Permissions", "Location permission is denied")
        }
      }

      function onLoadingErrorFound() {
        failedToLoadProjectBanner.pushNotificationMessage( qsTr( "There were issues loading the project." ) )
      }

      function onReportIssue( title, message ) {
        projectIssuesPanel.reportIssue( title, message )
      }

      function onProjectReloaded( project ) {
        map.clear()

        if ( __activeProject.isProjectLoaded() )
        {
          projectPanel.hidePanel()
        }

        __appSettings.defaultProject = __activeProject.localProject.qgisProjectFilePath ?? ""
        __appSettings.activeProject = __activeProject.localProject.qgisProjectFilePath ?? ""
      }

      function onProjectWillBeReloaded() {
        formsStackManager.reload()
      }

      function onProjectReadingFailed( message ) {
        projectErrorDialog.informativeText = qsTr( "Could not read the project file:" ) + "\n" + message
        projectErrorDialog.open()
      }
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
