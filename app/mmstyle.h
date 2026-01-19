/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MMSTYLE_H
#define MMSTYLE_H

#include <QObject>
#include <QFont>
#include <QColor>
#include <QUrl>

class MMStyle: public QObject
{
    Q_OBJECT

    // Fonts
    Q_PROPERTY( QFont h1 READ h1 CONSTANT )
    Q_PROPERTY( QFont h2 READ h2 CONSTANT )
    Q_PROPERTY( QFont h3 READ h3 CONSTANT )
    Q_PROPERTY( QFont t1 READ t1 CONSTANT )
    Q_PROPERTY( QFont t2 READ t2 CONSTANT )
    Q_PROPERTY( QFont t3 READ t3 CONSTANT )
    Q_PROPERTY( QFont t4 READ t4 CONSTANT )
    Q_PROPERTY( QFont t5 READ t5 CONSTANT )
    Q_PROPERTY( QFont p1 READ p1 CONSTANT )
    Q_PROPERTY( QFont p2 READ p2 CONSTANT )
    Q_PROPERTY( QFont p3 READ p3 CONSTANT )
    Q_PROPERTY( QFont p4 READ p4 CONSTANT )
    Q_PROPERTY( QFont p5 READ p5 CONSTANT )
    Q_PROPERTY( QFont p6 READ p6 CONSTANT )
    Q_PROPERTY( QFont p7 READ p7 CONSTANT )
    Q_PROPERTY( double fontLineHeight60 READ number60 CONSTANT )
    Q_PROPERTY( double fontLineHeight48 READ number48 CONSTANT )
    Q_PROPERTY( double fontLineHeight44 READ number44 CONSTANT )
    Q_PROPERTY( double fontLineHeight36 READ number36 CONSTANT )
    Q_PROPERTY( double fontLineHeight32 READ number32 CONSTANT )
    Q_PROPERTY( double fontLineHeight30 READ number30 CONSTANT )
    Q_PROPERTY( double fontLineHeight28 READ number28 CONSTANT )
    Q_PROPERTY( double fontLineHeight26 READ number26 CONSTANT )
    Q_PROPERTY( double fontLineHeight24 READ number24 CONSTANT )
    Q_PROPERTY( double fontLineHeight22 READ number22 CONSTANT )

    // Colors - primary palette
    Q_PROPERTY( QColor grassColor READ grassColor CONSTANT )
    Q_PROPERTY( QColor forestColor READ forestColor CONSTANT )
    Q_PROPERTY( QColor nightColor READ nightColor CONSTANT )
    Q_PROPERTY( QColor polarColor READ polarColor CONSTANT )
    Q_PROPERTY( QColor transparentColor READ transparentColor CONSTANT )

    // Colors - secondary palette
    Q_PROPERTY( QColor lightGreenColor READ lightGreenColor CONSTANT )
    Q_PROPERTY( QColor mediumGreenColor READ mediumGreenColor CONSTANT )
    Q_PROPERTY( QColor darkGreenColor READ darkGreenColor CONSTANT )
    Q_PROPERTY( QColor greyColor READ greyColor CONSTANT )
    Q_PROPERTY( QColor darkGreyColor READ darkGreyColor CONSTANT )
    Q_PROPERTY( QColor mediumGreyColor READ mediumGreyColor CONSTANT )

    // Colors - additional colors
    Q_PROPERTY( QColor sandColor READ sandColor CONSTANT )
    Q_PROPERTY( QColor sunsetColor READ sunsetColor CONSTANT )
    Q_PROPERTY( QColor sunColor READ sunColor CONSTANT )
    Q_PROPERTY( QColor earthColor READ earthColor CONSTANT )
    Q_PROPERTY( QColor roseColor READ roseColor CONSTANT )
    Q_PROPERTY( QColor skyColor READ skyColor CONSTANT )
    Q_PROPERTY( QColor grapeColor READ grapeColor CONSTANT )
    Q_PROPERTY( QColor grapeTransparentColor READ grapeTransparentColor CONSTANT )
    Q_PROPERTY( QColor deepOceanColor READ deepOceanColor CONSTANT )
    Q_PROPERTY( QColor purpleColor READ purpleColor CONSTANT )
    Q_PROPERTY( QColor fieldColor READ fieldColor CONSTANT )

    // Colors - sentiment colors
    Q_PROPERTY( QColor positiveColor READ positiveColor CONSTANT )
    Q_PROPERTY( QColor warningColor READ warningColor CONSTANT )
    Q_PROPERTY( QColor negativeColor READ negativeColor CONSTANT )
    Q_PROPERTY( QColor negativeLightColor READ negativeLightColor CONSTANT )
    Q_PROPERTY( QColor negativeUltraLightColor READ negativeUltraLightColor CONSTANT )
    Q_PROPERTY( QColor informativeColor READ informativeColor CONSTANT )

    // Colors - color picker default
    Q_PROPERTY( QColor photoSketchingBlackColor READ photoSketchingBlackColor CONSTANT )
    Q_PROPERTY( QColor photoSketchingWhiteColor READ photoSketchingWhiteColor CONSTANT )
    Q_PROPERTY( QColor photoSketchingGreenColor READ photoSketchingGreenColor CONSTANT )
    Q_PROPERTY( QColor photoSketchingYellowColor READ photoSketchingYellowColor CONSTANT )
    Q_PROPERTY( QColor photoSketchingOrangeColor READ photoSketchingOrangeColor CONSTANT )
    Q_PROPERTY( QColor photoSketchingBlueColor READ photoSketchingBlueColor CONSTANT )
    Q_PROPERTY( QColor photoSketchingPinkColor READ photoSketchingPinkColor CONSTANT )

    // Colors - others
    Q_PROPERTY( QColor shadowColor READ shadowColor CONSTANT )
    Q_PROPERTY( QColor snappingColor READ snappingColor CONSTANT )

