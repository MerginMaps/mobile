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

Item {
  id: root

  signal featureClicked( var featureIds )
  signal addFeatureClicked()
  signal unlinkClicked()
  signal backButtonClicked()
  signal searchTextChanged( string text )

  property bool toolbarVisible: true
  property var toolbarButtons: [] // pass button names in list, see currently supported buttons below in toolbar

  property bool allowMultiselect: false
  property bool allowSearch: true
  property string layerName: ""
  property var featuresModel: null
  property int featuresCount: featuresModel ? featuresModel.featuresCount : 0
  property int featuresLimit: featuresModel ? featuresModel.featuresLimit : 0
  property string pageTitle: layerName + " (" + featuresCount + ")"
  property var selectedFeatures: []

  property var deactivateSearch: function deactivateSearch() {
    searchBar.deactivate()
  }

  function featureToggled( featureId, toggleState ) {
    if ( !Array.isArray( selectedFeatures ) )
      selectedFeatures = []
    if ( toggleState === Qt.Checked )
    {
      selectedFeatures.push( featureId )
    }
    else if ( toggleState === Qt.Unchecked )
    {
      selectedFeatures = selectedFeatures.filter( _id => _id !== featureId )
    }
    browseDataView.preSelectedIds = selectedFeatures // update checkboxes when search changes
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

  Component.onCompleted: {
    if ( featuresCount > featuresLimit )
      __inputUtils.showNotification( qsTr( "Showing only the first %1 features" ).arg( featuresLimit ) )
  }

  Page {
    id: featuresPage
    anchors.fill: parent

    header: PanelHeader {
      id: featuresPageHeader
      height: InputStyle.rowHeightHeader
      width: parent.width
      color: InputStyle.clrPanelMain
      rowHeight: InputStyle.rowHeightHeader
      titleText: pageTitle
      
      onBack: {
        searchBar.deactivate()
        root.backButtonClicked()
       }

      withBackButton: true
    }

    SearchBar {
      id: searchBar

      visible: root.allowSearch
      allowTimer: true
      onSearchTextChanged: {
        root.searchTextChanged( text )
      }
    }

    BrowseDataView {
      id: browseDataView
      width: parent.width
      height: allowSearch? parent.height - searchBar.height : parent.height
      y: allowSearch ? searchBar.height : 0
      clip: true
      showAdditionalInfo: root.state == "search"
      featuresModel: root.featuresModel
      allowMultiselect: root.allowMultiselect
      preSelectedIds: selectedFeatures

      onFeatureClicked: root.featureClicked( featureId )
      onFeatureToggled: root.featureToggled( featureId, toggleState )
    }

    footer: BrowseDataToolbar {
      id: browseDataToolbar

      visible: root.toolbarVisible

      addButtonVisible: root.toolbarButtons.includes("add")
      doneButtonVisible: root.toolbarButtons.includes("done")
      unlinkButtonVisible: root.toolbarButtons.includes("unlink")

      onAddButtonClicked: addFeatureClicked()
      onDoneButtonClicked: root.featureClicked( root.selectedFeatures )
      onUnlinkButtonClicked: root.unlinkClicked()
    }
  }
}
