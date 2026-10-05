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

  interactive: true

  imageSource: __style.positiveMMSymbolImage
  title: qsTr( "Help us improve Mergin Maps" )
  description: qsTr( "Share anonymous usage data so we can focus on the features that matter most to you. No personal or project data is ever collected." )
  primaryButton.text: qsTr( "Yes, count me in" )
  secondaryButton.text: qsTr( "No, thanks" )

  additionalContent: MMComponents.MMText {
    width: parent.width
    text: qsTr( "You can change this anytime in Settings." )
    font: __style.p6
    color: __style.mediumGreyColor
    horizontalAlignment: Text.AlignHCenter
  }

  onPrimaryButtonClicked: {
    AppSettings.usageReportEnabled = true
    AppSettings.usageReportConsentAsked = true
    close()
  }

  onSecondaryButtonClicked: {
    AppSettings.usageReportConsentAsked = true
    close()
  }

  onClosed: {
    if ( !AppSettings.usageReportConsentAsked )
      AppSettings.usageReportConsentAsked = true
  }
}