    // Icons
    Q_PROPERTY( QUrl linkIcon READ linkIcon CONSTANT )
    Q_PROPERTY( QUrl closeAccountIcon READ closeAccountIcon CONSTANT )
    Q_PROPERTY( QUrl signOutIcon READ signOutIcon CONSTANT )
    Q_PROPERTY( QUrl workspacesIcon READ workspacesIcon CONSTANT )
    Q_PROPERTY( QUrl addIcon READ addIcon CONSTANT )
    Q_PROPERTY( QUrl layersIcon READ layersIcon CONSTANT )
    Q_PROPERTY( QUrl localChangesIcon READ localChangesIcon CONSTANT )
    Q_PROPERTY( QUrl mapThemesIcon READ mapThemesIcon CONSTANT )
    Q_PROPERTY( QUrl positionTrackingIcon READ positionTrackingIcon CONSTANT )
    Q_PROPERTY( QUrl zoomToProjectIcon READ zoomToProjectIcon CONSTANT )
    Q_PROPERTY( QUrl settingsIcon READ settingsIcon CONSTANT )
    Q_PROPERTY( QUrl addImageIcon READ addImageIcon CONSTANT )
    Q_PROPERTY( QUrl archaeologyIcon READ archaeologyIcon CONSTANT )
    Q_PROPERTY( QUrl arrowDownIcon READ arrowDownIcon CONSTANT )
    Q_PROPERTY( QUrl arrowLinkRightIcon READ arrowLinkRightIcon CONSTANT )
    Q_PROPERTY( QUrl arrowUpIcon READ arrowUpIcon CONSTANT )
    Q_PROPERTY( QUrl backIcon READ backIcon CONSTANT )
    Q_PROPERTY( QUrl briefcaseIcon READ briefcaseIcon CONSTANT )
    Q_PROPERTY( QUrl calendarIcon READ calendarIcon CONSTANT )
    Q_PROPERTY( QUrl checkmarkIcon READ checkmarkIcon CONSTANT )
    Q_PROPERTY( QUrl closeIcon READ closeIcon CONSTANT )
    Q_PROPERTY( QUrl deleteIcon READ deleteIcon CONSTANT )
    Q_PROPERTY( QUrl downloadIcon READ downloadIcon CONSTANT )
    Q_PROPERTY( QUrl uploadIcon READ uploadIcon CONSTANT )
    Q_PROPERTY( QUrl editIcon READ editIcon CONSTANT )
    Q_PROPERTY( QUrl electricityIcon READ electricityIcon CONSTANT )
    Q_PROPERTY( QUrl engineeringIcon READ engineeringIcon CONSTANT )
    Q_PROPERTY( QUrl environmentalIcon READ environmentalIcon CONSTANT )
    Q_PROPERTY( QUrl facebookIcon READ facebookIcon CONSTANT )
    Q_PROPERTY( QUrl featuresIcon READ featuresIcon CONSTANT )
    Q_PROPERTY( QUrl globeIcon READ globeIcon CONSTANT )
    Q_PROPERTY( QUrl globalIcon READ globalIcon CONSTANT )
    Q_PROPERTY( QUrl gpsIcon READ gpsIcon CONSTANT )
    Q_PROPERTY( QUrl gpsAntennaHeightIcon READ gpsAntennaHeightIcon CONSTANT )
    Q_PROPERTY( QUrl hideIcon READ hideIcon CONSTANT )
    Q_PROPERTY( QUrl homeIcon READ homeIcon CONSTANT )
    Q_PROPERTY( QUrl infoIcon READ infoIcon CONSTANT )
    Q_PROPERTY( QUrl linkedinIcon READ linkedinIcon CONSTANT )
    Q_PROPERTY( QUrl mastodonIcon READ mastodonIcon CONSTANT )
    Q_PROPERTY( QUrl minusIcon READ minusIcon CONSTANT )
    Q_PROPERTY( QUrl moreIcon READ moreIcon CONSTANT )
    Q_PROPERTY( QUrl moreVerticalIcon READ moreVerticalIcon CONSTANT )
    Q_PROPERTY( QUrl morePhotosIcon READ morePhotosIcon CONSTANT )
    Q_PROPERTY( QUrl remoteImageLoadErrorIcon READ remoteImageLoadErrorIcon CONSTANT )
    Q_PROPERTY( QUrl mouthIcon READ mouthIcon CONSTANT )
    Q_PROPERTY( QUrl naturalResourcesIcon READ naturalResourcesIcon CONSTANT )
    Q_PROPERTY( QUrl nextIcon READ nextIcon CONSTANT )
    Q_PROPERTY( QUrl otherIcon READ otherIcon CONSTANT )
    Q_PROPERTY( QUrl othersIcon READ othersIcon CONSTANT )
    Q_PROPERTY( QUrl plusIcon READ plusIcon CONSTANT )
    Q_PROPERTY( QUrl personalIcon READ personalIcon CONSTANT )
    Q_PROPERTY( QUrl projectsIcon READ projectsIcon CONSTANT )
    Q_PROPERTY( QUrl qgisIcon READ qgisIcon CONSTANT )
    Q_PROPERTY( QUrl qrCodeIcon READ qrCodeIcon CONSTANT )
    Q_PROPERTY( QUrl redditIcon READ redditIcon CONSTANT )
    Q_PROPERTY( QUrl satelliteIcon READ satelliteIcon CONSTANT )
    Q_PROPERTY( QUrl searchIcon READ searchIcon CONSTANT )
    Q_PROPERTY( QUrl smallCheckmarkIcon READ smallCheckmarkIcon CONSTANT )
    Q_PROPERTY( QUrl smallEditIcon READ smallEditIcon CONSTANT )
    Q_PROPERTY( QUrl showIcon READ showIcon CONSTANT )
    Q_PROPERTY( QUrl socialMediaIcon READ socialMediaIcon CONSTANT )
    Q_PROPERTY( QUrl stakeOutIcon READ stakeOutIcon CONSTANT )
    Q_PROPERTY( QUrl stateAndLocalIcon READ stateAndLocalIcon CONSTANT )
    Q_PROPERTY( QUrl stopIcon READ stopIcon CONSTANT )
    Q_PROPERTY( QUrl subscriptionsIcon READ subscriptionsIcon CONSTANT )
    Q_PROPERTY( QUrl syncIcon READ syncIcon CONSTANT )
    Q_PROPERTY( QUrl syncGreenIcon READ syncGreenIcon CONSTANT )
    Q_PROPERTY( QUrl teacherIcon READ teacherIcon CONSTANT )
    Q_PROPERTY( QUrl telecommunicationIcon READ telecommunicationIcon CONSTANT )
    Q_PROPERTY( QUrl termsIcon READ termsIcon CONSTANT )
    Q_PROPERTY( QUrl tractorIcon READ tractorIcon CONSTANT )
    Q_PROPERTY( QUrl transportationIcon READ transportationIcon CONSTANT )
    Q_PROPERTY( QUrl undoIcon READ undoIcon CONSTANT )
    Q_PROPERTY( QUrl waitingIcon READ waitingIcon CONSTANT )
    Q_PROPERTY( QUrl waterResourcesIcon READ waterResourcesIcon CONSTANT )
    Q_PROPERTY( QUrl studentIcon READ studentIcon CONSTANT )
    Q_PROPERTY( QUrl xTwitterIcon READ xTwitterIcon CONSTANT )
    Q_PROPERTY( QUrl youtubeIcon READ youtubeIcon CONSTANT )
    Q_PROPERTY( QUrl splitGeometryIcon READ splitGeometryIcon CONSTANT )
    Q_PROPERTY( QUrl streamingIcon READ streamingIcon CONSTANT )
    Q_PROPERTY( QUrl redrawGeometryIcon READ redrawGeometryIcon CONSTANT )
    Q_PROPERTY( QUrl cloudIcon READ cloudIcon CONSTANT )
    Q_PROPERTY( QUrl measurementToolIcon READ measurementToolIcon CONSTANT )
    Q_PROPERTY( QUrl closeShapeIcon READ closeShapeIcon CONSTANT )
    Q_PROPERTY( QUrl formIcon READ formIcon CONSTANT )
    Q_PROPERTY( QUrl drawIcon READ drawIcon CONSTANT )

