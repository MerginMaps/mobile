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

import "../../app/qml/components"
import "../../app/qml/inputs"
import mm 1.0 as MM

Page {
  id: pane

  Rectangle {
    anchors.fill: parent
    color: "white"
  }

  Column {
    width: parent.width - 60
    spacing: 20
    anchors.centerIn: parent

    MMTextWithButtonInput {
      buttonText: "Send"
      anchors.horizontalCenter: parent.horizontalCenter
      text: "Write an informative message"

      onButtonClicked: { __notificationModel.addInfo(text) }
    }
    MMTextWithButtonInput {
      buttonText: "Send"
      anchors.horizontalCenter: parent.horizontalCenter
      text: "Write a success message"

      onButtonClicked: { __notificationModel.addSuccess(text) }
    }
    MMTextWithButtonInput {
      buttonText: "Send"
      anchors.horizontalCenter: parent.horizontalCenter
      text: "Write a warning message"

      onButtonClicked: { __notificationModel.addWarning(text) }
    }
    MMTextWithButtonInput {
      buttonText: "Send"
      anchors.horizontalCenter: parent.horizontalCenter
      text: "Write an error message"

      onButtonClicked: { __notificationModel.addError(text) }
    }
    MMTextWithButtonInput {
      buttonText: "Send"
      anchors.horizontalCenter: parent.horizontalCenter
      text: "Stojí, stojí mohyla, Na mohyle zlá chvíľa, Na mohyle tŕnie chrastie A v tom tŕní, chrastí rastie, Rastie, kvety rozvíja Jedna žltá ľalia. Tá ľalia smutno vzdychá: „Hlávku moju tŕnie pichá A nožičky oheň páli – Pomôžte mi v mojom žiali!“ "

      onButtonClicked: { __notificationModel.addInfo(text) }
    }
    MMTextWithButtonInput {
      buttonText: "Send"
      anchors.horizontalCenter: parent.horizontalCenter
      text: "Click on notification to invoke action"

      onButtonClicked: { __notificationModel.addWarning(text, MM.NotificationType.ShowProjectIssuesAction) }
    }
  }

  Connections {
    target: __notificationModel
    function onShowProjectIssuesActionClicked() {
      console.log("showProjectIssuesActionClicked")
    }
  }

  MMNotificationView {}
}
