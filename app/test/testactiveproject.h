/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TESTACTIVEPROJECT_H
#define TESTACTIVEPROJECT_H

#include <QObject>
#include <merginapi.h>
#include <qgsapplication.h>
#include <qgsauthmanager.h>

const QString AUTH_CONFIG_FILENAME = QStringLiteral( "qgis_cfg.xml" );
const QString AUTH_CONFIG_PASSWORD = QStringLiteral( "1234" );

class TestActiveProject : public QObject
{
    Q_OBJECT
  public:
    explicit TestActiveProject( MerginApi *api );
    ~TestActiveProject();

  private slots:
    void init();
    void cleanup();

    void testProjectValidations();
    void testProjectLoadFailure();
    void testPositionTrackingFlag();
    void testRecordingAllowed();
    void testLoadingFlagFileExpiration();
    void testLoadingAuthFileFromConfiguration();

  private:
    MerginApi *mApi;
};

#endif // TESTACTIVEPROJECT_H
