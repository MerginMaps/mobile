/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
pragma Singleton
import QtQuick 2.0
import QgsQuick 0.1 as QgsQuick

QtObject {
  property string accountIcon: "qrc:/account.svg"
  property string accountMultiIcon: "qrc:/account-multi.svg"
  property color activeButtonColor: "#006146"
  property color activeButtonColorOrange: "#FD9626"
  property string backIcon: "qrc:/back.svg"

  // icons
  property string cameraIcon: "qrc:/add_photo.svg"
  property string checkIcon: "qrc:/check.svg"
  property color clrPanelBackground: fontColor

  // Secondary colors
  property color clrPanelMain: "white"
  property string comboboxIcon: "qrc:/combobox.svg"
  property real cornerRadius: 8 * QgsQuick.Utils.dp
  property string crosshairIcon: "qrc:/crosshair.svg"
  property color darkGreen: "#006146"
  property color darkOrange: "#FD9626"
  property string databaseIcon: "qrc:/database-solid.svg"
  property real delegateBtnHeight: rowHeight * 0.8
  property string detachIcon: "qrc:/unlink.svg"

  // used in scaling functions for high DPI screens
  property real deviceRatio: 1
  property string downloadIcon: "qrc:/download.svg"
  property string editIcon: "qrc:/edit.svg"
  property string envelopeIcon: "qrc:/envelope-solid.svg"
  property string exclamationIcon: "qrc:/exclamation-circle.svg"
  property string exclamationTriangleIcon: "qrc:/exclamation-triangle-solid.svg"
  property string exploreIcon: "qrc:/explore.svg"
  property string eyeIcon: "qrc:/eye.svg"
  property string eyeSlashIcon: "qrc:/eye-slash.svg"
  property real fieldHeight: scale(54)

  // Primary colors
  property color fontColor: "#006146"
  property color fontColorBright: "#679D70"
  property real fontPixelSizeNormal: QgsQuick.Utils.dp * 24 * rado2Lutra
  property real fontPixelSizeSmall: QgsQuick.Utils.dp * 20 * rado2Lutra
  property real fontPixelSizeTitle: QgsQuick.Utils.dp * 28 * rado2Lutra
  property real formSpacing: 10 * QgsQuick.Utils.dp
  property string galleryIcon: "qrc:/gallery.svg"
  property string gpsDirectionIcon: "qrc:/gps_direction.svg"
  property string gpsFixedIcon: "qrc:/ic_gps_fixed_48px.svg"
  property string gpsMarkerNoPositionIcon: "qrc:/gps_marker_no_position.svg"
  property string gpsMarkerPositionIcon: "qrc:/gps_marker_position.svg"
  property string gpsNotFixedIcon: "qrc:/ic_gps_not_fixed_48px.svg"
  property real highHighlightOpacity: 0.8
  property color highlighOutlineColor: "white"
  property real highlighOutlinePenWidth: 1 * QgsQuick.Utils.dp
  property color highlightColor: "#FD9626"
  property color highlightFillColor: Qt.rgba(1, 0.2, 0.2, InputStyle.lowHighlightOpacity)
  property color highlightLineColor: Qt.rgba(1, 0.2, 0.2, 1)
  property real highlightLineWidth: 6 * QgsQuick.Utils.dp
  property string homeIcon: "qrc:/home.svg"
  property string infoIcon: "qrc:/info.svg"
  property real innerFieldMargin: 10 * QgsQuick.Utils.dp  // TODO rename fieldMargin
  property string inputLogo: "qrc:/input.svg"
  property color invalidButtonColor: "red"
  property color labelColor: "#999999"
  property string linkIcon: "qrc:/link-rotated.svg"
  property string loadingIndicatorIcon: "qrc:/loadingindicator.svg"
  property string lockIcon: "qrc:/lock.svg"
  property real lowHighlightOpacity: 0.4
  property string lutraLogo: "qrc:/lutra_logo.svg"

  // map related styling constats
  property real mapLoadingIndicatorHeight: 7 * QgsQuick.Utils.dp
  property real mapMarkerAnchorY: 48 * QgsQuick.Utils.dp
  property real mapMarkerHeight: 70 * QgsQuick.Utils.dp
  property string mapMarkerIcon: "qrc:/marker.svg"
  property real mapMarkerWidth: 60 * QgsQuick.Utils.dp
  property real mapOutOfExtentBorder: scale(64) // when pair lays very close to device display border, center map extent
  property string mapThemesIcon: "qrc:/map_styles.svg"
  property string merginColorIcon: "qrc:/mergin_color.svg"
  property string moreMenuIcon: "qrc:/more_menu.svg"
  property string noIcon: "qrc:/no.svg"
  property real outerFieldMargin: 20 * QgsQuick.Utils.dp  // TODO change for PanelMargin
  property color panelBackground2: "#C6CCC7"
  property color panelBackgroundDark: "#B3B3B3"
  property color panelBackgroundDarker: "#575757"
  property color panelBackgroundLight: "#E6E6E6"
  property color panelItemHighlight: "#9ABFA0"
  property int panelMargin: scale(30)
  property real panelOpacity: 1
  property real panelSpacing: QgsQuick.Utils.dp * 5
  property string plusIcon: "qrc:/plus.svg"
  property string projectIcon: "qrc:/project.svg"
  property real projectItemHeight: rowHeightHeader * 1.2
  property string qrCodeIcon: "qrc:/qrcode.svg"
  property real rado2Lutra: 0.75 // TODO delete this, but later!
  property string removeIcon: "qrc:/trash.svg"
  property real rowHeight: scale(64)
  property real rowHeightHeader: scale(64)

  // Scaling function using QgsQuick screenDensity and deviceRatio
  property var scale: function scale(size) {
    return size * QgsQuick.Utils.dp;
  }
  property real scaleBarHeight: fontPixelSizeSmall * 3 //according scaleBar text

  // Scaling function for fonts using deviceRatio
  property var scaleFontPointSize: function scaleParam(size) {
    return size * deviceRatio;
  }
  property real scrollVelocityAndroid: 10000 // [px/s] scrolling on Android devices is too slow by default
  property string searchIcon: "qrc:/search.svg"
  property string settingsIcon: "qrc:/settings.svg"
  property real shadowRadius: 8 * QgsQuick.Utils.dp
  property real shadowSamples: 12
  property real shadowVerticalOffset: -2 * QgsQuick.Utils.dp
  property color softGreen: "#32AA3A"
  property color softOrange: "#FDD7B1"
  property color softRed: "#FC9FB1"
  property string stopIcon: "qrc:/stop.svg"
  property string syncIcon: "qrc:/sync.svg"
  property string tableIcon: "qrc:/table.svg"
  property string todayIcon: "qrc:/ic_today.svg"
  property string undoIcon: "qrc:/undo.svg"
  property string unlinkIcon: "qrc:/unlink.svg"
  property string uploadIcon: "qrc:/cloud-upload.svg"
  property string valueRelationIcon: "qrc:/value_relation_open.svg"
  property string vectorLineIcon: "qrc:/mIconLineLayer.svg"
  property string vectorPointIcon: "qrc:/mIconPointLayer.svg"
  property string vectorPolygonIcon: "qrc:/mIconPolygonLayer.svg"
  property color warningBannerColor: "#FFFF99"
  property string zoomToProjectIcon: "qrc:/zoom_to_project.svg"
}
