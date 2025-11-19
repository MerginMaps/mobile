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
import "./components" as MMDialogComponents

/**
  * Dialog bearing information about synchronization failure.
  * By default it has some text, but it is also possible to set "detailedText"
  * "detailedText" will be visible either under "details" button or appended to
  * the default text (based on platform).
  */

MMComponents.MMDrawerDialog {
  id: root

  property string detailedText
  property bool hasDetails: root.detailedText?.trim() ?? false

  title: qsTr( "Failed to synchronise your changes" )
  imageSource: __style.syncFailedImage

  description: qsTr( "Your changes could not be sent to the server, make sure you have a data connection and have permission to edit this project." )
  primaryButton.text: qsTr( "Ok, I understand" )

  additionalContent: Loader {
    width: parent.width
    active: root.hasDetails //status we receive if there is no data connection, either True or False
    sourceComponent: MMDialogComponents.MMDialogAdditionalText {
      width: parent.width
      text: qsTr("Details") + ": " + root.detailedText
    }
  }

  onPrimaryButtonClicked: {
    close()
  }
}
