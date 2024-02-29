/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Basic

/**
 * This is a white drawer with close button that shows items from model with icon + title
 * You can specify element to show where there are no items (use MMMessage component)
 */
Drawer {
  id: control

  property alias title: title.text
  property alias model: listView.model //! Model must implement count property, e.g. QML ListModel

  property alias noItemsDelegate: noItemsDelegate.sourceComponent /* usually MMMessage */

  property string valueRole: "type"
  property string textRole: "name"
  property string imageRole: "iconSource"

  property var activeValue /* which value defined by valueRole should be highlighted */

  property bool modelIsEmpty: control.model ? control.model.count === 0 : true

  padding: 20 * __dp

  signal clicked(type: string)

  width: window.width
  height: mainColumn.height
  edge: Qt.BottomEdge

  Rectangle {
    color: roundedRect.color
    anchors.top: parent.top
    anchors.left: parent.left
    anchors.right: parent.right
    height: 2 * radius
    anchors.topMargin: -radius
    radius: 20 * __dp
  }

  Rectangle {
    id: roundedRect

    anchors.fill: parent
    color: __style.whiteColor

    Column {
      id: mainColumn

      width: parent.width
      spacing: 20 * __dp
      leftPadding: control.padding
      rightPadding: control.padding
      bottomPadding: control.padding

      // TODO use MMPageHeader
      Row {
        width: parent.width - 2 * control.padding
        anchors.horizontalCenter: parent.horizontalCenter

        Item { width: closeButton.width; height: 1 }

        Text {
          id: title

          anchors.verticalCenter: parent.verticalCenter
          font: __style.t2
          width: parent.width - closeButton.width * 2
          color: __style.forestColor
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          elide: Text.ElideRight
        }

        MMRoundButton {
          id: closeButton
          iconSource: __style.closeIcon
          bgndColor: __style.lightGreenColor
          onClicked: control.visible = false
        }
      }

      Loader {
        id: noItemsDelegate

        height: control.modelIsEmpty ? item?.height ?? 0 : 0
        active: control.modelIsEmpty
        anchors.horizontalCenter: parent.horizontalCenter
      }

      ListView {
        id: listView

        width: parent.width - 2 * control.padding
        interactive: false

        height: control.model ? control.model.count * __style.menuDrawerHeight : 0
        maximumFlickVelocity: __androidUtils.isAndroid ? __style.scrollVelocityAndroid : maximumFlickVelocity

        delegate: Item {
          width: listView.width
          height: __style.menuDrawerHeight

          MMListDrawerItem {
            width: listView.width

            type: model[control.valueRole]
            text: model[control.textRole]
            iconSource: model[control.imageRole]
            isActive: control.activeValue ? control.activeValue === model[control.valueRole] : false

            onClicked: function(type) { control.clicked(type); control.visible = false }
          }
        }
      }
    }
  }
}
