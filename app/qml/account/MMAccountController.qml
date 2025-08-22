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

  signal projectsListRequested()

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

    if ( __merginApi.userAuth.isUsingSso() )
    {
      stackView.push( ssoPanel )
    }
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
      canSignUp: {
        if ( __merginApi.serverType === MM.MerginServerType.EE || __merginApi.serverType === MM.MerginServerType.SAAS ) {
          return __merginApi.userSelfRegistrationEnabled
        }
        return false
      }

      warningMsg: {
        if (__merginApi.apiVersionStatus === MM.MerginApiStatus.OK) {
          ""
        } else
        {
          if (__merginApi.apiVersionStatus === MM.MerginApiStatus.INCOMPATIBLE) {
            qsTr( "Please update the app to use the latest features." )
          } else if (__merginApi.apiVersionStatus === MM.MerginApiStatus.PENDING) {
            ""
          } else {
            qsTr( "Server is currently unavailable, check your connection or try again later." )
          }
        }
      }

      supportsSso: __merginApi.apiSupportsSso

      onSignInClicked: function ( login, password ) {
        stackView.pending = true
        __merginApi.authorize(login, password)
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

      onContinueWithSsoClicked: {
        stackView.push( ssoPanel )
      }
    }
  }

  Component {
    id: signUpPanel

    MMSignUpPage {

      objectName: "signUpPanel"
      tocString: __inputUtils.htmlLink(
          qsTr("I accept the %1Terms and Conditions%3 and %2Privacy Policy%3"),
          __style.forestColor,
          __inputHelp.merginTermsLink,
          __inputHelp.privacyPolicyLink
      )

      onBackClicked: {
        stackView.popOnePageOrClose()
        if ( !__merginApi.userAuth.hasAuthData() ) {
          root.resetView()
        }
      }

      onSignInClicked: {
        stackView.popOnePageOrClose()
      }

      onSignUpClicked: function ( email, password, tocAccept, newsletterSubscribe ) {
        if ( __merginApi.serverType !== MM.MerginServerType.SAAS ) {
          return; //should not happen
        }
        else {
          stackView.pending = true
          __merginApi.registerUser( email,
                                   password,
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
    id: ssoPanel

    MMSsoPage {

      objectName: "ssoPanel"

      onBackClicked: {
        __merginApi.abortSsoFlow()
        loadingDialog.close()
        stackView.popOnePageOrClose()
      }

      onLoginWithPasswordClicked: {
        __merginApi.abortSsoFlow()
        loadingDialog.close()
        stackView.popOnePageOrClose()
      }

      onSignInClicked: function( email ) {
        loadingDialog.open()
        __merginApi.requestSsoConnections(email)
      }

      Connections {
        target: __merginApi
        enabled: stackView.currentItem.objectName === "ssoPanel"

        function onSsoConfigIsMultiTenant() {
          loadingDialog.close()
        }

        function onUserInfoReplyFinished() {
          loadingDialog.close()
          if ( __merginApi.userInfo.hasInvitations ) {
            controller.invitation = __merginApi.userInfo.invitations()[0]
            stackView.push( acceptInvitationsPanelComponent )
          } else {
            controller.end()
          }
        }

        function onNotifyError() {
          loadingDialog.close()
          focusOnBrowser = false
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
          stackView.push( howYouFoundUsComponent )
        }
      }
    }
  }

  Component {
    id: acceptInvitationsPanelComponent

    MMAcceptInvitationPage {
      objectName: "acceptInvitationsPanel"

      invitation: controller.invitation
      showCreate: !__merginApi.userAuth.isUsingSso()

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
          controller.projectsListRequested()
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
