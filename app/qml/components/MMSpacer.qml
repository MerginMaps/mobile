import QtQuick
import "."
import ".."
import lc 1.0

Item {

  width: parent.width
  height: ( 1 * __dp ) < 1 ? 1 : 1 * __dp

  Rectangle {
    width: parent.width
    height: parent.height
    color: __style.greyColor
    visible: true
  }
}
