/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../components"

MMDrawerDialog {
  id: root

  property string featureTitle: ""
  property string layerName: ""

  signal resumeClicked()
  signal discardClicked()

  imageSource: __style.neutralMMSymbolImage
  title: featureTitle !== ""
         ? qsTr( "You have unsaved changes on feature %1" ).arg( featureTitle )
         : qsTr( "You have unsaved changes on a new feature" )
  description: qsTr( "The app closed before saving your changes on %1, click resume to start editing them again. If not, click discard." ).arg( layerName )

  primaryButton.text: qsTr( "Resume editing" )
  primaryButton.bgndColor: __style.warningColor
  primaryButton.bgndColorHover: __style.warningColor
  primaryButton.fontColor: __style.earthColor
  primaryButton.fontColorHover: __style.earthColor

  secondaryButton.text: qsTr( "Discard unsaved changes" )
  secondaryButton.bgndColor: "transparent"
  secondaryButton.bgndColorHover: "transparent"
  secondaryButton.fontColor: __style.earthColor
  secondaryButton.fontColorHover: __style.earthColor

  onPrimaryButtonClicked: {
    root.resumeClicked()
    close()
  }

  onSecondaryButtonClicked: {
    root.discardClicked()
    close()
  }
}
