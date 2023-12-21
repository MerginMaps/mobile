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

  width: parent.width

  signal backClicked
  signal signInClicked ( string username, string password )
  signal signUpClicked
  signal changeServerClicked ( string newServer )
  signal forgotPasswordClicked

  property string warningMsg //TODO
  property bool canSignUp: true //TODO
  property string apiRoot

  /**
  * Suppose to be true if auth request is pending. Then busy indicator is running and
  * the login button is disabled.
  */
  //TODO!!
  property bool pending: false

  readonly property real hPadding: width < __style.maxPageWidth
                                   ? 20 * __dp
                                   : (20 + (width - __style.maxPageWidth) / 2) * __dp

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
    headerTitle: qsTr("Log In")
    titleFont: __style.h3
    backColor: __style.lightGreenColor

    onBackClicked: root.backClicked()
  }

  ScrollView {
    width: parent.width
    height: parent.height - changeServerButton.height - header.height - 60 * __dp
    anchors.top: header.bottom
    anchors.topMargin: 20 * __dp

    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
    ScrollBar.vertical.policy: ScrollBar.AlwaysOff

    Column {
      id: mainColumn

      width: root.width
      spacing: 20 * __dp
      leftPadding: root.hPadding
      rightPadding: root.hPadding
      topPadding: 20 * __dp


      Item { width: 1; height: 1 }

      Text {
        // !TODO - need graphic designer input!
        visible: root.warningMsg
        width: parent.width - 2 * root.hPadding
        text: root.warningMsg
        font: __style.h2
        color: __style.negativeColor
        wrapMode: Text.WordWrap
        horizontalAlignment: Text.AlignHCenter
      }

      MMInputEditor {
        id: username
        width: parent.width - 2 * root.hPadding
        title: qsTr("Username")
        bgColor: __style.lightGreenColor
      }

      MMPasswordEditor {
        id: password
        width: parent.width - 2 * root.hPadding
        title: qsTr("Password")
        bgColor: __style.lightGreenColor
      }

      MMLink {
        width: parent.width - 2 * root.hPadding
        height: 20 * __dp
        text: qsTr("Forgot password?")

        onClicked: root.forgotPasswordClicked()
      }

      Item { width: 1; height: 1 }

      MMButton {
        width: parent.width - 2 * root.hPadding
        text: qsTr("Sign in")
        enabled: !pending
        onClicked: {
          root.signInClicked(
            username.text,
            password.text
          )
        }
      }

      Item { width: 1; height: 1 }

      MMHlineText {
        width: parent.width - 2 * root.hPadding
        title: qsTr("Don't have an account?")
        visible: root.canSignUp
      }

      MMLinkButton {
        width: parent.width - 2 * root.hPadding
        text: qsTr("Sign up")
        enabled: !pending
        visible: root.canSignUp

        onClicked: root.signUpClicked()
      }
    }
  }

  MMLink {
    id: changeServerButton

    width: parent.width
    height: 50 * __dp
    anchors.bottom: parent.bottom
    enabled: !pending
    text: root.apiRoot
    leftIcon: __style.globeIcon

    onClicked: {
      changeServerDrawer.newServerUrl = root.apiRoot
      changeServerDrawer.visible = true
    }
  }

  MMDrawer {
    id: changeServerDrawer

    property string newServerUrl

    width: root.width < __style.maxPageWidth ? root.width : root.width - 2 * root.hPadding
    x: root.width < __style.maxPageWidth ? 0 : root.hPadding
    title: qsTr("Change server")
    primaryButton: qsTr("Confirm")
    visible: false
    specialComponent: MMInputEditor {
      width: changeServerDrawer.width - 40 * __dp
      title: qsTr("Server address")
      bgColor: __style.lightGreenColor
      text: changeServerDrawer.newServerUrl
      onTextChanged: changeServerDrawer.newServerUrl = text
    }

    onPrimaryButtonClicked: {
      root.changeServerClicked( newServerUrl )
      visible = false
    }
  }
}
