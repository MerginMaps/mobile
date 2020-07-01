import QtQuick 2.0
import QtQuick.Controls 2.12

Item {

  signal itemClicked( string itemId )

  id: root

  ListView {
    implicitHeight: parent.height
    implicitWidth: parent.width

    model: __layerFeaturesModel
    delegate: Rectangle {
      width: parent.width
      height: 50
      border.width: 1

      Text {
        text: model.featureTitle + " " + model.description
        anchors.centerIn: parent
      }

      MouseArea {
        anchors.fill: parent
        onClicked: {
          root.itemClicked( model.featureTitle )
        }
      }
    }
  }
}

