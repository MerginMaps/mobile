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
#include <QDebug>

#include "test/testmerginapi.h"
#include "test/testlinks.h"
#include "test/testutilsfunctions.h"
#include "test/testattributepreviewcontroller.h"
#include "test/testattributecontroller.h"
#include "test/testidentifykit.h"
#include "test/testposition.h"
#include "test/testrememberattributescontroller.h"
#include "test/testscalebarkit.h"
#include "test/testvariablesmanager.h"
#include "test/testformeditors.h"
#include "test/testmodels.h"
#include "test/testcoreutils.h"

#if not defined APPLE_PURCHASING
#include "test/testpurchasing.h"
#endif

InputTests::InputTests() = default;

InputTests::~InputTests() = default;

void InputTests::parseArgs( int argc, char *argv[] )
{
  // use command line args we got, but filter out "--test*" that's recognized by us but not by QTest framework
  // (command line args may be used to filter function names that should be executed)
  for ( int i = 0; i < argc; ++i )
  {
    QString arg( argv[i] );
    if ( arg.startsWith( "--test" ) )
    {
      mTestRequested = arg;
    }
    else
    {
      mTestArgs << argv[i];
    }
  }
}

bool InputTests::testingRequested() const
{
  return !mTestRequested.isEmpty();
}

void InputTests::init( MerginApi *api, Purchasing *purchasing, InputUtils *utils, VariablesManager *varManager, PositionKit *kit )
{
  mApi = api;
  mPurchasing = purchasing;
  mInputUtils = utils;
  mVariablesManager = varManager;
  mPositionKit = kit;
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

int InputTests::runTest() const
{
  int nFailed = 0;

  if ( !mApi || !mPurchasing || !mInputUtils )
  {
    nFailed = 1000;
    qDebug() << "input tests not initialized";
    return nFailed;
  }

  qDebug() << "requested to run" << mTestRequested;
  if ( mTestRequested == "--testMerginApi" )
  {
    TestMerginApi merginApiTest( mApi );
    nFailed = QTest::qExec( &merginApiTest, mTestArgs );
  }
  else if ( mTestRequested == "--testLinks" )
  {
    TestLinks linksTest( mApi, mInputUtils );
    nFailed = QTest::qExec( &linksTest, mTestArgs );
  }
  else if ( mTestRequested == "--testUtils" )
  {
    TestUtilsFunctions utilsTest( mInputUtils );
    nFailed = QTest::qExec( &utilsTest, mTestArgs );
  }
  else if ( mTestRequested == "--testAttributePreviewController" )
  {
    TestAttributePreviewController formsTest;
    nFailed = QTest::qExec( &formsTest, mTestArgs );
  }
  else if ( mTestRequested == "--testAttributeController" )
  {
    TestAttributeController formsTest;
    nFailed = QTest::qExec( &formsTest, mTestArgs );
  }
  else if ( mTestRequested == "--testIdentifyKit" )
  {
    TestIdentifyKit ikTest;
    nFailed = QTest::qExec( &ikTest, mTestArgs );
  }
  else if ( mTestRequested == "--testPosition" )
  {
    TestPosition pkTest( mPositionKit );
    nFailed = QTest::qExec( &pkTest, mTestArgs );
  }
  else if ( mTestRequested == "--testRememberAttributesController" )
  {
    TestRememberAttributesController racTest;
    nFailed = QTest::qExec( &racTest, mTestArgs );
  }
  else if ( mTestRequested == "--testScaleBarKit" )
  {
    TestScaleBarKit sbkTest;
    nFailed = QTest::qExec( &sbkTest, mTestArgs );
  }
  else if ( mTestRequested == "--testVariablesManager" )
  {
    TestVariablesManager vmTest( mVariablesManager );
    nFailed = QTest::qExec( &vmTest, mTestArgs );
  }
  else if ( mTestRequested == "--testFormEditors" )
  {
    TestFormEditors edTest;
    nFailed = QTest::qExec( &edTest, mTestArgs );
  }
  else if ( mTestRequested == "--testModels" )
  {
    TestModels modelsTest;
    nFailed = QTest::qExec( &modelsTest, mTestArgs );
  }
  else if ( mTestRequested == "--testCoreUtils" )
  {
    TestCoreUtils coreUtilsTest;
    nFailed = QTest::qExec( &coreUtilsTest, mTestArgs );
  }
#if not defined APPLE_PURCHASING
  else if ( mTestRequested == "--testPurchasing" )
  {
    TestPurchasing purchasingTest( mApi, mPurchasing );
    nFailed = QTest::qExec( &purchasingTest, mTestArgs );
  }
#endif
  else
  {
    qDebug() << "invalid test requested" << mTestRequested;
    nFailed = 1001;
  }

  /*
   * WARNING:
   *
   * when you are adding new test here in the list, add
   * it also to scripts/run_all_tests.bash
   *
   */

  return nFailed;
}
