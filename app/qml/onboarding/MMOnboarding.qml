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

Item {
  id: root

  signal closeOnboarding()

  Component {
      id: mmsignup
      MMSignUp
      {
          onBackClicked: {
              console.log("Back Clicked on Sign Up Page")
              stackView.pop()
          }

          onSignInClicked: {
            console.log("Sign In Clicked on Sign Up Page")
            stackView.pop()
          }

          onSignUpClicked: {
            console.log("Sign Up Clicked on Sign Up Page")
            stackView.push( mmcreateworkspace )
          }
     }
  }


  MMLogin { id: mmlogin }
  MMAcceptInvitation { id: mmacceptinvitation }
  MMCreateWorkspace { id: mmcreateworkspace }
  MMHowYouFoundUs { id: mmhowyoufoundus }
  MMWhichIndustry { id: mmwhichindustry }

  StackView {
    id: stackView
    anchors.fill: parent
    initialItem: mmlogin
  }

  Connections {
      target: mmlogin

      function onBackClicked( ) {
          console.log("Back Clicked on Login Page")
          root.closeOnboarding()
      }

      function onSignInClicked( ) {
        console.log("Sign In Clicked on Login Page")
      }

      function onSignUpClicked( ) {
        console.log("Sign Up Clicked on Login Page")
        stackView.push( mmsignup, {} )
      }

      function onChangeServerClicked( ) {
        console.log("Change Server Clicked on Login Page")
      }
  }
}
