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

  title: qsTr( "Failed to open the project" )
  imageSource: __style.negativeMMSymbolImage

  description: qsTr( "Unfortunately, your project could not be opened, please refer to our documentation to understand common problems." )

  link: root.helpLink
  linkText: qsTr( "Learn more" )
}
