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

import lc 1.0

import "../components"
import "../inputs"

Page {
  id: root

  // width: parent.width

  /**
  * Suppose to be true if register request is pending. Then busy indicator is running and
  * the sign up button is disabled.
  */
  //TODO!!
  property bool pending: false

  signal backClicked
  signal signInClicked
  signal signUpClicked ( string username, string email, string password, string passwordConfirm, bool tocAccept, bool newsletterSubscribe )

  required property string tocString
  readonly property real hPadding: width < __style.maxPageWidth
                                   ? 20 * __dp
                                   : (20 + (width - __style.maxPageWidth) / 2) * __dp

  // show error message under the respective field
  function showErrorMessage( msg, field ) {

    // clear previous error messages
    username.errorMsg = ""
    email.errorMsg = ""
    password.errorMsg = ""
    passwordConfirm.errorMsg = ""
    // TODO tocAccept.errorMsg = ""
    // TODO errorText.text = ""

    if( field === RegistrationError.USERNAME ) {
      username.errorMsg = msg
      username.focus = true
    }
    else if( field === RegistrationError.EMAIL ) {
      email.errorMsg = msg
      email.focus = true
    }
    else if( field === RegistrationError.PASSWORD ) {
      password.errorMsg = msg
      password.focus = true
    }
    else if( field === RegistrationError.CONFIRM_PASSWORD ) {
      passwordConfirm.errorMsg = msg
      passwordConfirm.focus = true
    }
    else if( field === RegistrationError.TOC ) {
      // TODO where to show MMCheckBox missing errorMsg
      // tocAccept.errorMsg = msg
      // tocAccept.focus = true
      __notificationModel.add(
          msg,
          3,
          NotificationType.Error,
          NotificationType.None
      )
    }
    else if( field === RegistrationError.OTHER ) {
      __notificationModel.add(
          msg,
          3,
          NotificationType.Error,
          NotificationType.None
      )
    }
  }

  // background as Drawer design
  Rectangle {
    anchors.fill: parent
    color: __style.whiteColor

    Rectangle {
      width: parent.width
      height: 20 * __dp
      color: __style.forestColor
    }

    Rectangle {
      width: parent.width
      height: 40 * __dp
      color: __style.whiteColor
      radius: height / 2
    }
  }

  MMHeader {
    id: header

    x: mainColumn.leftPadding
    y: mainColumn.topPadding
    width: parent.width - 2 * root.hPadding
    headerTitle: qsTr("Sign Up")
    titleFont: __style.h3
    backColor: __style.lightGreenColor

    onBackClicked: root.backClicked()
  }

  ScrollView {
    width: parent.width + 40 * __dp
    height: parent.height - header.height - 40 * __dp
    anchors.top: header.bottom
    anchors.topMargin: 20 * __dp
    anchors.bottomMargin: 20 * __dp

    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
    ScrollBar.vertical.policy: ScrollBar.AlwaysOff

    Column {
      id: mainColumn

      width: root.width
      spacing: 20 * __dp
      leftPadding: root.hPadding
      rightPadding: root.hPadding
      topPadding: 20 * __dp
      bottomPadding: 20 * __dp

      MMInputEditor {
        id: username
        width: parent.width - 2 * root.hPadding
        title: qsTr("Username")
        bgColor: __style.lightGreenColor
      }

      MMInputEditor {
        id: email
        width: parent.width - 2 * root.hPadding
        title: qsTr("Email address")
        bgColor: __style.lightGreenColor
      }

      MMPasswordEditor {
        id: password
        width: parent.width - 2 * root.hPadding
        title: qsTr("Password")
        bgColor: __style.lightGreenColor
      }

      MMPasswordEditor {
        id: passwordConfirm
        width: parent.width - 2 * root.hPadding
        title: qsTr("Confirm password")
        bgColor: __style.lightGreenColor
      }

      Row {
        width: parent.width
        spacing: 10 * __dp

        MMCheckBox {
          id: tocAccept

          width: 24 * __dp
          anchors.verticalCenter: parent.verticalCenter
        }

        Text {
          // TODO replace with text in MMCheckBox
          width: parent.width - tocAccept.width - parent.spacing - 2 * root.hPadding
          anchors.verticalCenter: parent.verticalCenter

          text: root.tocString
          font: __style.p5
          color: __style.nightColor
          linkColor: __style.forestColor
          wrapMode: Text.WordWrap
          lineHeight: 1.5

          onLinkActivated: function(link) {
            Qt.openUrlExternally(link)
          }
        }
      }

      Row {
        width: parent.width
        spacing: 10 * __dp

        MMCheckBox {
          id: newsletterSubscribe

          width: 24 * __dp
          anchors.verticalCenter: parent.verticalCenter
        }

        Text {
          // TODO replace with text in MMCheckBox
          width: parent.width - newsletterSubscribe.width - parent.spacing - 2 * root.hPadding
          anchors.verticalCenter: parent.verticalCenter

          text: qsTr("I want to subscribe to the newsletter")
          font: __style.p5
          color: __style.nightColor
          linkColor: __style.forestColor
          wrapMode: Text.WordWrap
          lineHeight: 1.5
        }
      }

      Item { width: 1; height: 1 }

      MMButton {
        width: parent.width - 2 * root.hPadding
        text: qsTr("Sign up")
        enabled: !pending
        onClicked: {
          root.signUpClicked(
                username.text,
                email.text,
                password.text,
                passwordConfirm.text,
                tocAccept.checked,
                newsletterSubscribe.checked
          )
        }
      }

      Item { width: 1; height: 1 }

      MMHlineText {
        width: parent.width - 2 * root.hPadding
        title: qsTr("Already have an account?")
      }

      MMLinkButton {
        width: parent.width - 2 * root.hPadding
        text: qsTr("Sign in")
        enabled: !pending

        onClicked: root.signInClicked()
      }
    }
  }
}
