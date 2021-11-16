import QtQuick 2.7
import QtQuick.Controls 2.2
import lc 1.0
import QgsQuick 0.1 as QgsQuick
import "../"  // import InputStyle singleton

ListView {
  id: listView
  property int activeIndex: -1
  property string activeLayerId: ""
  property int borderWidth: 1
  property int cellHeight: InputStyle.rowHeight
  property int cellWidth: width
  property bool highlightingAllowed: true
  property string noLayersText: "(no-layers)" // should be set by parent

  clip: true
  implicitHeight: contentHeight
  implicitWidth: parent.width
  maximumFlickVelocity: __androidUtils.isAndroid ? InputStyle.scrollVelocityAndroid : maximumFlickVelocity

  signal listItemClicked(var layerId)

  TextHyperlink {
    anchors.fill: parent
    text: noLayersText
    visible: parent.count == 0
  }

  delegate: Component {
    id: delegateItem
    Rectangle {
      id: itemContainer
      property color primaryColor: InputStyle.clrPanelMain
      property color secondaryColor: InputStyle.panelItemHighlight

      anchors.leftMargin: InputStyle.panelMargin
      anchors.rightMargin: InputStyle.panelMargin
      color: highlightingAllowed && item.highlight ? secondaryColor : primaryColor
      height: listView.cellHeight
      visible: height ? true : false
      width: listView.cellWidth

      Component.onCompleted: if (layerName === __activeLayer.layerName)
        activeIndex = index

      MouseArea {
        anchors.fill: parent

        onClicked: {
          activeIndex = index;
          listItemClicked(model.layerId);
        }
      }
      ExtendedMenuItem {
        id: item
        anchors.leftMargin: InputStyle.panelMargin
        anchors.rightMargin: InputStyle.panelMargin
        contentText: layerName ? layerName : ""
        highlight: highlightingAllowed && layerId === activeLayerId
        imageSource: iconSource ? iconSource : ""
        overlayImage: false
        showBorder: highlightingAllowed ? !__appSettings.defaultLayer || (listView.activeIndex !== index && listView.activeIndex - 1 !== index) : true
      }
    }
  }
}
