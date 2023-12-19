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
import "../../app/qml/components"
import notificationType 1.0

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
    apiRoot: "app.merginmaps.com"
    warningMsg: "This is warning message like server offline"

    anchors.fill: parent
    visible: false

    onSignInClicked: function(username, password) {
      pending = true
      console.log("Sign in clicked: " + username + " ; " + password)
    }
    onSignUpClicked: console.log("Sign up clicked")
    onChangeServerClicked: function (newServer) {
      console.log("Change server clicked: " + newServer)
    }
    onBackClicked: {
      pending = false
      visible = false
    }
    onForgotPasswordClicked: console.log("Forgot password clicked")
  }

  MMSignUp {
    id: signUp

    anchors.fill: parent
    visible: false

    tocString: "Please read our Terms and Conditions"

    onSignInClicked: console.log("Sign in clicked")
    onSignUpClicked: function(username, email, password, passwordConfirm, tocAccept, newsletterSubscribe) {
      console.log("Sign up clicked: " + username + ";" +  email + ";" + password + ";" + passwordConfirm + ";" + tocAccept + ";" + newsletterSubscribe)
    }
    onBackClicked: visible = false

  }

  MMAcceptInvitation {
    id: acceptInvitation

    anchors.fill: parent
    visible: false
    user: "Lubos"
    workspace: "my-workspace.funny"
    workspaceUuid: "86c4c459-bb7b-4baa-b5d1-690fb05a9310"
    haveBack: true
    showCreate: true

    onBackClicked: visible = false
    onJoinWorkspaceClicked: function(workspaceUuid) { console.log("Join workspace clicked " + workspaceUuid) }
    onCreateWorkspaceClicked: console.log("Create new workspace clicked")
  }

  MMCreateWorkspace {
    id: createWorkspace

    anchors.fill: parent
    visible: false

    onCreateWorkspaceClicked: function (name) {
      visible = false
      console.log("Create workspace clicked " + name)
    }
  }

  MMHowYouFoundUs {
    id: howYouFoundUs

    anchors.fill: parent
    visible: false

    onBackClicked: visible = false
    onHowYouFoundUsSelected: function(selectedText) {
      console.log("Selected how you found us: " + selectedText)
      visible = false
    }
  }

  MMWhichIndustry {
    id: whichIndustry

    anchors.fill: parent
    visible: false

    onBackClicked: visible = false
    onIndustrySelected: function(selectedText) {
      console.log("Selected industry: " + selectedText)
      visible = false
    }
  }

  MMNotificationView {}
}
