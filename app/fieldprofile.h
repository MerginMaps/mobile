/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FIELDPROFILE_H
#define FIELDPROFILE_H

#include <QString>
#include <QVariantMap>

class QgsProject;
class MMStyle;
class TermsTranslator;

/**
 * The field profile customises the app per project. It is a JSON file named field-profile.json
 * placed next to the QGIS project file and synced with the project like any other file.
 *
 * Sections:
 *  - "quickCapture": one-tap point and media capture, see QuickCaptureController
 *  - "branding": colours, logo and font of the app
 *  - "terms": the organisation's own words for the app interface, see TermsTranslator
 *
 * {
 *   "version": 1,
 *   "branding": {
 *     "colors": {
 *       "primary": "#1F3A5F",        // text, icons and dark surfaces (default #004C45)
 *       "accent": "#9CC3E6",         // primary buttons and highlights (default #73D19C)
 *       "onAccent": "#1F3A5F",       // text and icons on accent backgrounds (default: primary)
 *       "background": "#F2F5F9",     // page backgrounds (default #EFF5F3)
 *       "secondary": "#C3D1E0",      // borders and secondary surfaces (default #B7CDC4)
 *       "secondaryDark": "#6B7F95"   // disabled and secondary text (default #6E9991)
 *     },
 *     "logo": "branding/logo.svg",   // path relative to the project folder
 *     "font": [ "branding/font-regular.ttf", "branding/font-bold.ttf" ]
 *                                    // TrueType or OpenType files of one family (or a single file), relative to the project folder
 *   },
 *   "terms": {                       // word in the app (in the app language) -> replacement
 *     "Projects": "Missions",
 *     "Project": "Mission"
 *   }
 * }
 *
 * The app icon, app name and the native splash screen are set when the app is built
 * and cannot be changed from the field profile.
 */
class FieldProfile
{
  public:
    static const QString FILE_NAME;

    //! Returns the path of the field profile of the project (the file may not exist)
    static QString filePath( const QgsProject *project );

    //! Returns the content of the field profile, or an empty map if there is none or it is invalid
    static QVariantMap read( const QgsProject *project );

    /**
     * Applies the "branding" section of the project's field profile to the style, or resets
     * the default style if the project has no branding. The branding is remembered so the app
     * starts with it next time.
     */
    static void applyBranding( MMStyle *style, const QgsProject *project );

    //! Applies the branding remembered from the last opened project
    static void restoreBranding( MMStyle *style );

    /**
     * Applies the "terms" section of the project's field profile, remembered for the next start.
     * Returns true if the terms changed, so the interface needs to be translated again.
     */
    static bool applyTerms( TermsTranslator *translator, const QgsProject *project );

    //! Applies the terms remembered from the last opened project
    static void restoreTerms( TermsTranslator *translator );

  private:
    static void setBranding( MMStyle *style, const QVariantMap &colors, const QString &logoPath, const QStringList &fontPaths );

    //! Loads the font files once and returns the family of the first one, or an empty string if it cannot be loaded
    static QString fontFamily( const QStringList &fontPaths );
};

#endif // FIELDPROFILE_H
