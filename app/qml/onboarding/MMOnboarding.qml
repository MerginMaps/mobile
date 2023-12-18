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

Item {
  id: root

  signal backClicked
  signal closeOnboarding
  signal signInRequested
  signal signUpRequested
  signal createWorkspaceRequested
  signal submitWorkspaceInfoRequested

  /*
  Component {
      id: onboarding
      MMOnboarding {
          anchors.fill: parent

          onCloseOnboarding: {
              loader.active = false
          }
      }
  }

  Loader {
      id: loader
      sourceComponent: onboarding
      anchors.fill: parent
      active: false
  }
  */

  Component {
    id: mmsignup
    MMSignUp {
      onBackClicked: {
        stackView.pop()
      }

      onSignInClicked: {
        stackView.pop()
      }

      onSignUpClicked: {
        // TODO depends on invitations either create or accept invitation page
        stackView.push(mmcreateworkspace)
        // stackView.push(mmacceptinvitation)
      }
    }
  }

  Component {
    id: mmlogin
    MMLogin {
      onBackClicked: {
        root.closeOnboarding()
      }

      onSignInClicked: {
        root.signInRequested()
      }

      onSignUpClicked: {
        stackView.push(mmsignup, {})
      }

      onChangeServerClicked: {

      }
    }
  }

  Component {
    id: mmacceptinvitation
    MMAcceptInvitation {
      onBackClicked: {
        stackView.pop()
      }

      onCreateWorkspaceClicked: {
        stackView.push(mmcreateworkspace, {})
      }

      onContinueClicked: {
        root.closeOnboarding()
      }
    }
  }

  Component {
    id: mmcreateworkspace
    MMCreateWorkspace {

      // no back button
      onContinueClicked: {
        stackView.push(mmhowyoufoundus, {})
      }
    }
  }

  Component {
    id: mmhowyoufoundus
    MMHowYouFoundUs {
      onBackClicked: {
        stackView.pop()
      }

      onContinueClicked: {
        stackView.push(mmwhichindustry, {})
      }
    }
  }

  Component {
    id: mmwhichindustry
    MMWhichIndustry {
      onBackClicked: {
        stackView.pop()
      }

      onContinueClicked: {
        root.submitWorkspaceInfoRequested()
      }
    }
  }

  StackView {
    id: stackView
    anchors.fill: parent
    initialItem: mmlogin
  }
}