    // Filled Icons - for visualizing of selected item in toolbar
    Q_PROPERTY( QUrl projectsFilledIcon READ projectsFilledIcon CONSTANT )
    Q_PROPERTY( QUrl globalFilledIcon READ globalFilledIcon CONSTANT )
    Q_PROPERTY( QUrl homeFilledIcon READ homeFilledIcon CONSTANT )
    Q_PROPERTY( QUrl infoFilledIcon READ infoFilledIcon CONSTANT )
    Q_PROPERTY( QUrl featuresFilledIcon READ featuresFilledIcon CONSTANT )

    // Icons with filled circle background
    Q_PROPERTY( QUrl xMarkCircleIcon READ xMarkCircleIcon CONSTANT )
    Q_PROPERTY( QUrl doneCircleIcon READ doneCircleIcon CONSTANT )
    Q_PROPERTY( QUrl errorCircleIcon READ errorCircleIcon CONSTANT )
    Q_PROPERTY( QUrl editCircleIcon READ editCircleIcon CONSTANT )
    Q_PROPERTY( QUrl warningCircleIcon READ warningCircleIcon CONSTANT )

    // Icons for which we do not use color overlay (so they have original color)
    Q_PROPERTY( QUrl lineLayerNoColorOverlayIcon READ lineLayerNoColorOverlayIcon CONSTANT )
    Q_PROPERTY( QUrl polygonLayerNoColorOverlayIcon READ polygonLayerNoColorOverlayIcon CONSTANT )
    Q_PROPERTY( QUrl rasterLayerNoColorOverlayIcon READ rasterLayerNoColorOverlayIcon CONSTANT )
    Q_PROPERTY( QUrl tableLayerNoColorOverlayIcon READ tableLayerNoColorOverlayIcon CONSTANT )
    Q_PROPERTY( QUrl pointLayerNoColorOverlayIcon READ pointLayerNoColorOverlayIcon CONSTANT )
    Q_PROPERTY( QUrl followGPSNoColorOverlayIcon READ followGPSNoColorOverlayIcon CONSTANT )

    // Images
    Q_PROPERTY( QUrl loadingIndicatorImage READ loadingIndicatorImage CONSTANT )
    Q_PROPERTY( QUrl acceptInvitationImage READ acceptInvitationImage CONSTANT )
    Q_PROPERTY( QUrl acceptInvitationLogoImage READ acceptInvitationLogoImage CONSTANT )
    Q_PROPERTY( QUrl reachedDataLimitImage READ reachedDataLimitImage CONSTANT )
    Q_PROPERTY( QUrl uploadImage READ uploadImage CONSTANT )
    Q_PROPERTY( QUrl noMapThemesImage READ noMapThemesImage CONSTANT )
    Q_PROPERTY( QUrl syncFailedImage READ syncFailedImage CONSTANT )
    Q_PROPERTY( QUrl noPermissionsImage READ noPermissionsImage CONSTANT )
    Q_PROPERTY( QUrl signInImage READ signInImage CONSTANT )
    Q_PROPERTY( QUrl warnLogoImage READ warnLogoImage CONSTANT )
    Q_PROPERTY( QUrl mmLogoImage READ mmLogoImage CONSTANT )
    Q_PROPERTY( QUrl lutraLogoImage READ lutraLogoImage CONSTANT )
    Q_PROPERTY( QUrl directionImage READ directionImage CONSTANT )
    Q_PROPERTY( QUrl mapPinImage READ mapPinImage CONSTANT )
    Q_PROPERTY( QUrl trackingDirectionImage READ trackingDirectionImage CONSTANT )
    Q_PROPERTY( QUrl mmSymbolImage READ mmSymbolImage CONSTANT )
    Q_PROPERTY( QUrl positionTrackingRunningImage READ positionTrackingRunningImage CONSTANT )
    Q_PROPERTY( QUrl positionTrackingStartImage READ positionTrackingStartImage CONSTANT )
    Q_PROPERTY( QUrl syncImage READ syncImage CONSTANT )
    Q_PROPERTY( QUrl externalGpsGreenImage READ externalGpsGreenImage CONSTANT )
    Q_PROPERTY( QUrl externalGpsRedImage READ externalGpsRedImage CONSTANT )
    Q_PROPERTY( QUrl negativeMMSymbolImage READ negativeMMSymbolImage CONSTANT )
    Q_PROPERTY( QUrl positiveMMSymbolImage READ positiveMMSymbolImage CONSTANT )
    Q_PROPERTY( QUrl neutralMMSymbolImage READ neutralMMSymbolImage CONSTANT )
    Q_PROPERTY( QUrl closeAccountImage READ closeAccountImage CONSTANT )
    Q_PROPERTY( QUrl attentionImage READ attentionImage CONSTANT )
    Q_PROPERTY( QUrl blueInfoImage READ blueInfoImage CONSTANT )
    Q_PROPERTY( QUrl bubbleImage READ bubbleImage CONSTANT )
    Q_PROPERTY( QUrl streamingBootsImage READ streamingBootsImage CONSTANT )
    Q_PROPERTY( QUrl streamingBootsOrangeImage READ streamingBootsOrangeImage CONSTANT )
    Q_PROPERTY( QUrl noWifiImage READ noWifiImage CONSTANT )
    Q_PROPERTY( QUrl crosshairBackgroundImage READ crosshairBackgroundImage CONSTANT )
    Q_PROPERTY( QUrl crosshairCenterImage READ crosshairCenterImage CONSTANT )
    Q_PROPERTY( QUrl crosshairCircleImage READ crosshairCircleImage CONSTANT )
    Q_PROPERTY( QUrl crosshairForegroundImage READ crosshairForegroundImage CONSTANT )
    Q_PROPERTY( QUrl crosshairPlusImage READ crosshairPlusImage CONSTANT )
    Q_PROPERTY( QUrl noWorkspaceImage READ noWorkspaceImage CONSTANT )

    // Sounds
    Q_PROPERTY( QUrl hapticSound READ hapticSound CONSTANT )

    /*
     * Pixel sizes used across the app
     */

    // Icon sizes
    Q_PROPERTY( double icon16 READ number16 CONSTANT )
    Q_PROPERTY( double icon24 READ number24 CONSTANT )
    Q_PROPERTY( double icon32 READ number32 CONSTANT )

