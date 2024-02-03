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

import "../../components"

Item {
  id: root

  width: parent.width
  height: column.height

  required property var model
  property string title
  property string warningMsg
  property string errorMsg
  property int maxVisiblePhotos: -1 // -1 for showing all photos
  property bool showAddImage: false

  signal showAll()
  signal clicked( var path )
  signal addImage()

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

        text: root.title
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
          onClicked: root.showAll()
        }
      }
    }

    ListView {
      id: rowView

      height: 120 * __dp
      width: parent.width
      spacing: root.maxVisiblePhotos !== 0 ? 20 * __dp : 0
      orientation: ListView.Horizontal

      model: {
        if(root.maxVisiblePhotos >= 0 && root.model.length > root.maxVisiblePhotos) {
          return root.model.slice(0, root.maxVisiblePhotos)
        }
        return root.model
      }

      delegate: MMPhoto {
        width: rowView.height

        photoUrl: model.modelData

        onClicked: function(path) { root.clicked(path) }
      }

      header: Row {
        visible: root.showAddImage

        Rectangle {
          width: visible ? height : 0
          height: rowView.height
          radius: 20 * __dp
          border.width: 2 * __dp
          border.color: errorMsg.length > 0 ? __style.negativeColor : warningMsg.length > 0 ? __style.warningColor : __style.whiteColor
          color: (errorMsg.length > 0 || warningMsg.length > 0) ? __style.errorBgInputColor : __style.whiteColor

          MMIcon {
            anchors.centerIn: parent
            source: __style.addImageIcon
            color: errorMsg.length > 0 ? __style.grapeColor : warningMsg.length > 0 ? __style.earthColor : __style.forestColor
          }

          MouseArea {
            anchors.fill: parent
            onClicked: root.addImage()
          }
        }

        Item {
          width: visible ? rowView.spacing : 0
          height: rowView.height
        }
      }

      footer: MMMorePhoto {
        width: visible ? rowView.height + rowView.spacing: 0

        hiddenPhotoCount: root.model.length - root.maxVisiblePhotos
        visible: root.maxVisiblePhotos >= 0 && root.model.length > root.maxVisiblePhotos
        photoUrl: visible ? model[root.maxVisiblePhotos] : ""
        space: visible ? rowView.spacing : 0

        onClicked: root.showAll()
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
