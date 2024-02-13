import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic

import "../app/qml/components"
import "../"

Column {
  id: root

  required property string name
  required property url source
  required property bool showRect

  Row {
    Rectangle { width: 100 * __dp; height: 50 * __dp
      Image { source: root.source; anchors.verticalCenter: parent.verticalCenter
        Rectangle { anchors.fill: parent; color: "#ff8888"; z: -1 }
      }
    }
    Rectangle { width: 50 * __dp; height: 50 * __dp
      MMIcon { size: __style.icon16; source: root.source; anchors.verticalCenter: parent.verticalCenter; color: __style.forestColor
        Rectangle { anchors.fill: parent; color: "lightGray"; z: -1; visible: root.showRect }
      }
    }
    Rectangle { width: 50 * __dp; height: 50 * __dp
      MMIcon { size: __style.icon24; source: root.source; anchors.verticalCenter: parent.verticalCenter; color: __style.forestColor
        Rectangle { anchors.fill: parent; color: "lightGray"; z: -1; visible: root.showRect }
      }
    }
    Rectangle { width: 50 * __dp; height: 50 * __dp
      MMIcon { size: __style.icon32; source: root.source; anchors.verticalCenter: parent.verticalCenter; color: __style.forestColor
        Rectangle { anchors.fill: parent; color: "lightGray"; z: -1; visible: root.showRect }
      }
    }
    Rectangle { width: 50 * __dp; height: 50 * __dp
      MMIcon { size: __style.icon40; source: root.source; anchors.verticalCenter: parent.verticalCenter; color: __style.forestColor
        Rectangle { anchors.fill: parent; color: "lightGray"; z: -1; visible: root.showRect }
      }
    }
  }
  Text { text: root.name; width: 250 * __dp; font.bold: true }
  Rectangle { width: parent.width; height: 1; color: "gray" }
}
