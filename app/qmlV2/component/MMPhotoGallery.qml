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
import "../Style.js" as Style
import "."

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
        font: Qt.font(Style.p6)
        wrapMode: Text.WordWrap
        width: column.width - showAllButton.width - 10 * __dp
        color: Style.night
      }

      Text {
        id: showAllButton

        text: qsTr("Show all")
        anchors.right: parent.right
        font: Qt.font(Style.t4)
        wrapMode: Text.WordWrap
        color: Style.forest

        MouseArea {
          anchors.fill: parent
          onClicked: control.showAll()
        }
      }
    }

    ScrollView {
      width: parent.width
      height: 120 * __dp

      ScrollBar.horizontal: ScrollBar {
        policy: ScrollBar.AlwaysOff
      }

      ListView {
        id: rowView

        spacing: 20 * __dp
        orientation: ListView.Horizontal
        height: 120 * __dp

        delegate: MMPhoto {
          visible: model.index < control.maxVisiblePhotos
          hiddenPhotoCount: (rowView.count > control.maxVisiblePhotos && model.index === control.maxVisiblePhotos - 1) ? rowView.count - control.maxVisiblePhotos + 1 : 0
          onClicked: control.clicked(path)
          onShowWholeGallery: control.showAll()
        }
      }
    }

    Row {
      id: msgRow

      spacing: 4 * __dp

      MMIcon {
        id: msgIcon

        source: visible ? Style.errorIcon : ""
        color: errorMsg.length > 0 ? Style.negative : Style.warning
        visible: errorMsg.length > 0 || warningMsg.length > 0
      }
      Text {
        text: errorMsg.length > 0 ? errorMsg : warningMsg
        font: Qt.font(Style.t4)
        wrapMode: Text.WordWrap
        width: column.width - msgRow.spacing - msgIcon.width
        visible: errorMsg.length > 0 || warningMsg.length > 0
      }
    }
  }
}
