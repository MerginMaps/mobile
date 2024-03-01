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

/**
 * Show accept invitation page directly without onboarding
 * e.g. on app start
 */
Item {
  id: controller

  required property bool enabled
  required property var stackView

  property MerginInvitation invitation

  Connections {
    id: openInvitationsListener

    target: __merginApi
    enabled: controller.enabled && !stackView.containsPage("signUpPanel")

    function onUserInfoReplyFinished() {
      // let's not show invitations when registration finish page is opened
      // this should be redundant - never reached
      if ( stackView.containsPage("signUpPanel") ) {
        return;
      }

      if ( !__merginApi.userAuth.hasAuthData() ) {
        return;
      }

      if ( __merginApi.userInfo.hasInvitations ) {
        controller.invitation = __merginApi.userInfo.invitations()[0]
        __notificationModel.addWarning( qsTr( "You have pending workspace invitations! You may accept or reject them in your workspace selection page" ) )
      }
    }
  }
}
