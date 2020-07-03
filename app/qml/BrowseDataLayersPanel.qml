import QtQuick 2.0
import QtQuick.Controls 2.12
import QgsQuick 0.1 as QgsQuick
import lc 1.0

Item {
  id: root

  signal backButtonClicked()
  signal layerClicked(var layer)

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
        implicitHeight: layersListPage.height - layersPageHeader.height
        width: parent.width
        model: __layersModel

        cellWidth: width
        cellHeight: InputStyle.rowHeight
        borderWidth: 1
        highlightingAllowed: false

        onListItemClicked: {
          var layer = __layersModel.data(__layersModel.index(index), LayersModel.VectorLayer)
          layerClicked( layer )
        }
    }
  }
}
