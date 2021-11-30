import QtQuick 2.7
import QtQuick.Controls 2.2
import lc 1.0

import ".."  // import InputStyle singleton

ListView {
  property int cellWidth: width
  property int cellHeight: InputStyle.rowHeight
  property int borderWidth: 1
  property bool highlightingAllowed: true
  property string noLayersText: "(no-layers)" // should be set by parent
  property string activeLayerId: ""
  property int activeIndex: -1

  signal listItemClicked( var layerId )

    id: listView
    implicitWidth: parent.width
    implicitHeight: contentHeight
    clip: true
    maximumFlickVelocity: __androidUtils.isAndroid ? InputStyle.scrollVelocityAndroid : maximumFlickVelocity

    delegate: Component {
      id: delegateItem

      Rectangle {
        id: itemContainer
        property color primaryColor: InputStyle.clrPanelMain
        property color secondaryColor: InputStyle.panelItemHighlight
        width: listView.cellWidth
        height: listView.cellHeight
        visible: height ? true : false
        anchors.leftMargin: InputStyle.panelMargin
        anchors.rightMargin: InputStyle.panelMargin
        color: highlightingAllowed && item.highlight ? secondaryColor : primaryColor

        MouseArea {
          anchors.fill: parent
          onClicked: {
            activeIndex = index
            listItemClicked( model.layerId )
          }
        }

        Component.onCompleted: if (layerName === __activeLayer.layerName) activeIndex = index

        ExtendedMenuItem {
            id: item
            anchors.rightMargin: InputStyle.panelMargin
            anchors.leftMargin: InputStyle.panelMargin
            contentText: layerName ? layerName : ""
            imageSource: iconSource ? iconSource : ""
            overlayImage: false
            highlight: highlightingAllowed && layerId === activeLayerId
            showBorder: highlightingAllowed ? !__appSettings.defaultLayer || (listView.activeIndex !== index && listView.activeIndex - 1 !== index) : true
        }
    }
  }

  TextHyperlink {
      anchors.fill: parent
      visible: parent.count == 0
      text: noLayersText
  }
}
