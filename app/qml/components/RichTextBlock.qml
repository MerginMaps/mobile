import QtQuick 2.12
import "../"

Text {
  id: root

  textFormat: Text.RichText
  color: InputStyle.fontColor
  font.pointSize: InputStyle.fontPointSizeNormal
  font.bold: true
  verticalAlignment: Text.AlignVCenter
  horizontalAlignment: Text.AlignHCenter
  wrapMode: Text.WordWrap
  padding: InputStyle.panelMargin/2
}
