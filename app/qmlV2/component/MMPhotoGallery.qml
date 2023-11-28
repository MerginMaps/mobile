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
import Qt5Compat.GraphicalEffects
import "."
import ".."

Item {
  id: control

  width: parent.width
  height: column.height

  property alias model: rowView.model
  property string title
  property string warningMsg
  property string errorMsg
  property int maxVisiblePhotos: 5

  signal showAll()
  signal clicked( var path )

  Column {
    id: column

    padding: 20 * __dp
    spacing: 10 * __dp
    width: parent.width - 40 * __dp

    Item {
      width: parent.width
      height: 15 * __dp

      Text {
        text: control.title
        font: StyleV2.p6
        wrapMode: Text.WordWrap
        width: column.width - showAllButton.width - 10 * __dp
        color: StyleV2.nightColor
      }

      Text {
        id: showAllButton

        text: qsTr("Show all")
        anchors.right: parent.right
        font: StyleV2.t4
        wrapMode: Text.WordWrap
        color: StyleV2.forestColor

        MouseArea {
          anchors.fill: parent
          onClicked: control.showAll()
        }
      }
    }

    ListView {
      id: rowView

      spacing: 20 * __dp
      orientation: ListView.Horizontal
      height: 120 * __dp
      width: parent.width

      delegate: MMPhoto {
        visible: model.index < control.maxVisiblePhotos
        onClicked: function(path) { control.clicked(path) }
      }

      footer: MMMorePhoto {
        hiddenPhotoCount: model.length - control.maxVisiblePhotos + 1
        visible: model.length > control.maxVisiblePhotos
        source: visible ? model[control.maxVisiblePhotos] : ""
        onClicked: control.showAll()
      }
    }

    Row {
      id: msgRow

      spacing: 4 * __dp

      MMIcon {
        id: msgIcon

        source: visible ? StyleV2.errorIcon : ""
        color: errorMsg.length > 0 ? StyleV2.negativeColor : StyleV2.warningColor
        visible: errorMsg.length > 0 || warningMsg.length > 0
      }
      Text {
        text: errorMsg.length > 0 ? errorMsg : warningMsg
        font: StyleV2.t4
        wrapMode: Text.WordWrap
        width: column.width - msgRow.spacing - msgIcon.width
        visible: errorMsg.length > 0 || warningMsg.length > 0
      }
    }
  }
}
