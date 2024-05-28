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

import "./components" as MMAccountComponents
import "../components"
import "../inputs"

MMPage {
  id: root

  property string apiRoot
  property bool canSignUp: true

  property string warningMsg

  /**
  * Suppose to be true if auth request is pending. Then busy indicator is running and
  * the login button is disabled.
  */
  property bool pending: false // ?

  signal signUpClicked
  signal signInClicked( string username, string password )
  signal changeServerClicked( string newServer )
  signal forgotPasswordClicked

  pageHeader {
    title: qsTr( "Log in" )
    titleFont: __style.h3
    baseHeaderHeight: __style.row80
  }

  pageBottomMarginPolicy: MMPage.BottomMarginPolicy.PaintBehindSystemBar

  pageContent: MMScrollView {
    id: contentScroller

    width: parent.width
    height: parent.height

    Column {
      width: parent.width

      spacing: 0

      Column {
        id: contentColumn

        width: parent.width

        spacing: __style.spacing20

        MMInfoBox {
          id: errorInfoBox

          width: parent.width

          visible: root.warningMsg

          title: root.warningMsg
          imageSource: __style.warnLogoImage

          color: __style.nightColor
          textColor: __style.polarColor
        }

        MMListSpacer {
          visible: !errorInfoBox.visible
          height: __style.margin20
        }

        MMTextInput {
          id: username

          width: parent.width

          title: qsTr( "Email or username" )
          textField.inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhEmailCharactersOnly
        }

        MMPasswordInput {
          id: password

          width: parent.width

          title: qsTr( "Password" )
        }

        MMButton {
          width: parent.width

          text: qsTr( "Forgot password?" )
          type: MMButton.Types.Tertiary

          onClicked: root.forgotPasswordClicked()
        }

        MMListSpacer { height: __style.margin20 }

        MMButton {
          width: parent.width

          text: qsTr( "Sign in" )

          enabled: !root.pending

          onClicked: root.signInClicked( username.text, password.text )
        }

        MMListSpacer { height: __style.margin20 }

        MMHlineText {
          width: parent.width

          title: qsTr("Don't have an account?")
          visible: root.canSignUp
        }

        MMButton {
          width: parent.width

          text: qsTr( "Sign up" )
          visible: root.canSignUp

          type: MMButton.Types.Secondary

          enabled: !root.pending

          onClicked: root.signUpClicked()
        }
      }

      MMListSpacer {
        height: Math.max( ( contentScroller.height - contentColumn.height - footerContent.height ), __style.margin40 )
      }

      Column {
        id: footerContent

        width: parent.width

        spacing: 0

        MMButton {
          width: parent.width

          type: MMButton.Types.Tertiary

          iconSourceLeft: __style.globeIcon
          fontColor: __style.nightColor

          hoverEnabled: false

          text: root.apiRoot

          onClicked: changeServerDrawerLoader.active = true
        }

        MMListFooterSpacer { height: __style.safeAreaBottom + __style.margin20 }
      }
    }
  }

  Loader {
    id: changeServerDrawerLoader

    active: false
    asynchronous: true

    sourceComponent: MMDrawer {
      id: changeServerDrawer

      drawerHeader.title: qsTr( "Change server" )

      onClosed: changeServerDrawerLoader.active = false

      drawerContent: MMScrollView {

        width: parent.width
        height: changeServerDrawer.maxHeightHit ? changeServerDrawer.drawerContentAvailableHeight : contentHeight

        Column {
          width: parent.width
          spacing: 0

          MMListSpacer { height: __style.spacing20 }

          MMTextInput {
            id: serverURLInput

            width: parent.width
            title: qsTr( "Server address" )

            textFieldBackground.color: __style.lightGreenColor

            text: root.apiRoot
            placeholderText: "https://my-server-app.com/"

            // Qt.ImhNoPredictiveText must be accompanied by Qt.ImhSensitiveData, see https://bugreports.qt.io/browse/QTBUG-86982
            textField.inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhSensitiveData | Qt.ImhNoAutoUppercase | Qt.ImhUrlCharactersOnly
          }

          MMListSpacer { height: __style.spacing40 }

          MMButton {
            width: parent.width

            text: qsTr( "Confirm" )

            onClicked: {
              root.changeServerClicked( serverURLInput.text )
              changeServerDrawer.close()
            }
          }
        }

        Component.onCompleted: open()
      }
    }
  }
}
