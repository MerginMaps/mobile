/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick 2.0
import QtQuick.Controls 2.12
import "components"

Item {
  id: root
  property bool allowMultiselect: false
  property bool allowSearch: true
  property var featuresModel: null
  property string pageTitle: featuresModel ? featuresModel.layer.name + " (" + featuresModel.layerFeaturesCount() + ")" : ""
  property var preselectedFeatures: []
  property bool resetAfterSelection: true
  property var toolbarButtons: [] // pass button names in list, see currently supported buttons below in toolbar
  property bool toolbarVisible: true

  signal addFeatureClicked
  signal backButtonClicked
  function finishSelection(featureIds) {
    if (resetAfterSelection) {
      searchBar.deactivate();
    }
    selectionFinished(featureIds);
  }
  signal selectionFinished(var featureIds)
  signal unlinkClicked

  Component.onCompleted: {
    if (featuresModel) {
      if (featuresModel.layerFeaturesCount() > featuresModel.featuresLimit)
        __inputUtils.showNotification(qsTr("Showing only the first %1 features").arg(featuresModel.featuresLimit));
    }
  }

  Page {
    id: featuresPage
    anchors.fill: parent

    Keys.onReleased: {
      if (event.key === Qt.Key_Back || event.key === Qt.Key_Escape) {
        event.accepted = true;
        deactivateSearch();
        root.backButtonClicked();
      }
    }

    SearchBar {
      id: searchBar
      allowTimer: true
      visible: root.allowSearch

      onSearchTextChanged: root.featuresModel.searchExpression = text
    }
    FeaturesList {
      id: listview
      allowMultiselect: root.allowMultiselect
      clip: true
      featuresModel: root.featuresModel
      height: allowSearch ? parent.height - searchBar.height : parent.height
      selectedFeatures: root.preselectedFeatures
      showAdditionalInfo: root.state == "search"
      width: parent.width
      y: allowSearch ? searchBar.height : 0

      onFeatureClicked: root.finishSelection(featureId)
    }

    footer: FeaturesListPageToolbar {
      id: browseDataToolbar
      addButtonVisible: root.toolbarButtons.includes("add")
      doneButtonVisible: root.toolbarButtons.includes("done")
      unlinkButtonVisible: root.toolbarButtons.includes("unlink")
      visible: root.toolbarVisible

      onAddButtonClicked: addFeatureClicked()
      onDoneButtonClicked: root.finishSelection(listview.selectedFeatures)
      onUnlinkButtonClicked: root.unlinkClicked()
    }
    header: PanelHeader {
      id: featuresPageHeader
      color: InputStyle.clrPanelMain
      height: InputStyle.rowHeightHeader
      rowHeight: InputStyle.rowHeightHeader
      titleText: pageTitle
      width: parent.width
      withBackButton: true

      onBack: {
        searchBar.deactivate();
        root.backButtonClicked();
      }
    }
  }

  states: [
    State {
      name: "view"
      when: searchBar.text == ""
    },
    State {
      name: "search"
      when: searchBar.text != ""
    }
  ]
}
