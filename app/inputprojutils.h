/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef INPUTPROJUTILS_H
#define INPUTPROJUTILS_H

#include <QObject>
#include <QtGlobal>

/**
 * QGIS uses PROJ library for doing CRS transformations.
 * We currently ship the basic PROJ resources QGIS needs to do most common projections by adding them to the assets in APKs (see https://github.com/lutraconsulting/input/tree/master/app/android/assets/qgis-data). There could be a custom PROJ datum shifts used by users as part of their mergin projects in the `<project>/proj/` folder.
 *
 * PROJ library is set up in QGIS initialization function. Before calling this function, we need to
 *  1. Expand the PROJ resources to the <device>/INPUT/proj folder
 *  2. Clean the <device>/INPUT/proj_custom folder and copy all custom PROJ resources from all projects present on the device to the folder <device>/INPUT/proj_custom folder.
 *  3. Set the defaul PROJ context with both paths from 1 and 2 with usage of `proj_context_set_search_paths`
 *  4. now call qgis init function -> this takes all paths from proj_context and adds some internal QGIS paths too. This context is then used for all QGIS projection transformations
 *
 * Since QGIS uses only one context and there is not easy way how to dynamically refresh it after initialization, we are currently unable to dynamically add/remove custom datum shift files. Therefore if user download a project with custom proj files, the InputApp has to be restarted. Also there could be bugs if 2 projects uses the custom proj files with the same name but different content (unlikely).
 *
 * The InputProjUtils also adds QgsCoordinateTransform::setCustom*Handlers that pops out the dialog to user mentioning missing custom grids and action to restart the app.
 *
 */
class InputProjUtils: public QObject
{
    Q_OBJECT
  public:
    explicit InputProjUtils( QObject *parent = nullptr );
    ~InputProjUtils() = default;

    /**
     * Sets the PROJ_LIB dir, should be called before qgis application is initialized
     * Copies all custom projections from all loaded projects to the custom PROJ folder.
     */
    void initProjLib( const QString &appBundleDir, const QString &dataDir, const QString &projectsPath );

    /**
     * Resets the custom handlers.
     */
    void resetHandlers();

  signals:
    //! Emitted where there is any error with transformations
    void projError( const QString &message );

  private:
    void warnUser( const QString &message );
    void logUser( const QString &message, bool &variable );

    void cleanCustomDir();
    void copyCustomProj( const QString &projectsPath );

    void setCurrentCustomProjDir( const QString &dataDir );
    void setProjDir( const QString &appBundleDir );

    void initCoordinateOperationHandlers();

    bool mPopUpShown = false;

    bool mMissingRequiredGridReported = false;
    bool mMissingPreferredGridReported = false;
    bool mCoordinateOperationCreationErrorReported = false;
    bool mMissingGridUsedByContextHandlerReported = false;
    bool mFallbackOperationOccurredReported = false;

    QString mCurrentCustomProjDir;
    QString mProjDir;
};

#endif // INPUTPROJUTILS_H
