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

  // set when features linked by a composition relation get deleted too
  property bool deletesLinkedFeatures: false

  signal deleteFeature()

  drawerBottomMargin: __style.margin20 + __style.safeAreaBottom

  imageSource: __style.negativeMMSymbolImage
  title: qsTr( "Delete feature" )
  description: deletesLinkedFeatures
               ? qsTr( "Are you sure you want to delete this feature? The linked features you added will also be deleted." )
               : qsTr( "Are you sure you want to delete this feature?" )
  primaryButton.text: qsTr( "Yes, I want to delete" )
  secondaryButton.text: qsTr( "No, thanks" )

  onPrimaryButtonClicked: {
    root.deleteFeature()
    close()
  }

  onSecondaryButtonClicked: {
    close()
  }
}
