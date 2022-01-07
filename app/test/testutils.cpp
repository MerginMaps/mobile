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

#include "testutils.h"
#include "merginapi.h"

void TestUtils::mergin_auth( QString &apiRoot, QString &username, QString &password )
{
  // these env variables really need to be set!
  Q_ASSERT( ::getenv( "TEST_MERGIN_URL" ) );
  Q_ASSERT( ::getenv( "TEST_API_USERNAME" ) );
  Q_ASSERT( ::getenv( "TEST_API_PASSWORD" ) );

  apiRoot = ::getenv( "TEST_MERGIN_URL" );
  username = ::getenv( "TEST_API_USERNAME" );
  password = ::getenv( "TEST_API_PASSWORD" );

  qDebug() << "MERGIN API ROOT:" << apiRoot;
  qDebug() << "MERGIN USERNAME:" << username;

  // let's make sure we do not mess with the public instance
  Q_ASSERT( apiRoot != MerginApi::sDefaultApiRoot );
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
