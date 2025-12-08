/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "qgsproject.h"
#include "qgsauthmanager.h"
#include "qgsapplication.h"
#include "qgsmaplayer.h"
#include "qgsproviderregistry.h"
#include "coreutils.h"
#include "activeproject.h"
#include "merginprojectmetadata.h"
#include "appsettings.h"

class AuthSync : public QObject
{
    Q_OBJECT

  public:
    explicit AuthSync( const QString &projectDir = QString(), QObject *parent = nullptr );

    QStringList getLayersAuthIds() const;
    QString getAuthConfigHash( const QStringList &authIds ) const;
    void exportAuth( const LocalProject &localProject );
    void importAuth();

  private:
    QgsProject *mProject;
    QString mProjectDir;
    QString mAuthFile;
    QgsAuthManager *mAuthMngr;


    QString getProjectUuid( const QString &projectDir ) const;
    void logError( const QString &message )  const;
    void logInfo( const QString &message ) const ;
    void logWarning( const QString &message ) const ;
};