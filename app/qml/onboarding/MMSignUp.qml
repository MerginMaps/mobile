

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

  ColumnLayout {
    id: layout

    anchors.fill: parent

    MMOnboardingHeader {
      headerTitle: qsTr("Sign Up")
      Layout.fillWidth: true
      onBackClicked: root.backClicked()
    }

    MMInput {
      title: qsTr("Username")
    }

    MMPasswordInput {
      title: qsTr("Password")
      regexp: '(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9])(?=.*[^A-Za-z0-9])(?=.{6,})'
      errorMsg: qsTr("Password must contain at least 6 characters\nMinimum 1 number, uppercase and lowercase letter and special character.")
    }

    MMPasswordInput {
      title: qsTr("Confirm Password")
      regexp: '(?=.*[a-z])(?=.*[A-Z])(?=.*[0-9])(?=.*[^A-Za-z0-9])(?=.{6,})'
      errorMsg: qsTr("Password must contain at least 6 characters\nMinimum 1 number, uppercase and lowercase letter and special character.")
    }

    CheckBox {
      // TODO external links
      // TODO branded checkbox
      text: qsTr("I accept the terms and Conditions and Privacy Policy")
    }

    MMButton {
      text: qsTr("Sign up")

      onClicked: root.signUpClicked()
    }

    MMOnboardingHlineText {
      title: qsTr("Already have an account?")
    }
    MMLinkButton {
      text: qsTr("Sign in")

      onClicked: root.signInClicked()
    }
  }
}
