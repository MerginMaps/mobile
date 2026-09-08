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

  property string layerName: ""

  signal discardDraft()

  imageSource: __style.negativeMMSymbolImage
  title: qsTr( "Discard unsaved changes?" )
  description: qsTr( "Tapping on 'Discard' deletes your unsaved changes on %1. This cannot be undone." ).arg( layerName )

  primaryButton.text: qsTr( "Discard changes" )
  primaryButton.bgndColor: __style.negativeColor
  primaryButton.bgndColorHover: __style.negativeColor
  primaryButton.fontColor: __style.grapeColor
  primaryButton.fontColorHover: __style.grapeColor

  secondaryButton.text: qsTr( "Do not discard" )

  onPrimaryButtonClicked: {
    root.discardDraft()
    close()
  }

  onSecondaryButtonClicked: {
    close()
  }
}
