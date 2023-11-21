import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "../Style.js" as Style
import "."

Image {
  id: control

  property int hiddenPhotoCount: 0

  signal clicked( var path )
  signal showWholeGallery()

  width: visible ? 120 * __dp : 0
  height: width
  source: visible ? modelData : ""
  asynchronous: true
  layer.enabled: true
  layer {
    effect: OpacityMask {
      maskSource: Item {
        width: control.width
        height: control.height
        Rectangle {
          anchors.centerIn: parent
          width: parent.width
          height: parent.height
          radius: 20 * __dp
        }
      }
    }
  }
  MouseArea {
    anchors.fill: parent
    onClicked: control.hiddenPhotoCount > 0 ? control.showWholeGallery() : control.clicked(model.modelData)
  }
  Rectangle {
    anchors.centerIn: parent
    width: parent.width
    height: parent.height
    radius: 20 * __dp
    color: Style.transparent
    border.color: Style.forest
    border.width: 1 * __dp
  }
  Image {
    id: bluredImage

    visible: control.hiddenPhotoCount > 0
    anchors.fill: parent
    source: parent.visible ? modelData : ""
    asynchronous: true
    layer.enabled: true

    layer {
      effect: FastBlur {
        anchors.fill: bluredImage
        source: bluredImage
        radius: 32
      }
    }
  }

  Column {
    anchors.centerIn: parent
    visible: control.hiddenPhotoCount > 0

    Image {
      source: Style.morePhotosIcon
      anchors.horizontalCenter: parent.horizontalCenter
    }
    Text {
      font: Qt.font(Style.t4)
      text: qsTr("+%1 more").arg(control.hiddenPhotoCount)
      color: Style.white
      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
      elide: Text.ElideRight
    }
  }

  onStatusChanged: {
    if (status === Image.Error) {
      console.error("Error loading image in MMPhotoGallery");
    }
  }
}
