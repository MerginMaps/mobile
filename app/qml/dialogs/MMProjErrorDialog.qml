/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import QtQuick

import "../components" as MMComponents
import "./components" as MMDialogComponents

MMComponents.MMDrawerDialog {
  id: root

  property string detailedDescription
  property string helpLink: __inputHelp.howToSetupProj

  title: qsTr( "PROJ Error" )
  imageSource: __style.negativeMMSymbolImage

  description: qsTr( "There was an issue with loading your PROJ files. Please refer to our documentation to learn how to set up PROJ." )

  link: helpLink
  linkText: qsTr( "Learn more" )

  additionalContent: MMDialogComponents.MMDialogAdditionalText {
    width: parent.width
    text: root.detailedDescription
  }
}
