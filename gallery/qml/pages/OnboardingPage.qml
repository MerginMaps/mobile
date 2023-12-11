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
import QtQuick.Layouts

import "../../app/qml/onboarding"

Page {
  id: pane

  function hideAll() {
      acceptInvitation.visible = false
      createWorkspace = false
      howYouFoundUs = false
      login = false
      signUp = false
      whichIndustry = false
  }

  Column {
    id: layout

    Label {
      text: "Onboarding (Login, Sign Up, ...)"
    }

    Button {
      onClicked: {
         login.visible = true
      }
      text: "Login"
    }

    Button {
      onClicked: {
         signUp.visible = true
      }
      text: "Sign Up"
    }

    Button {
      onClicked: {
         createWorkspace.visible = true
      }
      text: "Create Workspace"
    }

    Button {
      onClicked: {
         acceptInvitation.visible = true
      }
      text: "Accept Invitation"
    }

    Button {
      onClicked: {
         howYouFoundUs.visible = true
      }
      text: "How You Found Us"
    }

    Button {
      onClicked: {
         whichIndustry.visible = true
      }
      text: "Which Industry"
    }
  }

  MMAcceptInvitation {
      id: acceptInvitation
      anchors.fill: parent
      visible: false

      onBackClicked: {visible=false}
  }

  MMCreateWorkspace {
      id: createWorkspace
      anchors.fill: parent
      visible: false

      onContinueClicked: {visible=false}
  }

  MMHowYouFoundUs {
      id: howYouFoundUs
      anchors.fill: parent
      visible: false

      onBackClicked: {visible=false}
  }

  MMLogin {
      id: login
      anchors.fill: parent
      visible: false

      onBackClicked: {visible=false}
  }

  MMSignUp {
      id: signUp
      anchors.fill: parent
      visible: false

      onBackClicked: {visible=false}
  }

  MMWhichIndustry {
      id: whichIndustry
      anchors.fill: parent
      visible: false

      onBackClicked: {visible=false}
  }
}
