

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import "../components"

Page {
  id: root

  signal backClicked
  signal continueClicked
  signal createWorkspaceClicked

  Column {
    spacing: 20

    MMHeader {
      headerTitle: qsTr("Accept Invitation")

      onBackClicked: root.backClicked()
    }

   Item {
     width: bg.width
     height: bg.height

     MMIcon {
       id: bg
       source: __style.acceptInvitationImage
     }

     MMIcon {
        id: fg
        x: ( bg.width - fg.width ) / 2 + 7
        y: ( bg.height - fg.height ) / 2 + 5
        source: __style.acceptInvitationLogoImage
     }
   }

    Label {
        text: qsTr("You have been invited to workspace")
        font: __style.t1
        color: __style.forestColor
    }

    Label {
        text: qsTr("User %1 has invited you to join his workspace").arg("andrej")
        font: __style.p2
    }

    Label {
        text: "my-workspace-funny"
        font: __style.p2
    }

    MMButton {
      onClicked: root.continueClicked()
      text: qsTr("Join workspace")
    }

    MMHlineText {
      title: qsTr("or")
    }

    MMLinkButton {
      onClicked: root.createWorkspaceClicked()
      text: qsTr("Create new workspace")
    }
  }
}
