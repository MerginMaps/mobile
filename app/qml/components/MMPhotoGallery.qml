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
import "."

Item {
  id: control

  width: parent.width
  height: column.height

  required property var model
  property string title
  property string warningMsg
  property string errorMsg
  property int maxVisiblePhotos: 5 // -1 for showing all photos

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
        font: __style.p6
        elide: Text.ElideRight
        width: column.width - showAllButton.width - 10 * __dp
        color: __style.nightColor
      }

      Text {
        id: showAllButton

        text: qsTr("Show all")
        anchors.right: parent.right
        font: __style.t4
        color: __style.forestColor

        MouseArea {
          anchors.fill: parent
          onClicked: control.showAll()
        }
      }
    }

    ListView {
      id: rowView

      model: {
        if(control.maxVisiblePhotos >= 0 && control.model.length > control.maxVisiblePhotos) {
          return control.model.slice(0, control.maxVisiblePhotos)
        }
        return control.model
      }
      spacing: control.maxVisiblePhotos !== 0 ? 20 * __dp : 0
      orientation: ListView.Horizontal
      height: 120 * __dp
      width: parent.width

      delegate: MMPhoto {
        width: rowView.height
        onClicked: function(path) { control.clicked(path) }
      }

      footer: MMMorePhoto {
        hiddenPhotoCount: control.model.length - control.maxVisiblePhotos
        visible: control.maxVisiblePhotos >= 0 && control.model.length > control.maxVisiblePhotos
        source: visible ? model[control.maxVisiblePhotos] : ""
        width: visible ? rowView.height + rowView.spacing: 0
        space: visible ? rowView.spacing : 0
        onClicked: control.showAll()
      }
    }

    Row {
      id: msgRow

      spacing: 4 * __dp

      MMIcon {
        id: msgIcon

        source: visible ? __style.errorIcon : ""
        color: errorMsg.length > 0 ? __style.negativeColor : __style.warningColor
        visible: errorMsg.length > 0 || warningMsg.length > 0
      }

      Text {
        text: errorMsg.length > 0 ? errorMsg : warningMsg
        font: __style.t4
        wrapMode: Text.WordWrap
        width: column.width - msgRow.spacing - msgIcon.width
        visible: errorMsg.length > 0 || warningMsg.length > 0
      }
    }
  }
}
