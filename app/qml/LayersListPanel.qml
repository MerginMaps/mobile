import QtQuick 2.0
import QtQuick.Controls 2.12
import QgsQuick 0.1 as QgsQuick

Item {
  id: root

  signal backButtonTapped()
  signal layerClicked(string layerName)

  Page {
    id: layersListPage
    anchors.fill: parent

    header: PanelHeader {
      id: layersPageHeader
      height: InputStyle.rowHeightHeader
      width: parent.width
      color: InputStyle.clrPanelMain
      rowHeight: InputStyle.rowHeightHeader
      titleText: qsTr("Layers")
      
      onBack: root.backButtonTapped()
      withBackButton: true
    }
    
    Column {
      spacing: 10 * QgsQuick.Utils.dp
      anchors.centerIn: parent
      
      Button {
        text: "Layer A"
        onClicked: root.layerClicked("A")
      }
      
      Button {
        text: "Layer B"
        onClicked: root.layerClicked("B")
      }
    }
  }
}
