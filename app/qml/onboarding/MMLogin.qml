

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

Page {
  id: root

  anchors.fill: parent

  signal backClicked
  signal signInClicked
  signal signUpClicked
  signal changeServerClicked
  signal forgotPasswordClicked

  // TODO - lazy loaded?
  MMDrawer {
    id: changeServerDrawer

    title: qsTr("Change server")
    primaryButton: qsTr("Confirm")
    visible: false
    specialComponent: MMInput {
      title: qsTr("Server address")
    }

    onPrimaryButtonClicked: {
      changeServerButton.text = specialComponent.text
      visible = false
      root.changeServerClicked()
    }
  }

  ColumnLayout {
    id: layout

    anchors.fill: parent

    MMOnboardingHeader {
      headerTitle: qsTr("Log In")
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

    MMButton {
      text: qsTr("Forgot password?")
      transparent: true
      onClicked: root.signInClicked()
    }

    MMButton {
      text: qsTr("Sign in")

      onClicked: root.signInClicked()
    }

    MMOnboardingHlineText {
      title: qsTr("Don't have an account?")
    }

    MMLinkButton {
      text: qsTr("Sign up")

      onClicked: root.signUpClicked()
    }

    MMChangeServerButton {
      id: changeServerButton

      onClicked: {
        changeServerDrawer.visible = true
      }
    }
  }
}
