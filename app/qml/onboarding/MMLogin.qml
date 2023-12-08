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

Page {
  id: root

  signal signInClicked()
  signal signUpClicked()
  signal changeServerClicked()

  ColumnLayout {

    MMOnboardingHeader {
      headerTitle: tr("Sign In")
    }

    ToolButton {
      onClicked: root.signInClicked()
      text: "Sign in"
    }

    ToolButton {
      onClicked: root.signUpClicked()
      text: "Sign up"
    }

    ToolButton {
      onClicked: root.changeServerClicked()
      text: "Change Server"
    }
  }


}
