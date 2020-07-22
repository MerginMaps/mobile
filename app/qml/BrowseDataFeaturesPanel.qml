import QtQuick 2.0
import QtQuick.Controls 2.12

Item {
  id: root

  signal backButtonClicked()
  signal featureClicked( var featureId )
  signal addFeatureClicked()

  property bool layerHasGeometry: true
  property string layerName: ""
  property int featuresCount: 0

  Page {
    id: featuresPage
    anchors.fill: parent

    header: PanelHeader {
      id: featuresPageHeader
      height: InputStyle.rowHeightHeader
      width: parent.width
      color: InputStyle.clrPanelMain
      rowHeight: InputStyle.rowHeightHeader
      titleText: layerName + " (" + featuresCount + ")"
      
      onBack: root.backButtonClicked()
      withBackButton: true
    }

    BrowseDataView {
      id: browseDataView
      width: parent.width
      height: parent.height

      onFeatureClicked: root.featureClicked( featureId )
    }

    footer: BrowseDataToolbar {
      visible: !layerHasGeometry
      onAddButtonClicked: addFeatureClicked()
    }
  }
}
