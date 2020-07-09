import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtGraphicalEffects 1.0

import QgsQuick 0.1 as QgsQuick

Item {
  id: root

  signal featureClicked( var featureId )

  function chooseSourceImage( geometryType ) {
    switch ( geometryType ) {
    case 0:
     return InputStyle.vectorPointIcon
    case 1:
      return InputStyle.vectorLineIcon
    case 2:
      return InputStyle.vectorPolygonIcon
    default:
      return InputStyle.vectorPolygonIcon
    }
  }

  ListView {
    implicitHeight: parent.height
    implicitWidth: parent.width

    model: __featuresModel
    delegate: Rectangle {
      id: itemContainer
      width: parent.width
      height: 50 * QgsQuick.Utils.dp

      MouseArea {
        anchors.fill: parent
        onClicked: {
          root.featureClicked( model.FeatureId )
        }
      }

      RowLayout {
        id: layout
        anchors.fill: parent

        Item {
            id: iconContainer
            height: itemContainer.height
            width: 60 * QgsQuick.Utils.dp

            Image {
                id: icon
                anchors.centerIn: parent
                anchors.leftMargin: 10 * QgsQuick.Utils.dp
                source: chooseSourceImage( model.GeometryType )
                width: 30 * QgsQuick.Utils.dp
                height: width
                sourceSize.width: width
                sourceSize.height: height
                fillMode: Image.PreserveAspectFit
            }
        }

        Item {
            id: textContainer
            height: itemContainer.height
            Layout.fillWidth: true

            Text {
                id: featureTitleText
                text: model.FeatureTitle
                height: textContainer.height/2
                width: textContainer.width
                font.pixelSize: InputStyle.fontPixelSizeNormal
                color: InputStyle.fontColor
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignBottom
                elide: Text.ElideRight
            }

            Text {
                id: descriptionText
                height: textContainer.height/2
                text: model.Description
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.top: featureTitleText.bottom
                font.pixelSize: InputStyle.fontPixelSizeSmall
                color: InputStyle.panelBackgroundDark
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignTop
                elide: Text.ElideRight
            }
        }
      }
    }
  }
}
