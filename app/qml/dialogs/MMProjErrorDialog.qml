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

  picture: __style.negativeMMSymbolImage
  bigTitle: qsTr( "PROJ Error" )
  description: qsTr("Learn more about <a href='%1' style='color: %2;'>how to setup PROJ</a>.")
  .arg(__inputHelp.howToSetupProj)
  .arg(__style.forestColor)
}
