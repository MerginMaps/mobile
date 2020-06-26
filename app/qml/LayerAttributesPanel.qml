import QtQuick 2.0
import QtQuick.Controls 2.12

Item {
  id: root

  property string selectedLayer: ""
  signal backButtonTapped()

  
  Page {
    id: attributesPage
    anchors.fill: parent

    header: PanelHeader {
      id: attrPageHeader
      height: InputStyle.rowHeightHeader
      width: parent.width
      color: InputStyle.clrPanelMain
      rowHeight: InputStyle.rowHeightHeader
      titleText: qsTr("Layer " + selectedLayer)
      
      onBack: root.backButtonTapped()
      withBackButton: true
    }

    FeatureListComponent {
      width: parent.width
      height: parent.height
    }
  }
}
