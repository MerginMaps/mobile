

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
import QtQuick
import QtQuick.Layouts
import QtQuick.Controls

import "../components"
import "../inputs"

Page {
  id: root

  signal backClicked
  signal signInClicked
  signal signUpClicked

  Column {
    id: layout

    anchors.fill: parent

    MMHeader {
      headerTitle: qsTr("Sign Up")
      onBackClicked: root.backClicked()
    }

    MMInput {
      title: qsTr("Username")
    }

    MMPasswordInput {
      id: password
      title: qsTr("Password")
      regexp: '(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9])(?=.*[^A-Za-z0-9])(?=.{6,})'
      errorMsg: qsTr("Password must contain at least 6 characters\nMinimum 1 number, uppercase and lowercase letter and special character.")
      msgShowBehaviour: MMPasswordInput.Never
      onTextEdited: {
          msgShowBehaviour = MMPasswordInput.OnNotMatchingRegex
      }
    }

    MMPasswordInput {
      id: confirmPassword
      title: qsTr("Confirm Password")
      errorMsg: qsTr("Passwords do not match")
      onTextEdited: {
          if (password.text === confirmPassword.text)
             msgShowBehaviour = MMPasswordInput.Never
          else
             msgShowBehaviour = MMPasswordInput.Always
      }
   }

    MMCheckBox {
      // TODO external links
      small: false
      text: qsTr("I accept the terms and Conditions and Privacy Policy")
    }

    MMButton {
      text: qsTr("Sign up")

      onClicked: root.signUpClicked()
    }

    MMHlineText {
      title: qsTr("Already have an account?")
    }
    MMLinkButton {
      text: qsTr("Sign in")

      onClicked: root.signInClicked()
    }
  }
}