    // Map items
    Q_PROPERTY( double mapItemHeight READ number50 CONSTANT )
    Q_PROPERTY( double mapButtonsMargin READ number20 CONSTANT )

    // Toolbar
    Q_PROPERTY( double toolbarHeight READ toolbarHeight NOTIFY safeAreaBottomChanged )
    Q_PROPERTY( double menuDrawerHeight READ number67 CONSTANT )

    // Safe area sizes - to not draw content over notch and system bars (used on mobile devices)
    Q_PROPERTY( double safeAreaTop READ safeAreaTop WRITE setSafeAreaTop NOTIFY safeAreaTopChanged )
    Q_PROPERTY( double safeAreaRight READ safeAreaRight WRITE setSafeAreaRight NOTIFY safeAreaRightChanged )
    Q_PROPERTY( double safeAreaBottom READ safeAreaBottom WRITE setSafeAreaBottom NOTIFY safeAreaBottomChanged )
    Q_PROPERTY( double safeAreaLeft READ safeAreaLeft WRITE setSafeAreaLeft NOTIFY safeAreaLeftChanged )

    // Margins
    Q_PROPERTY( double margin1 READ number1 CONSTANT )
    Q_PROPERTY( double margin2 READ number2 CONSTANT )
    Q_PROPERTY( double margin4 READ number4 CONSTANT )
    Q_PROPERTY( double margin6 READ number6 CONSTANT )
    Q_PROPERTY( double margin8 READ number8 CONSTANT )
    Q_PROPERTY( double margin10 READ number10 CONSTANT )
    Q_PROPERTY( double margin11 READ number11 CONSTANT )
    Q_PROPERTY( double margin12 READ number12 CONSTANT )
    Q_PROPERTY( double margin13 READ number13 CONSTANT )
    Q_PROPERTY( double margin14 READ number14 CONSTANT )
    Q_PROPERTY( double margin16 READ number16 CONSTANT )
    Q_PROPERTY( double margin20 READ number20 CONSTANT )
    Q_PROPERTY( double margin28 READ number28 CONSTANT )
    Q_PROPERTY( double margin30 READ number30 CONSTANT )
    Q_PROPERTY( double margin32 READ number32 CONSTANT )
    Q_PROPERTY( double margin36 READ number36 CONSTANT )
    Q_PROPERTY( double margin40 READ number40 CONSTANT )
    Q_PROPERTY( double margin48 READ number48 CONSTANT )
    Q_PROPERTY( double margin54 READ number54 CONSTANT )

    // Page
    Q_PROPERTY( double pageMargins READ number20 CONSTANT ) // distance between screen edge and components
    Q_PROPERTY( double spacing2 READ number2 CONSTANT )
    Q_PROPERTY( double spacing5 READ number5 CONSTANT )
    Q_PROPERTY( double spacing10 READ number10 CONSTANT )
    Q_PROPERTY( double spacing12 READ number12 CONSTANT ) // distance between page header, page content and page footer
    Q_PROPERTY( double spacing20 READ number20 CONSTANT )
    Q_PROPERTY( double spacing30 READ number30 CONSTANT )
    Q_PROPERTY( double spacing40 READ number40 CONSTANT )
    Q_PROPERTY( double maxPageWidth READ number720 CONSTANT ) // maximum page width (desktop, tablets, landscape)

    // Other
    Q_PROPERTY( double row1 READ number1 CONSTANT )
    Q_PROPERTY( double row4 READ number4 CONSTANT )
    Q_PROPERTY( double row24 READ number24 CONSTANT )
    Q_PROPERTY( double row36 READ number36 CONSTANT )
    Q_PROPERTY( double row40 READ number40 CONSTANT )
    Q_PROPERTY( double row45 READ number45 CONSTANT )
    Q_PROPERTY( double row49 READ number49 CONSTANT )
    Q_PROPERTY( double row50 READ number50 CONSTANT )
    Q_PROPERTY( double row54 READ number54 CONSTANT )
    Q_PROPERTY( double row60 READ number60 CONSTANT )
    Q_PROPERTY( double row63 READ number63 CONSTANT )
    Q_PROPERTY( double row67 READ number67 CONSTANT )
    Q_PROPERTY( double row80 READ number80 CONSTANT )
    Q_PROPERTY( double row114 READ number114 CONSTANT )
    Q_PROPERTY( double row120 READ number120 CONSTANT )
    Q_PROPERTY( double row160 READ number160 CONSTANT )
    Q_PROPERTY( double radius2 READ number2 CONSTANT )
    Q_PROPERTY( double radius6 READ number6 CONSTANT )
    Q_PROPERTY( double radius8 READ number8 CONSTANT )
    Q_PROPERTY( double radius12 READ number12 CONSTANT )
    Q_PROPERTY( double radius16 READ number16 CONSTANT )
    Q_PROPERTY( double radius20 READ number20 CONSTANT )
    Q_PROPERTY( double radius30 READ number30 CONSTANT )
    Q_PROPERTY( double radius40 READ number40 CONSTANT )
    Q_PROPERTY( double width1 READ number1 CONSTANT )
    Q_PROPERTY( double width2 READ number2 CONSTANT )
    Q_PROPERTY( double scrollVelocityAndroid READ scrollVelocityAndroid CONSTANT ) // [px/s] scrolling on Android devices is too slow by default

    // Breakpoint we use in some screens to differentiate mobile landscape
    Q_PROPERTY( double heightBreakpointXS READ number400 CONSTANT )

  public:
    explicit MMStyle( QObject *parent,  qreal dp )
      : QObject( parent ), mDp( dp )
    {}
    ~MMStyle() = default;

    QFont h1() const {return fontFactory( 48, true );}
    QFont h2() const {return fontFactory( 36, true );}
    QFont h3() const {return fontFactory( 24, true );}

    QFont t1() const {return fontFactory( 18, true );}
    QFont t2() const {return fontFactory( 16, true );}
    QFont t3() const {return fontFactory( 14, true );}
    QFont t4() const {return fontFactory( 12, true );}
    QFont t5() const {return fontFactory( 10, true );}

    QFont p1() const {return fontFactory( 32, false );}
    QFont p2() const {return fontFactory( 24, false );}
    QFont p3() const {return fontFactory( 20, false );}
    QFont p4() const {return fontFactory( 16, false );}
    QFont p5() const {return fontFactory( 14, false );}
    QFont p6() const {return fontFactory( 12, false );}
    QFont p7() const {return fontFactory( 10, false );}

    QColor grassColor() const {return QColor::fromString( "#73D19C" );}
    QColor forestColor() const {return QColor::fromString( "#004C45" );}
    QColor nightColor() const {return QColor::fromString( "#12181F" );}
    QColor polarColor() const {return QColor::fromString( "#FFFFFF" );}
    QColor transparentColor() const {return QColor::fromString( "transparent" );}

