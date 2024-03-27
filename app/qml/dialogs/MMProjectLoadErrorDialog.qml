/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../components"
import "../inputs"

MMDrawerDialog {
  id: root

  property string helpLink: __inputHelp.projectLoadingErrorHelpLink

  picture: __style.negativeMMSymbolImage
  bigTitle: qsTr( "Failed to open the project" )
  description: __inputUtils.htmlLink(
                 qsTr("Learn more about %1loading errors%2."),
                 __style.forestColor,
                 root.helpLink
               )
}
