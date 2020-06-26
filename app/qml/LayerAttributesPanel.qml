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
      titleText: qsTr("Attributes")
      
      onBack: root.backButtonTapped()
      withBackButton: true
    }

//    Text {
//      anchors.centerIn: parent
//      text: qsTr("List of attributes for layer " + selectedLayer)
//    }

    ListView {
      model: __layerFeaturesModel
      anchors.fill: parent
      delegate: Rectangle {
        width: 50
        height: 50
        border.width: 1

        Text {
            text: model.id + " " + model.displayName
            anchors.centerIn: parent
        }
      }
    }
  }
}
