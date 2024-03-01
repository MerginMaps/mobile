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

    // Fonts - how to use
    // standard - font: __style.p5
    // extended - font.pixelSize: __style.p5.pixelSize; font.italic: true

    // Fonts - Heading
    Q_PROPERTY( QFont h1 READ h1 CONSTANT )
    Q_PROPERTY( QFont h2 READ h2 CONSTANT )
    Q_PROPERTY( QFont h3 READ h3 CONSTANT )

    // Fonts - Title
    Q_PROPERTY( QFont t1 READ t1 CONSTANT )
    Q_PROPERTY( QFont t2 READ t2 CONSTANT )
    Q_PROPERTY( QFont t3 READ t3 CONSTANT )
    Q_PROPERTY( QFont t4 READ t4 CONSTANT )
    Q_PROPERTY( QFont t5 READ t5 CONSTANT )

    // Fonts - Paragraph
    Q_PROPERTY( QFont p1 READ p1 CONSTANT )
    Q_PROPERTY( QFont p2 READ p2 CONSTANT )
    Q_PROPERTY( QFont p3 READ p3 CONSTANT )
    Q_PROPERTY( QFont p4 READ p4 CONSTANT )
    Q_PROPERTY( QFont p5 READ p5 CONSTANT )
    Q_PROPERTY( QFont p6 READ p6 CONSTANT )
    Q_PROPERTY( QFont p7 READ p7 CONSTANT )

    // Colors - primary palette
    Q_PROPERTY( QColor grassColor READ grassColor CONSTANT )
    Q_PROPERTY( QColor forestColor READ forestColor CONSTANT )
    Q_PROPERTY( QColor nightColor READ nightColor CONSTANT )
    Q_PROPERTY( QColor whiteColor READ whiteColor CONSTANT )
    Q_PROPERTY( QColor transparentColor READ transparentColor CONSTANT )

    // Colors - secondary palette
    Q_PROPERTY( QColor lightGreenColor READ lightGreenColor CONSTANT )
    Q_PROPERTY( QColor mediumGreenColor READ mediumGreenColor CONSTANT )
    Q_PROPERTY( QColor greyColor READ greyColor CONSTANT )

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
    Q_PROPERTY( QColor informativeColor READ informativeColor CONSTANT )

    // Colors - others
    Q_PROPERTY( QColor nightAlphaColor READ nightAlphaColor CONSTANT ) // placeholder input color
    Q_PROPERTY( QColor errorBgInputColor READ errorBgInputColor CONSTANT ) // error bg input color
    Q_PROPERTY( QColor shadowColor READ shadowColor CONSTANT )

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
    Q_PROPERTY( QUrl xTwitterIcon READ xTwitterIcon CONSTANT )
    Q_PROPERTY( QUrl youtubeIcon READ youtubeIcon CONSTANT )

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

    // Images
    Q_PROPERTY( QUrl acceptInvitationImage READ acceptInvitationImage CONSTANT )
    Q_PROPERTY( QUrl acceptInvitationLogoImage READ acceptInvitationLogoImage CONSTANT )
    Q_PROPERTY( QUrl reachedDataLimitImage READ reachedDataLimitImage CONSTANT )
    Q_PROPERTY( QUrl uploadImage READ uploadImage CONSTANT )
    Q_PROPERTY( QUrl noMapThemesImage READ noMapThemesImage CONSTANT )
    Q_PROPERTY( QUrl warnLogoImage READ warnLogoImage CONSTANT )
    Q_PROPERTY( QUrl mmLogoImage READ mmLogoImage CONSTANT )
    Q_PROPERTY( QUrl lutraLogoImage READ lutraLogoImage CONSTANT )
    Q_PROPERTY( QUrl directionImage READ directionImage CONSTANT )
    Q_PROPERTY( QUrl mapPinImage READ mapPinImage CONSTANT )
    Q_PROPERTY( QUrl trackingDirectionImage READ trackingDirectionImage CONSTANT )
    Q_PROPERTY( QUrl mmSymbolImage READ mmSymbolImage CONSTANT )
    Q_PROPERTY( QUrl positionTrackingRunningImage READ positionTrackingRunningImage CONSTANT )
    Q_PROPERTY( QUrl positionTrackingStartImage READ positionTrackingStartImage CONSTANT )
    Q_PROPERTY( QUrl lineLayerImage READ lineLayerImage CONSTANT )
    Q_PROPERTY( QUrl polygonLayerImage READ polygonLayerImage CONSTANT )
    Q_PROPERTY( QUrl rasterLayerImage READ rasterLayerImage CONSTANT )
    Q_PROPERTY( QUrl tableLayerImage READ tableLayerImage CONSTANT )
    Q_PROPERTY( QUrl pointLayerImage READ pointLayerImage CONSTANT )
    Q_PROPERTY( QUrl syncImage READ syncImage CONSTANT )
    Q_PROPERTY( QUrl externalGpsGreenImage READ externalGpsGreenImage CONSTANT )
    Q_PROPERTY( QUrl externalGpsRedImage READ externalGpsRedImage CONSTANT )
    Q_PROPERTY( QUrl negativeMMSymbolImage READ negativeMMSymbolImage CONSTANT )
    Q_PROPERTY( QUrl positiveMMSymbolImage READ positiveMMSymbolImage CONSTANT )
    Q_PROPERTY( QUrl closeAccountImage READ closeAccountImage CONSTANT )
    Q_PROPERTY( QUrl attentionImage READ attentionImage CONSTANT )
    Q_PROPERTY( QUrl bubbleImage READ bubbleImage CONSTANT )

    /*
     * Pixel sizes used across the app
     */

    // Icon sizes
    Q_PROPERTY( double icon16 READ icon16 CONSTANT )
    Q_PROPERTY( double icon24 READ icon24 CONSTANT )
    Q_PROPERTY( double icon32 READ icon32 CONSTANT )

    // Map items
    Q_PROPERTY( double mapItemHeight READ mapItemHeight CONSTANT )
    Q_PROPERTY( double mapButtonsMargin READ margin20 CONSTANT )

    // Toolbar
    Q_PROPERTY( double toolbarHeight READ toolbarHeight CONSTANT )
    Q_PROPERTY( double menuDrawerHeight READ menuDrawerHeight CONSTANT )

    // Safe area sizes - to not draw content over notch and system bars (used on mobile devices)
    Q_PROPERTY( double safeAreaTop READ safeAreaTop WRITE setSafeAreaTop NOTIFY safeAreaTopChanged )
    Q_PROPERTY( double safeAreaRight READ safeAreaRight WRITE setSafeAreaRight NOTIFY safeAreaRightChanged )
    Q_PROPERTY( double safeAreaBottom READ safeAreaBottom WRITE setSafeAreaBottom NOTIFY safeAreaBottomChanged )
    Q_PROPERTY( double safeAreaLeft READ safeAreaLeft WRITE setSafeAreaLeft NOTIFY safeAreaLeftChanged )

    // Margins
    Q_PROPERTY( double margin4 READ margin4 CONSTANT )
    Q_PROPERTY( double margin6 READ margin6 CONSTANT )
    Q_PROPERTY( double margin8 READ margin8 CONSTANT )
    Q_PROPERTY( double margin12 READ margin12 CONSTANT )
    Q_PROPERTY( double margin20 READ margin20 CONSTANT )
    Q_PROPERTY( double margin40 READ margin40 CONSTANT )

    // Page
    Q_PROPERTY( double pageMargins READ margin20 CONSTANT ) // distance between screen edge and components
    Q_PROPERTY( double spacing20 READ margin20 CONSTANT ) // distance between page header, page content and page footer
    Q_PROPERTY( double spacing40 READ margin40 CONSTANT ) // bigger distance between page header, page content and page footer
    Q_PROPERTY( double maxPageWidth READ maxPageWidth CONSTANT ) // maximum page width (desktop, tablets, landscape)

    // Other
    Q_PROPERTY( double row24 READ row24 CONSTANT )
    Q_PROPERTY( double row36 READ row36 CONSTANT )
    Q_PROPERTY( double row40 READ row40 CONSTANT )
    Q_PROPERTY( double row49 READ row49 CONSTANT )
    Q_PROPERTY( double row63 READ row63 CONSTANT )
    Q_PROPERTY( double row67 READ row67 CONSTANT )
    Q_PROPERTY( double row80 READ row80 CONSTANT )
    Q_PROPERTY( double row114 READ row114 CONSTANT )
    Q_PROPERTY( double inputRadius READ margin12 CONSTANT )
    Q_PROPERTY( double scrollVelocityAndroid READ scrollVelocityAndroid CONSTANT ) // [px/s] scrolling on Android devices is too slow by default

  public:
    explicit MMStyle( QObject *parent,  qreal dp )
      : QObject( parent ), mDp( dp )
    {}
    ~MMStyle() = default;

    QFont h1() {return fontFactory( 48, true );}
    QFont h2() {return fontFactory( 36, true );}
    QFont h3() {return fontFactory( 24, true );}

    QFont t1() {return fontFactory( 18, true );}
    QFont t2() {return fontFactory( 16, true );}
    QFont t3() {return fontFactory( 14, true );}
    QFont t4() {return fontFactory( 12, true );}
    QFont t5() {return fontFactory( 10, true );}

    QFont p1() {return fontFactory( 32, false );}
    QFont p2() {return fontFactory( 24, false );}
    QFont p3() {return fontFactory( 20, false );}
    QFont p4() {return fontFactory( 16, false );}
    QFont p5() {return fontFactory( 14, false );}
    QFont p6() {return fontFactory( 12, false );}
    QFont p7() {return fontFactory( 10, false );}

    QColor grassColor() {return QColor::fromString( "#73D19C" );}
    QColor forestColor() {return QColor::fromString( "#004C45" );}
    QColor nightColor() {return QColor::fromString( "#12181F" );}
    QColor whiteColor() {return QColor::fromString( "#FFFFFF" );}
    QColor transparentColor() {return QColor::fromString( "transparent" );}

    QColor lightGreenColor() {return QColor::fromString( "#EFF5F3" );}
    QColor mediumGreenColor() {return QColor::fromString( "#B7CDC4" );}
    QColor greyColor() {return QColor::fromString( "#E2E2E2" );}

    QColor sandColor() {return QColor::fromString( "#FFF4E2" );}
    QColor sunsetColor() {return QColor::fromString( "#FFB673" );}
    QColor sunColor() {return QColor::fromString( "#F4CB46" );}
    QColor earthColor() {return QColor::fromString( "#4D2A24" );}
    QColor roseColor() {return QColor::fromString( "#FFBABC" );}
    QColor skyColor() {return QColor::fromString( "#A6CBF4" );}
    QColor grapeColor() {return QColor::fromString( "#5A2740" );}
    QColor grapeTransparentColor() {return QColor( 0x5A, 0x27, 0x40, 102 );}
    QColor deepOceanColor() {return QColor::fromString( "#1C324A" );}
    QColor purpleColor() {return QColor::fromString( "#CCBDF5" );}
    QColor fieldColor() {return QColor::fromString( "#9BD1A9" );}

    QColor positiveColor() {return QColor::fromString( "#C0EBCF" );}
    QColor warningColor() {return QColor::fromString( "#F7DDAF" );}
    QColor negativeColor() {return QColor::fromString( "#F0C4BC" );}
    QColor informativeColor() {return QColor::fromString( "#BEDAF0" );}

    QColor nightAlphaColor() {return QColor::fromString( "#AA12181F" );}
    QColor errorBgInputColor() {return QColor::fromString( "#FEFAF9" );}
    QColor shadowColor() {return QColor::fromString( "#66777777" );}

    QUrl linkIcon() {return QUrl( "qrc:/Link.svg" );}
    QUrl closeAccountIcon() {return QUrl( "qrc:/CloseAccount.svg" );}
    QUrl signOutIcon() {return QUrl( "qrc:/SignOut.svg" );}
    QUrl workspacesIcon() {return QUrl( "qrc:/Workspaces.svg" );}
    QUrl addIcon() {return QUrl( "qrc:/Add.svg" );}
    QUrl layersIcon() {return QUrl( "qrc:/Layers.svg" );}
    QUrl localChangesIcon() {return QUrl( "qrc:/LocalChanges.svg" );}
    QUrl mapThemesIcon() {return QUrl( "qrc:/MapThemes.svg" );}
    QUrl positionTrackingIcon() {return QUrl( "qrc:/PositionTracking.svg" );}
    QUrl settingsIcon() {return QUrl( "qrc:/Settings.svg" );}
    QUrl zoomToProjectIcon() {return QUrl( "qrc:/ZoomToProject.svg" );}
    QUrl addImageIcon() {return QUrl( "qrc:/AddImage.svg" );}
    QUrl archaeologyIcon() {return QUrl( "qrc:/Archaeology.svg" );}
    QUrl arrowDownIcon() {return QUrl( "qrc:/ArrowDown.svg" );}
    QUrl arrowLinkRightIcon() {return QUrl( "qrc:/ArrowLinkRight.svg" );}
    QUrl arrowUpIcon() {return QUrl( "qrc:/ArrowUp.svg" );}
    QUrl backIcon() {return QUrl( "qrc:/Back.svg" );}
    QUrl briefcaseIcon() {return QUrl( "qrc:/Briefcase.svg" );}
    QUrl calendarIcon() {return QUrl( "qrc:/Calendar.svg" );}
    QUrl checkmarkIcon() {return QUrl( "qrc:/Checkmark.svg" );}
    QUrl closeIcon() {return QUrl( "qrc:/Close.svg" );}
    QUrl deleteIcon() {return QUrl( "qrc:/Delete.svg" );}
    QUrl featuresIcon() {return QUrl( "qrc:/Features.svg" );}
    QUrl downloadIcon() {return QUrl( "qrc:/Download.svg" );}
    QUrl uploadIcon() {return QUrl( "qrc:/Upload.svg" );}
    QUrl editIcon() {return QUrl( "qrc:/Edit.svg" );}
    QUrl electricityIcon() {return QUrl( "qrc:/Electricity.svg" );}
    QUrl engineeringIcon() {return QUrl( "qrc:/Engineering.svg" );}
    QUrl environmentalIcon() {return QUrl( "qrc:/Environmental.svg" );}
    QUrl facebookIcon() {return QUrl( "qrc:/Facebook.svg" );}
    QUrl globeIcon() {return QUrl( "qrc:/Globe.svg" );}
    QUrl globalIcon() {return QUrl( "qrc:/Global.svg" );}
    QUrl gpsIcon() {return QUrl( "qrc:/GPSIcon.svg" );}
    QUrl gpsAntennaHeightIcon() {return QUrl( "qrc:/GPSAntennaHeight.svg" );}
    QUrl hideIcon() {return QUrl( "qrc:/Hide.svg" );}
    QUrl homeIcon() {return QUrl( "qrc:/Home.svg" );}
    QUrl infoIcon() {return QUrl( "qrc:/Info.svg" );}
    QUrl linkedinIcon() {return QUrl( "qrc:/Linkedin.svg" );}
    QUrl mastodonIcon() {return QUrl( "qrc:/Mastodon.svg" );}
    QUrl minusIcon() {return QUrl( "qrc:/Minus.svg" );}
    QUrl moreIcon() {return QUrl( "qrc:/More.svg" );}
    QUrl moreVerticalIcon() {return QUrl( "qrc:/MoreVertical.svg" );}
    QUrl morePhotosIcon() {return QUrl( "qrc:/MorePhotos.svg" );}
    QUrl mouthIcon() {return QUrl( "qrc:/Mouth.svg" );}
    QUrl naturalResourcesIcon() {return QUrl( "qrc:/NaturalResources.svg" );}
    QUrl nextIcon() {return QUrl( "qrc:/Next.svg" );}
    QUrl otherIcon() {return QUrl( "qrc:/Other.svg" );}
    QUrl othersIcon() {return QUrl( "qrc:/Others.svg" );}
    QUrl plusIcon() {return QUrl( "qrc:/Plus.svg" );}
    QUrl personalIcon() {return QUrl( "qrc:/Personal.svg" );}
    QUrl projectsIcon() {return QUrl( "qrc:/Projects.svg" );}
    QUrl qgisIcon() {return QUrl( "qrc:/QGIS.svg" );}
    QUrl qrCodeIcon() {return QUrl( "qrc:/QRCode.svg" );}
    QUrl redditIcon() {return QUrl( "qrc:/Reddit.svg" );}
    QUrl satelliteIcon() {return QUrl( "qrc:/GPSSatellite.svg" );}
    QUrl searchIcon() {return QUrl( "qrc:/Search.svg" );}
    QUrl showIcon() {return QUrl( "qrc:/Show.svg" );}
    QUrl smallCheckmarkIcon() {return QUrl( "qrc:/SmallCheckmark.svg" );}
    QUrl smallEditIcon() {return QUrl( "qrc:/SmallEdit.svg" );}
    QUrl socialMediaIcon() {return QUrl( "qrc:/SocialMedia.svg" );}
    QUrl stakeOutIcon() {return QUrl( "qrc:/StakeOut.svg" );}
    QUrl stateAndLocalIcon() {return QUrl( "qrc:/StateAndLocal.svg" );}
    QUrl stopIcon() {return QUrl( "qrc:/Stop.svg" );}
    QUrl subscriptionsIcon() {return QUrl( "qrc:/Subscriptions.svg" );}
    QUrl syncIcon() {return QUrl( "qrc:/Sync.svg" );}
    QUrl syncGreenIcon() {return QUrl( "qrc:/SyncGreen.svg" );}
    QUrl teacherIcon() {return QUrl( "qrc:/Teacher.svg" );}
    QUrl telecommunicationIcon() {return QUrl( "qrc:/Telecommunication.svg" );}
    QUrl termsIcon() {return QUrl( "qrc:/Terms.svg" );}
    QUrl tractorIcon() {return QUrl( "qrc:/Tractor.svg" );}
    QUrl transportationIcon() {return QUrl( "qrc:/Transportation.svg" );}
    QUrl undoIcon() {return QUrl( "qrc:/Undo.svg" );}
    QUrl waitingIcon() {return QUrl( "qrc:/Waiting.svg" );}
    QUrl waterResourcesIcon() {return QUrl( "qrc:/WaterResources.svg" );}
    QUrl xTwitterIcon() {return QUrl( "qrc:/XTwitter.svg" );}
    QUrl youtubeIcon() {return QUrl( "qrc:/Youtube.svg" );}

    QUrl projectsFilledIcon() {return QUrl( "qrc:/ProjectsFilled.svg" );}
    QUrl globalFilledIcon() {return QUrl( "qrc:/GlobalFilled.svg" );}
    QUrl infoFilledIcon() {return QUrl( "qrc:/InfoFilled.svg" );}
    QUrl featuresFilledIcon() {return QUrl( "qrc:/FeaturesFilled.svg" );}
    QUrl homeFilledIcon() {return QUrl( "qrc:/HomeFilled.svg" );}

    QUrl xMarkCircleIcon() {return QUrl( "qrc:/XMarkCircle.svg" );}
    QUrl doneCircleIcon() {return QUrl( "qrc:/DoneCircle.svg" );}
    QUrl errorCircleIcon() {return QUrl( "qrc:/ErrorCircle.svg" );}
    QUrl warningCircleIcon() {return QUrl( "qrc:/WarningCircle.svg" );}
    QUrl editCircleIcon() {return QUrl( "qrc:/EditCircle.svg" );}

    QUrl mmLogoImage() {return QUrl( "qrc:/images/MMLogo.svg" );}
    QUrl lutraLogoImage() {return QUrl( "qrc:/images/LutraLogo4x.png" );}
    QUrl trackingDirectionImage() {return QUrl( "qrc:/images/TrackingDirection.svg" );}
    QUrl directionImage() {return QUrl( "qrc:/images/Direction.svg" );}
    QUrl mmSymbolImage() {return QUrl( "qrc:/images/MMSymbol.svg" );}
    QUrl acceptInvitationLogoImage() {return QUrl( "qrc:/images/AcceptInvitationLogoImage.svg" ); }
    QUrl acceptInvitationImage() {return QUrl( "qrc:/images/AcceptInvitationImage.svg" ); }
    QUrl uploadImage() {return QUrl( "qrc:/images/UploadImage.svg" );}
    QUrl noMapThemesImage() {return QUrl( "qrc:/images/NoMapThemesImage.svg" );}
    QUrl reachedDataLimitImage() {return QUrl( "qrc:/images/ReachedDataLimit.svg" );}
    QUrl warnLogoImage() {return QUrl( "qrc:/images/WarnLogoImage.svg" );}
    QUrl mapPinImage() {return QUrl( "qrc:/images/MapPin.svg" );}
    QUrl positionTrackingRunningImage() {return QUrl( "qrc:/images/PositionTrackingRunning.svg" );}
    QUrl positionTrackingStartImage() {return QUrl( "qrc:/images/PositionTrackingStart.svg" );}
    QUrl syncImage() {return QUrl( "qrc:/images/SyncImage.svg" );}
    QUrl externalGpsGreenImage() {return QUrl( "qrc:/images/ExternalGpsGreen.svg" );}
    QUrl externalGpsRedImage() {return QUrl( "qrc:/images/ExternalGpsRed.svg" );}
    QUrl negativeMMSymbolImage() {return QUrl( "qrc:/images/NegativeMMSymbol.svg" );}
    QUrl positiveMMSymbolImage() {return QUrl( "qrc:/images/PositiveMMSymbol.svg" );}
    QUrl closeAccountImage() {return QUrl( "qrc:/images/CloseAccount.svg" );}
    QUrl attentionImage() {return QUrl( "qrc:/images/Attention.svg" );}
    QUrl bubbleImage() {return QUrl( "qrc:/images/Bubble.svg" );}

    static QUrl lineLayerImage() {return QUrl( "qrc:/images/mIconLineLayer.svg" );}
    static QUrl pointLayerImage() {return QUrl( "qrc:/images/mIconPointLayer.svg" );}
    static QUrl polygonLayerImage() {return QUrl( "qrc:/images/mIconPolygonLayer.svg" );}
    static QUrl rasterLayerImage() {return QUrl( "qrc:/images/mIconRasterLayer.svg" );}
    static QUrl tableLayerImage() {return QUrl( "qrc:/images/mIconTableLayer.svg" );}

    double icon16() {return 16 * mDp;}
    double icon24() {return 24 * mDp;}
    double icon32() {return 32 * mDp;}

    double mapItemHeight() {return 60 * mDp;}
    double toolbarHeight() {return 68 * mDp;}
    double menuDrawerHeight() {return 67 * mDp;}
    double maxPageWidth() {return 720 * mDp;}

    double safeAreaTop() const { return mSafeAreaTop; };
    double safeAreaRight() const { return mSafeAreaRight; };
    double safeAreaBottom() const { return mSafeAreaBottom; };
    double safeAreaLeft() const { return mSafeAreaLeft; };

    double margin4() {return 4 * mDp;}
    double margin6() {return 6 * mDp;}
    double margin8() {return 8 * mDp;}
    double margin12() {return 12 * mDp;}
    double margin20() {return 20 * mDp;}
    double margin40() {return 40 * mDp;}

    double row24() {return 24 * mDp;}
    double row36() {return 36 * mDp;}
    double row40() {return 40 * mDp;}
    double row49() {return 49 * mDp;}
    double row63() {return 63 * mDp;}
    double row67() {return 67 * mDp;}
    double row80() {return 80 * mDp;}
    double row114() {return 114 * mDp;}

    double scrollVelocityAndroid() { return 10000; }

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
    QFont fontFactory( int pixelSize, bool bold )
    {
      QFont f;
      f.setBold( bold );
      f.setPixelSize( pixelSize * mDp );
      return f;
    }

    qreal mDp;

    double mSafeAreaTop = 0;
    double mSafeAreaRight = 0;
    double mSafeAreaBottom = 0;
    double mSafeAreaLeft = 0;
};

#endif // MMSTYLE_H