    QColor lightGreenColor() const {return QColor::fromString( "#EFF5F3" );}
    QColor mediumGreenColor() const {return QColor::fromString( "#B7CDC4" );}
    QColor darkGreenColor() const {return QColor::fromString( "#6E9991" );}
    QColor greyColor() const {return QColor::fromString( "#E2E2E2" );}
    QColor darkGreyColor() const {return QColor::fromString( "#41464C" );}
    QColor mediumGreyColor() const {return QColor::fromString( "#A0A3A5" );}

    QColor sandColor() const {return QColor::fromString( "#FFF4E2" );}
    QColor sunsetColor() const {return QColor::fromString( "#FFB673" );}
    QColor sunColor() const {return QColor::fromString( "#F4CB46" );}
    QColor earthColor() const {return QColor::fromString( "#4D2A24" );}
    QColor roseColor() const {return QColor::fromString( "#FFBABC" );}
    QColor skyColor() const {return QColor::fromString( "#A6CBF4" );}
    QColor grapeColor() const {return QColor::fromString( "#5A2740" );}
    QColor grapeTransparentColor() const {return QColor( 0x5A, 0x27, 0x40, 102 );}
    QColor deepOceanColor() const {return QColor::fromString( "#1C324A" );}
    QColor purpleColor() const {return QColor::fromString( "#CCBDF5" );}
    QColor fieldColor() const {return QColor::fromString( "#9BD1A9" );}

    QColor positiveColor() const {return QColor::fromString( "#C0EBCF" );}
    QColor warningColor() const {return QColor::fromString( "#F7DDAF" );}
    QColor negativeColor() const {return QColor::fromString( "#F0C4BC" );}
    QColor negativeLightColor() const {return QColor::fromString( "#FFF0ED" );}
    QColor negativeUltraLightColor() const {return QColor::fromString( "#FEFAF9" );}
    QColor informativeColor() const {return QColor::fromString( "#BEDAF0" );}
    QColor snappingColor() const {return QColor::fromString( "#BD74FF" );}

    QColor photoSketchingBlackColor() const {return QColor::fromString( "#12181F" );}
    QColor photoSketchingWhiteColor() const {return QColor::fromString( "#FFFFFF" );}
    QColor photoSketchingGreenColor() const {return QColor::fromString( "#57B46F" );}
    QColor photoSketchingYellowColor() const {return QColor::fromString( "#FDCB2A" );}
    QColor photoSketchingOrangeColor() const {return QColor::fromString( "#FF9C40" );}
    QColor photoSketchingBlueColor() const {return QColor::fromString( "#5E9EE4" );}
    QColor photoSketchingPinkColor() const {return QColor::fromString( "#FF8F93" );}

    QColor shadowColor() const {return QColor::fromString( "#66777777" );}

