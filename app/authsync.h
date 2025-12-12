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
#include "coreutils.h"
#include "merginprojectmetadata.h"

class AuthSync : public QObject
{
    Q_OBJECT

  public:
    explicit AuthSync( const QString &projectDir = QString(), QObject *parent = nullptr );
    void importAuth();
    bool fileExists( QString path );

  private:
    QgsProject *mProject;
    QString mProjectDir;
    QString mAuthFile;
    QgsAuthManager *mAuthMngr;

    QString getProjectUuid( const QString &projectDir ) const;
};