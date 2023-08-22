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

import "../../app/qmlV2/component"

Page {
  id: pane

  MMNotificationView {

  }

  Column {
    width: parent.width
    spacing: 20
    anchors.centerIn: parent

    MMInput {
      anchors.horizontalCenter: parent.horizontalCenter
      placeholderText: "Write an informative message"
      onEnterPressed: _notificationModel.add(text, 10, 0)
    }
    MMInput {
      anchors.horizontalCenter: parent.horizontalCenter
      placeholderText: "Write a warning message"
      onEnterPressed: _notificationModel.add(text, 10, 1)
    }
    MMInput {
      anchors.horizontalCenter: parent.horizontalCenter
      placeholderText: "Write an error message"
      onEnterPressed: _notificationModel.add(text, 10, 2)
    }
  }
}
