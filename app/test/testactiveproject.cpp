/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "testactiveproject.h"
#include "testutils.h"
#include "inpututils.h"
#include "activeproject.h"

#include <QtTest/QtTest>

TestActiveProject::TestActiveProject( MerginApi *api )
{
  mApi = api;
  Q_ASSERT( mApi );  // does not make sense to run without API
}

TestActiveProject::~TestActiveProject() = default;

void TestActiveProject::init()
{
}

void TestActiveProject::cleanup()
{
}

void TestActiveProject::testProjectLoadFailure()
{
  QString projectname = QStringLiteral( "testProjectLoadFailure" );
  QString projectdir = QDir::tempPath() + "/" + projectname;
  QString projectfilename = "project.qgz";

  InputUtils::cpDir( TestUtils::testDataDir() + "/load_failure", projectdir );

  AppSettings as;
  ActiveLayer al;
  LayersModel lm;
  LayersProxyModel lpm( &lm, LayerModelTypes::ActiveLayerSelection );
  ActiveProject activeProject( as, al, lpm, mApi->localProjectsManager() );

  mApi->localProjectsManager().addLocalProject( projectdir, projectname );

  QSignalSpy spy( &activeProject, &ActiveProject::projectReadingFailed );
  QVERIFY( !activeProject.load( projectdir + "/" + projectfilename ) );
  QVERIFY( !activeProject.localProject().isValid() );
  QVERIFY( spy.count() );
}
