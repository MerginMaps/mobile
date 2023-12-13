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
    padding: 20
    spacing: 10

    Text {
      text: "Onboarding (Login, Sign Up, ...)"
      color: "green"
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
        acceptInvitation.visible = true
      }
      text: "Accept Invitation"
    }

    Button {
      onClicked: {
        createWorkspace.visible = true
      }
      text: "Create Workspace"
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

  MMLogin {
    id: login

    anchors.fill: parent
    visible: false

    onSignInClicked: console.log("Sign in clicked")
    onSignUpClicked: console.log("Sign up clicked")
    onChangeServerClicked: console.log("Change server clicked")
    onBackClicked: visible = false
  }

  MMSignUp {
    id: signUp

    anchors.fill: parent
    visible: false

    onSignInClicked: console.log("Sign in clicked")
    onSignUpClicked: console.log("Sign up clicked")
    onBackClicked: visible = false
  }

  MMAcceptInvitation {
    id: acceptInvitation

    anchors.fill: parent
    visible: false
    user: "Lubos"
    workspace: "my-workspace.funny"

    onBackClicked: visible = false
    onContinueClicked: console.log("Join workspace clicked")
    onCreateWorkspaceClicked: console.log("Create new workspace clicked")
  }

  MMCreateWorkspace {
    id: createWorkspace

    anchors.fill: parent
    visible: false

    onContinueClicked: visible = false
  }

  MMHowYouFoundUs {
    id: howYouFoundUs

    anchors.fill: parent
    visible: false

    onBackClicked: visible = false
    onContinueClicked: function(selectedText) {
      console.log("Selected: " + selectedText)
      visible = false
    }
  }


  MMWhichIndustry {
    id: whichIndustry
    anchors.fill: parent
    visible: false

    onBackClicked: {visible=false}
  }
}
