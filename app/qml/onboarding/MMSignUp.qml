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
  signal signInClicked
  signal signUpClicked
  signal changeServerClicked
  signal forgotPasswordClicked

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

  ScrollView {
    width: parent.width
    height: parent.height

    ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
    ScrollBar.vertical.policy: ScrollBar.AlwaysOff

    Column {
      id: mainColumn

      width: root.width
      spacing: 20 * __dp
      padding: 20 * __dp

      MMHeader {
        width: parent.width - 2 * mainColumn.padding
        headerTitle: qsTr("Sign Up")
        titleFont: __style.h3
        backColor: __style.lightGreenColor

        onBackClicked: root.backClicked()
      }

      Item { width: 1; height: 1 }

      MMInputEditor {
        width: parent.width - 2 * mainColumn.padding
        title: qsTr("Username")
        bgColor: __style.lightGreenColor
      }

      MMInputEditor {
        width: parent.width - 2 * mainColumn.padding
        title: qsTr("Email address")
        bgColor: __style.lightGreenColor
      }

      MMPasswordEditor {
        width: parent.width - 2 * mainColumn.padding
        title: qsTr("Password")
        bgColor: __style.lightGreenColor
      }

      MMPasswordEditor {
        width: parent.width - 2 * mainColumn.padding
        title: qsTr("Confirm password")
        bgColor: __style.lightGreenColor
      }

      Row {
        width: parent.width
        spacing: 10 * __dp

        MMCheckBox {
          id: checkbox

          width: 24 * __dp
          anchors.verticalCenter: parent.verticalCenter
        }

        Text {
          width: parent.width - checkbox.width - parent.spacing - 2 * mainColumn.padding
          anchors.verticalCenter: parent.verticalCenter

          text: qsTr("I accept the Mergin <a href='https://merginmaps.com'>Terms and Conditions and Privacy Policy</a>")
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

      Item { width: 1; height: 1 }

      MMButton {
        width: parent.width - 2 * mainColumn.padding
        text: qsTr("Sign up")

        onClicked: root.signUpClicked()
      }

      Item { width: 1; height: 1 }

      MMHlineText {
        width: parent.width - 2 * mainColumn.padding
        title: qsTr("Already have an account?")
      }

      MMLinkButton {
        width: parent.width - 2 * mainColumn.padding
        text: qsTr("Sign in")

        onClicked: root.signUpClicked()
      }
    }
  }
}
