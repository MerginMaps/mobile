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
import "../../app/qml/components/private" as MMPrivateComponents
import "../../app/qml/inputs"

import mm 1.0 as MM

Page {
  id: pane

  background: Rectangle {
    color: __style.lightGreenColor
  }

  contentItem: Item {
    width: ( ApplicationWindow.window?.width ?? 0 )

    Column {
      width: parent.width * 2/3
      spacing: 20
      x: parent.width / 2 - width / 2

      MMPrivateComponents.MMBaseSingleLineInput {
        width: parent.width
        text: "Write an informative message"

        rightContent: MMIcon { source: __style.checkmarkIcon }
        onRightContentClicked: { __notificationModel.addInfo(text) }
      }

      MMPrivateComponents.MMBaseSingleLineInput {
        width: parent.width
        text: "Write a success message"

        rightContent: MMIcon { source: __style.checkmarkIcon }
        onRightContentClicked: { __notificationModel.addSuccess(text) }
      }
      MMPrivateComponents.MMBaseSingleLineInput {
        width: parent.width
        text: "Write a warning message"

        rightContent: MMIcon { source: __style.checkmarkIcon }
        onRightContentClicked: { __notificationModel.addWarning(text) }
      }
      MMPrivateComponents.MMBaseSingleLineInput {
        width: parent.width
        text: "Write an error message"

        rightContent: MMIcon { source: __style.checkmarkIcon }
        onRightContentClicked: { __notificationModel.addError(text) }
      }
      MMPrivateComponents.MMBaseSingleLineInput {
        width: parent.width
        text: "Stojí, stojí mohyla, Na mohyle zlá chvíľa, Na mohyle tŕnie chrastie A v tom tŕní, chrastí rastie, Rastie, kvety rozvíja Jedna žltá ľalia. Tá ľalia smutno vzdychá: „Hlávku moju tŕnie pichá A nožičky oheň páli – Pomôžte mi v mojom žiali!“ "

        rightContent: MMIcon { source: __style.checkmarkIcon }
        onRightContentClicked: { __notificationModel.addInfo(text) }
      }
      MMPrivateComponents.MMBaseSingleLineInput {
        width: parent.width
        text: "Click on notification to invoke action"

        rightContent: MMIcon { source: __style.checkmarkIcon }
        onRightContentClicked: { __notificationModel.addWarning(text, MM.NotificationType.ShowProjectIssuesAction) }
      }
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
