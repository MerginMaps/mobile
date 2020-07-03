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

    signal listItemClicked(var index)

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
          height: isVector && !isReadOnly && hasGeometry ? listView.cellHeight : 0
          visible: height ? true : false
          anchors.leftMargin: InputStyle.panelMargin
          anchors.rightMargin: InputStyle.panelMargin
          color: highlightingAllowed && item.highlight ? secondaryColor : primaryColor

          MouseArea {
            anchors.fill: parent
            onClicked: {
              listItemClicked(index)
            }
          }

          ExtendedMenuItem {
              id: item
              anchors.rightMargin: InputStyle.panelMargin
              anchors.leftMargin: InputStyle.panelMargin
              contentText: name ? name : ""
              imageSource: iconSource ? iconSource : ""
              overlayImage: false
              highlight: highlightingAllowed && __layersModel.activeIndex === index
              showBorder: highlightingAllowed ? !__appSettings.defaultLayer || __layersModel.activeIndex - 1 !== index : true
          }
      }
    }

    Label {
        anchors.fill: parent
        horizontalAlignment: Qt.AlignHCenter
        verticalAlignment: Qt.AlignVCenter
        visible: parent.count == 0
        text: qsTr("No editable layers in the project!")
        color: InputStyle.fontColor
        font.pixelSize: InputStyle.fontPixelSizeNormal
        font.bold: true
    }

  }
