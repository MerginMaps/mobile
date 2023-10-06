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
import "../../app/qmlV2/Style.js" as Style

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

    picture: Style.uploadImage
    title: "Upload project to Margin?"
    description: "This project is currently not uploaded on Mergin. Upload it to Mergin in order to activate synchronization and collaboration."
    primaryButton: "Yes, Upload Project"
    secondaryButton: "No Cancel"
  }

  MMDrawer {
    id: drawer2

    picture: Style.ReachedDataLimitImage
    title: "You have reached a data limit"
    primaryButton: "Manage Subscription"
    specialComponent: MMButton { text: "Special Component"; padding: 20 }
  }

  MMDrawer {
    id: drawer3

    picture: Style.uploadImage
    title: "Failed to synchronize your changes"
    description: "Your changes could not be sent to server, make sure you are connected to internet and have write access to this project."
    primaryButton: "Ok, I understand"
    boundedDescription: "Failed to push changes. Ask the project workspace owner to log in to their Mergin Maps dashboard for more information."
    visible: true
  }
}
