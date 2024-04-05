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

  property int maxProjectNumber: 2
  required property bool apiSupportsSubscription

  signal manageAccountClicked()

  imageSource: __style.reachedDataLimitImage

  title: qsTr( "You have reached a maximum number of projects" )

  primaryButton.text: apiSupportsSubscription ? qsTr( "Manage account" ) : ""

  additionalContent: Column {
    width: parent.width
    spacing: 10 * __dp
    topPadding: 20 * __dp

    Item {
      width: parent.width
      height: dataToSyncText.height

      MMComponents.MMText {
        width: parent.width

        text: qsTr( "Maximum number of projects" )
        font: __style.p5

        horizontalAlignment: Text.AlignLeft
        verticalAlignment: Text.AlignVCenter
      }

      MMComponents.MMText {
        id: dataToSyncText

        width: parent.width

        text: root.maxProjectNumber
        color: __style.nightColor
        font: __style.t3

        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
      }
    }
  }

  onPrimaryButtonClicked: {
    root.manageAccountClicked()
    close()
  }
}