    QUrl splitGeometryIcon() const {return QUrl( "qrc:/SplitGeometry.svg" );}
    QUrl streamingIcon() const {return QUrl( "qrc:/Streaming.svg" );}
    QUrl redrawGeometryIcon() const {return QUrl( "qrc:/RedrawGeometry.svg" );}
    QUrl cloudIcon() const {return QUrl( "qrc:/Cloud.svg" );}
    QUrl linkIcon() const {return QUrl( "qrc:/Link.svg" );}
    QUrl closeAccountIcon() const {return QUrl( "qrc:/CloseAccount.svg" );}
    QUrl signOutIcon() const {return QUrl( "qrc:/SignOut.svg" );}
    QUrl workspacesIcon() const {return QUrl( "qrc:/Workspaces.svg" );}
    QUrl addIcon() const {return QUrl( "qrc:/Add.svg" );}
    QUrl layersIcon() const {return QUrl( "qrc:/Layers.svg" );}
    QUrl localChangesIcon() const {return QUrl( "qrc:/LocalChanges.svg" );}
    QUrl mapThemesIcon() const {return QUrl( "qrc:/MapThemes.svg" );}
    QUrl positionTrackingIcon() const {return QUrl( "qrc:/PositionTracking.svg" );}
    QUrl settingsIcon() const {return QUrl( "qrc:/Settings.svg" );}
    QUrl zoomToProjectIcon() const {return QUrl( "qrc:/ZoomToProject.svg" );}
    QUrl addImageIcon() const {return QUrl( "qrc:/AddImage.svg" );}
    QUrl archaeologyIcon() const {return QUrl( "qrc:/Archaeology.svg" );}
    QUrl arrowDownIcon() const {return QUrl( "qrc:/ArrowDown.svg" );}
    QUrl arrowLinkRightIcon() const {return QUrl( "qrc:/ArrowLinkRight.svg" );}
    QUrl arrowUpIcon() const {return QUrl( "qrc:/ArrowUp.svg" );}
    QUrl backIcon() const {return QUrl( "qrc:/Back.svg" );}
    QUrl briefcaseIcon() const {return QUrl( "qrc:/Briefcase.svg" );}
    QUrl calendarIcon() const {return QUrl( "qrc:/Calendar.svg" );}
    QUrl checkmarkIcon() const {return QUrl( "qrc:/Checkmark.svg" );}
    QUrl closeIcon() const {return QUrl( "qrc:/Close.svg" );}
    QUrl deleteIcon() const {return QUrl( "qrc:/Delete.svg" );}
    QUrl featuresIcon() const {return QUrl( "qrc:/Features.svg" );}
    QUrl downloadIcon() const {return QUrl( "qrc:/Download.svg" );}
    QUrl uploadIcon() const {return QUrl( "qrc:/Upload.svg" );}
    QUrl editIcon() const {return QUrl( "qrc:/Edit.svg" );}
    QUrl electricityIcon() const {return QUrl( "qrc:/Electricity.svg" );}
    QUrl engineeringIcon() const {return QUrl( "qrc:/Engineering.svg" );}
    QUrl environmentalIcon() const {return QUrl( "qrc:/Environmental.svg" );}
    QUrl facebookIcon() const {return QUrl( "qrc:/Facebook.svg" );}
    QUrl globeIcon() const {return QUrl( "qrc:/Globe.svg" );}
    QUrl globalIcon() const {return QUrl( "qrc:/Global.svg" );}
    QUrl gpsIcon() const {return QUrl( "qrc:/GPSIcon.svg" );}
    QUrl gpsAntennaHeightIcon() const {return QUrl( "qrc:/GPSAntennaHeight.svg" );}
    QUrl hideIcon() const {return QUrl( "qrc:/Hide.svg" );}
    QUrl homeIcon() const {return QUrl( "qrc:/Home.svg" );}
    QUrl infoIcon() const {return QUrl( "qrc:/Info.svg" );}
    QUrl linkedinIcon() const {return QUrl( "qrc:/Linkedin.svg" );}
    QUrl mastodonIcon() const {return QUrl( "qrc:/Mastodon.svg" );}
    QUrl minusIcon() const {return QUrl( "qrc:/Minus.svg" );}
    QUrl moreIcon() const {return QUrl( "qrc:/More.svg" );}
    QUrl moreVerticalIcon() const {return QUrl( "qrc:/MoreVertical.svg" );}
    QUrl morePhotosIcon() const {return QUrl( "qrc:/MorePhotos.svg" );}
    QUrl remoteImageLoadErrorIcon() const {return QUrl( "qrc:/RemoteImageLoadError.svg" );}
    QUrl mouthIcon() const {return QUrl( "qrc:/Mouth.svg" );}
    QUrl measurementToolIcon() const {return QUrl( "qrc:/Measure.svg" );}
    QUrl closeShapeIcon() const {return QUrl( "qrc:/CloseShape.svg" );}
    QUrl naturalResourcesIcon() const {return QUrl( "qrc:/NaturalResources.svg" );}
    QUrl nextIcon() const {return QUrl( "qrc:/Next.svg" );}
    QUrl otherIcon() const {return QUrl( "qrc:/Other.svg" );}
    QUrl othersIcon() const {return QUrl( "qrc:/Others.svg" );}
    QUrl plusIcon() const {return QUrl( "qrc:/Plus.svg" );}
    QUrl personalIcon() const {return QUrl( "qrc:/Personal.svg" );}
    QUrl projectsIcon() const {return QUrl( "qrc:/Projects.svg" );}
    QUrl qgisIcon() const {return QUrl( "qrc:/QGIS.svg" );}
    QUrl qrCodeIcon() const {return QUrl( "qrc:/QRCode.svg" );}
    QUrl redditIcon() const {return QUrl( "qrc:/Reddit.svg" );}
    QUrl satelliteIcon() const {return QUrl( "qrc:/GPSSatellite.svg" );}
    QUrl searchIcon() const {return QUrl( "qrc:/Search.svg" );}
    QUrl showIcon() const {return QUrl( "qrc:/Show.svg" );}
    QUrl smallCheckmarkIcon() const {return QUrl( "qrc:/SmallCheckmark.svg" );}
    QUrl smallEditIcon() const {return QUrl( "qrc:/SmallEdit.svg" );}
    QUrl socialMediaIcon() const {return QUrl( "qrc:/SocialMedia.svg" );}
    QUrl stakeOutIcon() const {return QUrl( "qrc:/StakeOut.svg" );}
    QUrl stateAndLocalIcon() const {return QUrl( "qrc:/StateAndLocal.svg" );}
    QUrl stopIcon() const {return QUrl( "qrc:/Stop.svg" );}
    QUrl subscriptionsIcon() const {return QUrl( "qrc:/Subscriptions.svg" );}
    QUrl syncIcon() const {return QUrl( "qrc:/Sync.svg" );}
    QUrl syncGreenIcon() const {return QUrl( "qrc:/SyncGreen.svg" );}
    QUrl teacherIcon() const {return QUrl( "qrc:/Teacher.svg" );}
    QUrl telecommunicationIcon() const {return QUrl( "qrc:/Telecommunication.svg" );}
    QUrl termsIcon() const {return QUrl( "qrc:/Terms.svg" );}
    QUrl tractorIcon() const {return QUrl( "qrc:/Tractor.svg" );}
    QUrl transportationIcon() const {return QUrl( "qrc:/Transportation.svg" );}
    QUrl undoIcon() const {return QUrl( "qrc:/Undo.svg" );}
    QUrl waitingIcon() const {return QUrl( "qrc:/Waiting.svg" );}
    QUrl waterResourcesIcon() const {return QUrl( "qrc:/WaterResources.svg" );}
    QUrl studentIcon() const {return QUrl( "qrc:/Student.svg" );}
    QUrl xTwitterIcon() const {return QUrl( "qrc:/XTwitter.svg" );}
    QUrl youtubeIcon() const {return QUrl( "qrc:/Youtube.svg" );}
    QUrl formIcon() const {return QUrl( "qrc:/Terms.svg" );}
    QUrl drawIcon() const {return QUrl( "qrc:/Draw.svg" );}

    QUrl projectsFilledIcon() const {return QUrl( "qrc:/ProjectsFilled.svg" );}
    QUrl globalFilledIcon() const {return QUrl( "qrc:/GlobalFilled.svg" );}
    QUrl infoFilledIcon() const {return QUrl( "qrc:/InfoFilled.svg" );}
    QUrl featuresFilledIcon() const {return QUrl( "qrc:/FeaturesFilled.svg" );}
    QUrl homeFilledIcon() const {return QUrl( "qrc:/HomeFilled.svg" );}

    QUrl xMarkCircleIcon() const {return QUrl( "qrc:/XMarkCircle.svg" );}
    QUrl doneCircleIcon() const {return QUrl( "qrc:/DoneCircle.svg" );}
    QUrl errorCircleIcon() const {return QUrl( "qrc:/ErrorCircle.svg" );}
    QUrl warningCircleIcon() const {return QUrl( "qrc:/WarningCircle.svg" );}
    QUrl editCircleIcon() const {return QUrl( "qrc:/EditCircle.svg" );}

    static QUrl lineLayerNoColorOverlayIcon() {return QUrl( "qrc:/mIconLineLayer-nocoloroverlay.svg" );}
    static QUrl pointLayerNoColorOverlayIcon() {return QUrl( "qrc:/mIconPointLayer-nocoloroverlay.svg" );}
    static QUrl polygonLayerNoColorOverlayIcon() {return QUrl( "qrc:/mIconPolygonLayer-nocoloroverlay.svg" );}
    static QUrl rasterLayerNoColorOverlayIcon() {return QUrl( "qrc:/mIconRasterLayer-nocoloroverlay.svg" );}
    static QUrl tableLayerNoColorOverlayIcon() {return QUrl( "qrc:/mIconTableLayer-nocoloroverlay.svg" );}
    static QUrl followGPSNoColorOverlayIcon() {return QUrl( "qrc:/FollowGPS-nocoloroverlay.svg" );}

