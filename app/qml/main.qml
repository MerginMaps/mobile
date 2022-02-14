/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick 2.7
import QtQuick.Controls 2.2

// Required for iOS to get rid of "module "QtMultimedia" is not installed".
// It looks like static QT plugins are not copied to the distribution
import QtMultimedia 5.8
import QtQml.Models 2.2
import QtPositioning 5.8
import QtQuick.Dialogs 1.1

import lc 1.0
import "./map"
import "./misc"

ApplicationWindow {
    id: window
    visible: true
    width:  __appwindowwidth
    height: __appwindowheight
    visibility: __appwindowvisibility
    title: "Input" // Do not translate

    Item {
        id: stateManager
        state: "view"
        states: [
            // Browsing map in opened project
            State {
                name: "view"
            },
            // When a user is in recording session - creating new features, editing geometries..
            State {
                name: "record"
            },
            // Listing projects
            State {
                name: "projects"
            }
        ]

        onStateChanged: {
            if ( stateManager.state === "view" ) {
              projectPanel.hidePanel()
              map.state = "view"
            }
            else if ( stateManager.state === "record" ) {
              map.state = "recordFeature";
            }
            else if ( stateManager.state === "projects" ) {
              projectPanel.openPanel()
              map.state = "inactive";
            }
        }
    }

    function showMessage(message) {
        if ( !__androidUtils.isAndroid ) {
            popup.text = message
            popup.open()
        } else {
            __androidUtils.showToast( message )
        }
    }

    function showDialog(message) {
      alertDialog.text  = message
      alertDialog.open()
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
        map.centerToPair( pair, true )
        map.highlightPair( pair )
        formsStackManager.openForm( pair, "readOnly", "preview")
      }
    }

    Component.onCompleted: {
      // load default project
      if ( __appSettings.defaultProject ) {
        let path = __appSettings.defaultProject

        if ( __localProjectsManager.projectIsValid( path ) && __loader.load( path ) ) {
          projectPanel.activeProjectPath = path
          projectPanel.activeProjectId = __localProjectsManager.projectId( path )
          __appSettings.activeProject = path
        }
        else {
          // if default project load failed, delete default setting
          __appSettings.defaultProject = ""
          projectPanel.openPanel()
        }
      }
      else projectPanel.openPanel()

        // get focus when any project is active, otherwise let focus to merginprojectpanel
        if ( __appSettings.activeProject )
          mainPanel.forceActiveFocus()

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
        else if ( formsStackManager.takenPanelsSpace > 0 )
        {
          // if feature preview panel is opened
          return formsStackManager.takenPanelsSpace - mainPanel.height
        }
        return 0
      }

      onFeatureIdentified: formsStackManager.openForm( pair, "readOnly", "preview" );

      onNothingIdentified: formsStackManager.closeDrawer();

      onRecordingFinished: {
        formsStackManager.openForm( pair, "add", "form" )
        stateManager.state = "view"
        map.highlightPair( pair )
      }
      onRecordingCanceled: stateManager.state = "view"

      onEditingGeometryStarted: formsStackManager.geometryEditingStarted()
      onEditingGeometryFinished: {
        formsStackManager.geometryEditingFinished( pair )
        stateManager.state = "view"
      }
      onEditingGeometryCanceled: {
        formsStackManager.geometryEditingFinished( null, false )
        stateManager.state = "view"
      }

      onRecordInLayerFeatureStarted: formsStackManager.recordInLayerStarted()
      onRecordInLayerFeatureFinished: {
        formsStackManager.recordInLayerFinished( pair )
        stateManager.state = "view"
      }
      onRecordInLayerFeatureCanceled: {
        formsStackManager.recordInLayerFinished( null, false )
        stateManager.state = "view"
      }

      onNotify: showMessage( message )
      onAccuracyButtonClicked: {
        gpsDataPageLoader.active = true
        gpsDataPageLoader.focus = true
      }

      onStakeoutStarted: {
        stakeoutPanelLoader.active = true
        stakeoutPanelLoader.focus = true
        stakeoutPanelLoader.item.targetPair = pair
      }

      Component.onCompleted: {
        __loader.recording = map.digitizingController.recording
        __loader.mapSettings = map.mapSettings
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

        y: window.height - height

        visible: stateManager.state === "view"

        gpsIndicatorColor: map.gpsIndicatorColor

        onOpenProjectClicked: stateManager.state = "projects"
        onOpenMapThemesClicked: mapThemesPanel.visible = true
        onMyLocationClicked: map.centerToPosition()

        onMyLocationHold: {
            __appSettings.autoCenterMapChecked = !__appSettings.autoCenterMapChecked
            showMessage( __appSettings.autoCenterMapChecked ?  qsTr("GPS auto-center mode on") : qsTr("GPS auto-center mode off") )
        }
        onOpenSettingsClicked: settingsPanel.visible = true
        onZoomToProject: {
          if ( __appSettings.autoCenterMapChecked ) {
            mainPanel.myLocationHold()
          }
          __loader.zoomToProject( map.mapSettings )
        }
        onOpenBrowseDataClicked: browseDataPanel.visible = true
        onRecordClicked: {
            if ( __recordingLayersModel.rowCount() > 0 ) {
                stateManager.state = "record"
            } else {
                showMessage( qsTr( "No editable layers found." ) )
            }
        }
    }

    NotificationBanner {
      id: failedToLoadProjectBanner

      width: parent.width - failedToLoadProjectBanner.anchors.margins * 2
      height: InputStyle.rowHeight * 2

      onDetailsClicked: {
        projectIssuesPanel.projectLoadingLog = __loader.projectLoadingLog();
        projectIssuesPanel.visible = true;
      }
    }

    SettingsPanel {
      id: settingsPanel

      height: window.height
      width: window.width
      rowHeight: InputStyle.rowHeight

      onVisibleChanged: {
        if (settingsPanel.visible)
          settingsPanel.focus = true; // get focus
        else
          mainPanel.focus = true; // pass focus back to main panel
      }

      gpsIndicatorColor: map.gpsIndicatorColor
    }

    ProjectPanel {
        id: projectPanel

        height: window.height
        width: window.width

        onVisibleChanged: {
          if (projectPanel.visible)
            projectPanel.forceActiveFocus()
          else
          {
            mainPanel.forceActiveFocus()
          }
        }

        onOpenProjectRequested: {
          __appSettings.defaultProject = projectPath
          __appSettings.activeProject = projectPath
          __loader.load( projectPath )
        }

        onClosed: stateManager.state = "view"
    }

    BrowseDataPanel {
      id: browseDataPanel

      width: window.width
      height: window.height
      focus: true

      onFeatureSelectRequested: selectFeature( pair )

      onCreateFeatureRequested: {
        let newPair = map.createFeature( selectedLayer )
        formsStackManager.openForm( newPair, "add", "form" )
      }

      onVisibleChanged: {
        if ( !browseDataPanel.visible )
          mainPanel.forceActiveFocus()
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

        onAutoFollowClicked: map.autoFollowStakeoutPath = true
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
        id: alertDialog
        onAccepted: alertDialog.close()
        title: qsTr("Communication error")
    }

    MessageDialog {
        id: projDialog
        onAccepted: projDialog.close()
        title: qsTr("PROJ Error")
        standardButtons: StandardButton.Ignore |StandardButton.Help
        onHelp: Qt.openUrlExternally(__inputHelp.howToSetupProj)
    }

    FormsStackManager {
      id: formsStackManager

      height: window.height
      width: window.width
      previewHeight: window.height / 3

      project: __loader.project

      onCreateLinkedFeatureRequested: {
        let isNoGeoLayer = __inputUtils.geometryFromLayer( targetLayer ) === "nullGeo"

        if ( isNoGeoLayer ) {
          let newPair = map.createFeature( targetLayer )
          recordInLayerFinished( newPair, true )
        }
        else { // we will record geometry
          stateManager.state = "record"
          map.targetLayerToUse = targetLayer
          map.state = "recordInLayerFeature"
          map.centerToPair( parentPair )
        }
      }

      onEditGeometryRequested: {
        stateManager.state = "record"
        map.featurePairToEdit = pair
        map.centerToPair( pair )
        map.state = "editGeometry"
      }

      onClosed: {
        if ( browseDataPanel.visible ) {
          browseDataPanel.refreshFeaturesData()
          browseDataPanel.focus = true
        }
        else if ( gpsDataPageLoader.active )
        {
          // do nothing, gps page already has focus
        }
        else mainPanel.focus = true

        map.hideHighlight()
      }

      onStakeoutFeature: {
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

    ProjectLoadingScreen {
      id: projectLoadingScreen

      anchors.fill: parent
      visible: false
    }

    Connections {
        target: __merginApi
        onNetworkErrorOccurred: {
            var msg = message ? message : qsTr( "Failed to communicate with Mergin.%1Try improving your network connection." ).arg( "\n" )
            showAsDialog ? showDialog(msg) : showMessage(msg)
        }

        onStorageLimitReached: {
          __merginApi.getUserInfo()
          if (__merginApi.apiSupportsSubscriptions) {
            __merginApi.getSubscriptionInfo()
          }
          storageLimitDialog.uploadSize = uploadSize
          storageLimitDialog.open()
        }

        onNotify: showMessage(message)

        onProjectDataChanged: {
          //! if current project has been updated, refresh canvas
          if ( projectFullName === projectPanel.activeProjectId ) {
            map.mapSettings.extentChanged()
          }
        }
    }

    Connections {
        target: __inputProjUtils
        onProjError: {
          showProjError(message)
        }
    }

    Connections {
        target: __inputUtils
        onShowNotificationRequested: {
            showMessage(message)
        }
    }

    Connections {
      target: __loader
      onLoadingStarted: {
        projectLoadingScreen.visible = true;
        failedToLoadProjectBanner.reset();
        projectIssuesPanel.clear();
      }
      onLoadingFinished: projectLoadingScreen.visible = false
      onLoadingErrorFound: {
        failedToLoadProjectBanner.pushNotificationMessage( qsTr( "There were issues loading the project." ) )
      }

      onReportIssue: projectIssuesPanel.reportIssue( layerName, message )
      onSetProjectIssuesHeader: projectIssuesPanel.headerText = text

      onProjectReloaded: map.clear()
      onProjectWillBeReloaded: {
        formsStackManager.reload()
      }
    }

    LegacyFolderMigration {
      id: lfm

      z: 1000 // unfortunatelly we need this hack because some parts of application still sets z coord
      anchors.fill: parent
    }
}
