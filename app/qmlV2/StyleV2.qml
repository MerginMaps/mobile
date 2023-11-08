/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

pragma Singleton
import QtQuick

QtObject {
  // Colors - pripary palette
  property color grassColor: "#73D19C"
  property color forestColor: "#004C45"
  property color nightColor: "#12181F"
  property color whiteColor: "#FFFFFF"
  property color transparentColor: "transparent"

  // Colors - secondary palette
  property color lightGreenColor: "#EFF5F3"
  property color mediumGreenColor: "#B7CDC4"
  property color grayColor: "#E2E2E2"

  // Colors - additional colors
  property color sendColor: "#FFF4E2"
  property color sunsetColor: "#FFB673"
  property color sunColor: "#F4CB46"
  property color earthColor: "#4D2A24"
  property color roseColor: "#FFBABC"
  property color skyColor: "#A6CBF4"
  property color grapeColor: "#5A2740"
  property color deepOceanColor: "#1C324A"
  property color purpleColor: "#CCBDF5"
  property color fieldColor: "#9BD1A9"

  // Colors - sentiment colors
  property color positiveColor: "#C2FFA6"
  property color warningColor: "#FFD6A6"
  property color negativeColor: "#FFA6A6"
  property color informativeColor: "#A6F4FF"

  // Colors - special
  property color nightAlphaColor: "#AA12181F" // placeholder input color
  property color errorBgInputColor: "#FEFAF9" // error bg input color
}
