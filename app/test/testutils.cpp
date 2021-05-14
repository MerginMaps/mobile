/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "QtDebug"

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
