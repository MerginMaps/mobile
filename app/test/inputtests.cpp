/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "inputtests.h"

#include "testsketching.h"
#include "testmerginapi.h"
#include "testlinks.h"

#include "testutilsfunctions.h"
#include "testattributepreviewcontroller.h"
#include "testattributecontroller.h"
#include "testidentifykit.h"
#include "testposition.h"
#include "testrememberattributescontroller.h"
#include "testscalebarkit.h"
#include "testvariablesmanager.h"
#include "testformeditors.h"
#include "testmodels.h"
#include "testcoreutils.h"
#include "testimageutils.h"
#include "testmaptools.h"
#include "testlayertree.h"
#include "testactiveproject.h"
#include "testprojectchecksumcache.h"
#include "testmultieditmanager.h"

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

void InputTests::init( MerginApi *api, InputUtils *utils, VariablesManager *varManager, PositionKit *kit, AppSettings *settings )
{
  mApi = api;
  mInputUtils = utils;
  mVariablesManager = varManager;
  mPositionKit = kit;
  mAppSettings = settings;
}

void InputTests::initTestDeclarative()
{
  qRegisterMetaType<MerginProjectsList>( "MerginProjectsList" );
}

QString InputTests::initTestingDir()
{
  // override the path where local projects are stored
  // and wipe the temporary projects dir if it already exists
  QDir testDataDir( TEST_DATA_DIR );
  QDir testProjectsDir( testDataDir.path() + "/../temp_projects" );
  if ( testProjectsDir.exists() )
    testProjectsDir.removeRecursively();
  QDir( testDataDir.path() + "/.." ).mkpath( "temp_projects" );
  return testProjectsDir.canonicalPath();
}

int InputTests::runTest() const
{
  int nFailed = 0;

  if ( !mApi || !mInputUtils )
  {
    nFailed = 1000;
    qDebug() << "input tests not initialized";
    return nFailed;
  }

  qDebug() << "requested to run" << mTestRequested;
  if ( mTestRequested == "--testLinks" )
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
    TestVariablesManager vmTest( mApi, mVariablesManager, mPositionKit, mAppSettings );
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
  else if ( mTestRequested == "--testImageUtils" )
  {
    TestImageUtils imageUtilsTest;
    nFailed = QTest::qExec( &imageUtilsTest, mTestArgs );
  }
  else if ( mTestRequested == "--testMapTools" )
  {
    TestMapTools maptoolsTest( mPositionKit, mAppSettings );
    nFailed = QTest::qExec( &maptoolsTest, mTestArgs );
  }
  else if ( mTestRequested == "--testLayerTree" )
  {
    TestLayerTree layerTreeTest;
    nFailed = QTest::qExec( &layerTreeTest, mTestArgs );
  }
  else if ( mTestRequested == "--testActiveProject" )
  {
    TestActiveProject activeProjectTest( mApi );
    nFailed = QTest::qExec( &activeProjectTest, mTestArgs );
  }
  else if ( mTestRequested == "--testProjectChecksumCache" )
  {
    TestProjectChecksumCache projectChecksumTest;
    nFailed = QTest::qExec( &projectChecksumTest, mTestArgs );
  }
  else if ( mTestRequested == "--testSketching" )
  {
    TestSketching sketchingTest;
    nFailed = QTest::qExec( &sketchingTest, mTestArgs );
  }
  else if ( mTestRequested == "--testMerginApi" )
  {
    TestMerginApi merginApiTest( mApi );
    QStringList args = mTestArgs;
    if ( !args.contains( "-maxwarnings" ) )
    {
      args << "-maxwarnings" << "0"; //show all debug output
    }

    // To pick just one particular test, uncomment
    // following line and add function name
    // args << "testRegisterAndDelete";
    nFailed = QTest::qExec( &merginApiTest, args );
  }
  else if ( mTestRequested == "--testMultiEditManager" )
  {
    TestMultiEditManager multiEditManagerTest;
    nFailed = QTest::qExec( &multiEditManagerTest, mTestArgs );
  }
  else
  {
    qDebug() << "invalid test requested" << mTestRequested;
    nFailed = 1001;
  }

  /*
   * WARNING:
   *
   * when you are adding new test here in the list, add
   * it also to test/CMakeLists.txt
   *
   */

  return nFailed;
}
