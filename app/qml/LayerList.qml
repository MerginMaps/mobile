import QtQuick 2.7
import QtQuick.Controls 2.2
import lc 1.0
import QgsQuick 0.1 as QgsQuick
import "."  // import InputStyle singleton

ListView {
  property int cellWidth: width
  property int cellHeight: InputStyle.rowHeight
  property int borderWidth: 1
  property bool highlightingAllowed: true
  property string noLayersText: qsTr("No editable layers in the project!")
  property string activeLayerId: ""
  property int activeIndex: -1

  signal listItemClicked( var layerId )

    id: listView
    implicitWidth: parent.width
    implicitHeight: contentHeight
    clip: true

    delegate: Component {
      id: delegateItem

      Rectangle {
        id: itemContainer
        property color primaryColor: InputStyle.clrPanelMain
        property color secondaryColor: InputStyle.fontColorBright
        width: listView.cellWidth
        height: listView.cellHeight
        visible: height ? true : false
        anchors.leftMargin: InputStyle.panelMargin
        anchors.rightMargin: InputStyle.panelMargin
        color: highlightingAllowed && item.highlight ? secondaryColor : primaryColor

        MouseArea {
          anchors.fill: parent
          onClicked: {
            listItemClicked( model.layerId )
          }
        }

        ExtendedMenuItem {
            id: item
            anchors.rightMargin: InputStyle.panelMargin
            anchors.leftMargin: InputStyle.panelMargin
            contentText: layerName ? layerName : ""
            imageSource: iconSource ? iconSource : ""
            overlayImage: false
            highlight: highlightingAllowed && layerId === activeLayerId
            showBorder: highlightingAllowed ? !__appSettings.defaultLayer || activeIndex - 1 !== index : true
        }
    }
  }

  Label {
      anchors.fill: parent
      horizontalAlignment: Qt.AlignHCenter
      verticalAlignment: Qt.AlignVCenter
      visible: parent.count == 0
      text: noLayersText
      color: InputStyle.fontColor
      font.pixelSize: InputStyle.fontPixelSizeNormal
      font.bold: true
  }
}
