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

import mm 1.0 as MM

Item {
  id: controller

  required property bool enabled
  required property var stackView

  property bool inProgress: false
  property MM.MerginInvitation invitation

  QtObject {
    //! Data to send to postRegister endpoint
    id: postRegisterData

    property bool wantNewsletter: false
    property string howYouFoundUs: "" // one of the category of "other"
    property string whichIndustry: ""  // one of the category of "other"
  }

  // Start onboarding
  function start()
  {
    if (controller.inProgress)
      return;

    controller.inProgress = true;
    stackView.push( loginPageComp, {}, StackView.PushTransition )
  }

  // Finish onboarding
  function end()
  {
    controller.inProgress = false;
    stackView.pop(null);
  }

  Connections {
    target: __merginApi
    enabled: controller.inProgress

    function onRegistrationFailed( msg, field ) {
      stackView.pending = false
      if ( stackView.currentItem.objectName === "signUpPanel" ) {
        stackView.currentItem.showErrorMessage(msg, field)
      }
    }
  }

  Component {
    id: loginPageComp

    MMLoginPage {
      id: loginPage

      objectName: "loginPage"
      visible: false
      apiRoot: __merginApi.apiRoot
      pending: stackView.pending
      height: root.height
      width: root.width
      canSignUp:  (__merginApi.serverType === MM.MerginServerType.EE ) || ( __merginApi.serverType === MM.MerginServerType.SAAS )
      warningMsg: {
        if (__merginApi.apiVersionStatus === MM.MerginApiStatus.OK) {
          ""
        } else
        {
          if (__merginApi.apiVersionStatus === MM.MerginApiStatus.INCOMPATIBLE) {
            qsTr("Please update the app to use the latest features.")
          } else if (__merginApi.apiVersionStatus === MM.MerginApiStatus.PENDING) {
            ""
          } else {
            qsTr("Server is currently unavailable - please try again later.")
          }
        }
      }

      onSignInClicked: function ( username, password ) {
        stackView.pending = true
        __merginApi.authorize(username, password)
      }

      onBackClicked: {
        stackView.popOnePageOrClose()
        if ( !__merginApi.userAuth.hasAuthData() ) {
          root.resetView()
        }
        controller.end()
      }

      onSignUpClicked: {
        if (!canSignUp) // should not happen
          return;

        if ( __merginApi.serverType === MM.MerginServerType.EE ) {
          Qt.openUrlExternally( __merginApi.apiRoot )
        }
        else {
          stackView.push( signUpPanel )
        }
      }

      onChangeServerClicked: function ( newServer ) {
        __merginApi.apiRoot = newServer
      }

      onForgotPasswordClicked: {
        Qt.openUrlExternally(__merginApi.resetPasswordUrl());
      }

    }
  }

  Component {
    id: signUpPanel

    MMSignUpPage {

      objectName: "signUpPanel"
      tocString: qsTr("I accept the Mergin Maps %1Terms and Conditions%3 and %2Privacy Policy%3")
      .arg("<a href='"+ __inputHelp.merginTermsLink + "' style='color:" + __style.forestColor + "; font-weight: bold;'>")
      .arg("<a href='"+ __inputHelp.privacyPolicyLink +"' style='color:" + __style.forestColor + "; font-weight: bold;'>")
      .arg("</a>")

      onBackClicked: {
        stackView.popOnePageOrClose()
        if ( !__merginApi.userAuth.hasAuthData() ) {
          root.resetView()
        }
      }

      onSignInClicked: {
        stackView.popOnePageOrClose()
      }

      onSignUpClicked: function ( username, email, password, passwordConfirm, tocAccept, newsletterSubscribe ) {
        if ( __merginApi.serverType !== MM.MerginServerType.SAAS ) {
          return; //should not happen
        }
        else {
          stackView.pending = true
          __merginApi.registerUser( username,
                                   email,
                                   password,
                                   passwordConfirm,
                                   tocAccept )

          postRegisterData.wantNewsletter = newsletterSubscribe
        }
      }

      Connections {
        target: __merginApi
        enabled: stackView.currentItem.objectName === "signUpPanel"

        function onRegistrationSucceeded() {
          // Nothing to do yet, user info data are not yet updated
          // we need to act after userInfoReplyFinished
          stackView.pending = true
        }

        function onUserInfoReplyFinished() {
          if ( controller.inProgress )
          {
            stackView.pending = false
            if ( __merginApi.userInfo.hasInvitations ) {
              controller.invitation = __merginApi.userInfo.invitations()[0]
              stackView.push( acceptInvitationsPanelComponent )
            } else {
              stackView.push( createWorkspaceComponent )
            }
          }
        }
      }
    }
  }

  Component {
    id: createWorkspaceComponent

    MMCreateWorkspacePage {
      id: createWorkspacePanel

      objectName: "createWorkspacePanel"

      pageHeader.backVisible: false
      onCreateWorkspaceClicked: function (workspaceName) {
        __merginApi.createWorkspace(workspaceName)
      }

      Connections {
        target: __merginApi
        enabled: stackView.currentItem.objectName === "createWorkspacePanel"

        function onWorkspaceCreated(workspace) {
          stackView.push(howYouFoundUsComponent)
        }
      }
    }
  }

  Component {
    id: acceptInvitationsPanelComponent

    MMAcceptInvitationPage {
      objectName: "acceptInvitationsPanel"
      haveBack: false
      showCreate: true
      invitation: controller.invitation

      onJoinWorkspaceClicked: function (workspaceUuid) {
        __merginApi.processInvitation( workspaceUuid, true )
      }

      onCreateWorkspaceClicked: {
        if (stackView.containsPage("createWorkspacePanel"))
        {
          stackView.popOnePageOrClose()
        } else {
          stackView.push(createWorkspaceComponent)
        }
      }

      Connections {
        target: __merginApi

        function onProcessInvitationFinished( accepted ) {
          controller.end()
        }
      }
    }
  }

  Component {
    id: howYouFoundUsComponent

    MMHowYouFoundUsPage {
      id: howYouFoundUsPanel

      objectName: "howYouFoundUsPanel"

      onHowYouFoundUsSelected: function (selectedText) {
        postRegisterData.howYouFoundUs = selectedText
        stackView.push(whichIndustryComponent)
      }
    }
  }

  Component {
    id: whichIndustryComponent

    MMWhichIndustryPage {
      id: whichIndustryPanel

      objectName: "whichIndustryPanel"

      onBackClicked: {
        stackView.popOnePageOrClose()
      }

      onIndustrySelected: function (selectedText) {
        postRegisterData.whichIndustry = selectedText
        __merginApi.postRegisterUser( postRegisterData.howYouFoundUs, postRegisterData.whichIndustry, postRegisterData.wantNewsletter )
      }

      Connections {
        target: __merginApi

        function onPostRegistrationSucceeded() {
          controller.end()
        }

        function onPostRegistrationFailed() {
          controller.end()
        }
      }
    }
  }
}
