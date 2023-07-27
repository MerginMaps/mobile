import QtQuick
import QtQuick.Controls

Page {
  id: pane

  //FontLoader { id: fontx; source: "qrc:/fonts/SquarePeg-Regular.ttf" }
  Label {
    width: parent.width / 2
    text: "DP ratio: ~" + Math.round(__dp * 1000) / 1000
    anchors.centerIn: parent
    horizontalAlignment: Label.AlignHCenter
    verticalAlignment: Label.AlignVCenter
    wrapMode: Label.Wrap
    color: "white"
    font.pixelSize: 20
    //font.family: fontx.font.family
  }
}
