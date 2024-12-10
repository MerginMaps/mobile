/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../components" as MMComponents


MMComponents.MMDrawerDialog {
  id: root

  property int contributorsQuota

  signal manageAccountClicked()

  title: qsTr( "You've reached the maximum number of active monthly contributors (%1) for your current subscription." ).arg( contributorsQuota )
  imageSource: __style.reachedMonthlyContributorLimitImage

  description: qsTr( "Upgrade your subscription or wait until next month for the limit to reset." )

  primaryButton.text: qsTr( "Upgrade" )
  secondaryButton.text: qsTr( "Cancel" )

  onPrimaryButtonClicked: {
    root.manageAccountClicked()
    close()
  }

  onSecondaryButtonClicked: {
    close()
  }
}
