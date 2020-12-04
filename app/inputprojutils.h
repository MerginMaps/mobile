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
 * Sets the PROJ environment variables to the default Input proj folder
 * and also to currently loaded folder <namespace>/<project>/proj to
 * search for PROJ resources
 *
 * Also handles the exceptions from the QGIS PROJ transformations
 */
class InputProjUtils: public QObject
{
    Q_OBJECT
  public:
    explicit InputProjUtils( QObject *parent = nullptr );
    ~InputProjUtils() = default;

    /**
     * Sets the PROJ_LIB dir, should be called before qgis application is initialized
     */
    void initProjLib( const QString &pkgPath );

    /**
     * Modifies proj search paths to include proj files relative to project directory
     */
    void modifyProjPath( const QString &projectFile );

    /**
     * Stores the default proj search paths. Should be called after qgis application is initialized
     */
    void setDefaultProjPaths();

  signals:
    //! Emitted where there is any error with transformations
    void projError( const QString &message );

  private:
    void warnUser( const QString &message );
    void initCoordinateOperationHandlers();

    bool mPopUpShown = false;
    bool mMissingRequiredGridReported = false;
    bool mMissingPreferredGridReported = false;
    bool mCoordinateOperationCreationErrorReported = false;
    bool mMissingGridUsedByContextHandlerReported = false;
    bool mFallbackOperationOccurredReported = false;

    QStringList sDefaultProjPaths;
    QString mCurrentCustomProjDir;
};

#endif // INPUTPROJUTILS_H