    QUrl loadingIndicatorImage() const {return QUrl( "qrc:/images/LoadingIndicator.svg" );}
    QUrl mmLogoImage() const {return QUrl( "qrc:/images/MMLogo.svg" );}
    QUrl lutraLogoImage() const {return QUrl( "qrc:/images/LutraLogo.svg" );}
    QUrl trackingDirectionImage() const {return QUrl( "qrc:/images/TrackingDirection.svg" );}
    QUrl directionImage() const {return QUrl( "qrc:/images/Direction.svg" );}
    QUrl mmSymbolImage() const {return QUrl( "qrc:/images/MMSymbol.svg" );}
    QUrl acceptInvitationLogoImage() const {return QUrl( "qrc:/images/AcceptInvitationLogoImage.svg" ); }
    QUrl acceptInvitationImage() const {return QUrl( "qrc:/images/AcceptInvitationImage.svg" ); }
    QUrl uploadImage() const {return QUrl( "qrc:/images/UploadImage.svg" );}
    QUrl noMapThemesImage() const {return QUrl( "qrc:/images/NoMapThemesImage.svg" );}
    QUrl syncFailedImage() const {return QUrl( "qrc:/images/SyncFailed.svg" );}
    QUrl noPermissionsImage() const {return QUrl( "qrc:/images/NoPermissions.svg" );}
    QUrl signInImage() const {return QUrl( "qrc:/images/SignIn.svg" );}
    QUrl reachedDataLimitImage() const {return QUrl( "qrc:/images/ReachedDataLimit.svg" );}
    QUrl warnLogoImage() const {return QUrl( "qrc:/images/WarnLogoImage.svg" );}
    QUrl mapPinImage() const {return QUrl( "qrc:/images/MapPin.svg" );}
    QUrl positionTrackingRunningImage() const {return QUrl( "qrc:/images/PositionTrackingRunning.svg" );}
    QUrl positionTrackingStartImage() const {return QUrl( "qrc:/images/PositionTrackingStart.svg" );}
    QUrl syncImage() const {return QUrl( "qrc:/images/SyncImage.svg" );}
    QUrl externalGpsGreenImage() const {return QUrl( "qrc:/images/ExternalGpsGreen.svg" );}
    QUrl externalGpsRedImage() const {return QUrl( "qrc:/images/ExternalGpsRed.svg" );}
    QUrl negativeMMSymbolImage() const {return QUrl( "qrc:/images/NegativeMMSymbol.svg" );}
    QUrl positiveMMSymbolImage() const {return QUrl( "qrc:/images/PositiveMMSymbol.svg" );}
    QUrl neutralMMSymbolImage() const {return QUrl( "qrc:/images/NeutralMMSymbol.svg" );}
    QUrl closeAccountImage() const {return QUrl( "qrc:/images/CloseAccount.svg" );}
    QUrl attentionImage() const {return QUrl( "qrc:/images/Attention.svg" );}
    QUrl blueInfoImage() const {return QUrl( "qrc:/images/BlueInfo.svg" );}
    QUrl bubbleImage() const {return QUrl( "qrc:/images/Bubble.svg" );}
    QUrl streamingBootsImage() const {return QUrl( "qrc:/images/StreamingBoots.svg" );}
    QUrl streamingBootsOrangeImage() const {return QUrl( "qrc:/images/StreamingBootsOrange.svg" );}
    QUrl noWifiImage() const {return QUrl( "qrc:/images/NoWifi.svg" );}
    QUrl crosshairBackgroundImage() const {return QUrl( "qrc:/images/CrosshairBackground.svg" );}
    QUrl crosshairCenterImage() const {return QUrl( "qrc:/images/CrosshairCenter.svg" );}
    QUrl crosshairCircleImage() const {return QUrl( "qrc:/images/CrosshairCircle.svg" );}
    QUrl crosshairForegroundImage() const {return QUrl( "qrc:/images/CrosshairForeground.svg" );}
    QUrl crosshairPlusImage() const {return QUrl( "qrc:/images/CrosshairPlus.svg" );}
    QUrl noWorkspaceImage() const {return QUrl( "qrc:/images/NoWorkspace.svg" );}

    QUrl hapticSound() {return QUrl( "qrc:/sounds/haptic_sound.wav" );}

    double toolbarHeight() const
    {
      if ( mSafeAreaBottom > 2 )
      {
        return 54 * mDp;
      }
      else
      {
        return 58 * mDp;
      }
    }

    double safeAreaTop() const { return mSafeAreaTop; };
    double safeAreaRight() const { return mSafeAreaRight; };
    double safeAreaBottom() const { return mSafeAreaBottom; };
    double safeAreaLeft() const { return mSafeAreaLeft; };

    // Default (startup) window sizes on desktop platforms
    const int DEFAULT_WINDOW_X = 0;
    const int DEFAULT_WINDOW_Y = 0;
    const int DEFAULT_WINDOW_WIDTH = 640;
    const int DEFAULT_WINDOW_HEIGHT = 1136;

