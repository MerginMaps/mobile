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
import QtQuick.Controls.Basic

import "../../app/qmlV2/component"
import "../../app/qmlV2"

Page {
  id: pane

  Column {
    width: parent.width
    spacing: 10

    MMButton {
      text: "Upload"
      onClicked: drawer1.visible = true
    }
    MMButton {
      text: "Reached Data Limit"
      onClicked: drawer2.visible = true
    }
    MMButton {
      text: "Synchronization Failed"
      onClicked: drawer3.visible = true
    }
  }

  MMDrawer {
    id: drawer1

    picture: StyleV2.uploadImage
    title: "Upload project to Margin?"
    description: "This project is currently not uploaded on Mergin. Upload it to Mergin in order to activate synchronization and collaboration."
    primaryButton: "Yes, Upload Project"
    secondaryButton: "No Cancel"

    onPrimaryButtonClicked: visible = false
    onSecondaryButtonClicked: visible = false
  }

  MMDrawer {
    id: drawer2

    picture: StyleV2.reachedDataLimitImage
    title: "You have reached a data limit"
    primaryButton: "Manage Subscription"
    specialComponent: component.comp
    visible: true

    MMComponent_reachedDataLimit {
      id: component
      dataToSync: "643.8 MB"
      dataUsing: "9.23 MB / 10.0 MB"
      plan: "Individual"
      usedData: 0.923
    }

    onPrimaryButtonClicked: visible = false
    onSecondaryButtonClicked: visible = false
  }

  MMDrawer {
    id: drawer3

    picture: StyleV2.uploadImage
    title: "Failed to synchronize your changes"
    description: "Your changes could not be sent to server, make sure you are connected to internet and have write access to this project."
    primaryButton: "Ok, I understand"
    boundedDescription: "Failed to push changes. Ask the project workspace owner to log in to their Mergin Maps dashboard for more information."

    onPrimaryButtonClicked: visible = false
    onSecondaryButtonClicked: visible = false
  }
}
