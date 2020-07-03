import QtQuick 2.0
import QtQuick.Controls 2.12
import QgsQuick 0.1 as QgsQuick

Item {
  id: root

  signal backButtonClicked()
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
      
      onBack: root.backButtonClicked()
      withBackButton: true
    }
    
    LayerList {
        height: layersListPage.height - layersPageHeader.height
        width: parent.width
        y: layersPageHeader.height
        model: __layersModel

        cellWidth: width
        cellHeight: InputStyle.rowHeight
        borderWidth: 1

        onListItemClicked: {
          var layerName = __layersModel.data(__layersModel.index(index, LayersModel.Name))
          layerClicked(layerName)
        }
    }
  }
}
