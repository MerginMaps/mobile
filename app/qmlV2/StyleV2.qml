/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//pragma Singleton
import QtQuick

QtObject {
  // Fonts - how to use
  // standard - font: StyleV2.p5
  // extended - font.pixelSize: StyleV2.p5.pixelSize; font.italic: true

  // Fonts - Title
  readonly property font t1: Qt.font({ pixelSize: 18 * __dp, bold: true })
  readonly property font t2: Qt.font({ pixelSize: 16 * __dp, bold: true })
  readonly property font t3: Qt.font({ pixelSize: 14 * __dp, bold: true })
  readonly property font t4: Qt.font({ pixelSize: 12 * __dp, bold: true })
  readonly property font t5: Qt.font({ pixelSize: 10 * __dp, bold: true })

  // Fonts - Paragraph
  readonly property font p1: Qt.font({ pixelSize: 32 * __dp })
  readonly property font p2: Qt.font({ pixelSize: 24 * __dp })
  readonly property font p3: Qt.font({ pixelSize: 20 * __dp })
  readonly property font p4: Qt.font({ pixelSize: 16 * __dp })
  readonly property font p5: Qt.font({ pixelSize: 14 * __dp })
  readonly property font p6: Qt.font({ pixelSize: 12 * __dp })
  readonly property font p7: Qt.font({ pixelSize: 10 * __dp })

  // Colors - pripary palette
  readonly property color grassColor: "#73D19C"
  readonly property color forestColor: "#004C45"
  readonly property color nightColor: "#12181F"
  readonly property color whiteColor: "#FFFFFF"
  readonly property color transparentColor: "transparent"

  // Colors - secondary palette
  readonly property color lightGreenColor: "#EFF5F3"
  readonly property color mediumGreenColor: "#B7CDC4"
  readonly property color grayColor: "#E2E2E2"

  // Colors - additional colors
  readonly property color sendColor: "#FFF4E2"
  readonly property color sunsetColor: "#FFB673"
  readonly property color sunColor: "#F4CB46"
  readonly property color earthColor: "#4D2A24"
  readonly property color roseColor: "#FFBABC"
  readonly property color skyColor: "#A6CBF4"
  readonly property color grapeColor: "#5A2740"
  readonly property color deepOceanColor: "#1C324A"
  readonly property color purpleColor: "#CCBDF5"
  readonly property color fieldColor: "#9BD1A9"

  // Colors - sentiment colors
  readonly property color positiveColor: "#C2FFA6"
  readonly property color warningColor: "#FFD6A6"
  readonly property color negativeColor: "#FFA6A6"
  readonly property color informativeColor: "#A6F4FF"

  // Colors - others
  readonly property color nightAlphaColor: "#AA12181F" // placeholder input color
  readonly property color errorBgInputColor: "#FEFAF9" // error bg input color
  readonly property color shadowColor: "#66777777"

  // Icons
  readonly property url arrowLinkRightIcon: "qrc:/Arrow Link Right.svg"
  readonly property url searchIcon: "qrc:/Search.svg"
  readonly property url calendarIcon: "qrc:/Calendar.svg"
  readonly property url showIcon: "qrc:/Show.svg"
  readonly property url hideIcon: "qrc:/Hide.svg"
  readonly property url xMarkIcon: "qrc:/X Mark.svg"
  readonly property url errorIcon: "qrc:/Error.svg"
  readonly property url arrowUpIcon: "qrc:/Arrow Up.svg"
  readonly property url arrowDownIcon: "qrc:/Arrow Down.svg"
  readonly property url qrCodeIcon: "qrc:/QR Code.svg"
  readonly property url checkmarkIcon: "qrc:/Checkmark.svg"
  readonly property url closeButtonIcon: "qrc:/CloseButton.svg"
  readonly property url closeIcon: "qrc:/Close.svg"
  readonly property url waitingIcon: "qrc:/Waiting.svg"
  readonly property url deleteIcon: "qrc:/Delete.svg"
  readonly property url doneIcon: "qrc:/Done.svg"
  readonly property url editIcon: "qrc:/Edit.svg"
  readonly property url moreIcon: "qrc:/More.svg"

  // Images
  readonly property url uploadImage: "qrc:/UploadImage.svg"
  readonly property url reachedDataLimitImage: "qrc:/ReachedDataLimitImage.svg"

  // Map items
  readonly property double mapItemHeight: 50 * __dp

  // Toolbar
  readonly property double toolbarHeight: 89 * __dp
  readonly property double menuDrawerHeight: 67 * __dp
}
