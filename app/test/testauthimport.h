/***************************************************************************
 * *
 * This program is free software; you can redistribute it and/or modify  *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation; either version 2 of the License, or     *
 * (at your option) any later version.                                   *
 * *
 ***************************************************************************/

#ifndef TESTAUTHIMPORT_H
#define TESTAUTHIMPORT_H

#include <QObject>
#include <QtTest/QTest>
#include <QTemporaryDir>
#include <QDir>
#include <QFile>

#include <qgsapplication.h>
#include <qgsauthmanager.h>

#include "coreutils.h"
#include "merginprojectmetadata.h"
#include "localprojectsmanager.h"

const QString AUTH_CONFIG_FILENAME = "qgis_auth.xml";
const QString TEST_PROJECT_ID = "mobile_test_auth_1234";
const QString TEST_AUTH_ID = "mobile_test_config_id";
const QString TEST_XML_FILENAME = "test_auth_config.xml";

class TestAuthImport : public QObject
{
    Q_OBJECT

  public:
    TestAuthImport();

  private slots:
    void initTestCase();
    void cleanupTestCase();
    void cleanup();

    void test_successful_of_authFile();
    void test_import_fails_authFile_missing();
    void test_import_fails_master_password_not_set();
    void test_reload_after_configuration_update();

  private:
    std::unique_ptr<QTemporaryDir> mAppTempDir;
    const char *mPass = nullptr;

    void mockProjectMetadata( const QString &projectDir );
    void setup_ExportInitialConfig( const QString &projectDir, const QString &authId, const QString &username );
    void copyTestAuthFile( const QString &projectDir );
};

#endif // TESTAUTHIMPORT_H