    //Numbers from 1 to 150 and number 720
    double number1() const {return qMax<double>( 1 * mDp, 1 ); } // make sure these sizes are visible if mDP < 1
    double number2() const {return qMax<double>( 2 * mDp, 1 ); } // make sure these sizes are visible if mDP < 1
    double number3() const {return 3 * mDp;}
    double number4() const {return 4 * mDp;}
    double number5() const {return 5 * mDp;}
    double number6() const {return 6 * mDp;}
    double number7() const {return 7 * mDp;}
    double number8() const {return 8 * mDp;}
    double number9() const {return 9 * mDp;}
    double number10() const {return 10 * mDp;}
    double number11() const {return 11 * mDp;}
    double number12() const {return 12 * mDp;}
    double number13() const {return 13 * mDp;}
    double number14() const {return 14 * mDp;}
    double number15() const {return 15 * mDp;}
    double number16() const {return 16 * mDp;}
    double number17() const {return 17 * mDp;}
    double number18() const {return 18 * mDp;}
    double number19() const {return 19 * mDp;}
    double number20() const {return 20 * mDp;}
    double number21() const {return 21 * mDp;}
    double number22() const {return 22 * mDp;}
    double number23() const {return 23 * mDp;}
    double number24() const {return 24 * mDp;}
    double number25() const {return 25 * mDp;}
    double number26() const {return 26 * mDp;}
    double number27() const {return 27 * mDp;}
    double number28() const {return 28 * mDp;}
    double number29() const {return 29 * mDp;}
    double number30() const {return 30 * mDp;}
    double number31() const {return 31 * mDp;}
    double number32() const {return 32 * mDp;}
    double number33() const {return 33 * mDp;}
    double number34() const {return 34 * mDp;}
    double number35() const {return 35 * mDp;}
    double number36() const {return 36 * mDp;}
    double number37() const {return 37 * mDp;}
    double number38() const {return 38 * mDp;}
    double number39() const {return 39 * mDp;}
    double number40() const {return 40 * mDp;}
    double number41() const {return 41 * mDp;}
    double number42() const {return 42 * mDp;}
    double number43() const {return 43 * mDp;}
    double number44() const {return 44 * mDp;}
    double number45() const {return 45 * mDp;}
    double number46() const {return 46 * mDp;}
    double number47() const {return 47 * mDp;}
    double number48() const {return 48 * mDp;}
    double number49() const {return 49 * mDp;}
    double number50() const {return 50 * mDp;}
    double number51() const {return 51 * mDp;}
    double number52() const {return 52 * mDp;}
    double number53() const {return 53 * mDp;}
    double number54() const {return 54 * mDp;}
    double number55() const {return 55 * mDp;}
    double number56() const {return 56 * mDp;}
    double number57() const {return 57 * mDp;}
    double number58() const {return 58 * mDp;}
    double number59() const {return 59 * mDp;}
    double number60() const {return 60 * mDp;}
    double number61() const {return 61 * mDp;}
    double number62() const {return 62 * mDp;}
    double number63() const {return 63 * mDp;}
    double number64() const {return 64 * mDp;}
    double number65() const {return 65 * mDp;}
    double number66() const {return 66 * mDp;}
    double number67() const {return 67 * mDp;}
    double number68() const {return 68 * mDp;}
    double number69() const {return 69 * mDp;}
    double number70() const {return 70 * mDp;}
    double number71() const {return 71 * mDp;}
    double number72() const {return 72 * mDp;}
    double number73() const {return 73 * mDp;}
    double number74() const {return 74 * mDp;}
    double number75() const {return 75 * mDp;}
    double number76() const {return 76 * mDp;}
    double number77() const {return 77 * mDp;}
    double number78() const {return 78 * mDp;}
    double number79() const {return 79 * mDp;}
    double number80() const {return 80 * mDp;}
    double number81() const {return 81 * mDp;}
    double number82() const {return 82 * mDp;}
    double number83() const {return 83 * mDp;}
    double number84() const {return 84 * mDp;}
    double number85() const {return 85 * mDp;}
    double number86() const {return 86 * mDp;}
    double number87() const {return 87 * mDp;}
    double number88() const {return 88 * mDp;}
    double number89() const {return 89 * mDp;}
    double number90() const {return 90 * mDp;}
    double number91() const {return 91 * mDp;}
    double number92() const {return 92 * mDp;}
    double number93() const {return 93 * mDp;}
    double number94() const {return 94 * mDp;}
    double number95() const {return 95 * mDp;}
    double number96() const {return 96 * mDp;}
    double number97() const {return 97 * mDp;}
    double number98() const {return 98 * mDp;}
    double number99() const {return 99 * mDp;}
    double number100() const {return 100 * mDp;}
    double number101() const {return 101 * mDp;}
    double number102() const {return 102 * mDp;}
    double number103() const {return 103 * mDp;}
    double number104() const {return 104 * mDp;}
    double number105() const {return 105 * mDp;}
    double number106() const {return 106 * mDp;}
    double number107() const {return 107 * mDp;}
    double number108() const {return 108 * mDp;}
    double number109() const {return 109 * mDp;}
    double number110() const {return 110 * mDp;}
    double number111() const {return 111 * mDp;}
    double number112() const {return 112 * mDp;}
    double number113() const {return 113 * mDp;}
    double number114() const {return 114 * mDp;}
    double number115() const {return 115 * mDp;}
    double number116() const {return 116 * mDp;}
    double number117() const {return 117 * mDp;}
    double number118() const {return 118 * mDp;}
    double number119() const {return 119 * mDp;}
    double number120() const {return 120 * mDp;}
    double number121() const {return 121 * mDp;}
    double number122() const {return 122 * mDp;}
    double number123() const {return 123 * mDp;}
    double number124() const {return 124 * mDp;}
    double number125() const {return 125 * mDp;}
    double number126() const {return 126 * mDp;}
    double number127() const {return 127 * mDp;}
    double number128() const {return 128 * mDp;}
    double number129() const {return 129 * mDp;}
    double number130() const {return 130 * mDp;}
    double number131() const {return 131 * mDp;}
    double number132() const {return 132 * mDp;}
    double number133() const {return 133 * mDp;}
    double number134() const {return 134 * mDp;}
    double number135() const {return 135 * mDp;}
    double number136() const {return 136 * mDp;}
    double number137() const {return 137 * mDp;}
    double number138() const {return 138 * mDp;}
    double number139() const {return 139 * mDp;}
    double number140() const {return 140 * mDp;}
    double number141() const {return 141 * mDp;}
    double number142() const {return 142 * mDp;}
    double number143() const {return 143 * mDp;}
    double number144() const {return 144 * mDp;}
    double number145() const {return 145 * mDp;}
    double number146() const {return 146 * mDp;}
    double number147() const {return 147 * mDp;}
    double number148() const {return 148 * mDp;}
    double number149() const {return 149 * mDp;}
    double number150() const {return 150 * mDp;}
    double number160() const {return 160 * mDp;}
    double number250() const {return 250 * mDp;}
    double number400() const {return 400 * mDp;}
    double number720() const {return 720 * mDp;}

    double scrollVelocityAndroid() const { return 10000; }

    void setSafeAreaTop( double newSafeAreaTop )
    {
      if ( qFuzzyCompare( mSafeAreaTop, newSafeAreaTop ) )
        return;
      mSafeAreaTop = newSafeAreaTop;
      emit safeAreaTopChanged( mSafeAreaTop );
    }

    void setSafeAreaRight( double newSafeAreaRight )
    {
      if ( qFuzzyCompare( mSafeAreaRight, newSafeAreaRight ) )
        return;
      mSafeAreaRight = newSafeAreaRight;
      emit safeAreaRightChanged( mSafeAreaRight );
    }

    void setSafeAreaBottom( double newSafeAreaBottom )
    {
      if ( qFuzzyCompare( mSafeAreaBottom, newSafeAreaBottom ) )
        return;

      mSafeAreaBottom = newSafeAreaBottom;
      emit safeAreaBottomChanged( mSafeAreaBottom );
    }

    void setSafeAreaLeft( double newSafeAreaLeft )
    {
      if ( qFuzzyCompare( mSafeAreaLeft, newSafeAreaLeft ) )
        return;
      mSafeAreaLeft = newSafeAreaLeft;
      emit safeAreaLeftChanged( mSafeAreaLeft );
    }

  signals:
    void styleChanged();

    void safeAreaTopChanged( double safeAreaTop );
    void safeAreaRightChanged( double safeAreaRight );
    void safeAreaBottomChanged( double safeAreaBottom );
    void safeAreaLeftChanged( double safeAreaLeft );

  private:
    QFont fontFactory( int pixelSize, bool bold ) const
    {
      QFont f;
      f.setPixelSize( pixelSize * mDp );

      if ( bold )
      {
        f.setWeight( QFont::DemiBold );
      }

      return f;
    }

    qreal mDp;

    double mSafeAreaTop = 0;
    double mSafeAreaRight = 0;
    double mSafeAreaBottom = 0;
    double mSafeAreaLeft = 0;
};

#endif // MMSTYLE_H
