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

/**
 * The field profile customises the app per project. It is a JSON file named field-profile.json
 * placed next to the QGIS project file and synced with the project like any other file.
 *
 * Sections:
 *  - "quickCapture": one-tap point and media capture, see QuickCaptureController
 *  - "branding": colours and logo of the app
 *
 * {
 *   "version": 1,
 *   "branding": {
 *     "colors": {
 *       "primary": "#1F3A5F",        // text, icons and dark surfaces (default #004C45)
 *       "accent": "#9CC3E6",         // primary buttons and highlights (default #73D19C)
 *       "background": "#F2F5F9",     // page backgrounds (default #EFF5F3)
 *       "secondary": "#C3D1E0",      // borders and secondary surfaces (default #B7CDC4)
 *       "secondaryDark": "#6B7F95"   // disabled and secondary text (default #6E9991)
 *     },
 *     "logo": "branding/logo.svg"    // path relative to the project folder
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

  private:
    static void setBranding( MMStyle *style, const QVariantMap &colors, const QString &logoPath );
};

#endif // FIELDPROFILE_H
