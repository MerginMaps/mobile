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

/**
 * Show accept invitation page directly without onboarding
 * e.g. on app start
 */
Item {
  id: controller

  required property bool enabled
  required property var stackView

  property bool showInvitationList: false // TODO merge with enabled ?

  Connections {
    id: openInvitationsListener

    target: __merginApi
    enabled: root.enabled && root.showInvitationList

    function onUserInfoReplyFinished() {
     controller.showInvitationsList = false;

      /*
        TODO - interactions with onboarding!
        it should be enabled = false in this case from parent!

      // let's not show invitations when registration finish page is opened
      if ( stackView.containsPage("registrationFinishPanel") ) {
        return;
      }
      */

      if ( !__merginApi.userAuth.hasAuthData() ) {
        return;
      }

      if ( __merginApi.userInfo.hasInvitations ) {
        stackView.push( acceptInvitationsPanelComponent )
      }
    }
  }

  Connections {
    target: __merginApi
    enabled: root.enabled && root.showInvitationList

    function onProcessInvitationFinished( accepted ) {
      stackView.pop(null)
    }
  }

  Component {
    id: acceptInvitationsPanelComponent

    MMAcceptInvitation {
      objectName: "acceptInvitationsPanelDirect"
      haveBack: true
      showCreate: false

      onBackClicked: {
        stackView.popOnePageOrClose()
      }

      onJoinWorkspaceClicked: function (workspaceUuid) {
        __merginApi.processInvitation( workspaceUuid, true )
      }
    }
  }
}
