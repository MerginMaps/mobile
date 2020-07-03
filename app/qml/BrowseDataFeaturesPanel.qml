import QtQuick 2.0
import QtQuick.Controls 2.12

Item {
  id: root

  signal backButtonClicked()
  signal featureClicked( var featureId )
  
  Page {
    id: featuresPage
    anchors.fill: parent

    header: PanelHeader {
      id: featuresPageHeader
      height: InputStyle.rowHeightHeader
      width: parent.width
      color: InputStyle.clrPanelMain
      rowHeight: InputStyle.rowHeightHeader
      titleText: qsTr("Features")
      
      onBack: root.backButtonClicked()
      withBackButton: true
    }

    BrowseDataView {
      id: browseDataView
      width: parent.width
      height: parent.height

      onFeatureClicked: root.featureClicked( featureId )
    }
  }
}
