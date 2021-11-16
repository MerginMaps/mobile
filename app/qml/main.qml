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
import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "map"
import "misc"

ApplicationWindow {
  id: window
  height: __appwindowheight
  title: "Input" // Do not translate
  visibility: __appwindowvisibility
  visible: true
  width: __appwindowwidth

  function selectFeature(pair) {
    let hasNullGeometry = pair.feature.geometry.isNull;
    if (hasNullGeometry) {
      formsStackManager.openForm(pair, "readOnly", "form");
    } else if (pair.valid) {
      map.centerToPair(pair, true);
      map.highlightPair(pair);
      formsStackManager.openForm(pair, "readOnly", "preview");
    }
  }
  function showDialog(message) {
    alertDialog.text = message;
    alertDialog.open();
  }
  function showMessage(message) {
    if (!__androidUtils.isAndroid) {
      popup.text = message;
      popup.open();
    } else {
      __androidUtils.showToast(message);
    }
  }
  function showProjError(message) {
    projDialog.text = message;
    projDialog.open();
  }

  Component.onCompleted: {
    // load default project
    if (__appSettings.defaultProject) {
      let path = __appSettings.defaultProject;
      if (__localProjectsManager.projectIsValid(path) && __loader.load(path)) {
        projectPanel.activeProjectPath = path;
        projectPanel.activeProjectId = __localProjectsManager.projectId(path);
        __appSettings.activeProject = path;
      } else {
        // if default project load failed, delete default setting
        __appSettings.defaultProject = "";
        projectPanel.openPanel();
      }
    } else
      projectPanel.openPanel();

    // get focus when any project is active, otherwise let focus to merginprojectpanel
    if (__appSettings.activeProject)
      mainPanel.forceActiveFocus();
    console.log("Application initialized!");
  }

  Item {
    id: stateManager
    state: "view"

    onStateChanged: {
      if (stateManager.state === "view") {
        projectPanel.hidePanel();
        map.state = "view";
      } else if (stateManager.state === "record") {
        map.state = "recordFeature";
      } else if (stateManager.state === "projects") {
        projectPanel.openPanel();
        map.state = "inactive";
      }
    }

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
  }
  MapWrapper {
    id: map
    height: window.height - mainPanel.height
    previewPanelHeight: formsStackManager.previewHeight
    width: window.width

    Component.onCompleted: {
      __loader.positionKit = map.positionKit;
      __loader.recording = map.digitizingController.recording;
      __loader.mapSettings = map.mapSettings;
      __iosUtils.positionKit = map.positionKit;
      __iosUtils.compass = map.compass;
      __variablesManager.compass = map.compass;
      __variablesManager.positionKit = map.positionKit;
    }
    onEditingGeometryCanceled: {
      formsStackManager.geometryEditingFinished(null, false);
      stateManager.state = "view";
    }
    onEditingGeometryFinished: {
      formsStackManager.geometryEditingFinished(pair);
      stateManager.state = "view";
    }
    onEditingGeometryStarted: formsStackManager.geometryEditingStarted()
    onFeatureIdentified: formsStackManager.openForm(pair, "readOnly", "preview")
    onNothingIdentified: formsStackManager.closeDrawer()
    onNotify: showMessage(message)
    onRecordInLayerFeatureCanceled: {
      formsStackManager.recordInLayerFinished(null, false);
      stateManager.state = "view";
    }
    onRecordInLayerFeatureFinished: {
      formsStackManager.recordInLayerFinished(pair);
      stateManager.state = "view";
    }
    onRecordInLayerFeatureStarted: formsStackManager.recordInLayerStarted()
    onRecordingCanceled: stateManager.state = "view"
    onRecordingFinished: {
      formsStackManager.openForm(pair, "add", "form");
      stateManager.state = "view";
      map.highlightPair(pair);
    }
  }
  MainPanel {
    id: mainPanel
    gpsIndicatorColor: map.gpsIndicatorColor
    height: InputStyle.rowHeightHeader
    visible: stateManager.state === "view"
    width: window.width
    y: window.height - height

    onMyLocationClicked: map.centerToPosition()
    onMyLocationHold: {
      __appSettings.autoCenterMapChecked = !__appSettings.autoCenterMapChecked;
      showMessage(__appSettings.autoCenterMapChecked ? qsTr("GPS auto-center mode on") : qsTr("GPS auto-center mode off"));
    }
    onOpenBrowseDataClicked: browseDataPanel.visible = true
    onOpenMapThemesClicked: mapThemesPanel.visible = true
    onOpenProjectClicked: stateManager.state = "projects"
    onOpenSettingsClicked: settingsPanel.visible = true
    onRecordClicked: {
      if (__recordingLayersModel.rowCount() > 0) {
        stateManager.state = "record";
      } else {
        showMessage(qsTr("No editable layers found."));
      }
    }
    onZoomToProject: {
      if (__appSettings.autoCenterMapChecked) {
        mainPanel.myLocationHold();
      }
      __loader.zoomToProject(map.mapSettings);
    }
  }
  SettingsPanel {
    id: settingsPanel
    gpsIndicatorColor: map.gpsIndicatorColor
    height: window.height
    rowHeight: InputStyle.rowHeight
    width: window.width

    onVisibleChanged: {
      if (settingsPanel.visible)
        settingsPanel.focus = true;
        // get focus
      else
        mainPanel.focus = true; // pass focus back to main panel
    }
  }
  ProjectPanel {
    id: projectPanel
    height: window.height
    width: window.width

    onClosed: stateManager.state = "view"
    onOpenProjectRequested: {
      __appSettings.defaultProject = projectPath;
      __appSettings.activeProject = projectPath;
      __loader.load(projectPath);
    }
    onVisibleChanged: {
      if (projectPanel.visible)
        projectPanel.forceActiveFocus();
      else {
        mainPanel.forceActiveFocus();
      }
    }
  }
  BrowseDataPanel {
    id: browseDataPanel
    focus: true
    height: window.height
    width: window.width

    onCreateFeatureRequested: {
      let newPair = map.createFeature(selectedLayer);
      formsStackManager.openForm(newPair, "add", "form");
    }
    onFeatureSelectRequested: selectFeature(pair)
    onVisibleChanged: {
      if (!browseDataPanel.visible)
        mainPanel.forceActiveFocus();
    }
  }
  MapThemePanel {
    id: mapThemesPanel
    edge: Qt.BottomEdge
    height: window.height / 2
    width: window.width
  }
  Notification {
    id: popup
    height: 160 * QgsQuick.Utils.dp
    text: ""
    width: 400 * QgsQuick.Utils.dp
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
  }
  StorageLimitDialog {
    id: storageLimitDialog
    onOpenSubscriptionPlans: {
      storageLimitDialog.close();
      if (__merginApi.apiSupportsSubscriptions) {
        projectPanel.manageSubscriptionPlans();
      }
    }
  }
  MessageDialog {
    id: alertDialog
    title: qsTr("Communication error")

    onAccepted: alertDialog.close()
  }
  MessageDialog {
    id: projDialog
    standardButtons: StandardButton.Ignore | StandardButton.Help
    title: qsTr("PROJ Error")

    onAccepted: projDialog.close()
    onHelp: Qt.openUrlExternally(__inputHelp.howToSetupProj)
  }
  FormsStackManager {
    id: formsStackManager
    height: window.height
    previewHeight: window.height / 3
    project: __loader.project
    width: window.width

    onClosed: {
      if (browseDataPanel.visible) {
        browseDataPanel.refreshFeaturesData();
        browseDataPanel.focus = true;
      } else
        mainPanel.focus = true;
      map.hideHighlight();
    }
    onCreateLinkedFeatureRequested: {
      let isNoGeoLayer = __inputUtils.geometryFromLayer(targetLayer) === "nullGeo";
      if (isNoGeoLayer) {
        let newPair = map.createFeature(targetLayer);
        recordInLayerFinished(newPair, true);
      } else {
        // we will record geometry
        stateManager.state = "record";
        map.targetLayerToUse = targetLayer;
        map.state = "recordInLayerFeature";
        map.centerToPair(parentPair);
      }
    }
    onEditGeometryRequested: {
      stateManager.state = "record";
      map.featurePairToEdit = pair;
      map.centerToPair(pair);
      map.state = "editGeometry";
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
      var msg = message ? message : qsTr("Failed to communicate with Mergin.%1Try improving your network connection.").arg("\n");
      showAsDialog ? showDialog(msg) : showMessage(msg);
    }
    onNotify: showMessage(message)
    onProjectDataChanged: {
      //! if current project has been updated, refresh canvas
      if (projectFullName === projectPanel.activeProjectId) {
        map.mapSettings.extentChanged();
      }
    }
    onStorageLimitReached: {
      __merginApi.getUserInfo();
      if (__merginApi.apiSupportsSubscriptions) {
        __merginApi.getSubscriptionInfo();
      }
      storageLimitDialog.uploadSize = uploadSize;
      storageLimitDialog.open();
    }
  }
  Connections {
    target: __inputProjUtils

    onProjError: {
      showProjError(message);
    }
  }
  Connections {
    target: __inputUtils

    onShowNotificationRequested: {
      showMessage(message);
    }
  }
  Connections {
    target: __loader

    onLoadingFinished: projectLoadingScreen.visible = false
    onLoadingStarted: projectLoadingScreen.visible = true
    onProjectReloaded: map.clear()
    onProjectWillBeReloaded: {
      formsStackManager.reload();
    }
  }
  LegacyFolderMigration {
    id: lfm
    anchors.fill: parent
    z: 1000 // unfortunatelly we need this hack because some parts of application still sets z coord
  }
}
