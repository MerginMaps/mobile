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

/**
 * Show accept invitation page directly without onboarding
 * e.g. on app start
 */
Item {
  id: root

  property bool enabled: true
  property MM.MerginInvitation invitation

  Connections {
    target: __merginApi
    enabled: root.enabled

    function onUserInfoReplyFinished() {
      if ( !__merginApi.userAuth.hasAuthData() ) {
        return;
      }

      if ( __merginApi.userInfo.hasInvitations ) {
        __notificationModel.addWarning(
              __inputUtils.htmlLink(qsTr( "You have pending workspace invitations.<br>Please %1accept or reject%2 them." ), __style.grapeColor),
              MM.NotificationType.ShowSwitchWorkspaceAction
        )
      }
    }
  }
}
