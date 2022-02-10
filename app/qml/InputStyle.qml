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

QtObject {

    // Primary colors
    property color fontColor: "#006146"
    property color fontColorBright: "#679D70"
    property color panelBackground2: "#C6CCC7"
    property color activeButtonColor: "#006146"
    property color activeButtonColorOrange: "#FD9626"
    property color invalidButtonColor: "red"
    property color learnMoreLinkColor: "darkblue"

    // Secondary colors
    property color clrPanelMain: "white"
    property color clrPanelBackground: fontColor
    property color panelBackgroundDark: "#B3B3B3"
    property color panelBackgroundDarker: "#575757"
    property color panelBackgroundLight: "#E6E6E6"
    property color labelColor: "#999999"
    property color secondaryFontColor: "#B3B3B3"

    property color highlightColor: "#FD9626"
    property color panelItemHighlight: "#9ABFA0"
    property color warningBannerColor: "#FFFF99"
    property color softRed: "#FC9FB1"
    property color softOrange: "#FDD7B1"
    property color darkOrange: "#FD9626"
    property color softGreen: "#32AA3A"
    property color darkGreen: "#006146"

    property int fontPixelSizeSmall: scaleFont(15)
    property int fontPixelSizeNormal: scaleFont(18)
    property int fontPixelSizeBig: scaleFont(21)

    property int borderSize: scale(1)

    property int panelMargin: scale(30)
    property real rowHeight: scale(64)
    property real rowHeightHeader: scale(64)
    property real rowHeightSmall: scale(20)
    property real fieldHeight: scale(54)
    property real mapBtnHeight: scale(40)
    property real smallGap: scale(25)
    property real tinyGap: scale(5)
    property real delegateBtnHeight: rowHeight * 0.8
    property real scaleBarHeight: scale(45)
    property real projectItemHeight: rowHeightHeader * 1.2

    property real panelSpacing: 5 * __dp
    property real shadowVerticalOffset: -2 * __dp
    property real shadowRadius: 8 * __dp
    property real shadowSamples: 12
    property real panelOpacity: 1
    property real lowHighlightOpacity: 0.4
    property real highHighlightOpacity: 0.8
    property real cornerRadius: 8 * __dp
    property real innerFieldMargin: 10 * __dp  // TODO rename fieldMargin
    property real outerFieldMargin: 20 * __dp  // TODO change for PanelMargin
    property real formSpacing: 10 * __dp

    property real circleRadius: 100

    // icons
    property string cameraIcon: "qrc:/add_photo.svg"
    property string removeIcon: "qrc:/trash.svg"
    property string galleryIcon: "qrc:/gallery.svg"
    property string backIcon: "qrc:/back.svg"
    property string checkIcon: "qrc:/check.svg"
    property string plusIcon: "qrc:/plus.svg"
    property string noIcon: "qrc:/no.svg"
    property string yesIcon: "qrc:/yes.svg"
    property string editIcon: "qrc:/edit.svg"
    property string infoIcon: "qrc:/info.svg"
    property string tableIcon: "qrc:/table.svg"
    property string uploadIcon: "qrc:/cloud-upload.svg"
    property string detachIcon: "qrc:/unlink.svg"
    property string eyeIcon: "qrc:/eye.svg"
    property string eyeSlashIcon: "qrc:/eye-slash.svg"

    property string valueRelationIcon: "qrc:/value_relation_open.svg"
    property string comboboxIcon: "qrc:/combobox.svg"
    property string qrCodeIcon: "qrc:/qrcode.svg"

    property string exclamationIcon: "qrc:/exclamation-circle.svg"
    property string exclamationTriangleIcon: "qrc:/exclamation-triangle-solid.svg"
    property string syncIcon: "qrc:/sync.svg"
    property string downloadIcon: "qrc:/download.svg"
    property string stopIcon: "qrc:/stop.svg"

    property string moreMenuIcon: "qrc:/more_menu.svg"
    property string mapMarkerIcon: "qrc:/marker.svg"

    property string projectIcon: "qrc:/project.svg"
    property string zoomToProjectIcon: "qrc:/zoom_to_project.svg"
    property string mapThemesIcon: "qrc:/map_styles.svg"
    property string settingsIcon: "qrc:/settings.svg"

    property string gpsDirectionIcon: "qrc:/gps_direction.svg"
    property string gpsMarkerPositionIcon: "qrc:/gps_marker_position.svg"
    property string gpsMarkerNoPositionIcon: "qrc:/gps_marker_no_position.svg"
    property string gpsFixedIcon: "qrc:/ic_gps_fixed_48px.svg"
    property string gpsNotFixedIcon: "qrc:/ic_gps_not_fixed_48px.svg"

    property string vectorPointIcon: "qrc:/mIconPointLayer.svg"
    property string vectorLineIcon: "qrc:/mIconLineLayer.svg"
    property string vectorPolygonIcon: "qrc:/mIconPolygonLayer.svg"

    property string recordIcon: "qrc:/dot-circle.svg"
    property string crosshairIcon: "qrc:/crosshair.svg"
    property string undoIcon: "qrc:/undo.svg"
    property string merginColorIcon: "qrc:/mergin_color.svg"
    property string accountIcon: "qrc:/account.svg"
    property string accountMultiIcon: "qrc:/account-multi.svg"
    property string envelopeIcon: "qrc:/envelope-solid.svg"
    property string homeIcon: "qrc:/home.svg"
    property string exploreIcon: "qrc:/explore.svg"
    property string todayIcon: "qrc:/ic_today.svg"
    property string databaseIcon: "qrc:/database-solid.svg"
    property string lockIcon: "qrc:/lock.svg"
    property string searchIcon: "qrc:/search.svg"
    property string loadingIndicatorIcon: "qrc:/loadingindicator.svg"

    property string linkIcon: "qrc:/link-rotated.svg"
    property string unlinkIcon: "qrc:/unlink.svg"

    property string inputLogo: "qrc:/input.svg"
    property string lutraLogo: "qrc:/lutra_logo.svg"

    property string closeIcon: "qrc:/ic_clear_black.svg"
    property string stakeoutIcon: "qrc:/stakeout.png"
    
    property real scrollVelocityAndroid: 10000 // [px/s] scrolling on Android devices is too slow by default

    // map related styling constats
    property real mapLoadingIndicatorHeight: 7 * __dp

    property real mapMarkerWidth: 60 * __dp
    property real mapMarkerHeight: 70 * __dp
    property real mapMarkerAnchorY: 48 * __dp

    property color highlightLineColor: Qt.rgba( 1, 0.2, 0.2, 1 )
    property color highlightFillColor: Qt.rgba( 1, 0.2, 0.2, InputStyle.lowHighlightOpacity )
    property color highlightOutlineColor: "white"
    property real highlightLineWidth: 6 * __dp
    property real highlightOutlinePenWidth: 2 * __dp

    property real mapOutOfExtentBorder: scale(64) // when pair lays very close to device display border, center map extent

    function scale(size)
    {
        return size * __dp
    }

    function scaleFont(fontSize)
    {
      return fontSize * __dp
    }
}
