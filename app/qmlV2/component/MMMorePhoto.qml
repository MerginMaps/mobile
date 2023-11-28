import QtQuick
import QtQuick.Controls
import Qt5Compat.GraphicalEffects
import "."
import ".."

Image {
  id: control

  property int hiddenPhotoCount: 0

  signal clicked()

  width: visible ? 120 * __dp : 0
  height: width
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
    onClicked: control.clicked()
  }
  Rectangle {
    anchors.centerIn: parent
    width: parent.width
    height: parent.height
    radius: 20 * __dp
    color: StyleV2.transparentColor
    border.color: StyleV2.forestColor
    border.width: 1 * __dp
  }
  Image {
    id: bluredImage

    anchors.fill: parent
    source: control.source
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

    Image {
      source: StyleV2.morePhotosIcon
      anchors.horizontalCenter: parent.horizontalCenter
    }
    Text {
      font: StyleV2.t4
      text: qsTr("+%1 more").arg(control.hiddenPhotoCount)
      color: StyleV2.whiteColor
      horizontalAlignment: Text.AlignHCenter
      verticalAlignment: Text.AlignVCenter
      elide: Text.ElideRight
    }
  }

  onStatusChanged: {
    if (status === Image.Error) {
      console.error("MMMorePhoto: Error loading image");
    }
  }
}
