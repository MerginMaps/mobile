/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

import "../components"

MMDrawerDialog {

  imageSource: __style.warnLogoImage
  title: qsTr( "Enable system permissions to use haptic feedback" )
  description: qsTr( "Haptic feedback requires system settings for haptic feedback turned on too, to work properly. Please make sure " )
    + "<i>" + qsTr( "Settings -> Sound & vibration -> Vibration & haptics -> Touch feedback" ) + "</i>"
    + qsTr( " is turned on." )
}