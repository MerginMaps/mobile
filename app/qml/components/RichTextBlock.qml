import QtQuick 2.12
import "../"

Text {
  id: root
  color: InputStyle.fontColor
  font.bold: true
  font.pixelSize: InputStyle.fontPixelSizeNormal
  horizontalAlignment: Text.AlignHCenter
  padding: InputStyle.panelMargin / 2
  textFormat: Text.RichText
  verticalAlignment: Text.AlignVCenter
  wrapMode: Text.WordWrap
}
