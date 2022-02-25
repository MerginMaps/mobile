/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "QtDebug"
#include <QJsonDocument>
#include <QJsonArray>
#include <QSignalSpy>

#include "testutils.h"
#include "coreutils.h"
#include "merginapi.h"

void TestUtils::mergin_auth( MerginApi *api, QString &apiRoot, QString &username, QString &password )
{
  Q_ASSERT( api );

  // Test server url needs to be set
  Q_ASSERT( ::getenv( "TEST_MERGIN_URL" ) );

  apiRoot = ::getenv( "TEST_MERGIN_URL" );
  api->setApiRoot( apiRoot );

  qDebug() << "MERGIN API ROOT:" << apiRoot;

  username = ::getenv( "TEST_API_USERNAME" );
  password = ::getenv( "TEST_API_PASSWORD" );

  if ( username.isEmpty() )
  {
    // we need to register new user for tests and assign its credentials to env vars
    username = generateUsername();
    password = generatePassword();
    QString email = username + "@autotest.xy";

    qDebug() << "REGISTERING NEW TEST USER:" << username;

    QSignalSpy spy( api,  &MerginApi::registrationSucceeded );
    api->registerUser( username, email, password, password, true );
    QVERIFY( spy.wait( TestUtils::LONG_REPLY ) );

    qputenv( "TEST_API_USERNAME", username.toLatin1() );
    qputenv( "TEST_API_PASSWORD", password.toLatin1() );
  }

  Q_ASSERT( ::getenv( "TEST_API_USERNAME" ) );
  Q_ASSERT( ::getenv( "TEST_API_PASSWORD" ) );

  qDebug() << "MERGIN USERNAME:" << username;

  // let's make sure we do not mess with the public instance
  Q_ASSERT( apiRoot != MerginApi::sDefaultApiRoot );
}

QString TestUtils::generateUsername()
{
  QDateTime time = QDateTime::currentDateTime();
  QString uniqename = time.toString( QStringLiteral( "ddMMyyyy-hhmmss-z" ) );
  return QStringLiteral( "input-%1" ).arg( uniqename );
}

QString TestUtils::generatePassword()
{
  QString pass = CoreUtils::uuidWithoutBraces( QUuid::createUuid() ).right( 15 ).replace( "-", "" );
  return QStringLiteral( "_Pass12%1" ).arg( pass );
}

QString TestUtils::testDataDir()
{
  QString dataDir( TEST_DATA_DIR );
  return dataDir;
}

bool TestUtils::generateProjectFolder( const QString &rootPath, const QJsonDocument &structure )
{
  if ( !structure.isObject() )
    return false;

  if ( !QDir( rootPath ).exists() )
    return false;

  QJsonObject rootObj = structure.object();

  // generate files
  if ( rootObj.contains( "files" ) )
  {
    QJsonArray files = rootObj.value( "files" ).toArray();
    for ( int i = 0; i < files.count(); i++ )
    {
      QFile f( rootPath + "/" + files[i].toString() );
      f.open( QIODevice::WriteOnly );
      f.close();
    }
  }

  // go deeper to subfolders
  bool allGood = true;

  QStringList nodes = rootObj.keys();
  for ( int i = 0; i < nodes.count(); i++ )
  {
    if ( rootObj.value( nodes[i] ).isObject() )
    {
      QString newPath = rootPath + "/" + nodes[i];

      QDir d( newPath );
      d.mkpath( newPath );

      allGood = allGood && generateProjectFolder( rootPath + "/" + nodes[i], QJsonDocument( rootObj.value( nodes[i] ).toObject() ) );
    }
  }

  return allGood;
}
