/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "inputtests.h"

#include <QTest>

#include "test/testmerginapi.h"
#include "test/testlinks.h"
#include "test/testutilsfunctions.h"
#include "test/testforms.h"
#if not defined APPLE_PURCHASING
#include "test/testpurchasing.h"
#endif

InputTests::InputTests() = default;

InputTests::~InputTests() = default;

void InputTests::parseArgs(int argc, char *argv[]) {
  // use command line args we got, but filter out "--test*" that's recognized by us but not by QTest framework
  // (command line args may be used to filter function names that should be executed)
  for ( int i = 0; i < argc; ++i )
  {
    QString arg(argv[i]);
    if ( arg.startsWith("--test") )
    {
      mTestRequested = arg;
      break;
    } else {
      mTestArgs << argv[i];
    }
  }
}

bool InputTests::testingRequested() const {
  return !mTestRequested.isEmpty();
}

void InputTests::init(MerginApi* api, Purchasing* purchasing, InputUtils* utils)
{
  mApi = api;
  mPurchasing = purchasing;
  mInputUtils = utils;
}

void InputTests::initTestDeclarative()
{
  qRegisterMetaType<MerginProjectsList>( "MerginProjectsList" );
}

QString InputTests::initTestingDir()
{
  // override the path where local projects are stored
  // and wipe the temporary projects dir if it already exists
  QDir testDataDir( STR( INPUT_TEST_DATA_DIR ) );  // #defined in input.pro
  QDir testProjectsDir( testDataDir.path() + "/../temp_projects" );
  if ( testProjectsDir.exists() )
    testProjectsDir.removeRecursively();
  QDir( testDataDir.path() + "/.." ).mkpath( "temp_projects" );
  return testProjectsDir.canonicalPath();
}

int InputTests::runTest() const {
  int nFailed = 0;
  if ( mTestRequested == "--testMerginApi" )
  {
    TestMerginApi merginApiTest( mApi );
    nFailed = QTest::qExec( &merginApiTest, mTestArgs );
  }
  else if ( mTestRequested == "--testLinks" )
  {
    TestLinks linksTest( mApi, mInputUtils );
    nFailed = QTest::qExec( &linksTest, mTestArgs.count() );
  }
  else if ( mTestRequested == "--testUtils" )
  {
    TestUtilsFunctions utilsTest;
    nFailed = QTest::qExec( &utilsTest, mTestArgs );
  }
  else if ( mTestRequested == "--testForms" )
  {
    TestForms formsTest;
    nFailed = QTest::qExec( &formsTest, mTestArgs );
  }
#if not defined APPLE_PURCHASING
  else if ( mTestRequested == "--testPurchasing" )
  {
    TestPurchasing purchasingTest( mApi, mPurchasing );
    nFailed += QTest::qExec( &purchasingTest, mTestArgs );
  }
#endif

  return nFailed;
}
