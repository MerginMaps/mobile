/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick

import "../"
import QtQuick.Controls
import lc 1.0

Item {
  id: controller

  required property bool enabled
  required property var stackView

  property bool inProgress: false

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
    enabled: controller.enabled

    function onRegistrationFailed( msg, field ) {
      stackView.pending = false
      if ( stackView.currentItem.objectName === "signUpPanel" ) {
        stackView.currentItem.showErrorMessage(msg, field)
      }
    }

    function onRegistrationSucceeded() {
      stackView.pending = false
      stackView.push(createWorkspaceComponent)
    }

    function onWorkspaceCreated(workspace, result) {
      if (result) {
        stackView.push(howYouFoundUsComponent)
      }
    }
  }

  Component {
    id: loginPageComp

    MMLogin {
      id: loginPage

      objectName: "loginPage"
      visible: false
      apiRoot: __merginApi.apiRoot
      pending: stackView.pending
      height: root.height
      width: root.width
      canSignUp:  (__merginApi.serverType === MerginServerType.EE ) || ( __merginApi.serverType === MerginServerType.SAAS )
      warningMsg: {
        if (__merginApi.apiVersionStatus === MerginApiStatus.OK) {
          ""
        } else
        {
          if (__merginApi.apiVersionStatus === MerginApiStatus.INCOMPATIBLE) {
            qsTr("Please update the app to use the latest features.")
          } else if (__merginApi.apiVersionStatus === MerginApiStatus.PENDING) {
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
      }

      onSignUpClicked: {
        if (!canSignUp) // should not happen
          return;

        if ( __merginApi.serverType === MerginServerType.EE ) {
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

    MMSignUp {

      objectName: "signUpPanel"
      tocString: qsTr("I accept the Mergin %1Terms and Conditions%3 and %2Privacy Policy%3")
      .arg("<a href='"+ __inputHelp.merginTermsLink + "'>")
      .arg("<a href='"+ __inputHelp.privacyPolicyLink +"'>")
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
        if ( __merginApi.serverType !== MerginServerType.SAAS ) {
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
    }
  }

  Component {
    id: createWorkspaceComponent

    MMCreateWorkspace {
      id: createWorkspacePanel

      objectName: "createWorkspacePanel"
      onCreateWorkspaceClicked: function (workspaceName) {
        __merginApi.createWorkspace(workspaceName)
      }
    }
  }

  Component {
    // TODO -- open and test!
    id: acceptInvitationsPanelComponent

    MMAcceptInvitation {
      objectName: "acceptInvitationsPanel"
      haveBack: false
      showCreate: true

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

    MMHowYouFoundUs {
      id: howYouFoundUsPanel

      objectName: "howYouFoundUsPanel"
      onBackClicked: {
        stackView.popOnePageOrClose()
      }

      onHowYouFoundUsSelected: function (selectedText) {
        postRegisterData.howYouFoundUs = selectedText
        stackView.push(whichIndustryComponent)
      }
    }
  }

  Component {
    id: whichIndustryComponent

    MMWhichIndustry {
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
