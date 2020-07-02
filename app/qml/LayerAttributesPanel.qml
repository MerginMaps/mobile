import QtQuick 2.0
import QtQuick.Controls 2.12

Item {
  id: root

  property string selectedLayer: ""
  signal backButtonClicked()
  
  Page {
    id: attributesPage
    anchors.fill: parent

    header: PanelHeader {
      id: attrPageHeader
      height: InputStyle.rowHeightHeader
      width: parent.width
      color: InputStyle.clrPanelMain
      rowHeight: InputStyle.rowHeightHeader
      titleText: qsTr("Features")
      
      onBack: root.backButtonClicked()
      withBackButton: true
    }

    FeatureListComponent {
      width: parent.width
      height: parent.height

      onItemClicked: {
        console.log("Clicked on item: " + itemId)
      }
    }
  }
}
