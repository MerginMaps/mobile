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

import "../components"
import "../inputs"

MMDrawer {
  id: root

  property string username

  signal closeAccountClicked()

  drawerContent: MMScrollView {

    width: parent.width
    height: root.maxHeightHit ? root.drawerContentAvailableHeight : contentHeight

    Column {

      width: parent.width

      spacing: __style.spacing20

      MMMessage {
        width: parent.width

        image: __style.closeAccountImage

        title: qsTr("Do you really wish to close your account?")
        description: qsTr("This action will delete your Mergin Maps account. If you are a workspace owner, you need to transfer the ownership to somebody else or close the workspace.")
      }

      MMText {
        width: parent.width
        wrapMode: Text.Wrap

        text: root.username

        font: __style.t1
        color: __style.nightColor

        horizontalAlignment: Text.AlignHCenter
      }

      MMTextInput {
        id: usernameInput

        width: parent.width
        textFieldBackground.color: __style.lightGreenColor

        title: qsTr("Username")
        placeholderText: qsTr("Enter your username")
      }

      MMInfoBox {
        width: parent.width

        title: qsTr("Please note")
        description: qsTr("Type in your username and click ‘Yes’ to delete your account.")

        color: __style.lightGreenColor
        imageSource: __style.attentionImage
      }

      MMListSpacer { height: 1 }

      Row {
        width: parent.width
        spacing: __style.margin16

        MMButton {
          width: ( parent.width - parent.spacing ) / 2
          type: MMButton.Types.Secondary

          text: qsTr( "No" )

          onClicked: close()
        }

        MMButton {
          width: ( parent.width - parent.spacing ) / 2

          text: qsTr( "Yes" )

          fontColor: __style.grapeColor
          bgndColor: __style.negativeColor
          fontColorHover: __style.negativeColor
          bgndColorHover: __style.grapeColor
          fontColorDisabled: __style.grapeColor // TODO: replace with new color
          bgndColorDisabled: __style.negativeColor // TODO: replace with new color

          onClicked: {
            if ( usernameInput.text === root.username ) {
              close()
              usernameInput.text = ""
              usernameInput.errorMsg = ""
              root.closeAccountClicked()
            }
            else {
              usernameInput.errorMsg = qsTr("Usernames do not match.")
            }
          }
        }
      }
    }
  }
}
