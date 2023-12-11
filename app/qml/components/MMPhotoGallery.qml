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
        width: column.width - showAllButton.width - 10 * __dp

        text: control.title
        font: __style.p6
        elide: Text.ElideRight
        color: __style.nightColor
      }

      Text {
        id: showAllButton

        anchors.right: parent.right

        text: qsTr("Show all")
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

      height: 120 * __dp
      width: parent.width
      spacing: control.maxVisiblePhotos !== 0 ? 20 * __dp : 0
      orientation: ListView.Horizontal

      model: {
        if(control.maxVisiblePhotos >= 0 && control.model.length > control.maxVisiblePhotos) {
          return control.model.slice(0, control.maxVisiblePhotos)
        }
        return control.model
      }

      delegate: MMPhoto {
        width: rowView.height

        photoUrl: model.modelData

        onClicked: function(path) { control.clicked(path) }
      }

      footer: MMMorePhoto {
        width: visible ? rowView.height + rowView.spacing: 0

        hiddenPhotoCount: control.model.length - control.maxVisiblePhotos
        visible: control.maxVisiblePhotos >= 0 && control.model.length > control.maxVisiblePhotos
        photoUrl: visible ? model[control.maxVisiblePhotos] : ""
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
        width: column.width - msgRow.spacing - msgIcon.width

        text: errorMsg.length > 0 ? errorMsg : warningMsg
        font: __style.t4
        wrapMode: Text.WordWrap
        visible: errorMsg.length > 0 || warningMsg.length > 0
      }
    }
  }
}
