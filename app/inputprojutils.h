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
     * Copies all custom projections from all loaded projects to the custom PROJ folder.
     */
    void initProjLib( const QString &pkgPath, const QString &projectsPath );

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

    void initCoordinateOperationHandlers();

    bool mPopUpShown = false;

    bool mMissingRequiredGridReported = false;
    bool mMissingPreferredGridReported = false;
    bool mCoordinateOperationCreationErrorReported = false;
    bool mMissingGridUsedByContextHandlerReported = false;
    bool mFallbackOperationOccurredReported = false;

    QString mCurrentCustomProjDir;
};

#endif // INPUTPROJUTILS_H
