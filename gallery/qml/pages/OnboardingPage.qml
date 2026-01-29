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

import MMInput

import "../../app/qml/components"
import "../../app/qml/account"

Page {
  id: root

  MMStackView {
    id: stackview

    anchors.fill: parent

    initialItem: Page {
      id: pane

      Column {

        id: layout
        padding: 20
        spacing: 5

        Label {
          text: "Onboarding (Login, Sign Up, ...)"
          color: "green"
        }

        Button {
          onClicked: {
            stackview.push(loginComponent)
          }
          text: "Login"
        }

        Button {
          onClicked: {
            stackview.push(signUpComponent)
          }
          text: "Sign Up"
        }

        Button {
          onClicked: {
            stackview.push(acceptInvitationComponent)
          }
          text: "Accept Invitation"
        }

        Button {
          onClicked: {
            stackview.push(createWorkspaceComponent)
          }
          text: "Create Workspace"
        }

        Button {
          onClicked: {
            stackview.push(howYouFoundUsComponent)
          }
          text: "How You Found Us"
        }

        Button {
          onClicked: {
            stackview.push(whichIndustryComponent)
          }
          text: "Which Industry"
        }

        Label {
          text: "Account"
          color: "green"
        }

        Button {
          onClicked: {
            stackview.push(accountComponent)
          }
          text: "Account Page"
        }
      }
    }
  }

  Component {
    id: accountComponent

    MMAccountPage {
      abbrName: "PB"
      fullName: "Patrik Bell"
      userName: "Chuck Norris"
      workspaceRole: "owner"
      email: "patrik.bell@merginmaps.com"
      subscription: "Professional v2 (10GB)"
      storage: "120.6 MB / 1.8 GB"
      storageFill: 0.2
      workspaceName: "My-Workspace"
      invitationsCount: 2

      width: root.width
      height: root.height

      onBackClicked: {
        stackview.pop()
      }

      onSelectWorkspaceClicked: console.log("selectWorkspaceClicked")
      onManageAccountClicked: console.log("manageAccountClicked")
      onCloseAccountClicked: console.log("closeAccountClicked")
      onSignOutClicked: console.log("signOutClicked")
    }
  }

  Component {
    id: loginComponent

    MMLoginPage {
      id: login
      apiRoot: "app.merginmaps.com"
      warningMsg: "This is warning message like server offline"

      width: root.width
      height: root.height

      supportsSso: false

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
        stackview.pop()
      }
      onForgotPasswordClicked: console.log("Forgot password clicked")
    }
  }

  Component {
    id: signUpComponent

    MMSignUpPage {
      width: root.width
      height: root.height

      tocString: "Please read our Terms and Conditions"

      onBackClicked: stackview.pop()
    }
  }

  Component {
    id: acceptInvitationComponent

    MMAcceptInvitationPage {

      width: root.width
      height: root.height
      invitation: QtObject {
        property string workspace: "my-workspace.funny"
        property string uuid: "86c4c459-bb7b-4baa-b5d1-690fb05a9310"
      }
      haveBack: true
      showCreate: true

      onBackClicked: stackview.pop()
      onJoinWorkspaceClicked: function(workspaceUuid) { console.log("Join workspace clicked " + workspaceUuid) }
      onCreateWorkspaceClicked: console.log("Create new workspace clicked")
    }
  }

  Component {
    id: createWorkspaceComponent

    MMCreateWorkspacePage {
      width: root.width
      height: root.height

      onCreateWorkspaceClicked: function (name) {
        stackview.pop()
        console.log("Create workspace clicked " + name)
      }
    }
  }

  Component {
    id: howYouFoundUsComponent

    MMHowYouFoundUsPage {

      width: root.width
      height: root.height

      onBackClicked: stackview.pop()
      onHowYouFoundUsSelected: function(selectedText) {
        console.log("Selected how you found us: " + selectedText)
        stackview.pop()
      }
    }
  }

  Component {
    id: whichIndustryComponent

    MMWhichIndustryPage {
      id: whichIndustry

      width: root.width
      height: root.height

      onBackClicked: stackview.pop()
      onIndustrySelected: function(selectedText) {
        console.log("Selected industry: " + selectedText)
        stackview.pop()
      }
    }
  }

  MMNotificationView {}
}
