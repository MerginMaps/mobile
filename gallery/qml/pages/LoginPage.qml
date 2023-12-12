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

import "../../app/qml/onboarding"

MMLogin {
  onBackClicked: console.log("Back button clicked")
  onSignInClicked: console.log("Sign in clicked")
  onSignUpClicked: console.log("Sign up clicked")
  onChangeServerClicked: console.log("Change server clicked")
}
